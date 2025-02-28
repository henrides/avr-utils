#include "timer.h"

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <link_list.h>

head_t *timer_list = NULL;
static uint8_t started = 0;

typedef struct {
	timer_callback_t cb;
} timer_context_t;

void timer_register_callback(timer_callback_t *cb)
{
	timer_context_t *new_timer = (timer_context_t *)malloc(sizeof(timer_context_t));
	new_timer->cb.callback = cb->callback;
	new_timer->cb.callback_data = cb->callback_data;

	if (!timer_list) {
		timer_list = link_list_init();
	}

	link_list_append(timer_list, new_timer);

	if (!started) {
		started = 1;
		TCCR0 = (1<<CS02);
		TIMSK |= (1<<TOIE0);

		sei();
	}
}

ISR(TIMER0_OVF_vect)
{
	cli();
	node_t *i;
	for (i = timer_list->head; i != NULL; i = i->next) {
		timer_context_t *ctx = (timer_context_t *)i->data;
		timer_callback_t *cb = &ctx->cb;
		cb->callback(cb->callback_data);
	}
	sei();
}
