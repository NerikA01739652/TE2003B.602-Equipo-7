#include "main.h"
#include "user_pwm.h"

void USER_PWM_Init(void)
{
    /* Clocks */
    RCC->APB2ENR |= (1U << 2); /* GPIOA */
    RCC->APB2ENR |= (1U << 3); /* GPIOB */
    RCC->APB1ENR |= (1U << 1); /* TIM3 */

    /* PA6 PA7 AF Push Pull */
    GPIOA->CRL &= ~(0xFFU << 24);
    GPIOA->CRL |=  (0xBBU << 24);

    /* PB0 PB1 AF Push Pull */
    GPIOB->CRL &= ~(0xFFU << 0);
    GPIOB->CRL |=  (0xBBU << 0);

    /* PWM mode */
    TIM3->CCMR1 |= (6U << 4);
    TIM3->CCMR1 |= (6U << 12);

    TIM3->CCMR2 |= (6U << 4);
    TIM3->CCMR2 |= (6U << 12);

    /* preload enable */
    TIM3->CCMR1 |= (1U << 3);
    TIM3->CCMR1 |= (1U << 11);

    TIM3->CCMR2 |= (1U << 3);
    TIM3->CCMR2 |= (1U << 11);

    /* Enable outputs */
    TIM3->CCER |= (1U << 0);
    TIM3->CCER |= (1U << 4);
    TIM3->CCER |= (1U << 8);
    TIM3->CCER |= (1U << 12);

    /*
     * PWM frequency
     *
     * 64MHz / 64 = 1MHz
     * ARR = 1000
     * PWM = 1kHz
     */

    TIM3->PSC = 63;
    TIM3->ARR = 1000;

    TIM3->CCR1 = 0;
    TIM3->CCR2 = 0;
    TIM3->CCR3 = 0;
    TIM3->CCR4 = 0;

    TIM3->CR1 |= (1U << 0);
}

void USER_PWM_SetDuty(uint16_t duty)
{
    if(duty > 1000)
        duty = 1000;

    TIM3->CCR1 = duty;
    TIM3->CCR2 = duty;
    TIM3->CCR3 = duty;
    TIM3->CCR4 = duty;
}