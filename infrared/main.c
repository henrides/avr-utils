#include <stdio.h>
#include "mcu_settings.h"
#include <avr/pgmspace.h>

#include <stdlib.h>

#include <util/delay.h>

#include "lcd.h"

char rst_fmt[] PROGMEM = "rst = %d\n";
char count_fmt[] PROGMEM = "count = %d\n";

int main(void)
{
	lcd_init(LCD_FLAG_DISPLAY_NONE);

	static FILE lcd_stream = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
	lcd_home();

	/* PD0 is input with pull up resistor enabled */
	DDRD &= ~(1<<PD0) & ~(1<<PD1);
	PORTD |= (1<<PD0) | (1<<PD1);

	unsigned char old_val = 1;
	unsigned char count = 0;

	lcd_home();
	fprintf_P(&lcd_stream, count_fmt, count);

	while (1) {
		unsigned char bit_flip = PIND & (1<<PD0);
		unsigned char rst_btn = PIND & (1<<PD1);

		if (rst_btn == 0 && count != 0) {
			//lcd_home();
			//fprintf_P(&lcd_stream, rst_fmt, count);
			count = 0;
			old_val = 1;
			lcd_home();
			fprintf_P(&lcd_stream, count_fmt, count);
		}

		if (bit_flip != old_val) {
			old_val = bit_flip;
			count++;
			lcd_home();
			fprintf_P(&lcd_stream, count_fmt, count);
		}

	}

	return 0;
}

