 /******************************************************************************
 *
 * Module: CONTROL_ECU
 *
 * File Name: control_ecu.c
 *
 * Description: Source file for the Control ECU
 *
 * Author: Karima Mahmoud
 *
 *******************************************************************************/

#include "uart.h"
#include "external_eeprom.h"
#include "twi.h"
#include "timer1.h"
#include "std_types.h"
#include "dc_motor.h"
#include "buzzer.h"
#include <util/delay.h>
#include <avr/io.h>

#define HMI_ECU_READY             0x10
#define CONTROL_ECU_READY         0x20
#define PASSWORD_SIZE             5
#define UNMATCHED_PASSWORD        0x50
#define MATCHED_PASSWORD          0x05
#define FIRST_PASSWORD_DELIVERED  0x80
#define SECOND_PASSWORD_DELIVERED 0x08
#define OPEN_DOOR                 43   /* + */
#define CHANGE_PASS               45  /* - */
#define DOOR_IS_UNLOCKING         15
#define DOOR_IS_LOCKING           15
#define MOTOR_HOLD                3
#define WRONG_PASSWORD            0
#define EEPROM_START_ADDRESS      0x0311
#define PASS_TRIALS               3
#define WARNING                   0x3C

uint8 current_password[PASSWORD_SIZE];
uint8 g_ticks = 0;
uint8 pressed_key = 0;

void Timer1_callBack(void);
void Save_Password(void);
void Receive_Password(uint8 *password, uint8 password_size);
uint8 Compare_Password(uint8 *pass1, uint8 *pass2, uint8 size);
void EEPROM_savePass(uint8 *pass, uint8 pass_size);
void Open_Door(void);
uint8 EEPROM_comparePass(uint8 *pass, uint8 pass_size);
void Motor_Fun(void);
void Change_Password(void);
void Buzzer_function(void);

int main(void)
{
	/*Enable I-bit*/
	SREG|=(1<<7);

	/* Initialize TWI driver */
	TWI_ConfigType twi_configurations = {0x10,BIT_RATE_FAST_MODE};
	TWI_init(&twi_configurations);

	/* Initialize UART driver */
	UART_ConfigType uart_configurations = {EIGHT_BITS,EVEN,ONE_BIT,9600};
	UART_init(&uart_configurations);

	/* Initialize Timer1 Driver to make an interrupt every 1sec */
	Timer1_ConfigType configurations = {0,8000,F_CPU_1024,CTC};
	Timer1_init(&configurations);
	Timer1_setCallBack(Timer1_callBack);

	/* Initialize the Motor Driver */
	DcMotor_Init();

	/* Wait for the verification byte to start */
	while(UART_recieveByte() != HMI_ECU_READY);

	/* Call Save Password function */
	Save_Password();


	while(1)
	{
		/* Receive the pressed key is received */
		pressed_key = UART_recieveByte();

		if(pressed_key == OPEN_DOOR)
		{
			Open_Door();
		}
		if(pressed_key == CHANGE_PASS)
		{
			Change_Password();
		}
	}

}


void Timer1_callBack(void)
{
	g_ticks++;
}

void Save_Password(void)
{
	uint8 pass_state = UNMATCHED_PASSWORD;
	uint8 first_received_pass[PASSWORD_SIZE], second_received_pass[PASSWORD_SIZE];

	while(pass_state != MATCHED_PASSWORD)
	{
		/* Wait for the verification of the HMI_ECU */
		while(UART_recieveByte() != HMI_ECU_READY);

		/* Send the confirmation */
		UART_sendByte(CONTROL_ECU_READY);

		/* Receive the first Password */
		Receive_Password(first_received_pass,PASSWORD_SIZE);

		/* Send confirmation of complete reception of first password */
		UART_sendByte(FIRST_PASSWORD_DELIVERED);

		/* Receive the second Password */
		Receive_Password(second_received_pass,PASSWORD_SIZE);

		/* Compare the two received passwords */
		pass_state = Compare_Password(first_received_pass, second_received_pass, PASSWORD_SIZE);

		/* Send confirmation of complete reception of second password */
		UART_sendByte(SECOND_PASSWORD_DELIVERED);

		/* Send the state of passwords */
		UART_sendByte(pass_state);

		/* Check if the passwords are matched or not */
		/*1. if the passwords are matched*/
		if(pass_state == MATCHED_PASSWORD)
		{
			/* Send any one of the two received passwords to the EEPROM to be saved */
			EEPROM_savePass(first_received_pass, PASSWORD_SIZE);
		}
		else if(pass_state == UNMATCHED_PASSWORD)/* 2. if the two passwords are not matched */
		{
			pass_state = UNMATCHED_PASSWORD;
		}
	}
}

void Receive_Password(uint8 *password, uint8 password_size)
{
	uint8 counter = 0;

	for(counter = 0 ; counter < password_size ; counter++)
	{
		password[counter] = UART_recieveByte();
		_delay_ms(50);
	}
}

uint8 Compare_Password(uint8 *pass1, uint8 *pass2, uint8 size)
{
	uint8 counter = 0;
	uint8 pass_state = UNMATCHED_PASSWORD;

	for(counter = 0; counter < size ; counter++)
	{
		if(pass1[counter] != pass2[counter])
		{
			pass_state = UNMATCHED_PASSWORD;
			break;
		}
		else
		{
			pass_state = MATCHED_PASSWORD;
		}
	}
	return pass_state;
}

void EEPROM_savePass(uint8 *pass, uint8 pass_size)
{
	uint8 counter = 0;

	for(counter = 0 ; counter < pass_size ; counter++)
	{
		EEPROM_writeByte(EEPROM_START_ADDRESS + counter, pass[counter]);
		_delay_ms(10);
	}
}

void Open_Door(void)
{
	uint8 pass_state = UNMATCHED_PASSWORD;
	uint8 pass_trails = 0;

	while(pass_state != MATCHED_PASSWORD)
	{
		/* Wait for the verification pattern to be received */
		while(UART_recieveByte() != OPEN_DOOR);

		/* Send the confirmation byte */
		UART_sendByte(CONTROL_ECU_READY);

		/* Receive the password from the HMI_ECU */
		Receive_Password(current_password,PASSWORD_SIZE);

		/* Compare the received password to the one saved in the EEPROM */
		pass_state = EEPROM_comparePass(current_password, PASSWORD_SIZE);

		if(pass_state == MATCHED_PASSWORD)
		{
			/*return trials to zero again*/
			pass_trails = 0;
			/*process the motor rotation function*/
			UART_sendByte(MATCHED_PASSWORD);
			Motor_Fun();
			break;

		}
		/*for passwords unmatched try again you have 3 trials*/
		else if(pass_state == UNMATCHED_PASSWORD)
		{
			/*you have only 3 trials*/
			pass_trails++;
			if(pass_trails == PASS_TRIALS)
			{
				UART_sendByte(WARNING);
				/*reset  your pass_trails to zero*/
				Buzzer_function();
				pass_trails = 0;
				break;
			}
			else
			{
				UART_sendByte(UNMATCHED_PASSWORD);
			}
		}
	}
}

uint8 EEPROM_comparePass(uint8 *pass, uint8 pass_size)
{
	uint8 pass_state = UNMATCHED_PASSWORD;
	uint8 counter = 0;
	uint8 read_byte = 0;

	for(counter = 0 ; counter < pass_size ; counter++)
	{
		/* Read the password from EEPROM byte by byte */
		EEPROM_readByte(EEPROM_START_ADDRESS + counter, &read_byte);

		if(read_byte != pass[counter])
		{
			pass_state = UNMATCHED_PASSWORD;
			break;
		}
		else if(read_byte == pass[counter])
		{
			pass_state = MATCHED_PASSWORD;
		}
	}
	return pass_state;
}

void Motor_Fun(void)
{
	/*Opening the door in 15sec*/
	g_ticks=0;
	DcMotor_Rotate(CW,100); /* Rotate the DC Motor in clock wise direction with maximum speed */
	while (g_ticks < DOOR_IS_UNLOCKING);

	/*Holding the door in 3sec*/
	g_ticks=0;
	DcMotor_Rotate(STOP,0); /* Hold the DC Motor */
	while (g_ticks < MOTOR_HOLD);

	/*Closing the door in 15sec*/
	g_ticks=0;
	DcMotor_Rotate(A_CW,100); /* Rotate the DC Motor in anti-clock wise direction with maximum speed */
	while (g_ticks < DOOR_IS_LOCKING);

	/*Stop the Motor*/
	DcMotor_Rotate(STOP,0); /* Stop the DC Motor */
	_delay_ms(1000);
}

void Change_Password(void)
{
	uint8 pass_state = UNMATCHED_PASSWORD;
	uint8 pass_trails = 0;

	while(pass_state != MATCHED_PASSWORD)
	{
		/* Wait for the verification pattern to be received */
		while(UART_recieveByte() != CHANGE_PASS);

		/* Send the confirmation byte */
		UART_sendByte(CONTROL_ECU_READY);

		/* Receive the password from the HMI_ECU */
		Receive_Password(current_password,PASSWORD_SIZE);

		/* Compare the received password to the one saved in the EEPROM */
		pass_state = EEPROM_comparePass(current_password, PASSWORD_SIZE);

		if(pass_state == MATCHED_PASSWORD)
		{
			/*return trials to zero again*/
			pass_trails = 0;
			/*process the motor rotation function*/
			UART_sendByte(MATCHED_PASSWORD);
			Save_Password();

		}
		/*for passwords unmatched try again you have 3 trials*/
		else if(pass_state == UNMATCHED_PASSWORD)
		{
			/*you have only 3 trials*/
			pass_trails++;
			if(pass_trails == PASS_TRIALS)
			{
				UART_sendByte(WARNING);
				/*reset  your pass_trails to zero*/
				Buzzer_function();
				pass_trails = 0;
				break;
			}
			else
			{
				UART_sendByte(UNMATCHED_PASSWORD);
			}
		}
	}
}

void Buzzer_function(void){
	g_ticks=0;
	/*operate buzzer for 60 sec*/
	Buzzer_on();
	while (g_ticks < WARNING);
	Buzzer_off();
}
