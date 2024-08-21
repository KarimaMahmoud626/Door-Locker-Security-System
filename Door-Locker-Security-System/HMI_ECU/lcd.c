 /******************************************************************************
 *
 * Module: LCD
 *
 * File Name: lcd.c
 *
 * Description: Source file for the LCD driver
 *
 * Author: Karima Mahmoud
 *
 *******************************************************************************/

#include <util/delay.h>    /* For the delay functions */
#include "common_macros.h" /*For GET_BIT macro*/
#include "lcd.h"
#include "gpio.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*[FUNCTION NAME]	: LCD_Init
 *[DESCRIPTION]		: Initialize the LCD:
                      1. Setup the LCD pins directions by use the GPIO driver.
                      2. Setup the LCD Data Mode 4-bits or 8-bits.
 *[ARGUMENTS]		: void
 *[RETURNS]			: void
 */
void LCD_init(void)
{
	/* Configure the direction of RS and E pins as output pins */
	GPIO_setupPinDirection(LCD_RS_PORT_ID,LCD_RS_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_E_PORT_ID,LCD_E_PIN_ID,PIN_OUTPUT);

	_delay_ms(20);   /* LCD Power ON delay > 15ms */

#if(LCD_DATA_BITS_MODE == 4)
	/* Configure 4 pins in the data port as output pins */
	GPIO_setupPinDirectio(LCD_DATA_PORT_ID,LCD_DB4_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirectio(LCD_DATA_PORT_ID,LCD_DB5_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirectio(LCD_DATA_PORT_ID,LCD_DB6_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirectio(LCD_DATA_PORT_ID,LCD_DB7_PIN_ID,PIN_OUTPUT);

	/* Send for 4 bit initialization of LCD */
	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE_INT1);
	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE_INT2);

	/* Use 2-lines LCD + 4-bits Data Mode + 5*7 dot display Mode */
	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE);

#elif(LCD_DATA_BITS_MODE == 8)
	/* Configure the data port as output port */
	GPIO_setupPortDirection(LCD_DATA_PORT_ID,PORT_OUTPUT);

	/* Use 2-lines LCD + 4-bits Data Mode + 5*7 dot display Mode */
	LCD_sendCommand(LCD_TWO_LINES_EIGHT_BITS_MODE);

#endif
	LCD_sendCommand(LCD_CURSOR_OFF); /* Cursor OFF */
	LCD_sendCommand(LCD_CLEAR_COMMAND); /* Clear LCD at the beginning */

}

/*[FUNCTION NAME]	: LCD_sendCommand
 *[DESCRIPTION]		: Send the required command to the screen
 *[ARGUMENTS]		: command of type uint8
 *[RETURNS]			: void
 */
void LCD_sendCommand(uint8 cmd)
{
	GPIO_writePin(LCD_RS_PORT_ID,LCD_RS_PIN_ID,LOGIC_LOW); /* Instruction mode -> RS = 0 */
	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH); /* Enable LCD E = 1 */
	_delay_ms(1);

#if(LCD_DATA_BITS_MODE == 4)
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB4_PIN_ID,GET_BIT(cmd,4));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB5_PIN_ID,GET_BIT(cmd,5));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB6_PIN_ID,GET_BIT(cmd,6));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB7_PIN_ID,GET_BIT(cmd,7));

	_delay_ms(1); /* Delay for processing Tdsw = 100ns */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW); /* Disable LCD -> E = 0 */
	_delay_ms(1);/* Dealy for processing Th = 13ns */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH); /* Enable LCD E = 1 */
	_delay_ms(1);/* Delay for processing Tpw - Tdws  = 190ns */

	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB4_PIN_ID,GET_BIT(cmd,0));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB5_PIN_ID,GET_BIT(cmd,1));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB6_PIN_ID,GET_BIT(cmd,2));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB7_PIN_ID,GET_BIT(cmd,3));

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW); /* Disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */

#elif(LCD_DATA_BITS_MODE == 8)
	GPIO_writePort(LCD_DATA_PORT_ID,cmd); /* Write the required command to the data bus D0 --> D7 */
	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW); /* Disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */

#endif
}

/*[FUNCTION NAME]	: LCD_displayCharacter
 *[DESCRIPTION]		: Send the required command to the screen
 *[ARGUMENTS]		: data of type uint8
 *[RETURNS]			: void
 */
void LCD_displayCharacter(uint8 data)
{
	GPIO_writePin(LCD_RS_PORT_ID,LCD_RS_PIN_ID,LOGIC_HIGH); /* Data mode -> RS = 1 */
	_delay_ms(1);/* Dealy for processing Tas = 50ns */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH); /* Enable LCD E = 1 */
	_delay_ms(1);/* Dealy for processing Tpw - Tdws = 190ns */

#if(LCD_DATA_BITS_MODE == 4)
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB4_PIN_ID,GET_BIT(data,4));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB5_PIN_ID,GET_BIT(data,5));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB6_PIN_ID,GET_BIT(data,6));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB7_PIN_ID,GET_BIT(data,7));

	_delay_ms(1); /* Delay for processing Tdsw = 100ns */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW); /* Disable LCD -> E = 0 */
	_delay_ms(1);/* Dealy for processing Th = 13ns */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH); /* Enable LCD E = 1 */
	_delay_ms(1);/* Delay for processing Tpw - Tdws  = 190ns */

	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB4_PIN_ID,GET_BIT(data,0));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB5_PIN_ID,GET_BIT(data,1));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB6_PIN_ID,GET_BIT(data,2));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB7_PIN_ID,GET_BIT(data,3));

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW); /* Disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */

#elif(LCD_DATA_BITS_MODE == 8)
	GPIO_writePort(LCD_DATA_PORT_ID,data); /* Write the required command to the data bus D0 --> D7 */
	_delay_ms(1); /* delay for processing Tdsw = 100ns */

	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW); /* Disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */

#endif
}

/*[FUNCTION NAME]	: LCD_displayString
 *[DESCRIPTION]		: Display the required string on the screen
 *[ARGUMENTS]		: pointer to const data of type char
 *[RETURNS]			: void
 */
void LCD_displayString(const char *str)
{
	uint8 i = 0;
	while(str[i] != '\0')
	{
		LCD_displayCharacter(str[i]);
		i++;
	}
}

/*[FUNCTION NAME]	: LCD_moveCursor
 *[DESCRIPTION]		: Move the cursor to a specified row and column index on the screen
 *[ARGUMENTS]		: row number and column number of type uint8
 *[RETURNS]			: void
 */
void LCD_moveCursor(uint8 row,uint8 col)
{
	uint8 lcd_memory_address;

	/* Calculate the required address in the LCD DDRAM */
	switch(row)
	{
	case 0:
		lcd_memory_address = col;
		break;
	case 1:
		lcd_memory_address = col + 0x40;
		break;
	case 2:
		lcd_memory_address = col + 0x10;
		break;
	case 3:
		lcd_memory_address = col + 0x50;
		break;
	}
	/* Move the LCD cursor to this specific address */
	LCD_sendCommand(lcd_memory_address | LCD_SET_CURSOR_LOCATION);

}

/*[FUNCTION NAME]	: LCD_displayStringRowColumn
 *[DESCRIPTION]		: Display the required string in a specified row and column index on the screen
 *[ARGUMENTS]		: 1.row number and column number of type uint8
                      2.pointer to const data of type char
 *[RETURNS]			: void
 */
void LCD_displayStringRowColumn(uint8 row,uint8 col, const char *str)
{
	LCD_moveCursor(row,col); /* Go to the required LCD position */
	LCD_displayString(str);  /* Display String */
}

/*[FUNCTION NAME]	: LCD_integerToString
 *[DESCRIPTION]		: Display the required decimal value on the screen
 *[ARGUMENTS]		: data of type int
 *[RETURNS]			: void
 */
void LCD_integerToString(int data)
{
	char buff[16]; /* String to hold the ascii result */
	itoa(data,buff,10); /* Use itoa C function to convert the data to its corresponding ascii value, 10 for decimal */
	LCD_displayString(buff); /* Display the resulting String */

}

/*[FUNCTION NAME]	: LCD_clearScreen
 *[DESCRIPTION]		: Send the clear screen command
 *[ARGUMENTS]		: void
 *[RETURNS]			: void
 */
void LCD_clearScreen(void)
{
	LCD_sendCommand(LCD_CLEAR_COMMAND); /* Send clear display command */
}


