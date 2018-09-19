/*******************************************************************
*  File Name: stepper.c  
*  Version 1.1
*
*  Description: The file include two functions, one is stepping, the other is stopping.
*  Hardware: DVK Rev4, Drive Borad V1.0 
*  Author :  Dino Gu, Cypress APSC 
*  Date :    Sept,10, 2009
*
*  Note:
**********************************************************************
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
#include "bool.h"
#include "stepper.h"
#include "UI_paras.h"

/* 128 sinusoid table */
static unsigned char SinTable[] = {
 255,255,255,255,255,254,254,254,253,253,
 253,252,252,251,251,250,249,249,248,247,
 247,246,245,244,243,242,241,240,239,238,
 237,236,234,233,232,231,229,228,226,225,
 223,222,220,219,217,215,214,212,210,208,
 207,205,203,201,199,197,195,193,191,189,
 187,185,183,180,178,176,174,171,169,167,
 164,162,159,157,154,152,149,147,144,142,
 139,136,134,131,128,126,123,120,117,115,
 112,109,106,103,100,98,95,92,89,86,
 83,80,77,74,71,68,65,62,59,56,
 53,50,47,44,41,37,34,31,28,25,
 22,19,16,13,9,6,3,0
 };


STEPPER Sm = STEPPER_DEFAULT;
extern UI_DATA UI_Data;
uint8  CurrRef;

/*******************************************************************************
* Function Name: Stepping
********************************************************************************
*
* Summary:
*  Step function runs motor one step. This function is called in Timer ISR.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Stepping(void)
{
   if(Sm.MicroStepPointer == 0)
   {    
        Sm.MicroStepPace = UI_Data.MicroStepPace;
      
        Control_Reg_1_Write(Sm.StageIndex); 
     
        if(UI_Data.Dir)
        {
            Sm.StageIndex++;
            if(Sm.StageIndex>3)
            {
              Sm.StageIndex=0;
            } 
        }
        else
        {
            Sm.StageIndex--;
            if(Sm.StageIndex>3)
            {
               Sm.StageIndex=3;
            } 
        }             
    }
     
    if(UI_Data.Dir)
    {
        if (!(Sm.StageIndex%2))		/* Current Up */
        Sm.SinTblPointer = Sm.MicroStepPointer;
        else  						/* Current down */
        Sm.SinTblPointer = TABLETOP - Sm.MicroStepPointer; 
    }
    else
    {
        if (Sm.StageIndex%2)		/* Current Up */
        Sm.SinTblPointer = Sm.MicroStepPointer;
        else  						/* Current down */
        Sm.SinTblPointer = TABLETOP - Sm.MicroStepPointer;
    }
            
    Sm.CosTblPointer = TABLETOP - Sm.SinTblPointer;

    CurrRef = SinTable[Sm.SinTblPointer];
    VDAC8_IrefA_SetValue(CurrRef);
    
    CurrRef = SinTable[Sm.CosTblPointer];
   
    VDAC8_IrefB_SetValue(CurrRef);
    
    Sm.MicroStepPointer += Sm.MicroStepPace;
    Sm.MicroStepPointer &= 0x7F; 
    
}

/*******************************************************************************
* Function Name: Stopping
********************************************************************************
*
* Summary:
*  Stop function stops motor and reduce its current for power saving.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Stopping(void)
{   
    /* Reset index to zero*/
    Sm.StageIndex = 0;

    CurrRef = SinTable[Sm.SinTblPointer];
    CurrRef >>=2;		/* Reduce to 1/4 of normal value */
    VDAC8_IrefA_SetValue(CurrRef);
    
    CurrRef = SinTable[Sm.CosTblPointer];
    CurrRef >>=2;		/* Reduce to 1/4 of normal value */
    VDAC8_IrefB_SetValue(CurrRef);
}
