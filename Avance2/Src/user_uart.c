#include "main.h"
#include "user_uart.h"
/* ----------------------------------------------------
 * USART1 Init
 *
 * PA9  -> TX
 * PA10 -> RX
 * -------------------------------------------------- */

void USER_USART1_Init(void)
{
    /* Clock GPIOA + USART1 */
    RCC->APB2ENR |= (1U << 2);
    RCC->APB2ENR |= (1U << 14);

    /* ----------------------------- */
    /* PA9 TX -> AF Push Pull 50MHz  */
    /* ----------------------------- */

    GPIOA->CRH &= ~(0xFU << 4);

    GPIOA->CRH |= (0xBU << 4);

    /* ----------------------------- */
    /* PA10 RX -> Floating input     */
    /* ----------------------------- */

    GPIOA->CRH &= ~(0xFU << 8);

    GPIOA->CRH |= (0x4U << 8);

    /* Baudrate */
    USART1->BRR = USARTDIV;

    /* Enable USART + TX */
    USART1->CR1 =
        USART_CR1_UE |
        USART_CR1_TE;
}

/* ---------------------------------------------------- */

void USER_USART1_SendChar(char c)
{
    while(!(USART1->SR & USART_SR_TXE));

    USART1->DR = (uint32_t)c;
}

/* ---------------------------------------------------- */

void USER_USART1_SendString(const char *str)
{
    while(*str)
    {
        USER_USART1_SendChar(*str++);
    }
}