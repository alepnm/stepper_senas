/*
 * FreeModbus Libary: ATMega168 Port
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
 * File: $Id: porttimer.c,v 1.4 2006/09/03 11:53:10 wolti Exp $
 */

/* ----------------------- AVR includes -------------------------------------*/
/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
//#include "mbport.h"

#if !defined (USE_MODBUS)
    #warning PortTimer interaptai isjungti! Ijunkite #define USE_MODBUS faile "mb.h" \
    jai naudojate modbus funkcionala!
#endif

/* ----------------------- Defines ------------------------------------------*/
#define MB_TIMER_PRESCALER      ( 128UL )
#define MB_TIMER_TICKS          ( F_CPU / MB_TIMER_PRESCALER )
#define MB_50US_TICKS           ( 20000UL )

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usTimerOCRDelta = 0;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    /* Calculate overflow counter an OCR values for Timer1. */
    usTimerOCRDelta = (MB_TIMER_TICKS * usTim1Timerout50us)/(MB_50US_TICKS);

    MB_TCCRxB = 0x00;

    vMBPortTimersDisable(  );

    return TRUE;
}


void
vMBPortTimersEnable(  )
{
    MB_TCNTx = 0x0000;
    
    if( usTimerOCRDelta > 0 )
    {
        /* Enable the output compare interrupts */
        MB_TIMSKx |= (1<<MB_OCIEx);
        MB_OCRx = usTimerOCRDelta;
    }

    /* Enable the timer */
    MB_TCCRxB |= ((1<<MB_CSx2) | (1<<MB_CSx0));
}

void
vMBPortTimersDisable(  )
{
    /* Disable the timer. */
    MB_TCCRxB &= ~( (1<<MB_CSx2) | (1<<MB_CSx0) );
    /* Disable the output compare interrupts for channel A/B. */
    MB_TIMSKx &= ~(1<<MB_OCIEx);
    /* Clear output compare flags for channel A/B. */
    MB_TIFRx |= (1<<MB_OCFx) ;
}


#if defined (USE_MODBUS)
    #pragma vector=SIG_OUTPUT_COMPARE
    __interrupt void TMR2_COMP_Handler()
    {
        ( void )pxMBPortCBTimerExpired(  );
    }
#endif

