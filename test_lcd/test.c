#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <lcd.h>
#include <util/delay.h>

const char prog_mem_fmt[] PROGMEM = "progmem %s";
const char ram_mem_fmt[] = "rammem %s";

static void print_progmem()
{
	lcd_clear();
	fprintf_P(lcdout, prog_mem_fmt, "success");
}

static void print_rammem()
{
	lcd_clear();
	fprintf(lcdout, ram_mem_fmt, "success");
}

static void print_counter(uint16_t val)
{
    char str[5];
    uint8_t i = 0;

    lcd_set_addr(0x45);
    sprintf(str, "%5hu", val);

    for (i = 0; i < strlen(str); i++) {
        lcd_putc(str[i]);
    }
}

int main(void)
{
	lcd_init();

    uint16_t i = 0;
	while (1) {
        if (i%100 == 0) {
            if ((i/100)%2 == 0) {
                print_progmem();
            } else {
                print_rammem();
            }
        }
        print_counter(i);
		_delay_ms(50);
        i++;
	}
	return 0;
}
