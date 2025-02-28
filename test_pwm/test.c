#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <lcd.h>
#include <encoder.h>
#include <pwm.h>
#include <adc.h>

const char counter_str[] PROGMEM = "Counter:    %04hu\nValue:       %03hhu";

volatile uint16_t counter = 0;
volatile uint16_t prev_counter = 0;
volatile uint16_t adc_val = 0;
volatile uint16_t prev_adc_val = 0;

static void print_counter()
{
	lcd_clear();
	fprintf_P(lcdout, counter_str, counter, adc_val);
}

static void adc_cb(uint16_t val)
{
	adc_val = val;
}

void enc_cw_cb()
{
	if (counter < 0x03ff) {
		counter++;
	}
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

	adc_conf_t adc_conf;
	adc_conf.reference = ADC_REF_INT_AVCC;
	adc_conf.pin = ADC2;
	adc_conf.conversion_cb = adc_cb;

	lcd_init();
	encoder_init(&enc_conf);
	pwm_init();
	adc_init(&adc_conf);

	print_counter();
	set_pwm_value(counter);

	while (1) {
		if (counter != prev_counter || adc_val != prev_adc_val) {
			print_counter();
			set_pwm_value(counter);
		}
		prev_counter = counter;
		prev_adc_val = adc_val;
		_delay_ms(50);
	}
	return 0;
}
