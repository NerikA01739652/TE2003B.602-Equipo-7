#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>

/* ─── FLASH ─────────────────────────────────────────────────────────────── */
typedef struct {
	volatile uint32_t ACR, KEYR, OPTKEYR, SR, CR, AR, reserved, OBR, WRPR;
} FLASH_TypeDef;

/* ─── RCC ────────────────────────────────────────────────────────────────── */
typedef struct {
	volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR,
	                  AHBENR, APB2ENR, APB1ENR, BDCR, CSR;
} RCC_TypeDef;

/* ─── GPIO ───────────────────────────────────────────────────────────────── */
typedef struct {
	volatile uint32_t CRL, CRH, IDR, ODR, BSRR, BRR, LCKR;
} GPIO_TypeDef;

/* ─── USART ──────────────────────────────────────────────────────────────── */
typedef struct {
	volatile uint32_t SR, DR, BRR, CR1, CR2, CR3, GTPR;
} USART_TypeDef;

/*I2C*/
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t CCR;
    volatile uint32_t TRISE;
} I2C_TypeDef;

/* ─── ADC ────────────────────────────────────────────────────────────────── */
typedef struct {
	volatile uint32_t SR, CR1, CR2, SMPR1, SMPR2,
	                  JOFR1, JOFR2, JOFR3, JOFR4,
	                  HTR, LTR, SQR1, SQR2, SQR3,
	                  JSQR, JDR1, JDR2, JDR3, JDR4, DR;
} ADC_TypeDef;

/* ─── TIM ────────────────────────────────────────────────────────────────── */
typedef struct {
    volatile uint32_t CR1, CR2, SMCR, DIER, SR, EGR,
                      CCMR1, CCMR2, CCER, CNT, PSC, ARR,
                      RCR, CCR1, CCR2, CCR3, CCR4,
                      BDTR, DCR, DMAR;
} TIM_TypeDef;

/* ─── AFIO ───────────────────────────────────────────────────────────────── */
typedef struct {
    volatile uint32_t EVCR, MAPR, EXTICR1, EXTICR2,
                      EXTICR3, EXTICR4, MAPR2;
} AFIO_TypeDef;

/* ─── Direcciones base ───────────────────────────────────────────────────── */
#define FLASH_BASE   0x40022000UL
#define RCC_BASE     0x40021000UL
#define GPIOA_BASE   0x40010800UL
#define GPIOB_BASE   0x40010C00UL
#define GPIOC_BASE   0x40011000UL
#define USART1_BASE  0x40013800UL
#define USART2_BASE  0x40004400UL
#define ADC1_BASE    0x40012400UL
#define I2C1_BASE    0x40005400UL

#define AFIO_BASE   0x40010000UL
#define TIM2_BASE   0x40000000UL
#define TIM3_BASE   0x40000400UL

/* ─── Punteros a periféricos ─────────────────────────────────────────────── */
#define FLASH  ((FLASH_TypeDef *) FLASH_BASE)
#define RCC    ((RCC_TypeDef *)   RCC_BASE)
#define GPIOA  ((GPIO_TypeDef *)  GPIOA_BASE)
#define GPIOB  ((GPIO_TypeDef *)  GPIOB_BASE)
#define GPIOC  ((GPIO_TypeDef *)  GPIOC_BASE)
#define USART1 ((USART_TypeDef *) USART1_BASE)
#define USART2 ((USART_TypeDef *) USART2_BASE)
#define ADC1   ((ADC_TypeDef *)   ADC1_BASE)
#define I2C1   ((I2C_TypeDef *) I2C1_BASE)

#define AFIO ((AFIO_TypeDef *) AFIO_BASE)
#define TIM2 ((TIM_TypeDef *) TIM2_BASE)
#define TIM3 ((TIM_TypeDef *) TIM3_BASE)

/* ─── Prototipos globales ────────────────────────────────────────────────── */
void SystemClock_Config(void);

#endif /* MAIN_H_ */
