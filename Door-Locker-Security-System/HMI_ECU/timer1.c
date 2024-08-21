/******************************************************************************
 *
 * Module: TIMER1
 *
 * File Name: uart.c
 *
 * Description: Source file for the Timer1 AVR driver
 *
 * Author: Karima Mahmoud
 *
 *******************************************************************************/

/*******************************************************************************
 *                      Used Header Files                                      *
 *******************************************************************************/
#include "timer1.h"
#include <avr/io.h>
#include "common_macros.h"
#include <avr/interrupt.h>

/*******************************************************************************
 *                                Global Variables                             *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void(*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

/*******************************************************************************
 *                                Functions Definitions                        *
 *******************************************************************************/


void Timer1_init(const Timer1_ConfigType * Config_Ptr){

	/*first two bits is common in two modes 00 for both*/
	TCCR1A = (1 << FOC1A) | (Config_Ptr->mode & 3);
	if (Config_Ptr->mode == CTC) {
		/* Set the Compare value to value in Configuration */
		OCR1A = Config_Ptr->compare_value;

		/* Enable Timer1 Compare A Interrupt */
		TIMSK |= (1 << OCIE1A);
		TCCR1B = (Config_Ptr->prescaler) | ((Config_Ptr->mode >> 2) << 3);
	}
	if (Config_Ptr->mode == NORMAL) {
		/*Timer_ 1 Overflow  Interrupt Enable*/
		TIMSK |= (1<<TOIE1);
		/*set prescaler chosen in Configuration*/
		TCCR1B = (Config_Ptr->prescaler);
	}
	TCNT1 = Config_Ptr->initial_value;
}

void Timer1_deInit(void)
{
	/* Clear all initialized registers */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 0;

	/* Reset the global pointer value */
	g_callBackPtr = NULL_PTR;

	/* Disable Interrupts */
	CLEAR_BIT(TIMSK,OCIE1A);
	CLEAR_BIT(TIMSK,TOIE1);
}

void Timer1_setCallBack(void(*a_ptr)(void))
{
	g_callBackPtr = a_ptr;
}

