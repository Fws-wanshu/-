/**
 * STM32F103VET6 双通道示波器 - HAL版本备用实现
 * 此文件包含HAL库版本的实现，主程序在Oscilloscope.c中（纯寄存器级实现）
 * 
 * 如需使用此HAL版本：
 * 1. 取消注释下面的 #if 0 块
 * 2. 注释 Oscilloscope.c 中的 main() 函数
 * 3. 确保已添加STM32 HAL库到工程中
 */

#if 0

#include "stm32f10x.h"
#include "stm32f1xx_hal.h"

// 类型定义
typedef enum {
    STATE_RUNNING = 0,
    STATE_STOPPED
} RunState_Type;

// 常量定义
#define ADC_CHANNEL_COUNT    2
#define ADC_BUFFER_SIZE      512
#define CMD_BUFFER_SIZE      16

// 全局变量
volatile RunState_Type g_run_state = STATE_STOPPED;
volatile uint8_t g_uart_rx_complete = 0;
volatile uint16_t g_adc_dma_pos = 0;

// 通道数据（环形缓冲区）
volatile int16_t g_adc_buf[ADC_CHANNEL_COUNT][ADC_BUFFER_SIZE];
volatile uint16_t g_adc_wp[ADC_CHANNEL_COUNT] = {0};
volatile uint8_t g_adc_data_ready[ADC_CHANNEL_COUNT] = {0};

// UART
uint8_t g_uart_rx_byte = 0;
char g_cmd_buffer[CMD_BUFFER_SIZE];
uint8_t g_cmd_index = 0;

// 串口发送缓冲
uint8_t g_tx_buf[4];

// 函数声明
void SystemClock_Config(void);
void GPIO_Config(void);
void ADC_DMA_Config(void);
void Timer_Config(void);
void USART_Config(void);
void NVIC_Config(void);
void Send_String(char *str);
void Send_Channel_Data(uint8_t ch);
void Process_Command(void);
int Parse_Command(char *cmd);

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_adc;
ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim2;

// 主函数
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    GPIO_Config();
    ADC_DMA_Config();
    Timer_Config();
    USART_Config();
    NVIC_Config();

    Send_String("\r\n=== STM32 Oscilloscope v1.0 ===\r\n");
    Send_String("Commands: START, STOP, CH1, CH2\r\n");

    while (1)
    {
        // 按键扫描（PA0） - 切换运行/停止
        if (GPIOA->IDR & GPIO_IDR_IDR0)
        {
            HAL_Delay(50);
            if (GPIOA->IDR & GPIO_IDR_IDR0)
            {
                if (g_run_state == STATE_RUNNING)
                {
                    g_run_state = STATE_STOPPED;
                    TIM2->CR1 &= ~TIM_CR1_CEN;
                    Send_String("STOP\r\n");
                }
                else
                {
                    g_run_state = STATE_RUNNING;
                    g_adc_wp[0] = g_adc_wp[1] = 0;
                    TIM2->CR1 |= TIM_CR1_CEN;
                    Send_String("START\r\n");
                }
                while (GPIOA->IDR & GPIO_IDR_IDR0);
            }
        }

        // 处理UART命令
        if (g_uart_rx_complete)
        {
            g_uart_rx_complete = 0;
            Process_Command();
        }
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void ADC_DMA_Config(void)
{
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    hdma_adc.Instance = DMA1_Channel1;
    hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc.Init.Mode = DMA_CIRCULAR;
    hdma_adc.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_adc);

    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc);

    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = ADC_CHANNEL_COUNT;
    HAL_ADC_Init(&hadc1);

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = 2;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)g_adc_buf, ADC_BUFFER_SIZE * ADC_CHANNEL_COUNT);
}

void Timer_Config(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 7200 - 1;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&htim2);

    HAL_TIMEx_SelectOutputTrigger(&htim2, TIM_TRGO_OC1REF);
}

void USART_Config(void)
{
    __HAL_RCC_USART1_CLK_ENABLE();

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);

    HAL_UART_Receive_IT(&huart1, &g_uart_rx_byte, 1);
}

void NVIC_Config(void)
{
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}

void Process_Command(void)
{
    int cmd = Parse_Command(g_cmd_buffer);

    switch (cmd)
    {
        case 0:
            g_run_state = STATE_RUNNING;
            g_adc_wp[0] = g_adc_wp[1] = 0;
            TIM2->CR1 |= TIM_CR1_CEN;
            Send_String("START\r\n");
            break;

        case 1:
            g_run_state = STATE_STOPPED;
            TIM2->CR1 &= ~TIM_CR1_CEN;
            Send_String("STOP\r\n");
            break;

        case 2:
            Send_Channel_Data(0);
            break;

        case 3:
            Send_Channel_Data(1);
            break;

        default:
            Send_String("ERR: Unknown command\r\n");
            break;
    }

    g_cmd_index = 0;
    for (int i = 0; i < CMD_BUFFER_SIZE; i++)
        g_cmd_buffer[i] = 0;
}

int Parse_Command(char *cmd)
{
    if (cmd[0] == 'S' && cmd[1] == 'T' && cmd[2] == 'A' && cmd[3] == 'R' && cmd[4] == 'T')
        return 0;
    if (cmd[0] == 'S' && cmd[1] == 'T' && cmd[2] == 'O' && cmd[3] == 'P')
        return 1;
    if (cmd[0] == 'C' && cmd[1] == 'H' && cmd[2] == '1')
        return 2;
    if (cmd[0] == 'C' && cmd[1] == 'H' && cmd[2] == '2')
        return 3;
    return -1;
}

void Send_String(char *str)
{
    while (*str)
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)str, 1, 100);
        str++;
    }
}

void Send_Channel_Data(uint8_t ch)
{
    uint8_t header = 0xAA;
    uint8_t channel = ch + 1;
    uint16_t length = ADC_BUFFER_SIZE;
    uint16_t checksum = 0;

    HAL_UART_Transmit(&huart1, &header, 1, 100);
    HAL_UART_Transmit(&huart1, &channel, 1, 100);
    HAL_UART_Transmit(&huart1, (uint8_t*)&length, 2, 100);

    for (uint16_t i = 0; i < ADC_BUFFER_SIZE; i++)
    {
        uint8_t low = g_adc_buf[ch][i] & 0xFF;
        uint8_t high = (g_adc_buf[ch][i] >> 8) & 0xFF;
        HAL_UART_Transmit(&huart1, &low, 1, 100);
        HAL_UART_Transmit(&huart1, &high, 1, 100);
        checksum += low + high;
    }

    uint8_t cs = checksum & 0xFF;
    HAL_UART_Transmit(&huart1, &cs, 1, 100);

    uint8_t tail = 0x55;
    HAL_UART_Transmit(&huart1, &tail, 1, 100);
}

void DMA1_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc);
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if (g_cmd_index < CMD_BUFFER_SIZE - 1)
        {
            g_cmd_buffer[g_cmd_index++] = g_uart_rx_byte;

            if (g_uart_rx_byte == '\n' || g_uart_rx_byte == '\r')
            {
                g_cmd_buffer[g_cmd_index - 1] = 0;
                g_uart_rx_complete = 1;
            }
        }
        else
        {
            g_cmd_index = 0;
        }

        HAL_UART_Receive_IT(&huart1, &g_uart_rx_byte, 1);
    }
}

#endif
