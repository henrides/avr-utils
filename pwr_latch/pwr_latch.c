#include "pwr_latch.h"

#include <stdlib.h>
#include <string.h>

#include <timer.h>

static uint8_t init = 0;
static uint8_t shutdown = 0;
static pwr_latch_conf_t config;

void pwr_button_timer_callback(void *data)
{
	pwr_latch_conf_t *conf = (pwr_latch_conf_t *)data;
	uint8_t pwr_btn = get_pin_value(&conf->pwr_button);
	if (!init) {
		// Make sure that we wait until the button has been release before monitoring the button
		if (pwr_btn) {
			init = 1;
		}
	} else if (!pwr_btn) {
		// The button has been press, wait until it is release before actually cutting the power
		shutdown = 1;
	} else if (pwr_btn && shutdown) {
		set_pin_value(&conf->pwr_latch, AVR_IO_PIN_OUTPUT_LOW);
		shutdown = 0;
	}
}

void pwr_latch_init(pwr_latch_conf_t *conf)
{
	set_pin_direction(&conf->pwr_latch, AVR_IO_PIN_DIR_OUTPUT);
	set_pin_value(&conf->pwr_latch, AVR_IO_PIN_OUTPUT_HIGH);

	set_pin_direction(&conf->pwr_button, AVR_IO_PIN_DIR_INPUT);
	set_pin_pullup(&conf->pwr_button, AVR_IO_PIN_PULLUP_ENABLED);

	memcpy(&config, conf, sizeof(pwr_latch_conf_t));
	timer_callback_t timer_cb;
	timer_cb.callback = pwr_button_timer_callback;
	timer_cb.callback_data = &config;
	timer_register_callback(&timer_cb);
}
