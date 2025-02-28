#include "button.h"

#include <stdlib.h>
#include <string.h>

#include <link_list.h>
#include <timer.h>

static head_t *button_list = NULL;
typedef struct {
	button_conf_t config;
	uint8_t state;
} button_context_t;

static void button_timer_callback(void *data)
{
	node_t *i;
	for (i = button_list->head; i != NULL; i = i->next) {
		button_context_t *ctx = (button_context_t *)i->data;
		uint8_t btn_val = get_pin_value(&ctx->config.button);
		if (btn_val != ctx->state) {
			if (!btn_val && ctx->config.pressed_cb) {
				ctx->config.pressed_cb();
			}
			if (btn_val && ctx->config.released_cb) {
				ctx->config.released_cb();
			}
			ctx->state = btn_val;
		}
	}
}

void button_init(button_conf_t *conf) {
	button_context_t *new_button = (button_context_t *)malloc(sizeof(button_context_t));

	set_pin_direction(&conf->button, AVR_IO_PIN_DIR_INPUT);
	if (conf->internal_pullup) {
		set_pin_pullup(&conf->button, AVR_IO_PIN_PULLUP_ENABLED);
	} else {
		set_pin_pullup(&conf->button, AVR_IO_PIN_PULLUP_DISABLED);
	}

	memcpy(&new_button->config, conf, sizeof(button_conf_t));
	new_button->state = get_pin_value(&conf->button);

	if (!button_list) {
		button_list = link_list_init();

		timer_callback_t timer_cb;
		timer_cb.callback = button_timer_callback;
		timer_cb.callback_data = NULL;
		timer_register_callback(&timer_cb);
	}
	link_list_append(button_list, new_button);
}
