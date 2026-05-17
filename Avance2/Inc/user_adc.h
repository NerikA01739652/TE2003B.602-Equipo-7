#ifndef USER_ADC_H_
#define USER_ADC_H_

/* ─────────────────────────────────────────────────────────────────────────────
 *  ADC utilizado: ADC1, Canal 0
 *
 *  Pin físico:
 *    PA0  →  ADC1_IN0  (pin central / wiper del potenciómetro)
 *
 *  Conexión del potenciómetro:
 *    Extremo 1  →  3.3 V
 *    Extremo 2  →  GND
 *    Pin central → PA0
 *
 *  Resolución: 12 bits  →  valores 0–4095
 * ───────────────────────────────────────────────────────────────────────────── */

#include <stdint.h>

/* Bits ADC */
#define ADC_CR2_ADON     (1U <<  0)   /* ADC ON               */
#define ADC_CR2_CAL      (1U <<  2)   /* Calibración          */
#define ADC_CR2_SWSTART  (1U << 22)   /* Inicio por software  */
#define ADC_CR2_EXTSEL   (7U << 17)   /* SWSTART como trigger */
#define ADC_CR2_EXTTRIG  (1U << 20)   /* Habilitar trigger    */
#define ADC_SR_EOC       (1U <<  1)   /* End Of Conversion    */

/* Prototipos */
void USER_ADC1_Init(void);
uint16_t USER_ADC1_Read(void);

#endif /* USER_ADC_H_ */
