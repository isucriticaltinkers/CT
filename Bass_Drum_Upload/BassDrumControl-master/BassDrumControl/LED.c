/*
 * LED.c
 *
 * Created: 10/30/2016 6:51:14 PM
 *  Author: Administrator
 */ 

#include "LED.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define LED_PIN		3

static uint16_t __times[3];

static void __setPWM(uint8_t);

static struct {
	enum {
		RISING = 0,
		ON,
		FALLING,
		OFF
	} state;
	uint16_t time;
} __ledStatus;

void LED_init(uint16_t riseTime, uint16_t onTime, uint16_t fallTime) {
	//Convert times from ms to timer overflows
	__times[0] = riseTime;
	__times[1] = onTime;
	__times[2] = fallTime;
	
	//Set status
	__ledStatus.state = OFF;
	
	//Set as input pin (PWM off)
	DDRD &= ~(1 << LED_PIN);
	PORTD &= ~(1 << LED_PIN);
	
	//Non-inverting PWM output on OC2B
	//WGM mode 3
	TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
	
	//Prescalar=8
	TCCR2B = (1 << CS21);
	
	//Configure timer0 for 1ms updates
	//Normal mode
	//Prescalar=64
	TCCR0A = 0;//(1 << WGM01);
	TCCR0B = (1 <<CS01) | (1 << CS00);
	TIMSK0 = OCIE0A;
	
	//Enable global interrupts
	sei();
}

void LED_trigger() {
	__ledStatus.state = RISING;
	__ledStatus.time = __times[0];
}

void __setPWM(uint8_t value) {
	static uint8_t pwmOn = 0;
	
	if(value > 0 && !pwmOn) {
		pwmOn = 1;
		
		//Enable PWM output
		DDRD |= (1 << LED_PIN);
	}
	else if(pwmOn && value == 0) {
		pwmOn = 0;
		
		//Disable PWM output
		DDRD &= ~(1 << LED_PIN);
	}
	
	//Set PWM value
	OCR2B = value;
}

ISR(TIMER0_OVF_vect) {
	PORTD ^= (1 << 2);
	
	if(__ledStatus.state != OFF) {
		__ledStatus.time--;
		
		if(__ledStatus.time == 0) {
			//Go to next state
			__ledStatus.state++;
			
			if(__ledStatus.state != OFF) {
				//Load the new state time
				__ledStatus.time = __times[(uint8_t)__ledStatus.state];
			}
		}
		
		switch(__ledStatus.state) {
			case RISING:
				__setPWM(255 - 255L * __ledStatus.time / __times[0]);
			break;
			
			case ON:
				__setPWM(255);
			break;
			
			case FALLING:
				__setPWM(255L * __ledStatus.time / __times[2]);
			break;
			
			default:
				__setPWM(0);
			break;
		}
	}
}