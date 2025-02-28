#include "pwm.h"

#include <avr/io.h>
#include <avr_io_pin.h>

void pwm_init()
{
	/* WGMxx = 0011 -> phase correct 10bit */
	/* COM1A1 -> non-inverting */
	/* CSxx = 001 -> no prescaling */
	TCCR1A |= _BV(COM1A1) | _BV(WGM11) | _BV(WGM10);
	TCCR1B |= _BV(CS10);

	avr_io_pin_t out_pin;
	out_pin.port = &PORTB;
	out_pin.pin = PB1;
	set_pin_direction(&out_pin, AVR_IO_PIN_DIR_OUTPUT);
}

void set_pwm_value(uint16_t value)
{
	OCR1A = value & 0x03ff;
}
