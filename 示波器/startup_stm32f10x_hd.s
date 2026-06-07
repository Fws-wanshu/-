;******************************************************************************
;* File Name          : startup_stm32f10x_hd.s
;* Description        : STM32F10x High Density Devices vector table for MDK-ARM
;*                      toolchain.  
;******************************************************************************

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     0x20010000              ; Top of Stack
                DCD     Reset_Handler           ; Reset Handler
                DCD     NMI_Handler             ; NMI Handler
                DCD     HardFault_Handler       ; Hard Fault Handler
                DCD     MemManage_Handler       ; MPU Fault Handler
                DCD     BusFault_Handler        ; Bus Fault Handler
                DCD     UsageFault_Handler      ; Usage Fault Handler
                DCD     0                       ; Reserved
                DCD     0                       ; Reserved
                DCD     0                       ; Reserved
                DCD     0                       ; Reserved
                DCD     SVC_Handler             ; SVCall Handler
                DCD     DebugMon_Handler        ; Debug Monitor Handler
                DCD     0                       ; Reserved
                DCD     PendSV_Handler          ; PendSV Handler
                DCD     SysTick_Handler         ; SysTick Handler

                ; External Interrupts
                DCD     WWDG_IRQHandler         ; Window Watchdog
                DCD     PVD_IRQHandler          ; PVD through EXTI Line detect
                DCD     TAMPER_IRQHandler       ; Tamper
                DCD     RTC_IRQHandler          ; RTC
                DCD     FLASH_IRQHandler        ; Flash
                DCD     RCC_IRQHandler          ; RCC
                DCD     EXTI0_IRQHandler        ; EXTI Line 0
                DCD     EXTI1_IRQHandler        ; EXTI Line 1
                DCD     EXTI2_IRQHandler        ; EXTI Line 2
                DCD     EXTI3_IRQHandler        ; EXTI Line 3
                DCD     EXTI4_IRQHandler        ; EXTI Line 4
                DCD     DMA1_Channel1_IRQHandler ; DMA1 Channel 1
                DCD     DMA1_Channel2_IRQHandler ; DMA1 Channel 2
                DCD     DMA1_Channel3_IRQHandler ; DMA1 Channel 3
                DCD     DMA1_Channel4_IRQHandler ; DMA1 Channel 4
                DCD     DMA1_Channel5_IRQHandler ; DMA1 Channel 5
                DCD     DMA1_Channel6_IRQHandler ; DMA1 Channel 6
                DCD     DMA1_Channel7_IRQHandler ; DMA1 Channel 7
                DCD     ADC1_2_IRQHandler       ; ADC1 and ADC2
                DCD     USB_HP_CAN1_TX_IRQHandler ; USB High Priority or CAN1 TX
                DCD     USB_LP_CAN1_RX0_IRQHandler ; USB Low Priority or CAN1 RX0
                DCD     CAN1_RX1_IRQHandler     ; CAN1 RX1
                DCD     CAN1_SCE_IRQHandler     ; CAN1 SCE
                DCD     EXTI9_5_IRQHandler      ; EXTI Line 9..5
                DCD     TIM1_BRK_IRQHandler     ; TIM1 Break
                DCD     TIM1_UP_IRQHandler      ; TIM1 Update
                DCD     TIM1_TRG_COM_IRQHandler ; TIM1 Trigger and Commutation
                DCD     TIM1_CC_IRQHandler      ; TIM1 Capture Compare
                DCD     TIM2_IRQHandler         ; TIM2
                DCD     TIM3_IRQHandler         ; TIM3
                DCD     TIM4_IRQHandler         ; TIM4
                DCD     I2C1_EV_IRQHandler      ; I2C1 Event
                DCD     I2C1_ER_IRQHandler      ; I2C1 Error
                DCD     I2C2_EV_IRQHandler      ; I2C2 Event
                DCD     I2C2_ER_IRQHandler      ; I2C2 Error
                DCD     SPI1_IRQHandler         ; SPI1
                DCD     SPI2_IRQHandler         ; SPI2
                DCD     USART1_IRQHandler       ; USART1
                DCD     USART2_IRQHandler       ; USART2
                DCD     USART3_IRQHandler       ; USART3
                DCD     EXTI15_10_IRQHandler    ; EXTI Line 15..10
                DCD     RTC_Alarm_IRQHandler    ; RTC Alarm through EXTI Line
                DCD     USBWakeUp_IRQHandler    ; USB Wakeup from suspend
                DCD     TIM8_BRK_IRQHandler     ; TIM8 Break
                DCD     TIM8_UP_IRQHandler      ; TIM8 Update
                DCD     TIM8_TRG_COM_IRQHandler ; TIM8 Trigger and Commutation
                DCD     TIM8_CC_IRQHandler      ; TIM8 Capture Compare
                DCD     ADC3_IRQHandler         ; ADC3
                DCD     FSMC_IRQHandler         ; FSMC
                DCD     SDIO_IRQHandler         ; SDIO
                DCD     TIM5_IRQHandler         ; TIM5
                DCD     SPI3_IRQHandler         ; SPI3
                DCD     UART4_IRQHandler        ; UART4
                DCD     UART5_IRQHandler        ; UART5
                DCD     TIM6_IRQHandler         ; TIM6
                DCD     TIM7_IRQHandler         ; TIM7
                DCD     DMA2_Channel1_IRQHandler ; DMA2 Channel1
                DCD     DMA2_Channel2_IRQHandler ; DMA2 Channel2
                DCD     DMA2_Channel3_IRQHandler ; DMA2 Channel3
                DCD     DMA2_Channel4_5_IRQHandler ; DMA2 Channel4 & Channel5
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset Handler
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  main
                IMPORT  SystemInit
                
                LDR     R0, =SystemInit
                BLX     R0
                
                LDR     R0, =main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler              [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler        [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler        [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler         [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler       [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler              [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler         [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler           [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler          [WEAK]
                B       .
                ENDP

                ; External Interrupt Handlers
WWDG_IRQHandler\
                PROC
                EXPORT  WWDG_IRQHandler          [WEAK]
                B       .
                ENDP
PVD_IRQHandler\
                PROC
                EXPORT  PVD_IRQHandler           [WEAK]
                B       .
                ENDP
TAMPER_IRQHandler\
                PROC
                EXPORT  TAMPER_IRQHandler        [WEAK]
                B       .
                ENDP
RTC_IRQHandler\
                PROC
                EXPORT  RTC_IRQHandler           [WEAK]
                B       .
                ENDP
FLASH_IRQHandler\
                PROC
                EXPORT  FLASH_IRQHandler         [WEAK]
                B       .
                ENDP
RCC_IRQHandler\
                PROC
                EXPORT  RCC_IRQHandler           [WEAK]
                B       .
                ENDP
EXTI0_IRQHandler\
                PROC
                EXPORT  EXTI0_IRQHandler         [WEAK]
                B       .
                ENDP
EXTI1_IRQHandler\
                PROC
                EXPORT  EXTI1_IRQHandler         [WEAK]
                B       .
                ENDP
EXTI2_IRQHandler\
                PROC
                EXPORT  EXTI2_IRQHandler         [WEAK]
                B       .
                ENDP
EXTI3_IRQHandler\
                PROC
                EXPORT  EXTI3_IRQHandler         [WEAK]
                B       .
                ENDP
EXTI4_IRQHandler\
                PROC
                EXPORT  EXTI4_IRQHandler         [WEAK]
                B       .
                ENDP
DMA1_Channel1_IRQHandler\
                PROC
                EXPORT  DMA1_Channel1_IRQHandler [WEAK]
                B       .
                ENDP
DMA1_Channel2_IRQHandler\
                PROC
                EXPORT  DMA1_Channel2_IRQHandler [WEAK]
                B       .
                ENDP
DMA1_Channel3_IRQHandler\
                PROC
                EXPORT  DMA1_Channel3_IRQHandler [WEAK]
                B       .
                ENDP
DMA1_Channel4_IRQHandler\
                PROC
                EXPORT  DMA1_Channel4_IRQHandler [WEAK]
                B       .
                ENDP
DMA1_Channel5_IRQHandler\
                PROC
                EXPORT  DMA1_Channel5_IRQHandler [WEAK]
                B       .
                ENDP
DMA1_Channel6_IRQHandler\
                PROC
                EXPORT  DMA1_Channel6_IRQHandler [WEAK]
                B       .
                ENDP
DMA1_Channel7_IRQHandler\
                PROC
                EXPORT  DMA1_Channel7_IRQHandler [WEAK]
                B       .
                ENDP
ADC1_2_IRQHandler\
                PROC
                EXPORT  ADC1_2_IRQHandler        [WEAK]
                B       .
                ENDP
USB_HP_CAN1_TX_IRQHandler\
                PROC
                EXPORT  USB_HP_CAN1_TX_IRQHandler [WEAK]
                B       .
                ENDP
USB_LP_CAN1_RX0_IRQHandler\
                PROC
                EXPORT  USB_LP_CAN1_RX0_IRQHandler [WEAK]
                B       .
                ENDP
CAN1_RX1_IRQHandler\
                PROC
                EXPORT  CAN1_RX1_IRQHandler      [WEAK]
                B       .
                ENDP
CAN1_SCE_IRQHandler\
                PROC
                EXPORT  CAN1_SCE_IRQHandler      [WEAK]
                B       .
                ENDP
EXTI9_5_IRQHandler\
                PROC
                EXPORT  EXTI9_5_IRQHandler       [WEAK]
                B       .
                ENDP
TIM1_BRK_IRQHandler\
                PROC
                EXPORT  TIM1_BRK_IRQHandler      [WEAK]
                B       .
                ENDP
TIM1_UP_IRQHandler\
                PROC
                EXPORT  TIM1_UP_IRQHandler       [WEAK]
                B       .
                ENDP
TIM1_TRG_COM_IRQHandler\
                PROC
                EXPORT  TIM1_TRG_COM_IRQHandler  [WEAK]
                B       .
                ENDP
TIM1_CC_IRQHandler\
                PROC
                EXPORT  TIM1_CC_IRQHandler       [WEAK]
                B       .
                ENDP
TIM2_IRQHandler\
                PROC
                EXPORT  TIM2_IRQHandler          [WEAK]
                B       .
                ENDP
TIM3_IRQHandler\
                PROC
                EXPORT  TIM3_IRQHandler          [WEAK]
                B       .
                ENDP
TIM4_IRQHandler\
                PROC
                EXPORT  TIM4_IRQHandler          [WEAK]
                B       .
                ENDP
I2C1_EV_IRQHandler\
                PROC
                EXPORT  I2C1_EV_IRQHandler       [WEAK]
                B       .
                ENDP
I2C1_ER_IRQHandler\
                PROC
                EXPORT  I2C1_ER_IRQHandler       [WEAK]
                B       .
                ENDP
I2C2_EV_IRQHandler\
                PROC
                EXPORT  I2C2_EV_IRQHandler       [WEAK]
                B       .
                ENDP
I2C2_ER_IRQHandler\
                PROC
                EXPORT  I2C2_ER_IRQHandler       [WEAK]
                B       .
                ENDP
SPI1_IRQHandler\
                PROC
                EXPORT  SPI1_IRQHandler          [WEAK]
                B       .
                ENDP
SPI2_IRQHandler\
                PROC
                EXPORT  SPI2_IRQHandler          [WEAK]
                B       .
                ENDP
USART1_IRQHandler\
                PROC
                EXPORT  USART1_IRQHandler        [WEAK]
                B       .
                ENDP
USART2_IRQHandler\
                PROC
                EXPORT  USART2_IRQHandler        [WEAK]
                B       .
                ENDP
USART3_IRQHandler\
                PROC
                EXPORT  USART3_IRQHandler        [WEAK]
                B       .
                ENDP
EXTI15_10_IRQHandler\
                PROC
                EXPORT  EXTI15_10_IRQHandler     [WEAK]
                B       .
                ENDP
RTC_Alarm_IRQHandler\
                PROC
                EXPORT  RTC_Alarm_IRQHandler     [WEAK]
                B       .
                ENDP
USBWakeUp_IRQHandler\
                PROC
                EXPORT  USBWakeUp_IRQHandler     [WEAK]
                B       .
                ENDP
TIM8_BRK_IRQHandler\
                PROC
                EXPORT  TIM8_BRK_IRQHandler      [WEAK]
                B       .
                ENDP
TIM8_UP_IRQHandler\
                PROC
                EXPORT  TIM8_UP_IRQHandler       [WEAK]
                B       .
                ENDP
TIM8_TRG_COM_IRQHandler\
                PROC
                EXPORT  TIM8_TRG_COM_IRQHandler  [WEAK]
                B       .
                ENDP
TIM8_CC_IRQHandler\
                PROC
                EXPORT  TIM8_CC_IRQHandler       [WEAK]
                B       .
                ENDP
ADC3_IRQHandler\
                PROC
                EXPORT  ADC3_IRQHandler          [WEAK]
                B       .
                ENDP
FSMC_IRQHandler\
                PROC
                EXPORT  FSMC_IRQHandler          [WEAK]
                B       .
                ENDP
SDIO_IRQHandler\
                PROC
                EXPORT  SDIO_IRQHandler          [WEAK]
                B       .
                ENDP
TIM5_IRQHandler\
                PROC
                EXPORT  TIM5_IRQHandler          [WEAK]
                B       .
                ENDP
SPI3_IRQHandler\
                PROC
                EXPORT  SPI3_IRQHandler          [WEAK]
                B       .
                ENDP
UART4_IRQHandler\
                PROC
                EXPORT  UART4_IRQHandler         [WEAK]
                B       .
                ENDP
UART5_IRQHandler\
                PROC
                EXPORT  UART5_IRQHandler         [WEAK]
                B       .
                ENDP
TIM6_IRQHandler\
                PROC
                EXPORT  TIM6_IRQHandler          [WEAK]
                B       .
                ENDP
TIM7_IRQHandler\
                PROC
                EXPORT  TIM7_IRQHandler          [WEAK]
                B       .
                ENDP
DMA2_Channel1_IRQHandler\
                PROC
                EXPORT  DMA2_Channel1_IRQHandler [WEAK]
                B       .
                ENDP
DMA2_Channel2_IRQHandler\
                PROC
                EXPORT  DMA2_Channel2_IRQHandler [WEAK]
                B       .
                ENDP
DMA2_Channel3_IRQHandler\
                PROC
                EXPORT  DMA2_Channel3_IRQHandler [WEAK]
                B       .
                ENDP
DMA2_Channel4_5_IRQHandler\
                PROC
                EXPORT  DMA2_Channel4_5_IRQHandler [WEAK]
                B       .
                ENDP

                ALIGN

                END
