/******************************************************************************
* Project Name		: Stepper Motor Control
* File Name			: uart.h
* Version 			: 1.0
* Device Used		: CY8C4245AXI-483
* Software Used		: PSoC Creator 4.0
* Compiler    		: ARM GCC 4.9-2015-q1-update
* Related Hardware	: 
*
* Owner             : Ventmatika Inc.
*******************************************************************************/
#if !defined(_UART_H_)
#define _UART_H_
    
/* --------------------------- System includes ------------------------------*/
#include <project.h>
#include "common.h"

    
#define UART_DATA_WIDTH         (8u)
    
#pragma pack(push,1)
typedef struct{
    union{
        uint8_t         data[7];
        struct{
            union{
                uint8_t     data[4];
                uint32_t    val;
            }Baudrate;
            union{
                uint8_t     data[1];
                uint8_t     val;
            }Parity;
            union{
                uint8_t     data[1];
                uint8_t     val;
            }StopBits;
            union{
                uint8_t     data[1];
                uint8_t     val;
            }DataBits;
        };
    }Settings;    
}UART_HandleTypeDef;
#pragma pack(pop)


void UartConfig( uint32_t baudrate, uint8_t parity, uint8_t stopbits, uint8_t databits );
void UartStart( void );
void UartStop( void );

#endif /* _UART_H_ */ 
/* [] END OF FILE */
