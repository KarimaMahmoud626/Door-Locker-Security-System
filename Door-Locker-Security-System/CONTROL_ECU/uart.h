 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Karima Mahmoud
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

/*******************************************************************************
 *                      Used Header Files                                      *
 *******************************************************************************/
#include "std_types.h"

/*******************************************************************************
 *                      Structs, Enums, and Types                              *
 *******************************************************************************/

typedef uint32 UART_BaudRate;

typedef enum{
	DISABLED, RESERVED, EVEN, ODD
}UART_Parity;

typedef enum{
	ONE_BIT, TWO_BIT
}UART_StopBit;

typedef enum{
	FIVE_BITS, SIX_BITS, SEEN_BITS, EIGHT_BITS, RESERVED_4,RSERVED_5,RESERVED_6,NINE_BITS
}UART_BitData;

typedef struct{
	 UART_BitData bit_data;
	 UART_Parity parity;
	 UART_StopBit stop_bit;
	 UART_BaudRate baud_rate;
}UART_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType *uartConfig_ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
