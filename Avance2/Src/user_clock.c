#include "main.h"
#include "user_clock.h"

void SystemClock_Config(void)
{
    /* FLASH latency = 2WS */
    FLASH->ACR |= (0x2U << 0);

    /* HSE ON */
    RCC->CR |= (1U << 16);

    while(!(RCC->CR & (1U << 17)));

    /*
     * PLL source = HSE
     * PLL x8
     */

    RCC->CFGR |= (1U << 16);

    RCC->CFGR &= ~(0xFU << 18);
    RCC->CFGR |=  (0x6U << 18);

    /*
     * APB1 = SYSCLK/2 = 32MHz
     */

    RCC->CFGR &= ~(0x7U << 8);
    RCC->CFGR |=  (0x4U << 8);

    /*
     * ADC prescaler
     * PCLK2 / 6
     * 64MHz / 6 = 10.6MHz
     */

    RCC->CFGR &= ~(0x3U << 14);
    RCC->CFGR |=  (0x2U << 14);

    /* PLL ON */
    RCC->CR |= (1U << 24);

    while(!(RCC->CR & (1U << 25)));

    /* Select PLL */
    RCC->CFGR &= ~(0x3U << 0);
    RCC->CFGR |=  (0x2U << 0);

    while(((RCC->CFGR >> 2) & 0x3U) != 0x2U);
}