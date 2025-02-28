#ifndef _ADJ_DL_FSM_H_
#define _ADJ_DL_FSM_H_

#include <stdio.h>
#include <avr_io_pin.h>

void fsm_init(avr_io_pin_t *enc_a, avr_io_pin_t *enc_b, avr_io_pin_t *sel);
void get_fsm_status_line(char *buf, size_t buflen);
uint16_t get_output_value_mv(uint16_t sensed_mv);

#endif /* _ADJ_DL_FSM_H_ */
