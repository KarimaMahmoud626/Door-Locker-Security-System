 /******************************************************************************
 *
 * Module: HMI_ECU
 *
 * File Name: hmi_ecu.c
 *
 * Description: Source file for the Human Machine Interface ECU
 *
 * Author: Karima Mahmoud
 *
 *******************************************************************************/

#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer1.h"
#include <util/delay.h>
#include <avr/io.h>

#define HMI_ECU_READY             0x10
#define CONTROL_ECU_READY         0x20
#define PASSWORD_SIZE             5
#define UNMATCHED_PASSWORD        0x50
#define MATCHED_PASSWORD          0x05
#define FIRST_PASSWORD_DELIVERED  0x80
#define SECOND_PASSWORD_DELIVERED 0x08
#define ENTER                     61    /* = */
#define OPEN_DOOR                 43   /* + */
#define CHANGE_PASS               45  /* - */
#define DOOR_IS_UNLOCKING         15
#define DOOR_IS_LOCKING           15
#define MOTOR_HOLD                3
#define WARNING                   0x3C


void Send_Password(uint8 *password, uint8 password_size);
void Enter_passMessage(void);
void ReEnter_passMessage(void);
void Set_Password(void);
void Get_Password(uint8 *password,uint8 pass_size);
void Timer1_callBack(void);
void Main_Options(void);
void Motor_Fun(void);
void Open_Door(void);
void Change_Password(void);
void Warning_Message(void);
void Change_passMessage(void);


uint8 Current_Password[PASSWORD_SIZE];
uint8 g_ticks = 0;
uint8 pressed_key = 0;


int main(void)
{
	/*Enable I-bit*/
	SREG|=(1<<7);

	/* Initialize UART driver */
	UART_ConfigType uart_configurations = {EIGHT_BITS,EVEN,ONE_BIT,9600};
	UART_init(&uart_configurations);

	/* Initialize Timer1 Driver to make an interrupt every 1sec */
	Timer1_ConfigType configurations = {0,8000,F_CPU_1024,CTC};
	Timer1_init(&configurations);
	Timer1_setCallBack(Timer1_callBack);

	/*Initialize the LCD driver*/
	LCD_init();

	/* LCD Initialization completed and ready to communication */
	UART_sendByte(HMI_ECU_READY);

	LCD_displayStringRowColumn(0,2,"Door Locker");
	LCD_displayStringRowColumn(1,0,"Security System!");
	_delay_ms(2500);

	/* Call the Set Password Function */
	Set_Password();

	while(1)
	{
		pressed_key = KEYPAD_getPressedKey();

		/* Send the pressed key to the CONTROL_ECU */
		UART_sendByte(pressed_key);

		/* The LCD will always display main options */
		Main_Options();

		if(pressed_key == OPEN_DOOR)
		{
			Open_Door();
		}
		else if(pressed_key == CHANGE_PASS)
		{
			Change_Password();
		}
	}
}


void Timer1_callBack(void)
{
	g_ticks++;
}

void Set_Password(void)
{
	uint8 entered_password[PASSWORD_SIZE], reentered_password[PASSWORD_SIZE];
	uint8 pass_state = UNMATCHED_PASSWORD;

	while(pass_state != MATCHED_PASSWORD)
	{
		/* Display enter password message */
		Enter_passMessage();

		/* Get the entered password from the user */
		Get_Password(entered_password,PASSWORD_SIZE);

		/* Display message to ask user to re-enter the password */
		ReEnter_passMessage();

		/* Get the re-entered password from the user */
		Get_Password(reentered_password,PASSWORD_SIZE);

		/* Send a verification pattern to the CONTROL_ECU */
		UART_sendByte(HMI_ECU_READY);

		/* Wait for confirmation from the CONTROL_ECU */
		while(UART_recieveByte() != CONTROL_ECU_READY);

		/* Send the first entered password to the CONTROL_ECU */
		Send_Password(entered_password,PASSWORD_SIZE);

		/* Wait for the Sending is completed */
		while(UART_recieveByte() != FIRST_PASSWORD_DELIVERED);

		/* Send the second entered password/re-entered password to the CONTROL_ECU */
		Send_Password(reentered_password,PASSWORD_SIZE);

		/* Wait for the Sending is completed */
		while(UART_recieveByte() != SECOND_PASSWORD_DELIVERED);

		/* Read the matching state of the passwords */
		pass_state = UART_recieveByte();

		/* Check the state */
		if(pass_state == UNMATCHED_PASSWORD)
		{
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"Unmatched Pass");
			LCD_displayStringRowColumn(0,1,"Try again!!");

		}
	}

	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"New Pass is Set!");
	_delay_ms(1000);
	LCD_clearScreen();
	/*If Password MATCHED display main menu one time before while*/
	Main_Options();
}

void Send_Password(uint8 *password, uint8 password_size)
{
	uint8 counter = 0;

	for(counter = 0 ; counter < password_size ; counter++)
	{
		UART_sendByte(password[counter]);
		_delay_ms(50);
	}
}

void Enter_passMessage(void){
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"PLZ Enter Pass:");
	LCD_moveCursor(1,0);
}

void ReEnter_passMessage(void)
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"PLZ re-enter the ");
	LCD_displayStringRowColumn(1,0,"same pass:");
}

void Open_Door(void)
{
	uint8 pass_state = UNMATCHED_PASSWORD;
	uint8 received_byte = 0;

	while(pass_state != MATCHED_PASSWORD)
	{
		/* Display message to ask user to enter the old password */
		Enter_passMessage();

		/* Get the entered password */
		Get_Password(Current_Password,PASSWORD_SIZE);

		/* Send a verification pattern to the CONTROL_ECU */
		UART_sendByte(OPEN_DOOR);

		/* Wait for confirmation from the CONTROL_ECU */
		while(UART_recieveByte() != CONTROL_ECU_READY);

		/* Send the first entered password to the CONTROL_ECU */
		Send_Password(Current_Password,PASSWORD_SIZE);

		/* Read the matching state of the passwords */
		received_byte = UART_recieveByte();

		/* Check the state */
		if(received_byte == MATCHED_PASSWORD)
		{
			/* Open the door */
			Motor_Fun();
			pass_state = MATCHED_PASSWORD;
			break;
		}
		else if(received_byte == UNMATCHED_PASSWORD)
		{
			pass_state = UNMATCHED_PASSWORD;
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,1,"WRONG PASS!!");
			_delay_ms(1500);
		}
		else if(received_byte  == WARNING){
			Warning_Message();
			break;
		}
	}
}

void Change_Password(void)
{
	uint8 pass_state = UNMATCHED_PASSWORD;
	uint8 received_byte = 0;

	while(pass_state != MATCHED_PASSWORD)
	{
		/* Display message to ask user to enter the old password */
		Change_passMessage();

		/* Get the entered password */
		Get_Password(Current_Password,PASSWORD_SIZE);

		/* Send a verification pattern to the CONTROL_ECU */
		UART_sendByte(CHANGE_PASS);

		/* Wait for confirmation from the CONTROL_ECU */
		while(UART_recieveByte() != CONTROL_ECU_READY);

		/* Send the first entered password to the CONTROL_ECU */
		Send_Password(Current_Password,PASSWORD_SIZE);

		/* Read the matching state of the passwords */
		received_byte = UART_recieveByte();

		/* Check the state */
		if(received_byte == MATCHED_PASSWORD)
		{
			/* Set the new Password */
			Set_Password();
			break;
		}
		else if(received_byte == UNMATCHED_PASSWORD)
		{
			pass_state = UNMATCHED_PASSWORD;
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,1,"WRONG PASS!!");
			_delay_ms(2000);
		}
		else if(received_byte  == WARNING){
			Warning_Message();
			break;
		}
	}

}

void Get_Password(uint8 *password,uint8 pass_size)
{
	uint8 key;
	uint8 i =0;

	for(i=0 ; i<pass_size ; i++){
		_delay_ms(400);
		key =  KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		password[i] =  key;
		_delay_ms(200);
	}

	while(KEYPAD_getPressedKey() != ENTER);
}

void Motor_Fun(void)
{
	/*Opening the door in 15sec*/
	g_ticks=0;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,4,"Door is");
	LCD_displayStringRowColumn(1,4,"Unlocking!");
	while (g_ticks < DOOR_IS_UNLOCKING);

	/*Holding the door in 3sec*/
	g_ticks=0;
	LCD_clearScreen();
	LCD_displayString("Door is Unlock!");
	while (g_ticks < MOTOR_HOLD);

	/*Closing the door in 15sec*/
	g_ticks=0;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,4,"Door is");
	LCD_displayStringRowColumn(1,4,"Locking!");
	while (g_ticks < DOOR_IS_LOCKING);

	/*Stop the Motor*/
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,4,"Door is");
	LCD_displayStringRowColumn(1,4,"Locked!");
	_delay_ms(1000);
	/*The LCD will always display the main system options*/
	LCD_clearScreen();
	Main_Options();
}

void Main_Options(void)
{
	LCD_clearScreen();

	/* Display system main options */
	LCD_displayStringRowColumn(0,0,"+ : Open Door");
	LCD_displayStringRowColumn(1,0,"- : Change Pass");
}

void Warning_Message(void){
	g_ticks=0;
	LCD_clearScreen();
	while(g_ticks < WARNING){
		LCD_displayStringRowColumn(0,2,"!!!Warning!!!");
	}
	LCD_clearScreen();
	/*The LCD will always display the main system options*/
	Main_Options();

}

void Change_passMessage(void)
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"PLZ enter the ");
	LCD_displayStringRowColumn(1,0,"old pass:");
}
