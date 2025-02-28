#include "encoder.h"

#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <link_list.h>
#include <timer.h>

enum ENC_STATE { ENC_STATE_00 = 1, ENC_STATE_10, ENC_STATE_11, ENC_STATE_01 };
typedef struct {
	encoder_conf_t config;
	enum ENC_STATE state;
} encoder_context_t;

static head_t *encoder_list = NULL;

static void pin_init(avr_io_pin_t *pin)
{
	set_pin_direction(pin, AVR_IO_PIN_DIR_INPUT);
	set_pin_pullup(pin, AVR_IO_PIN_PULLUP_ENABLED);
}

static enum ENC_STATE get_current_state(encoder_conf_t *config)
{
	enum ENC_STATE state = ENC_STATE_00;

	uint8_t enc_a = get_pin_value(&config->pin_a);
	uint8_t enc_b = get_pin_value(&config->pin_b);

	if (!enc_a && !enc_b) {
		state = ENC_STATE_00;
	} else if (!enc_a && enc_b) {
		state = ENC_STATE_01;
	} else if (enc_a && enc_b) {
		state = ENC_STATE_11;
	} else if (enc_a && !enc_b) {
		state = ENC_STATE_10;
	}

	return state;
}

static void kick_encoder_state_machine(encoder_context_t *encoder)
{
	enum ENC_STATE new_state = get_current_state(&encoder->config);
	enum ENC_STATE current_state = encoder->state;
	int8_t turn = 0;

	if (new_state == current_state) {
		// Optimization, if the state hasn't change, don't bother with the rest.
		return;
	}

	switch (current_state) {
		case ENC_STATE_00:
			if (new_state == ENC_STATE_01) {
				turn = -1;
			} else if (new_state == ENC_STATE_10) {
				turn = 1;
			}
			break;
		case ENC_STATE_01:
			if (new_state == ENC_STATE_11) {
				turn = -1;
			} else if (new_state == ENC_STATE_00) {
				turn = 1;
			}
			break;
		case ENC_STATE_11:
			if (new_state == ENC_STATE_10) {
				turn = -1;
			} else if (new_state == ENC_STATE_01) {
				turn = 1;
			}
			break;
		case ENC_STATE_10:
			if (new_state == ENC_STATE_00) {
				turn = -1;
			} else if (new_state == ENC_STATE_11) {
				turn = 1;
			}
			break;
	}

	if (turn > 0) {
		encoder->config.increase_cb();
	} else if (turn < 0) {
		encoder->config.decrease_cb();
	}

	encoder->state = new_state;
}

static void encoder_timer_callback(void *data)
{
	node_t *i;
	for (i = encoder_list->head; i != NULL; i = i->next) {
		kick_encoder_state_machine((encoder_context_t *)i->data);
	}
}

void encoder_init(encoder_conf_t *config)
{
	encoder_context_t *new_encoder = (encoder_context_t *)malloc(sizeof(encoder_context_t));

	pin_init(&config->pin_a);
	pin_init(&config->pin_b);

	memcpy(&new_encoder->config, config, sizeof(encoder_conf_t));
	new_encoder->state = get_current_state(config);

	if (!encoder_list) {
		encoder_list = link_list_init();

		timer_callback_t timer_cb;
		timer_cb.callback = encoder_timer_callback;
		timer_cb.callback_data = NULL;
		timer_register_callback(&timer_cb);
	}
	link_list_append(encoder_list, new_encoder);
}
