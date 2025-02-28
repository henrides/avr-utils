#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <lcd.h>
#include <util/delay.h>

#include <adc.h>

const char adc_str[] PROGMEM = "Value1:     %04d\nValue2:     %04d";

volatile uint16_t adc1_val = 0;
volatile uint16_t adc2_val = 0;
volatile uint16_t prev_adc1_val = 0;

static void print_adc()
{
	lcd_clear();
	fprintf_P(lcdout, adc_str, adc1_val, adc2_val);
}

static void adc1_cb(uint16_t val)
{
	adc1_val = val;
}

static void adc2_cb(uint16_t val)
{
	adc2_val = val;
}

int main(void)
{
	adc_conf_t adc1_conf;
	adc1_conf.reference = ADC_REF_INT_AVCC;
	adc1_conf.pin = ADC2;
	adc1_conf.conversion_cb = adc1_cb;

	adc_conf_t adc2_conf;
	adc2_conf.reference = ADC_REF_INT_AVCC;
	adc2_conf.pin = ADC5;
	adc2_conf.conversion_cb = adc2_cb;

	lcd_init();

	lcd_clear();
	fprintf_P(lcdout, adc_str, adc1_val);

	adc_init(&adc1_conf);
	adc_init(&adc2_conf);

	while (1) {
		if (adc1_val != prev_adc1_val) {
			print_adc();
			prev_adc1_val = adc1_val;
		}
		_delay_ms(50);
	}
	return 0;
}
