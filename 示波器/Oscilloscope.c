/**
 * STM32F103VET6 双通道示波器 - 完整源码
 *
 * 主程序，包含所有外设配置和中断处理
 */

#include "stm32f10x.h"

/* 常量定义 */
#define ADC_CHANNEL_COUNT    2
#define ADC_BUFFER_SIZE      512
#define CMD_BUFFER_SIZE      16

/* 类型定义 */
typedef enum {
    STATE_RUNNING = 0,
    STATE_STOPPED
} RunState_Type;

typedef enum {
    CMD_START = 0,
    CMD_STOP,
    CMD_CH1,
    CMD_CH2,
    CMD_UNKNOWN
} Command_Type;

/* 全局变量 */
volatile RunState_Type g_run_state = STATE_STOPPED;
volatile uint8_t g_uart_rx_complete = 0;
volatile uint8_t g_new_data_flag = 0;

// ADC数据缓冲区（双通道）
volatile int16_t g_adc_buf[ADC_CHANNEL_COUNT][ADC_BUFFER_SIZE];

// DMA传输计数
volatile uint32_t g_dma_transfer_count = 0;

// UART
uint8_t g_uart_rx_byte = 0;
char g_cmd_buffer[CMD_BUFFER_SIZE];
uint8_t g_cmd_index = 0;

/* 函数声明 */
void SystemInit(void);
void SystemClock_Config(void);
void GPIO_Config(void);
void ADC_Config(void);
void DMA_Config(void);
void Timer_Config(void);
void USART_Config(void);
void NVIC_Config(void);
void RingBuffer_Init(void);
void Send_String(char *str);
void Send_Channel_Data(uint8_t channel);
Command_Type Parse_Command(char *cmd);
void Delay_ms(uint32_t ms);

/**
 * 主函数
 */
int main(void)
{
    // 初始化
    SystemInit();
    SystemClock_Config();

    GPIO_Config();
    DMA_Config();
    ADC_Config();
    Timer_Config();
    USART_Config();
    NVIC_Config();

    // 初始化环形缓冲区
    RingBuffer_Init();

    // 发送就绪信息
    Send_String("\r\n==============================\r\n");
    Send_String("  STM32 Oscilloscope Ready\r\n");
    Send_String("  Commands: START/STOP/CH1/CH2\r\n");
    Send_String("==============================\r\n\r\n");

    // 主循环
    while (1)
    {
        // ===== 按键检测 (PA0) =====
        // 按键按下时为低电平，松开为高电平（有上拉）
        if (!(GPIOA->IDR & (1 << 0)))  // PA0 按下
        {
            Delay_ms(50);  // 消抖
            if (!(GPIOA->IDR & (1 << 0)))  // 确认按键
            {
                if (g_run_state == STATE_RUNNING)
                {
                    // 停止采集
                    g_run_state = STATE_STOPPED;
                    TIM2->CR1 &= ~TIM_CR1_CEN;  // 停止定时器
                    Send_String("[KEY] STOP\r\n");
                }
                else
                {
                    // 开始采集
                    g_run_state = STATE_RUNNING;
                    g_dma_transfer_count = 0;
                    TIM2->CR1 |= TIM_CR1_CEN;   // 启动定时器
                    Send_String("[KEY] START\r\n");
                }

                // 等待按键释放
                while (!(GPIOA->IDR & (1 << 0)));
            }
        }

        // ===== UART命令处理 =====
        if (g_uart_rx_complete)
        {
            g_uart_rx_complete = 0;

            Command_Type cmd = Parse_Command(g_cmd_buffer);

            switch (cmd)
            {
                case CMD_START:
                    g_run_state = STATE_RUNNING;
                    g_dma_transfer_count = 0;
                    TIM2->CR1 |= TIM_CR1_CEN;
                    Send_String("START\r\n");
                    break;

                case CMD_STOP:
                    g_run_state = STATE_STOPPED;
                    TIM2->CR1 &= ~TIM_CR1_CEN;
                    Send_String("STOP\r\n");
                    break;

                case CMD_CH1:
                    Send_Channel_Data(0);
                    break;

                case CMD_CH2:
                    Send_Channel_Data(1);
                    break;

                default:
                    Send_String("ERR: Unknown command\r\n");
                    break;
            }

            // 清空命令缓冲区
            g_cmd_index = 0;
            for (uint8_t i = 0; i < CMD_BUFFER_SIZE; i++)
                g_cmd_buffer[i] = 0;
        }
    }
}

/**
 * 系统时钟配置
 * HSE = 8MHz, PLL = 9, SYSCLK = 72MHz
 */
void SystemClock_Config(void)
{
    // 启用HSE
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));  // 等待HSE就绪

    // 配置PLL: HSE / 1 * 9 = 72MHz
    RCC->CFGR = (RCC->CFGR & ~((uint32_t)0x003C0000)) | (0x07 << 18);  // PLL 9倍
    RCC->CFGR = (RCC->CFGR & ~((uint32_t)0x00020000)) | RCC_CFGR_PLLSRC;  // HSE作为PLL输入

    // 启用PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));  // 等待PLL就绪

    // 设置AHB分频: HCLK = 72MHz
    RCC->CFGR = (RCC->CFGR & ~((uint32_t)0x000000F0)) | (0 << 4);  // AHB 1分频

    // 设置APB1分频: PCLK1 = 36MHz (最大允许值)
    RCC->CFGR = (RCC->CFGR & ~((uint32_t)0x00000700)) | (0x04 << 8);  // APB1 2分频

    // 设置APB2分频: PCLK2 = 72MHz
    RCC->CFGR = (RCC->CFGR & ~((uint32_t)0x00003800)) | (0 << 11);  // APB2 1分频

    // 选择PLL作为系统时钟
    RCC->CFGR = (RCC->CFGR & ~((uint32_t)0x00000003)) | (0x02 << 0);  // PLL作为系统时钟
    while (((RCC->CFGR >> 2) & 0x03) != 0x02);  // 等待切换完成
}

/**
 * GPIO配置
 */
void GPIO_Config(void)
{
    // 使能GPIOA时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA0: 按键输入（带上拉）
    // 配置为输入模式，50MHz
    GPIOA->CRL = (GPIOA->CRL & ~((uint32_t)0x0000000F)) | (0x08 << 0);  // 输入上拉

    // PA1, PA2: ADC模拟输入
    // 配置为模拟输入
    GPIOA->CRL = (GPIOA->CRL & ~((uint32_t)0x000000F0)) | (0x00 << 4);  // PA1 模拟输入
    GPIOA->CRL = (GPIOA->CRL & ~((uint32_t)0x00000F00)) | (0x00 << 8);  // PA2 模拟输入

    // PA9: USART1 TX (复用推挽输出, 50MHz)
    // PA10: USART1 RX (浮空输入)
    GPIOA->CRH = (GPIOA->CRH & ~((uint32_t)0x00000FF0)) | (0x0B << 4);  // PA9 AF PP 50MHz
    GPIOA->CRH = (GPIOA->CRH & ~((uint32_t)0x0000F000)) | (0x04 << 12);  // PA10 INPUT

    // PA0 上拉使能
    GPIOA->ODR |= (1 << 0);
}

/**
 * DMA配置
 */
void DMA_Config(void)
{
    // 使能DMA1时钟
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    // DMA1 Channel 1 配置 (ADC1)
    // 外设地址: ADC1->DR
    // 内存地址: g_adc_buf
    // 传输方向: 外设到内存
    // 模式: 循环模式
    DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR);  // 外设地址
    DMA1_Channel1->CMAR = (uint32_t)g_adc_buf;    // 内存地址

    // 数量: 2通道 x 512点
    DMA1_Channel1->CNDTR = ADC_CHANNEL_COUNT * ADC_BUFFER_SIZE;

    // 配置: 外设8位，内存16位，循环模式
    DMA1_Channel1->CCR = DMA_CCR_MINC | DMA_CCR_PSIZE_HALFWORD |
                         DMA_CCR_MSIZE_HALFWORD | DMA_CCR_CIRC |
                         DMA_CCR_PL1;  // 高优先级
}

/**
 * ADC配置
 */
void ADC_Config(void)
{
    // 使能ADC1时钟
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // ADC时钟分频: PCLK2 / 6 = 12MHz (ADC最大14MHz)
    RCC->CFGR = (RCC->CFGR & ~((uint32_t)0x0000C000)) | (0x03 << 14);

    // ADC配置
    ADC1->CR1 = ADC_CR1_SCAN;  // 扫描模式
    ADC1->CR2 = ADC_CR2_DMA | ADC_CR2_EXTTRIG | ADC_EXTTRIG_T2TRGO;  // DMA使能, 外部触发(TIM2)
    ADC1->SMPR1 = 0;  // 采样时间
    ADC1->SMPR2 = 0;  // 采样时间

    // 通道配置
    // CH1 (PA1) -> rank 1
    ADC1->SQR3 = (ADC1->SQR3 & ~((uint32_t)0x1F)) | (1 << 0);
    // CH2 (PA2) -> rank 2
    ADC1->SQR3 = (ADC1->SQR3 & ~((uint32_t)0x3E0)) | (2 << 5);

    // 规则序列长度: 2个通道
    ADC1->SQR1 = (ADC1->SQR1 & ~((uint32_t)0x00F00000)) | ((ADC_CHANNEL_COUNT - 1) << 20);

    // 使能DMA请求
    ADC1->CR2 |= ADC_CR2_DMA;

    // 校准ADC
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while (ADC1->CR2 & ADC_CR2_RSTCAL);
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL);

    // 等待ADC稳定 (2us)
    for (volatile uint32_t i = 0; i < 100; i++);

    // 启动ADC
    ADC1->CR2 |= ADC_CR2_ADON;

    // 等待ADC就绪
    while (!(ADC1->SR & ADC_SR_EOC));

    // 清除EOC标志
    ADC1->SR = 0;
}

/**
 * 定时器2配置
 * 触发ADC，10kHz采样率
 * APB1 = 36MHz, Timer2 = 72MHz (2x)
 * 72MHz / 7200 = 10kHz
 */
void Timer_Config(void)
{
    // 使能TIM2时钟
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // 定时器配置
    TIM2->PSC = 0;           // 不分频
    TIM2->ARR = 7200 - 1;    // 重装载值 -> 10kHz

    // 选择更新事件作为触发输出
    TIM2->CR2 = (TIM2->CR2 & ~((uint16_t)0x0070)) | TIM_TRGO_OC1;

    // 配置触发输出
    TIM2->CCMR1 = (TIM2->CCMR1 & ~((uint16_t)0x0070)) | (0x04 << 4);  // OC1REF trigger

    // 使能自动重装载
    TIM2->CR1 = TIM_CR1_ARPE | TIM_CR1_DIR;

    // 不启动定时器，等待START命令
    // TIM2->CR1 |= TIM_CR1_CEN;
}

/**
 * USART1配置
 * 115200, 8N1
 */
void USART_Config(void)
{
    // 使能USART1时钟
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // 波特率: 72MHz / 115200 / 16 = 39.0625
    USART1->BRR = 39 << 4 | 1;  // 39.0625 -> 39

    // 配置: 8位数据, 1位停止, 无校验
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
    USART1->CR2 = 0;
    USART1->CR3 = 0;

    // 使能USART
    USART1->CR1 |= USART_CR1_UE;
}

/**
 * NVIC配置
 */
void NVIC_Config(void)
{
    // USART1中断
    // 这里使用软件方式配置简化的中断
}

/**
 * 环形缓冲区初始化
 */
void RingBuffer_Init(void)
{
    for (uint8_t ch = 0; ch < ADC_CHANNEL_COUNT; ch++)
    {
        for (uint16_t i = 0; i < ADC_BUFFER_SIZE; i++)
        {
            g_adc_buf[ch][i] = 0;
        }
    }
}

/**
 * 解析命令
 */
Command_Type Parse_Command(char *cmd)
{
    if (cmd[0] == 'S' && cmd[1] == 'T' && cmd[2] == 'A' && cmd[3] == 'R' && cmd[4] == 'T')
        return CMD_START;
    if (cmd[0] == 'S' && cmd[1] == 'T' && cmd[2] == 'O' && cmd[3] == 'P')
        return CMD_STOP;
    if (cmd[0] == 'C' && cmd[1] == 'H' && cmd[2] == '1')
        return CMD_CH1;
    if (cmd[0] == 'C' && cmd[1] == 'H' && cmd[2] == '2')
        return CMD_CH2;
    return CMD_UNKNOWN;
}

/**
 * 发送字符串
 */
void Send_String(char *str)
{
    while (*str)
    {
        USART1->DR = *str;
        while (!(USART1->SR & USART_SR_TXE));
        str++;
    }
}

/**
 * 发送通道数据（二进制格式）
 * 帧格式: 0xAA + Channel + Length(2B) + Data(1024B) + Checksum + 0x55
 */
void Send_Channel_Data(uint8_t channel)
{
    uint8_t header = 0xAA;
    uint8_t ch_id = channel + 1;
    uint16_t length = ADC_BUFFER_SIZE;
    uint16_t checksum = 0;

    // 等待发送缓冲为空
    while (!(USART1->SR & USART_SR_TXE));

    // 发送帧头
    USART1->DR = header;
    while (!(USART1->SR & USART_SR_TXE));

    // 发送通道号
    USART1->DR = ch_id;
    while (!(USART1->SR & USART_SR_TXE));

    // 发送长度（低字节在前）
    USART1->DR = length & 0xFF;
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = (length >> 8) & 0xFF;
    while (!(USART1->SR & USART_SR_TXE));

    // 发送数据
    for (uint16_t i = 0; i < ADC_BUFFER_SIZE; i++)
    {
        int16_t value = g_adc_buf[channel][i];
        uint8_t low = value & 0xFF;
        uint8_t high = (value >> 8) & 0xFF;

        USART1->DR = low;
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = high;
        while (!(USART1->SR & USART_SR_TXE));

        checksum += low + high;
    }

    // 发送校验和
    USART1->DR = checksum & 0xFF;
    while (!(USART1->SR & USART_SR_TXE));

    // 发送尾部
    USART1->DR = 0x55;
    while (!(USART1->SR & USART_SR_TXE));
}

/* ==================== 中断处理 ==================== */

/**
 * USART1中断处理
 */
void USART1_IRQHandler(void)
{
    if (USART1->SR & USART_SR_RXNE)
    {
        uint8_t byte = USART1->DR;

        if (g_cmd_index < CMD_BUFFER_SIZE - 1)
        {
            g_cmd_buffer[g_cmd_index++] = byte;

            // 检测命令结束
            if (byte == '\n' || byte == '\r')
            {
                g_cmd_buffer[g_cmd_index - 1] = 0;
                g_uart_rx_complete = 1;
            }
        }
        else
        {
            g_cmd_index = 0;
        }
    }
}

/* ==================== 延时函数 ==================== */

volatile uint32_t g_ms_tick = 0;

void Delay_ms(uint32_t ms)
{
    uint32_t start = g_ms_tick;
    while ((g_ms_tick - start) < ms);
}

void SysTick_Handler(void)
{
    g_ms_tick++;
}

/* ==================== 异常处理 ==================== */

void NMI_Handler(void) {}
void HardFault_Handler(void) { while (1); }
void MemManage_Handler(void) { while (1); }
void BusFault_Handler(void) { while (1); }
void UsageFault_Handler(void) { while (1); }
void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}
