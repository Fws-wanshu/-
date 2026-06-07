/**
 * 系统初始化和系统时钟配置
 */

#include "stm32f10x.h"

#if !defined(HSE_VALUE)
#define HSE_VALUE ((uint32_t)8000000)
#endif

#if !defined(HSI_VALUE)
#define HSI_VALUE ((uint32_t)8000000)
#endif

/**
 * 初始化FLASH和设置PLL
 */
void SystemInit(void)
{
    RCC->CR |= (uint32_t)0x00000001;

    RCC->CFGR &= (uint32_t)0xF0FF0000;

    RCC->CR &= (uint32_t)0xFEF6FFFF;

    RCC->CR &= (uint32_t)0xFFFBFFFF;

    RCC->CFGR &= (uint32_t)0xFF80FFFF;

    RCC->CR &= (uint32_t)0xFEFFFFFF;

    RCC->CIR = 0x009F0000;

    SCB->CPACR |= ((uint32_t)0xFA050800);

    // 初始化SysTick定时器 (1ms中断)
    SysTick->LOAD = (72000000 / 1000) - 1;  // 72MHz / 1000 = 1ms
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

/**
 * 更新SystemCoreClock变量
 */
void SystemCoreClockUpdate(void)
{
    uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllmul = 0;

    tmp = RCC->CFGR & RCC_CFGR_SWS;

    switch (tmp)
    {
        case 0x00:
            SystemCoreClock = HSI_VALUE;
            break;
        case 0x04:
            SystemCoreClock = HSE_VALUE;
            break;
        case 0x08:
            pllsource = (RCC->CFGR & RCC_CFGR_PLLSRC) >> 16;
            pllmul = (RCC->CFGR & RCC_CFGR_PLLMULL) >> 18;

            if (pllsource == 0)
            {
                pllvco = (HSI_VALUE >> 1) * pllmul;
            }
            else
            {
                pllvco = (HSE_VALUE >> 1) * pllmul;
            }
            pllp = ((RCC->CFGR & RCC_CFGR_PLLXTPRE) >> 3) + 1;
            SystemCoreClock = pllvco / pllp;
            break;
        default:
            SystemCoreClock = HSI_VALUE;
            break;
    }

    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
    SystemCoreClock >>= tmp;
}

uint32_t SystemCoreClock = 72000000;
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
