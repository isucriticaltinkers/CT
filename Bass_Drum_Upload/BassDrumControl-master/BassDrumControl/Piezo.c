/*
 * Piezo.c
 *
 * Created: 10/30/2016 4:17:55 PM
 *  Author: Eric Middleton
 */ 


#include "Piezo.h"

#include <avr/io.h>
#include <avr/interrupt.h>


static uint32_t __minDuration;
static PiezoHandler __handler;

static uint16_t __overflows;
static uint32_t __lastTime;

void Piezo_init(uint16_t minDuration, PiezoHandler handler) {
	//Convert from ms to timer ticks
	__minDuration = (uint32_t)minDuration * 16000L / 256;
	__handler = handler;
	__overflows = 0;
	__lastTime = 0;
	
	
	//Enable GPIO input
	DDRB &= ~(1 << 0);
	PORTB &= ~(1 << 0);
	
	//Normal operation, no outputs
	TCCR1A = 0;
	
	//Enable input capture noise canceler
	//Trigger on rising edge
	//Prescalar=256
	TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS12);
	
	//Clear bits (only active in OC mode)
	TCCR1C = 0;
	
	//Clear timer
	TCNT1 = 0;
	
	//Clear the interrupt flags
	TIFR1 |= (1 << ICF1) | (1 << TOV1);
	
	//Set input capture, overflow interrupt mask
	TIMSK1 = (1 << ICIE1) | (1 << TOIE1);
	
	//Enable global interrupts
	sei();
}

ISR(TIMER1_OVF_vect) {
	//Timer1 overflow
	__overflows++;
}

ISR(TIMER1_CAPT_vect) {
	//Timer1 input capture

	//Calculate current timestamp
	uint32_t curTime = ICR1 + ((uint32_t)__overflows << 16);
	
	//Compare to last time
	if((curTime - __lastTime) >= __minDuration) {
		__lastTime = curTime;
		
		//Call the event handler
		__handler();
	}
	
}