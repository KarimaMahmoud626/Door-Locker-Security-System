 /******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name: dc_motor.c
 *
 * Description: source file for the DC Motor driver
 *
 * Author: Karima Mahmoud
 *
 *******************************************************************************/

/*******************************************************************************
 *                                Used Header Files                            *
 *******************************************************************************/

#include "dc_motor.h"
#include "pwm.h"
#include "gpio.h"
#include <avr/io.h>

/*******************************************************************************
 *                                Functions Definitions                        *
 *******************************************************************************/

/*[FUNCTION NAME]	: DcMotor_Init
 *[DESCRIPTION]		: The function responsible for the DC Motor initialization
 *[ARGUMENTS]		: void
 *[RETURNS]			: void
 */
void DcMotor_Init(void)
{
	/* Configure the IN1, IN2 as output pins */
	GPIO_setupPinDirection(DC_MOTOR_PORT_ID,DC_MOTOR_IN1_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_PORT_ID,DC_MOTOR_IN2_PIN_ID,PIN_OUTPUT);

	/* Stop DC Motor initially */
	GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_LOW);

}

/*[FUNCTION NAME]	: DcMotor_Rotate
 *[DESCRIPTION]		: The function responsible for rotate the DC Motor CW/ or A-CW or
                      stop the motor in a certain speed
 *[ARGUMENTS]		: void
 *[RETURNS]			: temperature value of type uint8
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{
	switch(state)
	{
	case STOP:
		/* Stop DC Motor */
		GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_LOW);
		break;
	case CW:
		/* Rotate DC Motor in clockwise direction */
		GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_HIGH);
		break;
	case A_CW:
		/* Rotate DC Motor in anti-clockwise direction */
		GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_HIGH);
		GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_LOW);
		break;
	}
	/* Setup Motor Speed */
	PWM_Timer0_Start(speed);
}



