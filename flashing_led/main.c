#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define F_CPU 8000000UL
#include <util/delay.h>

volatile unsigned char button_pressed = 0;

void flash_led() {
	cli();
	unsigned char i;
	for (i = 0; i < 5; i++) {
		/* turn the LED on */
		PORTB |= (1<<PB0);

		_delay_ms(150);

		/* turn it off */
		PORTB &= ~(1<<PB0);

		_delay_ms(150);
	}
	button_pressed = 0;
	sei();
}

ISR(INT1_vect) {
	button_pressed = 1;
}

void setup_external_interrupt_1()
{
	MCUCR &= ~(1<<ISC10) & ~(1<<ISC11);

	GICR |= (1<<INT1);
}

int main(void)
{
	/* set PB0 as an output pin */
	DDRB = (1<<PB0);
	
	/* set INT1 pin as input with pull up */
	DDRD &= ~(1<<PD3);
	PORTD = (1<<PD3);

	SFIOR &= ~(1<<PUD);

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	setup_external_interrupt_1();

	while(1) {
		sei();
		sleep_enable();
		sleep_cpu();
		sleep_disable();
		if (button_pressed) {
			flash_led();
		}
	}

	return 0;
}

