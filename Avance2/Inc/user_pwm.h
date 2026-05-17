#ifndef USER_PWM_H_
#define USER_PWM_H_

#include <stdint.h>

void USER_PWM_Init(void);
void USER_PWM_SetDuty(uint16_t duty);

#endif