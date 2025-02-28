#include "adc.h"

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <link_list.h>

static head_t *adc_list = NULL;
static node_t *current_adc = NULL;

static void prepare_admux(adc_conf_t *conf)
{
	/* ADMUX = REFS1 | REFS0 | ADLAR | - | MUX3 | MUX2 | MUX1 | MUX0 */
	uint8_t admux = ADMUX;
	switch (conf->reference) {
		case ADC_REF_EXT_AREF:
			admux &= ~_BV(REFS1) & ~_BV(REFS0);
			break;
		case ADC_REF_INT_AVCC:
			admux |= _BV(REFS0);
			admux &= ~_BV(REFS1);
			break;
		case ADC_REF_INT_256:
			admux |= _BV(REFS1) | _BV(REFS0);
			break;
	}

	admux &= 0xF0;
	admux |= conf->pin;

	ADMUX = admux;
}

ISR(ADC_vect)
{
	uint16_t value = 0;
	adc_conf_t *conf = (adc_conf_t *)current_adc->data;

	cli();
	value = ADCL;
	value |= (ADCH<<8);
	conf->conversion_cb(value);

	if (!current_adc->next) {
		current_adc = adc_list->head;
	} else {
		current_adc = current_adc->next;
	}
	conf = (adc_conf_t *)current_adc->data;
	prepare_admux(conf);
	sei();

	ADCSRA |= _BV(ADSC); /* start conversion */
}

void adc_init(adc_conf_t *conf)
{
	node_t *new_node = NULL;
	if (!adc_list) {
		adc_list = link_list_init();
	}
	new_node = link_list_append(adc_list, conf);

	if (!current_adc) {
		current_adc = new_node;
		prepare_admux(conf);

		/* ADCSRA = ADEN | ADSC | ADFR | ADIF | ADIE | ADSP2 | ADSP1 | ADSP0 */
		ADCSRA |= _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); /* Interrupt enable & Prescaler 128 */

		sei();
		ADCSRA |= _BV(ADEN) | _BV(ADSC); /* ADC enable & start conversion */
	}
}
