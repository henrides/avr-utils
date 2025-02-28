#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include <pwr_latch.h>

int main(void)
{
	pwr_latch_conf_t conf;
	
	conf.pwr_latch.port = &PORTD;
	conf.pwr_latch.pin = PD6;
	conf.pwr_button.port = &PORTD;
	conf.pwr_button.pin = PD7;

	pwr_latch_init(&conf);

	while (1) {
		_delay_ms(500);
	}
	return 0;
}
