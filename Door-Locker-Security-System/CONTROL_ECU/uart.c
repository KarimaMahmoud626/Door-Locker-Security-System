 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author: Karima Mahmoud
 *
 *******************************************************************************/

/*******************************************************************************
 *                      Used Header Files                                      *
 *******************************************************************************/

#include "uart.h"
#include <avr/io.h>
#include "common_macros.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*[FUNCTION NAME]	: UART_init
 *[DESCRIPTION]		: Initialize the UART:
 *                    1. Setup the Frame format like number of data bits,
 *                    parity bit type and number of stop bits.
 *                    2. Enable the UART.
 *                    3. Setup the UART baud rate.
 *[ARGUMENTS]		: void
 *[RETURNS]			: void
 */

void UART_init(const UART_ConfigType *uartConfig_ptr)
{
	uint16 ubrr_value = 0;

	/* Set U2X bit to Double the USART Transmission Speed */
	UCSRA = (1<<U2X);

	/************************** UCSRB Description **************************
	 * RXCIE = 0 Disable USART RX Complete Interrupt Enable
	 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
	 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 * UCSZ2 = 0 For 8-bit data mode
	 * RXB8 & TXB8 not used for 8-bit data mode
	 ***********************************************************************/
	UCSRB = (1<<RXEN) | (1<<TXEN);
	UCSRB = (UCSRB & 0xfb) | (((uartConfig_ptr->bit_data) & 0xfb) << 2);

	/************************** UCSRC Description **************************
	 * URSEL   = 1 The URSEL must be one when writing the UCSRC
	 * UMSEL   = 0 Asynchronous Operation
	 * UPM1:0  = parity type
	 * USBS    = stop bit type
	 * UCSZ1:0 = 11 For 8-bit data mode
	 * UCPOL   = 0 Used with the Synchronous operation only
	 ***********************************************************************/
	UCSRC = (1<<URSEL);
	UCSRC = (UCSRC & 0xf9) | (((uartConfig_ptr->bit_data) & 0xf9) << 1);
	UCSRC = (UCSRC & 0xcf) | ((uartConfig_ptr->parity) << 4);
	UCSRC = (UCSRC & 0xf7) | ((uartConfig_ptr->stop_bit) << 3);

	/* Calculate the UBRR register value */
	ubrr_value = (uint16)(((F_CPU / (uartConfig_ptr->baud_rate * 8UL))) - 1);

	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRH = ubrr_value>>8;
	UBRRL = ubrr_value;
}

/*[FUNCTION NAME]	: UART_sendByte
 *[DESCRIPTION]		: Send byte to another UART device
 *[ARGUMENTS]		: data of type uint8
 *[RETURNS]			: void
 */

void UART_sendByte(const uint8 data)
{
	/* UDRE flag is set when the Tx buffer (UDR) is empty and ready for
	 * transmitting a new byte so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,UDRE));

	/* Put the required data in the UDR register and it also clear the UDR slag
	 * as the UDR register is not empty now */
	UDR = data;
}

/*[FUNCTION NAME]	: UART_recieveByte
 *[DESCRIPTION]		: Receive byte from another UART device
 *[ARGUMENTS]		: void
 *[RETURNS]			: data of type uint8
 */

uint8 UART_recieveByte(void)
{
	/* RXC flag is set when the UART receive data so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,RXC));

	/* Read the received data from the Rx buffer (UDR)
	 * the RXC flag will be cleared after read the data */
	return UDR;
}

/*[FUNCTION NAME]	: UART_sendString
 *[DESCRIPTION]		: Send string to another UART device
 *[ARGUMENTS]		: string (pointer to uint8/char)
 *[RETURNS]			: void
 */

void UART_sendString(const uint8 *str)
{
	uint8 i = 0;

	/* Send the whole string */
	while(str[i] != '\0')
	{
		UART_sendByte(str[i]);
		i++;
	}
}

/*[FUNCTION NAME]	: UART_receiveString
 *[DESCRIPTION]		: Receive string from another UART device
 *[ARGUMENTS]		: void
 *[RETURNS]			: string (pointer to uint8/char)
 */

void UART_receiveString(uint8 *str)
{
	uint8 i = 0;

	/* Receive first byte */
	str[i] = UART_recieveByte();

	/* Receive the whole string until the '#' */
	while(str[i] != '=')
	{
		i++;
		str[i] = UART_recieveByte();
	}

	/* After receiving the string until the '#', replace the '#' with '\0' */
	str[i] = '\0';
}
