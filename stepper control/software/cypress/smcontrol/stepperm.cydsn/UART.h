/*******************************************************************************
* File Name: UART.h  
* Version 1.1
*
*  Description:
*    This file contains the function prototypes and constants used in
*    the GUI control.
*
*   Note:
*     
*
********************************************************************************
* Copyright (2010), Cypress Semiconductor Corporation.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is 
* protected by and subject to worldwide patent protection (United States and 
* foreign), United States copyright laws and international treaty provisions. 
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable 
* license to copy, use, modify, create derivative works of, and compile the 
* Cypress Source Code and derivative works for the sole purpose of creating 
* custom software in support of licensee product to be used only in conjunction 
* with a Cypress integrated circuit as specified in the applicable agreement. 
* Any reproduction, modification, translation, compilation, or representation of 
* this software except as specified above is prohibited without the express 
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. Use may be 
* limited by and subject to the applicable Cypress software license agreement. 
*******************************************************************************/
#ifndef _UART_H_
#define _UART_H_

extern void RxData(void);
extern void UART_Ack(void);

#define SYNC_HEADER     	0xAA
#define ACK             	0x00
#define NACK            	0xFF

#define CONN            	0x0F
#define DISCONN         	0xF0

/*Set Command*/
#define MOTOR_RUN       	0x90
#define MOTOR_STOP      	0x91
#define SET_SPEED       	0x15
#define SET_DIR         	0x93
#define SET_MAX_CURR    	0x92
#define SET_POLES       	0x12
#define SET_MAX_SPEED   	0x10
#define SET_MIN_SPEED   	0x11
#define SET_KP          	0x13
#define SET_KI          	0x14

#define SET_MICROSTEP       0x50
#define SET_LINE_START    	0x51
#define SET_LINE_END      	0x52
#define SET_LINE_CNT      	0x53
#define SET_LINE_WAIT_CNT 	0x54
#define SET_CURR_GAIN       0x55
#define SET_LINE_TOTAL_CNT  0x56

/*Get Command*/
#define GET_TYPE        	0xB0
#define GET_SPEED       	0xC0
#define GET_STEPCNT     	0xC1
#define GET_CURR        	0xC2
#define GET_STATUS      	0xE0

#define SEND_NACK()   { \
UART_1_WriteTxData(SYNC_HEADER); \
UART_1_WriteTxData(SYNC_HEADER); \
UART_1_WriteTxData(NACK); \
UART_1_WriteTxData(0xFF); \
UART_1_WriteTxData(0x00); \
}

#define SEND_ACK()   { \
UART_1_WriteTxData(SYNC_HEADER); \
UART_1_WriteTxData(SYNC_HEADER); \
UART_1_WriteTxData(ACK); \
UART_1_WriteTxData(pkg[1]); \
UART_1_WriteTxData(0x00); \
}

#define RX_BUF_SIZE     	16

#endif	/* End _UART_H_ */