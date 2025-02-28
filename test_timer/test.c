#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <lcd.h>
#include <util/delay.h>

#include <timer.h>

const char counter_str[] PROGMEM = "timer: %04d\n";

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

void timer_callback(void *data)
{
	counter++;
}

int main(void)
{
	lcd_init();
	print_counter();

	timer_callback_t timer_cb;
	timer_cb.callback = timer_callback;
	timer_cb.callback_data = NULL;
	timer_register_callback(&timer_cb);

	while (1) {
		print_counter();
		_delay_ms(50);
	}
	return 0;
}
