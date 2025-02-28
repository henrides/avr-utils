#ifndef _PWR_LATCH_H_
#define _PWR_LATCH_H_

#include <avr_io_pin.h>

typedef struct {
	avr_io_pin_t pwr_latch;
	avr_io_pin_t pwr_button;
} pwr_latch_conf_t;

void pwr_latch_init(pwr_latch_conf_t *);

#endif /* _PWR_LATCH_H_ */
