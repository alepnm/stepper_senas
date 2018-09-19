
/*******************************************************************************
* File Name: UART.c  
* Version 1.1
*
* Description:
*  This file includes function of speed close loop control.
*
* Note:
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

#include <device.h>
#include "UART.h"
#include "bool.h"
#include "UI_paras.h"
#include "stepper.h"
#include "userinterface.h"

uint8 RxFlag =0;
uint8 bData =0;
uint8 PkgInCnt =0;

extern uint8   CtrlStatus;
extern uint16  TotalCnt;
extern uint16  SpeedFrq;
extern uint8   MenuState;
extern uint8 LineWaitFlag;
extern UI_CMD  UI_Cmd;
extern UI_DATA UI_Data;

void RxData(void)
{ 

    static uint8 index = 0;
    static uint8 PkgInCnt = 0;
    static uint8 pkgReadDone = 0;
    static uint8 size = 0;
    uint8 pkg_a[RX_BUF_SIZE];

    uint8 *pkg = pkg_a;

  
    if(RxFlag)
    {
        bData = UART_1_ReadRxData();		/* read the data */
  
            if(bData == 0xAA && PkgInCnt < 2)
            {
                /* sync header */
                PkgInCnt++;
            }
            else if(PkgInCnt == 2 || PkgInCnt == 3)
            {
                /* command and type */
                pkg[index++] = bData;
                PkgInCnt++;
            }
            else if(PkgInCnt == 4)
            {
                /* Data Length*/
                pkg[index++] = bData;
                PkgInCnt++;
                if(bData == 0)
                {
                    pkgReadDone = 1;
                    PkgInCnt = 0;
                    index = 0;
                }
                else
                {
                    size = bData;    
                }
            }
            else if(PkgInCnt > 4 && ((PkgInCnt - 1) <= RX_BUF_SIZE))
            {   
                pkg[index++] = bData;
                PkgInCnt++; 
                size--;
                if(size == 0)
                {
                    pkgReadDone = 1;
                    PkgInCnt = 0;
                    index = 0;
                }
            }
            else
            {
                
                pkgReadDone = 0;
                PkgInCnt = 0;
                size = 0;
                index = 0;
            }           
            
            if(pkgReadDone)
            {
                switch(pkg[0])
                {   
                    case SET_MICROSTEP:		/* L1 Start Speed in Period Value */
                        SEND_ACK();
                        UI_Data.MicroStepPace = pkg[3];
                        break;
                    case SET_LINE_START:	/* L1 Start Speed in Period Value */
                        SEND_ACK();
                        UI_Data.LineCmdIndex = pkg[3];
                        UI_Data.LineStart[UI_Data.LineCmdIndex] =  (uint16)pkg[4]<<8;
                        UI_Data.LineStart[UI_Data.LineCmdIndex] += (uint16)pkg[5];
                        break;
                    case SET_LINE_END:
                        SEND_ACK();
                        UI_Data.LineCmdIndex = pkg[3];
                        UI_Data.LineEnd[UI_Data.LineCmdIndex] =  (uint16)pkg[4]<<8;
                        UI_Data.LineEnd[UI_Data.LineCmdIndex] += (uint16)pkg[5];
                        break;
                    case SET_LINE_CNT:
                        SEND_ACK();
                        UI_Data.LineCmdIndex = pkg[3];
                        UI_Data.LineCnt[UI_Data.LineCmdIndex] =  (uint16)pkg[4]<<8;
                        UI_Data.LineCnt[UI_Data.LineCmdIndex] += (uint16)pkg[5];
                        break;
                    case SET_LINE_WAIT_CNT:
                        SEND_ACK();
                        UI_Data.LineCmdIndex = pkg[3];
                        UI_Data.LineWaitCnt[UI_Data.LineCmdIndex] =  (uint16)pkg[4]<<8;
                        UI_Data.LineWaitCnt[UI_Data.LineCmdIndex] += (uint16)pkg[5];
                        break;
                    case SET_LINE_TOTAL_CNT:
                        SEND_ACK();
                        UI_Data.TotalLineNumber = pkg[3];                        
                    case GET_TYPE:
                        UART_1_WriteTxData(SYNC_HEADER); 
                        UART_1_WriteTxData(SYNC_HEADER); 
                        UART_1_WriteTxData(ACK); 
                        UART_1_WriteTxData(pkg[1]); 
                        UART_1_WriteTxData(0x01); /* Size */
                        UART_1_WriteTxData(0x00); /* Stepper */
                        break;                        
                    case SET_DIR:
                        SEND_ACK();
                        UI_Data.Dir = pkg[3];
                        break;                        
                    case SET_CURR_GAIN:
                        SEND_ACK();
                        UI_Data.PhaseCurr = pkg[3]; 
                        if((UI_Data.PhaseCurr<=CURR_HIGH)&&(UI_Data.PhaseCurr>=CURR_LOW))
                        {
                          SetMaxCurr(UI_Data.PhaseCurr);
                        }
                        break;   
                    case GET_SPEED:
                        UART_1_WriteTxData(SYNC_HEADER); 
                        UART_1_WriteTxData(SYNC_HEADER); 
                        UART_1_WriteTxData(ACK); 
                        UART_1_WriteTxData(pkg[1]); 
                        UART_1_WriteTxData(0x02); 
						UART_1_WriteTxData(0x00);
                        UART_1_WriteTxData(0x00);                    
                    case MOTOR_RUN:
                        SEND_ACK(); 
                        UI_Cmd.Run = TRUE;
                        UI_Data.LineIndex = 0; /* start from line1*/
                        SpeedFrq = UI_Data.LineStart[0];
                        TotalCnt = 0;
                        LineWaitFlag = FALSE;  /* Start with constant speed */ 
                        break;  
                    case MOTOR_STOP:
                        SEND_ACK();
                        UI_Cmd.Run = FALSE;
                        SpeedFrq = UI_Data.LineStart[0];
                        TotalCnt = 0;
                        break;                        
                    case CONN:
                        SEND_ACK(); 
                        CtrlStatus = GUI_CONTROL;
                        UI_Cmd.Run = FALSE;
                        break;                        
                    case DISCONN:
                        SEND_ACK(); 
                        CtrlStatus = UI_CONTROL;
                        UI_Cmd.Run = FALSE;
                        SpeedFrq = 150;
                        MenuState = MENU_READY;
                        break;
                    default:
                        SEND_NACK();
                        break;                    
                }
                pkgReadDone = 0;
            }
    }    
}



