#include "main.h"

#include "user_clock.h"
#include "user_uart.h"
#include "user_adc.h"
#include "user_pwm.h"
#include "user_gpio.h"
#include "user_timer.h"

#include "EngTrModel.h"

#include "lcd.h"
#include "user_i2c.h"
#include <stdio.h>

volatile uint8_t model_flag = 0;

/* ---------------------------------------------------- */
/* UART integer print                                   */
/* ---------------------------------------------------- */
static void Send_Int(uint32_t num)
{
    char buf[12];
    int i = 0;

    if(num == 0)
    {
        USER_USART1_SendChar('0');
        return;
    }

    while(num > 0)
    {
        buf[i++] = (num % 10U) + '0';
        num /= 10U;
    }

    while(i--)
    {
        USER_USART1_SendChar(buf[i]);
    }
}

/* ---------------------------------------------------- */
/* TIM2 ISR                                             */
/* ---------------------------------------------------- */
void TIM2_IRQHandler(void)
{
    if(TIM2->SR & 1U)
    {
        TIM2->SR &= ~(1U);

        model_flag = 1;
    }
}

/* ---------------------------------------------------- */
/* MAIN                                                 */
/* ---------------------------------------------------- */
int main(void)
{
    char line1[20];
    char line2[20];

    uint16_t adc;

    double throttle;
    double speed;
    double rpm;
    double gear;

    uint16_t pwm;

    /* Clock 64MHz */
    SystemClock_Config();

    /* Init peripherals */
    USER_USART1_Init();

    USER_ADC1_Init();

    USER_PWM_Init();

    USER_Button_Init();

    USER_TIM2_Init();

    /* LCD */
    USER_I2C1_Init();
    LCD_Init();
    LCD_Clear();

    /* Init model */
    EngTrModel_initialize();

    USER_USART1_SendString("System Ready\r\n");

    while(1)
    {
        if(model_flag)
        {
            model_flag = 0;

            /* ------------------------------------ */
            /* ADC                                  */
            /* ------------------------------------ */

            adc = USER_ADC1_Read();

           

            /*
             * Escalamiento:
             * 0-4095 -> 1.5 a 100
             */

            throttle = 1.5 + (((double)adc * 98.5) / 4095.0);

            /* ------------------------------------ */
            /* Entradas del modelo                  */
            /* ------------------------------------ */

            EngTrModel_U.Throttle = throttle;

            /*
             * USER_Button_Read()
             * devuelve:
             *
             * 1 -> botón presionado
             * 0 -> botón liberado
             */

            if(USER_Button_Read())
            {
                EngTrModel_U.BrakeTorque = 5000.0;
            }
            else
            {
                EngTrModel_U.BrakeTorque = 0.0;
            }

            /* ------------------------------------ */
            /* Ejecutar modelo                      */
            /* ------------------------------------ */

            EngTrModel_step();

            /* ------------------------------------ */
            /* Salidas del modelo                   */
            /* ------------------------------------ */

            speed = EngTrModel_Y.VehicleSpeed;

            rpm   = EngTrModel_Y.EngineSpeed;

            gear  = EngTrModel_Y.Gear;

            /* ------------------------------------ */
            /* PWM LEDs                             */
            /* ------------------------------------ */

            pwm = (uint16_t)(speed * 10.0);

            if(pwm > 1000)
            {
                pwm = 1000;
            }

            USER_PWM_SetDuty(pwm);

            /* ------------------------------------ */
            /* UART                                 */
            /* ------------------------------------ */

            Send_Int((uint32_t)rpm);

            USER_USART1_SendChar(',');

            Send_Int((uint32_t)speed);

            USER_USART1_SendChar(',');

            Send_Int((uint32_t)gear);

            USER_USART1_SendString("\r\n");


            /*lcd salidas*/
            sprintf(line1, "RPM:%lu", (uint32_t)rpm);

            sprintf(line2, "V:%lu G:%lu",
                    (uint32_t)speed,
                    (uint32_t)gear);

            LCD_Print(line1, line2);
        }
    }
}