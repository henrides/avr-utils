#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <avr_io_pin.h>

typedef void (*button_callback)();

typedef struct {
	avr_io_pin_t button;
	uint8_t internal_pullup;
	button_callback pressed_cb;
	button_callback released_cb;
} button_conf_t;

void button_init(button_conf_t *);


#endif /* _BUTTON_H_ */
