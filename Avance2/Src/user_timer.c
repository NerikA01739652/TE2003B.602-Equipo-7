#include "main.h"
#include "user_timer.h"

#define NVIC_ISER0 (*(volatile uint32_t*)0xE000E100)

void USER_TIM2_Init(void)
{
    RCC->APB1ENR |= (1U << 0);

    /*
     * 64MHz
     * PSC = 63999
     * timer = 1kHz
     *
     * ARR = 39
     * overflow = 40ms
     */

    TIM2->PSC = 63999;
    TIM2->ARR = 39;

    TIM2->DIER |= (1U << 0);

    NVIC_ISER0 |= (1U << 28);

    TIM2->CR1 |= (1U << 0);
}