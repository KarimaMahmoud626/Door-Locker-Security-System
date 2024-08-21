/******************************************************************************
 *
 * Module: TIMER1
 *
 * File Name: timer1.h
 *
 * Description: Header file for the Timer1 AVR driver
 *
 * Author: Karima Mahmoud
 *
 *******************************************************************************/

#ifndef TIMER1_H_
#define TIMER1_H_

/*******************************************************************************
 *                      Used Header Files                                      *
 *******************************************************************************/
#include "std_types.h"

/*******************************************************************************
 *                      Structs, Enums, and Types                              *
 *******************************************************************************/

typedef enum{
	NO_CLK, F_CPU_, F_CPU_8, F_CPU_64, F_CPU_256,F_CPU_1024, EXT_CLK_FALLING, EXT_CLK_RISING
}Timer1_Prescaler;

typedef enum{
	NORMAL, CTC = 4
}Timer1_Mode;

typedef struct{
	uint16 initial_value;
	uint16 compare_value;
	Timer1_Prescaler prescaler;
	Timer1_Mode mode;
}Timer1_ConfigType;

/*******************************************************************************
 *                                Functions Prototypes                         *
 *******************************************************************************/

/* Description: Function to initialize the Timer driver
 * */
void Timer1_init(const Timer1_ConfigType * Config_Ptr);

/* Description: Function to disable the Timer1.
 * */
void Timer1_deInit(void);

/* Description: Function to set the Call Back function address
 * */
void Timer1_setCallBack(void(*a_ptr)(void));

#endif /* TIMER1_H_ */
