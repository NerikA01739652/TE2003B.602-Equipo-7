#ifndef USER_UART_H_
#define USER_UART_H_

#include "main.h"
#include <stdint.h>

/* ----------------------------------------------------
 * USART1
 *
 * PA9  -> TX
 * PA10 -> RX
 *
 * Baudrate: 9600
 * PCLK2 = 64MHz
 * -------------------------------------------------- */


#define USARTDIV 0x1A0B

/* Bits */
#define USART_CR1_UE    (1U << 13)
#define USART_CR1_TE    (1U << 3)
#define USART_SR_TXE    (1U << 7)

/* Prototipos */
void USER_USART1_Init(void);
void USER_USART1_SendChar(char c);
void USER_USART1_SendString(const char *str);

#endif