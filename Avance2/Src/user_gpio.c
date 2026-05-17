#include "main.h"
#include "user_gpio.h"

void USER_Button_Init(void)
{
    RCC->APB2ENR |= (1U << 3);

    /*
     * PB10 input pull-up
     */

    GPIOB->CRH &= ~(0xFU << 8);
    GPIOB->CRH |=  (0x8U << 8);

    GPIOB->ODR |= (1U << 10);
}

uint8_t USER_Button_Read(void)
{
    return ((GPIOB->IDR & (1U << 10)) == 0);
}