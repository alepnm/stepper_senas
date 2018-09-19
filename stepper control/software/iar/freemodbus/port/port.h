/*
 * FreeModbus Libary: AVR Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *   - Initial version + ATmega168 support
 * Modfications Copyright (C) 2006 Tran Minh Hoang:
 *   - ATmega8, ATmega16, ATmega32 support
 *   - RS485 support for DS75176
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: port.h,v 1.6 2006/09/17 16:45:52 wolti Exp $
 */

#ifndef _PORT_H
#define _PORT_H

/* ----------------------- Platform includes --------------------------------*/
/* ----------------------- Defines ------------------------------------------*/
#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

#define ENTER_CRITICAL_SECTION( )   __disable_interrupt();
#define EXIT_CRITICAL_SECTION( )    __enable_interrupt();
/*
#define assert( x )

typedef char    BOOL;

typedef unsigned char UCHAR;
typedef char    CHAR;

typedef unsigned short USHORT;
typedef short   SHORT;

typedef unsigned long ULONG;
typedef long    LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif
*/

/* ----------------------- AVR platform specifics ---------------------------*/
#if defined(__ATmega8__)
	#include "at8.h"
#elif defined(__ATmega16__)
	#include "at16.h"
#elif defined(__ATmega328P__)
	#include "at328p.h"
#endif

/* ----------------------- RS485 specifics ----------------------------------*/
#define RTS_ENABLE

#ifdef  RTS_ENABLE

    #define RTS_PIN         PB0
    #define RTS_DDR         DDRB
    #define RTS_PORT        PORTB
    
    #define RTS_INIT        \
        do { \
            RTS_DDR |= (1<<RTS_PIN); \
            RTS_PORT &= ~(1<<RTS_PIN); \
        } while( 0 );
    
    #define RTS_HIGH        \
        do { \
            RTS_PORT |= (1<<RTS_PIN); \
        } while( 0 );
    
    #define RTS_LOW         \
        do { \
            RTS_PORT &= ~(1<<RTS_PIN); \
        } while( 0 );

#endif

#endif	/* _PORT_H */
