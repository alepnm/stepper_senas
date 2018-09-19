/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
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
 * File: $Id: port.h,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

#ifndef _PORT_H
#define _PORT_H

#include <CyLib.h>

//#define	INLINE
#ifdef __cplusplus
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }
#endif
    
#define SLAVE_RS485_SEND_MODE       DE_Write(PIN_HIGH);
#define SLAVE_RS485_RECEIVE_MODE    DE_Write(PIN_LOW);
    
#define ENTER_CRITICAL_SECTION() disable_interrupts()
#define EXIT_CRITICAL_SECTION( ) restore_interrupts()
    
typedef uint8_t         BOOL;
typedef unsigned char   UCHAR;
typedef char            CHAR;
typedef uint16_t        USHORT;
typedef int16_t         SHORT;
typedef uint32_t        ULONG;
typedef int32_t         LONG;

#ifndef TRUE
    #define TRUE        (1u)
#endif

#ifndef FALSE
    #define FALSE       (0u)
#endif


static uint8_t irq_state;

/* Function prototypes */

/* DISABLE INTERRUPTS  */
static inline void disable_interrupts( void )
{
    irq_state = CyEnterCriticalSection();
}

/* RESTORE INTERRUPTS */
static inline void restore_interrupts( void )
{
    CyExitCriticalSection(irq_state);    
}


#endif
