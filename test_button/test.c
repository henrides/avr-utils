#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <lcd.h>
#include <util/delay.h>

#include <button.h>

const char btn_str[] PROGMEM = "Button:\n  %s";

volatile uint8_t btn_state = 0;
volatile uint8_t print_btn_state = 0;

static void print_button()
{
	lcd_clear();
	fprintf_P(lcdout, btn_str, btn_state?"Pressed":"Not Pressed");
}

void btn_pressed_cb()
{
	btn_state = 1;
}

void btn_released_cb()
{
	btn_state = 0;
}

int main(void)
{
	button_conf_t btn_conf;

	btn_conf.button.port = &PORTB;
	btn_conf.button.pin = PB0;
	btn_conf.internal_pullup = 1;
	btn_conf.pressed_cb = btn_pressed_cb;
	btn_conf.released_cb = btn_released_cb;

	lcd_init();
	print_button();

	button_init(&btn_conf);

	while (1) {
		if (btn_state != print_btn_state) {
			print_button();
			print_btn_state = btn_state;
		}
		_delay_ms(50);
	}
	return 0;
}
