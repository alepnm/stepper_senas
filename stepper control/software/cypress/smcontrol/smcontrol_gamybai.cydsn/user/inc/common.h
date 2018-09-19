/******************************************************************************
* Project Name		: Stepper Motor Control
* File Name			: common.h
* Version 			: 1.0
* Device Used		: CY8C4245AXI-483
* Software Used		: PSoC Creator 4.0
* Compiler    		: ARM GCC 4.9-2015-q1-update
* Related Hardware	: 
*
* Owner             : Ventmatika Inc.
*******************************************************************************/
#if !defined(COMMON_H)
#define COMMON_H

#include <project.h>
#include <stdbool.h>

/*************************Macro Definitions**********************************/
#define die()   while(1);

/*  Bendri defainai  */
enum { FALSE = 0u, TRUE = 1u };
enum { ZERO = 0u, NONZERO = 1u }; 
enum { LOW = 0u, HIGH = 1u };
enum { DISABLE = 0u, ENABLE = 1u };
enum { LED_OFF = 0u, LED_ON = 1u };
enum { CW = 1u, CCW = 255u };
enum { PIN_LOW = 0u, PIN_HIGH = 1u };
enum { STOP = 0u, START = 1u };
    
#define DISABLED            0x00
#define NON_APPLICABLE      0x00
#define ENABLED             0x01
    
#define BYTE2INT(hi,low)    (int)((hi << 8) | low)
#define INT2DINT(hi,low)    (int)((hi << 16) | low) 
    
/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

/* x=target variable, y=mask */
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLIP(x,y) ((x) ^= (y))
#define BITMASK_CHECK(x,y) (((x) & (y)) == (y))


    
#define N_ELEMENTS(x)       (sizeof(x)/sizeof(*(x)))
    
    
/*****************************************************************************/
#endif
/* [] END OF FILE */
