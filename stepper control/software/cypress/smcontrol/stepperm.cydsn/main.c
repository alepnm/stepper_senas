/*******************************************************************
*  File Name: main.c  
*  Version 1.2
*
*  Description: Stepper Motor Control. 
*               Support up to 128 micro-steps. 
*               Current is adjustable
*               LCD for UI
*               GUI Interface
*  Revision:   Feb 20 2010, Update UI interface.
*
*  Author :  Dino Gu, Cypress APSC (Shanghai)
*  Original Date :    Sept,10, 2009
*  Original Version:  V1.0
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
#include "stepper.h"
#include "UI_paras.h"
#include "bool.h"
#include "UART.h"
#include "userinterface.h"

uint16  SpeedFrq=150;
uint8   StepEnable = FALSE;
uint8   CtrlStatus = UI_CONTROL; /* Default to UI control Method */

UI_CMD          UI_Cmd;
UI_DATA         UI_Data;

extern STEPPER  Sm;
uint8  SpdRPS =0;
uint16 SpeedFrq0,SpeedFrq1;
uint16 DeltCnt=0;
uint16 ChangePrdThreshold=0;
uint8  SpeedUpFlag;
uint16 TotalCnt=0;
uint8  FastAcc = FALSE;
uint16 DeltSpeedFrq =0;
uint16 PWMCnt =0;
extern uint8  MenuState;
extern uint16 SliderCnt;

extern uint8 S1NextState;
extern uint8 S2NextState;
extern uint8 PotNextState;
extern uint8 TimerNextState;

void CalcTimerPeriod(void);
/*******************************************************************************
* Function Name: Main
********************************************************************************
*
* Summary:
*  Main Function
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void main()
{

	uint16 PrdTmp = 0;
	uint8  k;

    /* Set default micro-step pace */
    UI_Data.MicroStepPace = 16;

    /* Initialize GUI line commands */
    for(k=0;k<10;k++)
    {
        UI_Data.LineStart[k] = 0;
        UI_Data.LineEnd[k] = 0;
        UI_Data.LineCnt[k] = 0;
        UI_Data.LineWaitCnt[k] = 0;
    }
    UI_Data.LineIndex =0;
    
  
    /* Start PWM */
	PWM_A_Start();
	PWM_B_Start();
    PWM_A_WriteCompare(255);
    PWM_B_WriteCompare(255);
    
    /* PGA Initialize */
    PGA_1_Start();
    PGA_2_Start();
    PGA_1_SetGain(PGA_1_GAIN_32); 	/* Medium current level */
    PGA_2_SetGain(PGA_1_GAIN_32);
    
    /* Comparator and DAC Initialize */
    Comp_A_Start();
    Comp_B_Start();
   	VDAC8_IrefA_Start();
    VDAC8_IrefB_Start();
    
    /* UART Initialize */
    UART_1_EnableRxInt();
	UART_1_Start();
    
    /* Timer Initialize */
	Timer_1_Start();
    Timer_1_WritePeriod(SpeedFrq*128);	/* 120 = Full step is 250rpm */
	isr_timer_Start();
    isr_pwm_Start();
    
    /* Motor Initialization */
    Control_Start_Write(0);
	UI_Data.Dir = TRUE;
    	
	CYGlobalIntEnable;

	/* LCD and UI Initialization */
	LCD_Start();
	ADC_DelSig_1_Start();
	MenuState = MENU_READY;
	S1NextState = MENU_IGNORE_INPUT;
	S2NextState = MENU_IGNORE_INPUT;
    PotNextState = MENU_IGNORE_INPUT;
	TimerNextState = MENU_IGNORE_INPUT;
	
	/* main loop */
    for(;;)
    {        
        if(SpeedFrq!=0)
		{
			Timer_1_WritePeriod(SpeedFrq);	/* update step rate */
		} 
		if(CtrlStatus != GUI_CONTROL)		/* Update UI Control */
		{
			if(PWMCnt++ >= 4000)
			{  
				PWMCnt=0;
				InterfaceProcess();
			}
		}
		else								/* Update GUI Control */
        {
			CalcTimerPeriod();
        }
              

        if(UI_Cmd.Run)						/* Start Motor */
        {
          	Control_Start_Write(1);			
          	StepEnable = TRUE;
        }
        else								/* Stop motor */
        {
           	StepEnable = FALSE; 
           	Stopping();
        } 
    }
}





/*******************************************************************************
* Function Name: SetMaxCurr
********************************************************************************
*
* Summary:
*  SetMaxCurr Function updates the VDAC current based on motor rated current. 
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void SetMaxCurr(uint8 level)
{
            if(level==CURR_LOW)
            {

                    PGA_1_SetGain(PGA_1_GAIN_48); /* 16 => 1.5A Ipeak. DAC rang is 1V. */
                    PGA_2_SetGain(PGA_1_GAIN_48);
             }
             else if (level==CURR_MEDIUM)
             {

                    PGA_1_SetGain(PGA_1_GAIN_32); /* 16 => 1.5A Ipeak. DAC rang is 1V. */
                    PGA_2_SetGain(PGA_1_GAIN_32);
             }
             else if(level==CURR_HIGH)
             {  

                    PGA_1_SetGain(PGA_1_GAIN_16); /* 16 => 1.5A Ipeak. DAC rang is 1V. */
                    PGA_2_SetGain(PGA_1_GAIN_16);
             }
}
/*******************************************************************************
* Function Name: CalcTimerPeriod
********************************************************************************
*
* Summary:
*  CalcTimerPeriod Function calculates delta period of every step according to input 
*  command. 
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void CalcTimerPeriod(void)
{

    SpeedFrq0 = UI_Data.LineStart[UI_Data.LineIndex];
    SpeedFrq1 = UI_Data.LineEnd[UI_Data.LineIndex];
   
   	/* Speed Up */
    if(SpeedFrq0 > SpeedFrq1)
    {   
        SpeedUpFlag = TRUE;
        /* Decrease period value every n steps */
        if(UI_Data.LineCnt[UI_Data.LineIndex] >= (SpeedFrq0 - SpeedFrq1) )
        {
        	ChangePrdThreshold = UI_Data.LineCnt[UI_Data.LineIndex]/(SpeedFrq0 - SpeedFrq1);
           	FastAcc = FALSE;
        }
        /* Decrease period value every step */
        else
        {
        	DeltSpeedFrq = (SpeedFrq0 - SpeedFrq1)/UI_Data.LineCnt[UI_Data.LineIndex];
           	FastAcc = TRUE;
        }
    }
    /* Speed Down*/
    else if(SpeedFrq0 < SpeedFrq1)
    {   
        SpeedUpFlag = FALSE;
        /* Increase period value every n steps */
        if(UI_Data.LineCnt[UI_Data.LineIndex] >= (SpeedFrq1 - SpeedFrq0) )
        {
        	ChangePrdThreshold = UI_Data.LineCnt[UI_Data.LineIndex]/(SpeedFrq1 - SpeedFrq0);
           	FastAcc = FALSE;
        }
        /* Increase period value every step */
        else
        {
           	DeltSpeedFrq = (SpeedFrq1 - SpeedFrq0)/UI_Data.LineCnt[UI_Data.LineIndex];
           	FastAcc = TRUE;
        }         
    } 
}

/* [] END OF FILE */