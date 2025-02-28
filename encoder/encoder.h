#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <avr_io_pin.h>

typedef void (*encoder_change_callback)();

typedef struct {
	avr_io_pin_t pin_a;
	avr_io_pin_t pin_b;
	encoder_change_callback increase_cb;
	encoder_change_callback decrease_cb;
} encoder_conf_t;

void encoder_init(encoder_conf_t *);

#endif /* _ENCODER_H_ */
