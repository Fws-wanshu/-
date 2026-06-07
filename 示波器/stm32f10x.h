/**
 * STM32F10x 外设寄存器定义
 * 完整版 - 包含所有需要的寄存器定义
 */

#ifndef __STM32F10x_H
#define __STM32F10x_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 类型定义
#define __IO volatile
#define __I  volatile const

// 外设基地址
#define PERIPH_BASE           ((uint32_t)0x40000000)
#define APB1_BASE             PERIPH_BASE
#define APB2_BASE             (PERIPH_BASE + 0x10000)
#define AHB_BASE              (PERIPH_BASE + 0x20000)
#define SCS_BASE              (0xE000E000)
#define SCB_BASE              (SCS_BASE + 0x0D00)
#define SysTick_BASE          (SCS_BASE + 0x0010)

// RCC
#define RCC_BASE              (AHB_BASE + 0x1000)
#define RCC                   ((RCC_TypeDef *) RCC_BASE)

// GPIO
#define GPIOA_BASE            (APB2_BASE + 0x0800)
#define GPIOA                 ((GPIO_TypeDef *) GPIOA_BASE)

// USART1
#define USART1_BASE           (APB2_BASE + 0x3800)
#define USART1                ((USART_TypeDef *) USART1_BASE)

// TIM2
#define TIM2_BASE             (APB1_BASE + 0x0000)
#define TIM2                  ((TIM_TypeDef *) TIM2_BASE)

// ADC1
#define ADC1_BASE             (APB2_BASE + 0x2000)
#define ADC1                  ((ADC_TypeDef *) ADC1_BASE)

// DMA1
#define DMA1_BASE             (AHB_BASE + 0x0000)
#define DMA1                  ((DMA_TypeDef *) DMA1_BASE)
#define DMA1_Channel1         ((DMA_Channel_TypeDef *) (DMA1_BASE + 0x0008))

// SCB
#define SCB                   ((SCB_TypeDef *) SCB_BASE)

// SysTick
#define SysTick               ((SysTick_TypeDef *) SysTick_BASE)

// RCC寄存器结构
typedef struct {
  __IO uint32_t CR;
  __IO uint32_t CFGR;
  __IO uint32_t CIR;
  __IO uint32_t APB2RSTR;
  __IO uint32_t APB1RSTR;
  __IO uint32_t AHBENR;
  __IO uint32_t APB2ENR;
  __IO uint32_t APB1ENR;
  __IO uint32_t BDCR;
  __IO uint32_t CSR;
} RCC_TypeDef;

// GPIO寄存器结构
typedef struct {
  __IO uint32_t CRL;
  __IO uint32_t CRH;
  __IO uint32_t IDR;
  __IO uint32_t ODR;
  __IO uint32_t BSRR;
  __IO uint32_t BRR;
  __IO uint32_t LCKR;
} GPIO_TypeDef;

// USART寄存器结构
typedef struct {
  __IO uint32_t SR;
  __IO uint32_t DR;
  __IO uint32_t BRR;
  __IO uint32_t CR1;
  __IO uint32_t CR2;
  __IO uint32_t CR3;
  __IO uint32_t GTPR;
} USART_TypeDef;

// TIM寄存器结构
typedef struct {
  __IO uint16_t CR1;
  __IO uint16_t CR2;
  __IO uint16_t SMCR;
  __IO uint16_t DIER;
  __IO uint16_t SR;
  __IO uint16_t EGR;
  __IO uint16_t CCMR1;
  __IO uint16_t CCMR2;
  __IO uint16_t CCER;
  __IO uint16_t CNT;
  __IO uint16_t PSC;
  __IO uint16_t ARR;
  __IO uint16_t RCR;
  __IO uint16_t CCR1;
  __IO uint16_t CCR2;
  __IO uint16_t CCR3;
  __IO uint16_t CCR4;
  __IO uint16_t BDTR;
  __IO uint16_t DCR;
  __IO uint16_t DMAR;
} TIM_TypeDef;

// ADC寄存器结构
typedef struct {
  __IO uint32_t SR;
  __IO uint32_t CR1;
  __IO uint32_t CR2;
  __IO uint32_t SMPR1;
  __IO uint32_t SMPR2;
  __IO uint32_t JOFR1;
  __IO uint32_t JOFR2;
  __IO uint32_t JOFR3;
  __IO uint32_t JOFR4;
  __IO uint32_t HTR;
  __IO uint32_t LTR;
  __IO uint32_t SQR1;
  __IO uint32_t SQR2;
  __IO uint32_t SQR3;
  __IO uint32_t JSQR;
  __IO uint32_t JDR1;
  __IO uint32_t JDR2;
  __IO uint32_t JDR3;
  __IO uint32_t JDR4;
  __IO uint32_t DR;
} ADC_TypeDef;

// DMA通道寄存器结构
typedef struct {
  __IO uint32_t CCR;
  __IO uint32_t CNDTR;
  __IO uint32_t CPAR;
  __IO uint32_t CMAR;
} DMA_Channel_TypeDef;

// DMA寄存器结构
typedef struct {
  __IO uint32_t ISR;
  __IO uint32_t IFCR;
} DMA_TypeDef;

// SCB寄存器结构
typedef struct {
  __I  uint32_t CPUID;
  __IO uint32_t ICSR;
  __IO uint32_t VTOR;
  __IO uint32_t AIRCR;
  __IO uint32_t SCR;
  __IO uint32_t CCR;
  __IO uint8_t  SHP[12];
  __IO uint32_t SHCSR;
  __IO uint32_t CFSR;
  __IO uint32_t HFSR;
  __IO uint32_t DFSR;
  __IO uint32_t MMFAR;
  __IO uint32_t BFAR;
  __IO uint32_t AFSR;
  __IO uint32_t CPACR;
} SCB_TypeDef;

// SysTick寄存器结构
typedef struct {
  __IO uint32_t CTRL;
  __IO uint32_t LOAD;
  __IO uint32_t VAL;
  __I  uint32_t CALIB;
} SysTick_TypeDef;

// RCC位定义
#define RCC_CR_HSEON          ((uint32_t)0x00010000)
#define RCC_CR_HSERDY         ((uint32_t)0x00020000)
#define RCC_CR_PLLON          ((uint32_t)0x01000000)
#define RCC_CR_PLLRDY         ((uint32_t)0x02000000)

#define RCC_CFGR_PLLSRC       ((uint32_t)0x00010000)
#define RCC_CFGR_PLLMULL      ((uint32_t)0x003C0000)
#define RCC_CFGR_SW           ((uint32_t)0x00000003)
#define RCC_CFGR_SWS          ((uint32_t)0x0000000C)
#define RCC_CFGR_HPRE         ((uint32_t)0x000000F0)
#define RCC_CFGR_PPRE1        ((uint32_t)0x00000700)
#define RCC_CFGR_PPRE2        ((uint32_t)0x00003800)
#define RCC_CFGR_PLLXTPRE     ((uint32_t)0x00020000)

#define RCC_APB2ENR_ADC1EN    ((uint32_t)0x00000200)
#define RCC_APB2ENR_USART1EN  ((uint32_t)0x00004000)
#define RCC_APB2ENR_IOPAEN    ((uint32_t)0x00000004)

#define RCC_APB1ENR_TIM2EN    ((uint32_t)0x00000001)

#define RCC_AHBENR_DMA1EN     ((uint32_t)0x00000001)

// GPIO位定义
#define GPIO_IDR_IDR0         ((uint32_t)0x00000001)

// USART位定义
#define USART_SR_TXE          ((uint32_t)0x00000080)
#define USART_SR_RXNE         ((uint32_t)0x00000020)

#define USART_CR1_TE          ((uint32_t)0x00000008)
#define USART_CR1_RE          ((uint32_t)0x00000004)
#define USART_CR1_RXNEIE      ((uint32_t)0x00000020)
#define USART_CR1_UE          ((uint32_t)0x00000001)

// TIM位定义
#define TIM_CR1_CEN           ((uint16_t)0x0001)
#define TIM_CR1_ARPE          ((uint16_t)0x0080)
#define TIM_CR1_DIR           ((uint16_t)0x0010)
#define TIM_CR2_MMS           ((uint16_t)0x0070)
#define TIM_TRGO_OC1          ((uint16_t)0x0000)

// ADC位定义
#define ADC_CR1_SCAN          ((uint32_t)0x00000100)
#define ADC_CR2_DMA           ((uint32_t)0x00000100)
#define ADC_CR2_EXTTRIG       ((uint32_t)0x00000100)
#define ADC_EXTTRIG_T2TRGO    ((uint32_t)0x00020000)
#define ADC_CR2_ADON          ((uint32_t)0x00000001)
#define ADC_CR2_RSTCAL        ((uint32_t)0x00000008)
#define ADC_CR2_CAL           ((uint32_t)0x00000004)
#define ADC_CR2_CONT          ((uint32_t)0x00000002)

#define ADC_SR_EOC            ((uint32_t)0x00000002)

// DMA位定义
#define DMA_CCR_EN            ((uint32_t)0x00000001)
#define DMA_CCR_TCIE          ((uint32_t)0x00000002)
#define DMA_CCR_MINC          ((uint32_t)0x00000080)
#define DMA_CCR_PINC          ((uint32_t)0x00000040)
#define DMA_CCR_CIRC          ((uint32_t)0x00000020)
#define DMA_CCR_DIR           ((uint32_t)0x00000010)
#define DMA_CCR_PL            ((uint32_t)0x00000C00)
#define DMA_CCR_PL0           ((uint32_t)0x00000000)
#define DMA_CCR_PL1           ((uint32_t)0x00000400)

#define DMA_CCR_PSIZE         ((uint32_t)0x00000100)
#define DMA_CCR_MSIZE         ((uint32_t)0x00000200)
#define DMA_CCR_PSIZE_BYTE    ((uint32_t)0x00000000)
#define DMA_CCR_PSIZE_HALFWORD ((uint32_t)0x00000100)
#define DMA_CCR_MSIZE_BYTE    ((uint32_t)0x00000000)
#define DMA_CCR_MSIZE_HALFWORD ((uint32_t)0x00000200)

// SysTick位定义
#define SysTick_CTRL_CLKSOURCE_Msk    ((uint32_t)0x00000004)
#define SysTick_CTRL_TICKINT_Msk      ((uint32_t)0x00000002)
#define SysTick_CTRL_ENABLE_Msk       ((uint32_t)0x00000001)

// 系统时钟变量
extern uint32_t SystemCoreClock;
extern const uint8_t AHBPrescTable[16];

#ifdef __cplusplus
}
#endif

#endif
