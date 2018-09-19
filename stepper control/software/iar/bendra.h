/****************************************************************
// bendra.h
//
// Created: 11/27/2014 1:54:37 PM
//  Author: Alex
//
****************************************************************/
#if !defined(BENDRA_H)
#define BENDRA_H


#include <intrinsics.h>
#include <ioavr.h>
#include "macro.h"

/*============================================================================
Typedefs
============================================================================*/
typedef char	 	bool, BOOL, CHAR, int8_t;
//typedef signed char     sint8;
typedef unsigned char	byte, UCHAR, uint8, uint8_t;
//typedef int      	int16;
//typedef signed int      sint16;
typedef unsigned int    word, uint16;
typedef unsigned short	ushort, USHORT, uint16_t;
typedef short 	SHORT, int16_t;
//typedef long		LONG;
typedef signed long     SLONG, sint32, int32_t;
typedef unsigned long	ULONG, uint32, uint32_t;

typedef union{
	uint8_t		data[2];
	uint16_t	val;
}reg16_t;

typedef union{
	uint8_t		data[4];
	uint32_t	val;
}reg32_t;


/*============================================================================
Defines, Macros
============================================================================*/
#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }
#define EnterCriticalSection()      __disable_interrupt()
#define LeaveCriticalSection()      __enable_interrupt()

/* unsigned char var */
/* var = SAVE_ENV() -- save interrupt flags */
#define SAVE_ENV()		        __save_interrupt()
/* RESTORE_ENV(var) -- restore interrupts flags */
#define RESTORE_ENV(E)		    __restore_interrupt(E);

#if(!defined  F_CPU)
    #define	F_CPU	8000000UL
#endif

#define delay_us(temp) __delay_cycles((F_CPU/1000000UL)  *(temp))
#define delay_ms(temp) __delay_cycles((F_CPU/1000UL)  *(temp))

#define false	    0
#define true	    1

#define ZERO		false

//#define FALSE		false
//#define TRUE		true

//#define NO			false
//#define YES			true

//#define LOW			false
//#define HIGH		true

//#define STOP		false
//#define RUN			true

//#define DISABLED	false
//#define ENABLED		true

enum { FALSE = 0, TRUE = 1 };
enum { NO = FALSE, YES = TRUE };
enum { OFF = FALSE, ON = TRUE };
enum { LOW = FALSE, HIGH = TRUE };
enum { STOP = FALSE, RUN = TRUE };
enum { DISABLED = FALSE, ENABLED = TRUE };
enum { NO_ERROR = FALSE, IS_ERROR = TRUE };


//#define _BV(PIN) (1<<PIN)

#define assert( x )

#endif /* BENDRA_H */
/*============================================================================
  END OF FILE 
============================================================================*/