/*
 * FreeModbus Libary: LPC214X Port
 * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
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
 * File: $Id: porttimer.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include <project.h>
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"


/* Statics */
static USHORT usT35TimeOut50us;

/* ----------------------- static functions ---------------------------------*/
CY_ISR_PROTO(prvvTIMERExpiredISR);

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTimeOut50us )
{
    /* backup T35 ticks */
    usT35TimeOut50us = usTimeOut50us;
    
    mbPortTimerISR_StartEx(prvvTIMERExpiredISR);
    mbPortTimerISR_ClearPending();
    
    vMBPortTimersDisable();

    return TRUE;
}


void vMBPortTimersEnable( )
{
    //Status_LED_Write( TRUE );
    
    mbPortTimer_Start();
    
    mbPortTimer_WriteCounter(0);
    mbPortTimer_WritePeriod(usT35TimeOut50us);
    mbPortTimer_SetPrescaler(mbPortTimer_PRESCALE_DIVBY64);
    
    mbPortTimerISR_Enable(); 
}

void vMBPortTimersDisable( )
{
    mbPortTimerISR_Disable();    
    mbPortTimer_Stop();
}

CY_ISR(prvvTIMERExpiredISR)
{
    //Status_LED_Write( FALSE );
    
    mbPortTimer_ClearInterrupt(mbPortTimer_INTR_MASK_TC);
    
    mbPortTimerISR_ClearPending();
    mbPortTimerISR_Disable();
    
	( void )pxMBPortCBTimerExpired();
}

