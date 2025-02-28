#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <lcd.h>
#include <util/delay.h>

#include <encoder.h>

const char counter_str[] PROGMEM = "Counter: %04d\n";

volatile uint16_t counter = 0;
volatile uint16_t prev_counter = 1;

static void print_counter()
{
	if (counter != prev_counter) {
		lcd_clear();
		fprintf_P(lcdout, counter_str, counter);
	}
	prev_counter = counter;
}

void enc_cw_cb()
{
	counter++;
}

void enc_ccw_cb()
{
	if (counter > 0) {
		counter--;
	}
}

int main(void)
{
	encoder_conf_t enc_conf;

	enc_conf.pin_a.port = &PORTC;
	enc_conf.pin_a.pin = PC3;
	enc_conf.pin_b.port = &PORTC;
	enc_conf.pin_b.pin = PC4;
	enc_conf.increase_cb = enc_cw_cb;
	enc_conf.decrease_cb = enc_ccw_cb;

	lcd_init();
	encoder_init(&enc_conf);

	while (1) {
		print_counter();
		_delay_ms(50);
	}
	return 0;
}
