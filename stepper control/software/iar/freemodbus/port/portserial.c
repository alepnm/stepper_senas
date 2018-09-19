/*
 * FreeModbus Libary: ATMega168 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *   - Initial version and ATmega168 support
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
 * File: $Id: portserial.c,v 1.6 2006/09/17 16:45:53 wolti Exp $
 */

#include "port.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"

#if !defined (USE_MODBUS)
    #warning PortSerial interaptai isjungti! Ijunkite #define USE_MODBUS faile "mb.h" \
    jai naudojate Modbus funkcionala!
#endif


#define UART_BAUD_RATE          9600
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) \
    ( ( F_OSC ) / ( ( UART_BAUD_RATE ) * 16UL ) - 1 )


void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
#ifdef RTS_ENABLE
    MB_UCSRxB |= (1<<MB_TXENx)|(1<<MB_TXCIEx);
#else
    MB_UCSRxB |= (1<<MB_TXENx);
#endif

    if( xRxEnable )
    {
        MB_UCSRxB |= (1<<MB_RXENx)|(1<<MB_RXCIEx);
    }
    else
    {
        MB_UCSRxB &= ~((1<<MB_RXENx)|(1<<MB_RXCIEx));
    }

    if( xTxEnable )
    {
        MB_UCSRxB |= (1<<MB_TXENx)|(1<<MB_UDRIEn);
#ifdef RTS_ENABLE
        RTS_HIGH;
#endif
    }
    else
    {
        MB_UCSRxB &= ~(1<<MB_UDRIEn);
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    UCHAR ucUCSRC = 0;

    /* prevent compiler warning. */
    (void)ucPORT;
	
    /*UBRR = UART_BAUD_CALC( ulBaudRate, F_CPU );*/
    MB_UBRRn = UART_BAUD_CALC( ulBaudRate, F_CPU );

    switch ( eParity )
    {
        case MB_PAR_EVEN:
            ucUCSRC |= (1<<MB_UPMx1);
            break;
        case MB_PAR_ODD:
            ucUCSRC |= (1<<MB_UPMx1)|(1<<MB_UPMx0);
            break;
        case MB_PAR_NONE:
            break;
    }

    switch ( ucDataBits )
    {
        case 8:
            ucUCSRC |= (1<<MB_UCSZx0)|(1<<MB_UCSZx1);
            break;
        case 7:
            ucUCSRC |= (1<<MB_UCSZx1);
            break;
    }
	
#if defined (__ATmega8__)
	MB_UCSRxC = (1<<MB_URSEL) | ucUCSRC;
#elif defined (__ATmega328P__)
    MB_UCSRxC |= ucUCSRC;
#endif

    vMBPortSerialEnable( FALSE, FALSE );

#ifdef RTS_ENABLE
    RTS_INIT;
#endif
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    MB_UDRx = ucByte;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = MB_UDRx;
    return TRUE;
}


#if defined (USE_MODBUS)
    #pragma vector=SIG_USART_DATA
    __interrupt void USART_RXC_Handler()
    {
        pxMBFrameCBTransmitterEmpty(  );
    }    
    
    #pragma vector=SIG_USART_RECV
    __interrupt void USART_Rec_Handler()
    {
        pxMBFrameCBByteReceived(  );	// -> xMBRTUReceiveFSM()
    }

    #ifdef RTS_ENABLE
        #pragma vector=SIG_USART_TRANS
        __interrupt void USART_TXC_Handler()
        {
            RTS_LOW;
        }
    #endif
#endif

