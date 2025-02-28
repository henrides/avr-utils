#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <lcd.h>
#include <pwm.h>
#include <adc.h>
#include <pwr_latch.h>

#include "defines.h"
#include "adj_dl_fsm.h"

volatile uint16_t v_sense_val_mv = 0;
volatile uint16_t i_sense_val_mv = 0;

#define PWM_DIVIDER 1
#define MAX_PWM_VALUE 0x03ff /* 10bits */
#define VCC_VALUE 5000

#define MAX_ADC_VALUE 0x03ff /* 10bits */
#define ADC_REF 2560

#define V_SENSE_THRESHOLD 1000 /* 1V */

static void redraw_status_line()
{
    lcd_set_addr(0x40);
    char status_str[17] = {0};
    get_fsm_status_line(status_str, sizeof(status_str));
    fprintf(lcdout, status_str);
}

static void redraw_sense_line()
{
    lcd_set_addr(0x00);
    char v_sense_str[5] = {0};
    if (v_sense_val_mv >= 10000) {
        sprintf(v_sense_str, " %2dV", v_sense_val_mv/1000);
    } else {
        sprintf(v_sense_str, "%1d.%01dV", v_sense_val_mv/1000, (v_sense_val_mv%1000)/100);
    }

    char p_sense_str[6] = {0};
    uint16_t power_mw = ((uint32_t)v_sense_val_mv*i_sense_val_mv)/1000;
    if (power_mw >= 10000) {
        sprintf(p_sense_str, "%2d.%1dW", power_mw/1000, (power_mw%1000)/100);
    } else if (power_mw >= 1000) {
        sprintf(p_sense_str, "%1d.%02dW", power_mw/1000, (power_mw%1000)/10);
    } else {
        sprintf(p_sense_str, "%3dmW", power_mw);
    }

    char i_sense_str[6] = {0};
    if (i_sense_val_mv >= 1000) {
        sprintf(i_sense_str, "%1d.%02dA", i_sense_val_mv/1000, (i_sense_val_mv%1000)/10);
    } else {
        sprintf(i_sense_str, "%3dmA", i_sense_val_mv);
    }
    fprintf(lcdout, "%s %s %s", v_sense_str, p_sense_str, i_sense_str);
}

static uint16_t convert_adc_to_mv_value(uint16_t val)
{
    return ((uint32_t)val * ADC_REF)/MAX_ADC_VALUE;
}

static void v_sense_cb(uint16_t val)
{
    uint16_t mv = (convert_adc_to_mv_value(val)*V_SENSE_DIVIDER);
    if (v_sense_val_mv != mv) {
        v_sense_val_mv = mv;
    }
}

static void i_sense_cb(uint16_t val)
{
    uint16_t mv = (convert_adc_to_mv_value(val)*I_SENSE_DIVIDER);
    if (i_sense_val_mv != mv) {
        i_sense_val_mv = mv;
    }
}

static uint16_t convert_set_mv_to_pwm_value(uint16_t set_mv)
{
    uint16_t pwm_val = 0;

    pwm_val = ((uint32_t)(set_mv * PWM_DIVIDER) * (MAX_PWM_VALUE+1))/VCC_VALUE;

    return pwm_val;
}


int main(void)
{
    pwr_latch_conf_t pwr_conf;
    SET_IO_PIN(pwr_conf.pwr_latch, PWR_LATCH);
    SET_IO_PIN(pwr_conf.pwr_button, PWR_BUTTON);

    pwr_latch_init(&pwr_conf);

    avr_io_pin_t enc_a, enc_b;
    SET_IO_PIN(enc_a, ENCODER_A);
    SET_IO_PIN(enc_b, ENCODER_B);

    avr_io_pin_t sel_btn;
    SET_IO_PIN(sel_btn, SELECT_BUTTON);

    adc_conf_t adc_v_sense_conf;
    adc_v_sense_conf.reference = ADC_REF_INT_256;
    adc_v_sense_conf.pin = V_SENSE;
    adc_v_sense_conf.conversion_cb = v_sense_cb;

    adc_conf_t adc_i_sense_conf;
    adc_i_sense_conf.reference = ADC_REF_INT_256;
    adc_i_sense_conf.pin = I_SENSE;
    adc_i_sense_conf.conversion_cb = i_sense_cb;


    lcd_init();
    fsm_init(&enc_a, &enc_b, &sel_btn);
    pwm_init();

    adc_init(&adc_v_sense_conf);
    adc_init(&adc_i_sense_conf);

    set_pwm_value(0);

    while (1) {
        lcd_clear();
        redraw_status_line();
        redraw_sense_line();

        if (v_sense_val_mv > V_SENSE_THRESHOLD) {
            uint16_t pwm_val_mv = get_output_value_mv(v_sense_val_mv);
            set_pwm_value(convert_set_mv_to_pwm_value(pwm_val_mv));
        } else {
            set_pwm_value(0);
        }
        _delay_ms(100);
    }
    return 0;
}
