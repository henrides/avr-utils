#ifndef _PWM_H_
#define _PWM_H_

#include <avr/io.h>

void pwm_init();
void set_pwm_value(uint16_t);

#endif /* _PWM_H_ */
