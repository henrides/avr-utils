#ifndef _ADC_H_
#define _ADC_H_

#include <avr_io_pin.h>

typedef void (*adc_conversion_complete_cb)(uint16_t);

enum ADC_REF { ADC_REF_EXT_AREF, ADC_REF_INT_AVCC, ADC_REF_INT_256 };
enum ADC_PIN { ADC0 = 0x00, ADC1 = 0x01, ADC2 = 0x02, ADC3 = 0x03, ADC4 = 0x04, ADC5 = 0x05, ADC6 = 0x06, ADC7=0x07 };

typedef struct {
	enum ADC_REF reference;
	enum ADC_PIN pin;
	adc_conversion_complete_cb conversion_cb;
} adc_conf_t;

void adc_init(adc_conf_t *);
#endif /* _ADC_H_ */
