/******************************************************************************
 *
 * Module: Pulse Width Modulation
 *
 * File Name: pwm.c
 *
 * Description: source file for the PWM driver
 *
 * Author: Karima Mahmoud
 *
 *******************************************************************************/

#include <avr/io.h>
#include "pwm.h"
#include "gpio.h"

/*******************************************************************************
 *                                Functions Definitions                        *
 *******************************************************************************/

void PWM_Timer0_Start(uint8 duty_cycle)
{
	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);

	TCNT0 = 0;

	/* Set Compare Value */
	OCR0  = (uint8)((uint16)(duty_cycle*255)/ 100);

	/* Set OC0 -> PB3 as output pin*/
	GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT);
}
