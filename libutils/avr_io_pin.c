#include "avr_io_pin.h"

#include <stdlib.h>
#include <avr/io.h>

volatile uint8_t *get_ddr_from_port(volatile uint8_t *port)
{
	volatile uint8_t *ddr = NULL;
	if (port == &PORTB) {
		ddr = &DDRB;
	} else if (port == &PORTC) {
		ddr = &DDRC;
	} else if (port == &PORTD) {
		ddr = &DDRD;
	}
	return ddr;
}

static volatile uint8_t *get_pin_from_port(volatile uint8_t *port)
{
	volatile uint8_t *pin = NULL;
	if (port == &PORTB) {
		pin = &PINB;
	} else if (port == &PORTC) {
		pin = &PINC;
	} else if (port == &PORTD) {
		pin = &PIND;
	}
	return pin;
}

void set_pin_direction(avr_io_pin_t *pin, avr_io_pin_dir_t direction)
{
	volatile uint8_t *ddr = get_ddr_from_port(pin->port);
	switch (direction) {
		case AVR_IO_PIN_DIR_INPUT:
			*ddr &= ~_BV(pin->pin);
			break;
		case AVR_IO_PIN_DIR_OUTPUT:
			*ddr |= _BV(pin->pin);
			break;
	}
}

void set_pin_pullup(avr_io_pin_t *pin, avr_io_pin_pullup_t pullup)
{
	switch (pullup) {
		case AVR_IO_PIN_PULLUP_ENABLED:
			*pin->port |= _BV(pin->pin);
			break;
		case AVR_IO_PIN_PULLUP_DISABLED:
			*pin->port &= ~_BV(pin->pin);
			break;
	}
}

uint8_t get_pin_value(avr_io_pin_t *pin)
{
	volatile uint8_t *pin_reg = get_pin_from_port(pin->port);
	uint8_t pin_val = (*pin_reg & _BV(pin->pin));

	return pin_val;
}

void set_pin_value(avr_io_pin_t *pin, avr_io_pin_output_t output)
{
	switch (output) {
		case AVR_IO_PIN_OUTPUT_HIGH:
			*pin->port |= _BV(pin->pin);
			break;
		case AVR_IO_PIN_OUTPUT_LOW:
			*pin->port &= ~_BV(pin->pin);
			break;
	}
}
