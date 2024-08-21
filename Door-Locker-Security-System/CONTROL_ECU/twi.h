 /******************************************************************************
 *
 * Module: TWI(I2C)
 *
 * File Name: twi.h
 *
 * Description: Header file for the TWI(I2C) AVR driver
 *
 * Author: Karima Mahmoud
 *
 *******************************************************************************/

#ifndef TWI_H_
#define TWI_H_

/*******************************************************************************
 *                      Used Header Files                                      *
 *******************************************************************************/

#include "std_types.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

/* I2C Status Bits in the TWSR Register */
#define TWI_START         0x08 /* start has been sent */
#define TWI_REP_START     0x10 /* repeated start */
#define TWI_MT_SLA_W_ACK  0x18 /* Master transmit ( slave address + Write request ) to slave + ACK received from slave. */
#define TWI_MT_SLA_R_ACK  0x40 /* Master transmit ( slave address + Read request ) to slave + ACK received from slave. */
#define TWI_MT_DATA_ACK   0x28 /* Master transmit data and ACK has been received from Slave. */
#define TWI_MR_DATA_ACK   0x50 /* Master received data and send ACK to slave. */
#define TWI_MR_DATA_NACK  0x58 /* Master received data but doesn't send ACK to slave. */

/*******************************************************************************
 *                      Structs, Enums, and Types                              *
 *******************************************************************************/

typedef uint8 TWI_Adress;

typedef enum{
	BIT_RATE_NORMAL_MODE = 100000,
	BIT_RATE_FAST_MODE = 400000,
	BIT_RATE_FAST_MODE_PLUS = 1000000,
	BIT_RATE_HIGH_SPEED_MODE = 3400000
}TWI_BaudRate;

typedef struct{
	TWI_Adress address;
	TWI_BaudRate bit_rate;
}TWI_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*Description:
 * Function responsible for initialize the TWI device as required */
void TWI_init(TWI_ConfigType * Config_Ptr);

/* Description:
 * Function responsible for sending the start bit to start frame communication  */
void TWI_start(void);

/* Description:
 * Function responsible for sending the stop bit */
void TWI_stop(void);

/* Description:
 * Function responsible for writing/sending a byte to another device to stop frame communication */
void TWI_writeByte(uint8 data);

/* Function responsible for reading/receiving a byte from transmitter with ACK */
uint8 TWI_readByteWithACK(void);

/* Description:
 * Function responsible for reading/receiving a byte from transmitter with NACK */
uint8 TWI_readByteWithNACK(void);

/* Description:
 * Function responsible for get five bits that reflect status of the TWI logic and two-wire Serial Bus */
uint8 TWI_getStatus(void);

#endif /* TWI_H_ */
