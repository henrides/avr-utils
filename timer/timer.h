#ifndef _TIMER_H_
#define _TIMER_H_

typedef void (*timer_timeout_cb)(void *);

typedef struct {
	timer_timeout_cb callback;
	void *callback_data;
} timer_callback_t;

void timer_register_callback(timer_callback_t *);

#endif /* _TIMER_H_ */

