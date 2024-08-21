/******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: uart.c
 *
 * Description: Source file for the Buzzer driver
 *
 * Author: Karima Mahmoud
 *
 *******************************************************************************/

/*******************************************************************************
 *                      Used Header Files                                      *
 *******************************************************************************/

#include "buzzer.h"
#include "gpio.h"

/*******************************************************************************
 *                      Function Definitions                                      *
 *******************************************************************************/
/*[FUNCTION NAME]	: Buzzer_init
 *[DESCRIPTION]		: Function to initialize the Buzzer
 *[ARGUMENTS]		: void
 *[RETURNS]			: void
 */
void Buzzer_init(void)
{
	GPIO_setupPinDirection(BUZZER_PORT_ID,BUZZER_PIN_ID,PIN_OUTPUT);

	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_LOW);
}

/*[FUNCTION NAME]	: Buzzer_init
 *[DESCRIPTION]		: Function to turn on the Buzzer
 *[ARGUMENTS]		: void
 *[RETURNS]			: void
 */
void Buzzer_on(void)
{
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_HIGH);
}

/*[FUNCTION NAME]	: Buzzer_init
 *[DESCRIPTION]		: Function to turn off the Buzzer
 *[ARGUMENTS]		: void
 *[RETURNS]			: void
 */
void Buzzer_off(void)
{
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_LOW);
}
