 /******************************************************************************
 *
 * Module: Pulse Width Modulation
 *
 * File Name: pwm.h
 *
 * Description: header file for the PWM driver
 *
 * Author: Karima Mahmoud
 *
 *******************************************************************************/

#ifndef PWM_H_
#define PWM_H_

#include"std_types.h"

/*******************************************************************************
 *                                Functions Prototype                          *
 *******************************************************************************/

/*Description:
 *The function responsible for trigger the Timer0 with the PWM Mode.
 */
void PWM_Timer0_Start(uint8 duty_cycle);

#endif /* PWM_H_ */
