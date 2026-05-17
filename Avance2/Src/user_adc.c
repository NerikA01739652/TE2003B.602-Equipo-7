#include "main.h"
#include "user_adc.h"

/* ─────────────────────────────────────────────────────────────────────────────
 *  USER_ADC1_Init
 *
 *  Configura ADC1, canal 0 (PA0) en modo single conversion por software.
 *
 *  Conexión del potenciómetro:
 *    Pin central  → PA0
 *    Un extremo          → 3.3 V
 *    Otro extremo        → GND
 * ───────────────────────────────────────────────────────────────────────────── */
void USER_ADC1_Init(void) {

    /* 1. Habilitar clocks: GPIOA y ADC1 (APB2) */
    RCC->APB2ENR |= (1U <<  2);   /* GPIOA clock                        */
    RCC->APB2ENR |= (1U <<  9);   /* ADC1 clock                         */

    /* 2. PA0 como entrada analógica (CNF=00, MODE=00 → 0x0) */
    GPIOA->CRL &= ~(0xFU << 0);   /* Limpiar bits 3:0 → analógico       */

    /* 3. Seleccionar trigger por software y habilitarlo */
    ADC1->CR2 |= ADC_CR2_EXTSEL;   /* EXTSEL = 111 → SWSTART            */
    ADC1->CR2 |= ADC_CR2_EXTTRIG;  /* Habilitar trigger externo         */

    /* 4. Canal 0, 1 conversión, sample time = 55.5 ciclos (SMPR2 bits 2:0) */
    ADC1->SQR3  = 0;               /* Canal 0 en la secuencia            */
    ADC1->SMPR2 = (7U << 0);     /* 55.5 ciclos para CH0              */

    /* 5. Encender ADC */
    ADC1->CR2 |= ADC_CR2_ADON;

    /* 6. Calibración (requerida después del primer encendido)            */
    /* Pequeño delay de estabilización (~1 µs con 8 MHz)                 */
    for (volatile int i = 0; i < 5000; i++);

    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL);  /* Esperar fin de calibración     */
}

/* ─────────────────────────────────────────────────────────────────────────────
 *  USER_ADC1_Read
 *  Inicia una conversión por software y devuelve el resultado (0–4095).
 * ───────────────────────────────────────────────────────────────────────────── */
uint16_t USER_ADC1_Read(void) {
    ADC1->CR2 |= ADC_CR2_SWSTART;           /* Iniciar conversión        */
    while (!(ADC1->SR & ADC_SR_EOC));        /* Esperar End Of Conversion */
    return (uint16_t)(ADC1->DR & 0xFFF);    /* Regresar 12 bits          */
}
