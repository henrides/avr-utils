#include "adj_dl_fsm.h"

#include <stdio.h>
#include <avr/pgmspace.h>

#include <string.h>
#include <encoder.h>
#include <button.h>

const char menu_cc_str[] = "Const Current ";
const char menu_cp_str[] = "Const Power   ";
const char menu_cr_str[] = "Const Resist. ";

const char active_cc_str[] = "C.Current";
const char active_cp_str[] = "C.Power  ";
const char active_cr_str[] = "C.Resist.";

static enum DL_STATE {
	DL_STATE_MENU,
	DL_STATE_ACTIVE
} current_state;

enum DL_MODE {
	DL_MODE_CONST_CURRENT,
	DL_MODE_CONST_POWER,
	DL_MODE_CONST_RESISTANCE
};

static enum DL_MODE current_mode;
static enum DL_MODE previous_mode;

#define MENU_SENSITIVITY 3
static int8_t menu_counter = 0;

#define MAX_CONST_CURRENT_VAL 1500 /* mA */
#define MIN_CONST_CURRENT_VAL 0 /* mA */
#define SENS_CONST_CURRENT 3 /* detent / mA */

#define MAX_CONST_POWER_VAL 18000 /* mW */
#define MIN_CONST_POWER_VAL 0 /* mW */
#define SENS_CONST_POWER 1 /* detent / mW */

#define MAX_CONST_RESISTANCE_VAL 12000 /* Ohm */
#define MIN_CONST_RESISTANCE_VAL 8 /* Ohm */
#define SENS_CONST_RESISTANCE 1 /* detent / Ohm */

volatile uint16_t set_counter = 0;

static uint16_t get_max_set_counter(enum DL_MODE mode)
{
	uint16_t range = 0;
	uint16_t sensitivity = 0;
	switch (mode) {
		case DL_MODE_CONST_CURRENT:
			range = MAX_CONST_CURRENT_VAL - MIN_CONST_CURRENT_VAL;
			sensitivity = SENS_CONST_CURRENT;
			break;
		case DL_MODE_CONST_POWER:
			range = MAX_CONST_POWER_VAL - MIN_CONST_POWER_VAL;
			sensitivity = SENS_CONST_POWER;
			break;
		case DL_MODE_CONST_RESISTANCE:
			range = MAX_CONST_RESISTANCE_VAL - MIN_CONST_RESISTANCE_VAL;
			sensitivity = SENS_CONST_RESISTANCE;
			break;
	}
	return range*sensitivity;
}

static void sel_btn_cb()
{
	if (current_state != DL_STATE_MENU) {
		current_state = DL_STATE_MENU;
	} else {
		current_state = DL_STATE_ACTIVE;
        if (current_mode != previous_mode) {
            if (current_mode != DL_MODE_CONST_RESISTANCE) {
                set_counter = 0;
            } else {
                // for constant resistance, starts in the middle to avoid burst of current
                set_counter = get_max_set_counter(current_mode)/2;
            }
            previous_mode = current_mode;
        }
	}
}

static uint16_t convert_set_counter_to_value(enum DL_MODE mode)
{
	uint16_t range = 0, min = 0;
	switch (mode) {
		case DL_MODE_CONST_CURRENT:
			min = MIN_CONST_CURRENT_VAL;
			range = MAX_CONST_CURRENT_VAL - min;
			break;
		case DL_MODE_CONST_POWER:
			min = MIN_CONST_POWER_VAL;
			range = MAX_CONST_POWER_VAL - min;
			break;
		case DL_MODE_CONST_RESISTANCE:
			min = MIN_CONST_RESISTANCE_VAL;
			range = MAX_CONST_RESISTANCE_VAL - min;
			break;
	}
	return (((uint32_t)set_counter * range)/get_max_set_counter(mode))+min;
}

static void enc_inc_cb()
{
	if (current_state == DL_STATE_MENU) {
		menu_counter++;
		if (menu_counter > MENU_SENSITIVITY) {
			menu_counter = 0;
			switch (current_mode) {
				case DL_MODE_CONST_CURRENT:
					current_mode = DL_MODE_CONST_POWER;
					break;
				case DL_MODE_CONST_POWER:
					current_mode = DL_MODE_CONST_RESISTANCE;
					break;
				case DL_MODE_CONST_RESISTANCE:
					current_mode = DL_MODE_CONST_CURRENT;
					break;
			}
		}
	} else {
		if (set_counter < get_max_set_counter(current_mode)) {
			set_counter++;
		}
	}
}

static void enc_dec_cb()
{
	if (current_state == DL_STATE_MENU) {
		menu_counter--;
		if (menu_counter < -MENU_SENSITIVITY) {
			menu_counter = 0;
			switch (current_mode) {
				case DL_MODE_CONST_CURRENT:
					current_mode = DL_MODE_CONST_RESISTANCE;
					break;
				case DL_MODE_CONST_POWER:
					current_mode = DL_MODE_CONST_CURRENT;
					break;
				case DL_MODE_CONST_RESISTANCE:
					current_mode = DL_MODE_CONST_POWER;
					break;
			}
		}
	} else {
		if (set_counter > 0) {
			set_counter--;
		}
	}
}

uint16_t get_output_value_mv(uint16_t sensed_mv)
{
    uint16_t output_val = 0;
	if (current_state == DL_STATE_MENU) {
		return 0;
	} else {
		uint16_t value = convert_set_counter_to_value(current_mode);
		switch (current_mode) {
			case DL_MODE_CONST_CURRENT:
                output_val = value;
				break;
			case DL_MODE_CONST_POWER:
                output_val = ((uint32_t)value*1000)/sensed_mv;
				break;
			case DL_MODE_CONST_RESISTANCE:
                // I = V/R
                output_val = sensed_mv/value;
                break;
        }
	}

	return output_val;
}

void get_fsm_status_line(char *buf, size_t buflen)
{
	if (current_state == DL_STATE_MENU) {
		const char *menu_str = NULL;
		switch (current_mode) {
			case DL_MODE_CONST_CURRENT:
				menu_str = menu_cc_str;
				break;
			case DL_MODE_CONST_POWER:
				menu_str = menu_cp_str;
				break;
			case DL_MODE_CONST_RESISTANCE:
				menu_str = menu_cr_str;
				break;
		}
		snprintf(buf, buflen, "%c%s%c", 0x7f, menu_str, 0x7e);
	} else {
		const char *label = NULL;
		uint16_t value = convert_set_counter_to_value(current_mode);
		char value_str[8] = { 0 };
		switch (current_mode) {
			case DL_MODE_CONST_CURRENT:
				label = active_cc_str;
				if (value >= 1000) {
					sprintf(value_str, "  %1d.%02dA", value/1000, (value%1000)/10);
				} else {
					sprintf(value_str, "  %3dmA", value);
				}
				break;
			case DL_MODE_CONST_POWER:
				label = active_cp_str;
				if (value >= 1000) {
					sprintf(value_str, " %2d.%02dW", value/1000, (value%1000)/10);
				} else {
					sprintf(value_str, "  %3dmW", value);
				}
				break;
			case DL_MODE_CONST_RESISTANCE:
				label = active_cr_str;
				if (value >= 10000) {
					sprintf(value_str, "%2d.%02dk%c", value/1000, (value%1000)/10, 0xf4);
				} else {
					sprintf(value_str, "  %4d%c", value, 0xf4);
				}
				break;
		}
		snprintf(buf, buflen, "%s%s", label, value_str);
	}
}

void fsm_init(avr_io_pin_t *enc_a, avr_io_pin_t *enc_b, avr_io_pin_t *sel)
{
	current_state = DL_STATE_MENU;
	current_mode = DL_MODE_CONST_CURRENT;
	previous_mode = DL_MODE_CONST_CURRENT;

	encoder_conf_t enc_conf;
	enc_conf.pin_a.port = enc_a->port;
	enc_conf.pin_a.pin = enc_a->pin;
	enc_conf.pin_b.port = enc_b->port;
	enc_conf.pin_b.pin = enc_b->pin;
	enc_conf.increase_cb = enc_inc_cb;
	enc_conf.decrease_cb = enc_dec_cb;
	encoder_init(&enc_conf);

	button_conf_t btn_conf;
	btn_conf.button.port = sel->port;
	btn_conf.button.pin = sel->pin;
	btn_conf.internal_pullup = 1;
	btn_conf.pressed_cb = NULL;
	btn_conf.released_cb = sel_btn_cb;
	button_init(&btn_conf);
}
