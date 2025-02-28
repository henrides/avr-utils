#ifndef _AVR_IO_PIN_H_
#define _AVR_IO_PIN_H_

#include <stdint.h>

#define SET_IO_PIN_(var, p, x) \
    var.port = &PORT##p; \
    var.pin = x;

#define SET_IO_PIN(var, x) SET_IO_PIN_(var, x)

typedef struct {
	volatile uint8_t *port;
	uint8_t pin;
} avr_io_pin_t;

typedef enum { AVR_IO_PIN_DIR_INPUT, AVR_IO_PIN_DIR_OUTPUT} avr_io_pin_dir_t;
typedef enum { AVR_IO_PIN_PULLUP_ENABLED, AVR_IO_PIN_PULLUP_DISABLED} avr_io_pin_pullup_t;
typedef enum { AVR_IO_PIN_OUTPUT_HIGH, AVR_IO_PIN_OUTPUT_LOW} avr_io_pin_output_t;

void set_pin_direction(avr_io_pin_t *, avr_io_pin_dir_t);
void set_pin_pullup(avr_io_pin_t *, avr_io_pin_pullup_t);

uint8_t get_pin_value(avr_io_pin_t *);

void set_pin_value(avr_io_pin_t *, avr_io_pin_output_t);

volatile uint8_t *get_ddr_from_port(volatile uint8_t *port);

#endif /* _AVR_IO_PIN_H_ */
