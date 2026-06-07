/**
 * STM32F103VET6 双通道示波器
 *
 * 硬件配置:
 *   - 主控: STM32F103VET6 (72MHz)
 *   - ADC通道: PA1 (CH1), PA2 (CH2)
 *   - 按键: PA0
 *   - 串口: USART1 (PA9/PA10)
 *
 * 功能:
 *   - 双通道ADC采集，10kHz采样率，10位分辨率
 *   - 定时器触发，DMA传输
 *   - 环形缓冲区，每通道512点
 *   - 串口命令控制: START/STOP/CH1/CH2
 *   - 按键切换运行/停止状态
 */

#ifndef __OSCILLOSCOPE_H
#define __OSCILLOSCOPE_H

#include <stdint.h>

/* 常量定义 */
#define ADC_CHANNEL_COUNT    2       // ADC通道数
#define ADC_BUFFER_SIZE     512     // 每通道缓冲区深度
#define ADC_RESOLUTION       10      // ADC分辨率位数
#define SAMPLING_RATE_HZ    10000    // 采样率 10kHz

/* 命令类型 */
typedef enum {
    CMD_START = 0,
    CMD_STOP,
    CMD_CH1,
    CMD_CH2,
    CMD_CH3,
    CMD_UNKNOWN
} Command_Type;

/* 运行状态 */
typedef enum {
    STATE_RUNNING = 0,
    STATE_STOPPED
} RunState_Type;

/* 通道数据结构 */
typedef struct {
    volatile int16_t buffer[ADC_BUFFER_SIZE];  // 数据缓冲区
    volatile uint16_t write_index;            // 写指针
    volatile uint8_t data_ready;              // 数据就绪标志
} ChannelData_Type;

/* 全局变量声明 */
extern volatile RunState_Type g_run_state;
extern volatile uint8_t g_uart_rx_byte;
extern volatile uint8_t g_uart_rx_complete;

#endif
