#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <lcd.h>
#include <util/delay.h>

#define UI_DECREMENT_BTN	PC3
#define UI_INCREMENT_BTN	PC4
#define UI_SELECT_BTN		PC5

enum COUNTDOWN_MODE { CD_SET_TIME, CD_COUNT_DOWN_INIT, CD_COUNT_DOWN, CD_TIMEOUT };
enum COUNTDOWN_SELECT { CD_SEL_HOUR = 0x08, CD_SEL_MIN = 0x0B, CD_SEL_SEC = 0x0E, CD_SEL_START = 0x42, CD_SEL_RST = 0x49 };

/* UI strings */
const char set_timer_str[] PROGMEM = "Timer: %02d:%02d:%02d\n  Start  Reset";
const char timer_str[] PROGMEM = "%02d:%02d:%02d";
const char timer_cd_str[] PROGMEM = "%02d:%02d:%02d.%03d";
const char timed_out[] PROGMEM = "Timeout!";

static FILE lcd_stream = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

volatile unsigned char timer_hour = 0;
volatile unsigned char timer_min = 0;
volatile unsigned char timer_sec = 0;
volatile unsigned short int timer_mil = 0;

static void init_user_input()
{
	/* Port C pin 3, 4 and 5 are inputs with pull up resistor enabled */
	DDRC &= ~(1<<PC3) & ~(1<<PC4) & ~(1<<PC5);
	PORTC |= (1<<PC3) | (1<<PC4) | (1<<PC5);
}

static void change_selection(enum COUNTDOWN_SELECT *selection, char direction)
{
	switch (*selection) {
		case CD_SEL_HOUR:
			if (direction == 0) {
				*selection = CD_SEL_MIN;
			} else {
				*selection = CD_SEL_HOUR;
			}
			break;
		case CD_SEL_MIN:
			if (direction == 0) {
				*selection = CD_SEL_SEC;
			} else {
				*selection = CD_SEL_HOUR;
			}
			break;
		case CD_SEL_SEC:
			if (direction == 0) {
				*selection = CD_SEL_START;
			} else {
				*selection = CD_SEL_MIN;
			}
			break;
		case CD_SEL_START:
			if (direction == 0) {
				*selection = CD_SEL_RST;
			} else {
				*selection = CD_SEL_SEC;
			}
			break;
		case CD_SEL_RST:
			if (direction == 0) {
				*selection = CD_SEL_RST;
			} else {
				*selection = CD_SEL_START;
			}
			break;
	}
}

static void change_timer_value(enum COUNTDOWN_SELECT selection, char direction, 
		unsigned char *hour, unsigned char *minute, unsigned char *second)
{
	switch (selection) {
		case CD_SEL_HOUR:
			if (direction == 0) {
				if (*hour == 99) {
					*hour = 0;
				} else {
					(*hour)++;
				}
			} else {
				if (*hour == 0) {
					*hour = 99;
				} else {
					(*hour)--;
				}
			}
			break;
		case CD_SEL_MIN:
			if (direction == 0) {
				if (*minute == 59) {
					*minute = 0;
				} else {
					(*minute)++;
				}
			} else {
				if (*minute == 0) {
					*minute = 59;
				} else {
					(*minute)--;
				}
			}
			break;
		case CD_SEL_SEC:
			if (direction == 0) {
				if (*second == 59) {
					*second = 0;
				} else {
					(*second)++;
				}
			} else {
				if (*second == 0) {
					*second = 59;
				} else {
					(*second)--;
				}
			}
			break;
		default:
			break;
	}
}

ISR(TIMER1_COMPA_vect)
{
	timer_mil++;
	if (timer_mil == 1000) {
		timer_mil = 0;
		if (timer_sec > 0) {
			timer_sec--;
		} else {
			timer_sec = 59;
			if (timer_min > 0) {
				timer_min--;
			} else {
				timer_min = 59;
				if (timer_hour > 0) {
					timer_hour--;
				} else {
					timer_hour = 0;
					timer_min = 0;
					timer_sec = 0;
					timer_mil = 0;
				}
			}
		}
	}
}

static void init_timer(unsigned char hour,unsigned char minute,unsigned char second)
{
	timer_hour = hour;
	timer_min = minute;
	timer_sec = second;
	timer_mil = 999;

	OCR1A = 249;
	TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<CS10);
	TIMSK |= (1<<OCIE1A);

	lcd_home();
	lcd_set_cursor_display(LCD_FLAG_DISPLAY_NONE);

	sei();

}

static void timer_timed_out()
{
	cli();
	TCCR1B &= ~(1<<CS12) & ~(1<<CS11) & ~(1<<CS10);

	lcd_home();
	fprintf_P(&lcd_stream, timed_out);

	_delay_ms(5000);

}

int main(void)
{

	enum COUNTDOWN_MODE mode = CD_SET_TIME;
	enum COUNTDOWN_SELECT selection = CD_SEL_HOUR;
	unsigned char hour = 0;
	unsigned char minute = 0;
	unsigned char second = 0;

	char edit = 0;

	lcd_init(LCD_FLAG_DISPLAY_C);

	init_user_input();

	unsigned char old_btn_state = (1<<UI_DECREMENT_BTN)|(1<<UI_INCREMENT_BTN)|(1<<UI_SELECT_BTN);
	unsigned char new_btn_state;

	lcd_home();
	fprintf_P(&lcd_stream, set_timer_str, hour, minute, second);
	lcd_set_cursor_pos(selection);

	while (1) {
		switch (mode) {
			case CD_SET_TIME:
				new_btn_state = PINC & ((1<<UI_DECREMENT_BTN)|(1<<UI_INCREMENT_BTN)|(1<<UI_SELECT_BTN));
				if (old_btn_state != new_btn_state) {
					unsigned char update_display = 0;
					unsigned char update_cursor = 0;

					/* decrement button pressed */
					if ((old_btn_state & (1<<UI_DECREMENT_BTN)) &&
							!(new_btn_state & (1<<UI_DECREMENT_BTN))) {
						if (edit) {
							change_timer_value(selection, 1, &hour, &minute, &second);
							update_display = 1;
						} else {
							change_selection(&selection, 1);
							update_cursor = 1;
						}
					}

					/* increment button pressed */
					if ((old_btn_state & (1<<UI_INCREMENT_BTN)) &&
							!(new_btn_state & (1<<UI_INCREMENT_BTN))) {
						if (edit) {
							change_timer_value(selection, 0, &hour, &minute, &second);
							update_display = 1;
						} else {
							change_selection(&selection, 0);
							update_cursor = 1;
						}
					}
					
					/* select button pressed */
					if ((old_btn_state & (1<<UI_SELECT_BTN)) &&
							!(new_btn_state & (1<<UI_SELECT_BTN))) {
						switch (selection) {
							case CD_SEL_HOUR:
							case CD_SEL_MIN:
							case CD_SEL_SEC:
								edit = !edit;
								update_cursor = 1;
								break;
							case CD_SEL_START:
								mode = CD_COUNT_DOWN_INIT;
								break;
							case CD_SEL_RST:
								hour = 0;
								minute = 0;
								second = 0;
								update_display = 1;
								update_cursor = 1;
								break;
						}
					}

					if (update_display) {
						lcd_set_cursor_pos(0x07);
						fprintf_P(&lcd_stream, timer_str, hour, minute, second);
					}

					if (update_cursor || update_display) {
						lcd_set_cursor_pos(selection);
						if (edit) {
							lcd_set_cursor_display(LCD_FLAG_DISPLAY_C | LCD_FLAG_DISPLAY_B);
						} else {
							lcd_set_cursor_display(LCD_FLAG_DISPLAY_C);
						}
					}
					old_btn_state = new_btn_state;
				}
				break;
			case CD_COUNT_DOWN_INIT:
				{
					init_timer(hour, minute, second);
					mode = CD_COUNT_DOWN;
				}
				break;
			case CD_COUNT_DOWN:
				{
					lcd_set_cursor_pos(0x02);
					unsigned short display_mil = (timer_mil == 0) ? 999 : (1000 - timer_mil);
					fprintf_P(&lcd_stream, timer_cd_str, 
							timer_hour, timer_min, timer_sec, display_mil);

					if (timer_hour == 0 && timer_min == 0 && timer_sec == 0) {
						mode = CD_TIMEOUT;
					}

				}
				break;
			case CD_TIMEOUT:
				{
					timer_timed_out();

					mode = CD_SET_TIME;

					lcd_home();
					fprintf_P(&lcd_stream, set_timer_str, hour, minute, second);
					lcd_set_cursor_pos(selection);
					lcd_set_cursor_display(LCD_FLAG_DISPLAY_C);

				}
				break;
			default:
				break;
		}
	}

	return 0;
}
