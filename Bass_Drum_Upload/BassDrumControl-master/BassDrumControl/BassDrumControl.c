/*
 * BassDrumControl.c
 *
 * Created: 10/30/2016 4:04:45 PM
 *  Author: Eric Middleton
 */ 


#include <avr/io.h>

#include "Piezo.h"
#include "LED.h"

#define PIEZO_MIN_DURATION	50

#define LED_RISE_TIME		10
#define LED_ON_TIME			30
#define LED_FALL_TIME		10

int main(void)
{
	DDRD |= (1 << 2);
	//PORTD |= (1 << 2);
	
	//Initialize the LED output
	LED_init(LED_RISE_TIME, LED_ON_TIME, LED_FALL_TIME);
	
	//Initialize the piezo library
	Piezo_init(PIEZO_MIN_DURATION, &LED_trigger);
	
    while(1);
}