/*******************************************************************
*  File Name: userinterface.c  
*  Version 1.1
*
*  Description: User interface LCD based menu and event handler
*  Date :    Feb,10, 2010
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
#include "UI_paras.h"
#include "bool.h"
#include "stepper.h"
#include "userinterface.h"



int16 PotValue = 0;
uint8 PotValuePrevious;
uint8 PotCounter;
uint8 PotSample[3];


uint16 TempSetting;
uint8 SliderCnt;

uint8	MenuState = MENU_READY;
uint8	S1NextState = MENU_IDLE;
uint8	S2NextState = MENU_IDLE;
uint8	PotNextState = MENU_IDLE;
uint8	TimerNextState = MENU_IDLE;

uint8	TimerThreshold;
uint8	TimerValue;

uint8 S1State = FALSE;
uint8 S1Pressed = FALSE;
uint8 S1DebounceCount = FALSE;

uint8 S2State = FALSE;
uint8 S2Pressed = FALSE;
uint8 S2DebounceCount = FALSE;

uint16 Result;


extern UI_CMD UI_Cmd;
extern UI_DATA UI_Data;
extern uint16 wSpdRef;
extern uint16 wSpdCur;

extern uint8 ControlMode;


extern uint8  SpdRPS;

extern STEPPER Sm;

extern uint16 SpeedFrq;
/*******************************************************************************
* Function Name: UpdateButtonState
********************************************************************************
*
* Summary:
*  Updates the LCD screen with the current button state by displaying and empty
*  box if inactive or a filled-in box if active. LED's are also updated 
*  according to button state.
*
* Parameters:
*  slot_1: Button state for B1
*  slot_2: Button state for B2
*
* Return:
*  void
*
*******************************************************************************/
void InterfaceProcess(void)
{
	
		/* Read and Debounce switches */		
		if(!S1State)
		{
			if(!Switch1_Read())
			{
				S1DebounceCount++;
				if(S1DebounceCount > 2)
				{
					S1DebounceCount = 0;
					S1Pressed = TRUE;
					S1State = TRUE;
				}
			}
		}
		else
		{
			if(Switch1_Read())
			{
				S1DebounceCount++;
				if(S1DebounceCount > 2)
				{
					S1DebounceCount = 0;
					S1State = FALSE;
				}
			}
		}
		
		if(!S2State)
		{
			if(!Switch2_Read())
			{
				S2DebounceCount++;
				if(S2DebounceCount > 2)
				{
					S2DebounceCount = 0;
					S2Pressed = TRUE;
					S2State = TRUE;
				}
			}
		}
		else
		{
			if(Switch2_Read())
			{
				S2DebounceCount++;
				if(S2DebounceCount > 2)
				{
					S2DebounceCount = 0;
					S2State = FALSE;
				}
			}
		}
		
		
		/* Read Potentiometer */
		ADC_DelSig_1_StartConvert();
		ADC_DelSig_1_IsEndConversion(ADC_DelSig_1_WAIT_FOR_RESULT);
		PotValue = ADC_DelSig_1_GetResult16() >> 1;
        
        /* Limit and Filter Potentiometer value */
		if(PotValue < 5)
		{
			PotValue = 5;
		}
		if(PotValue > 205)
		{
			PotValue = 205;
		}
	
	
		TimerValue++;
		
		
		if(S1Pressed)
		{
			S1Pressed = FALSE;
			if(S1NextState != MENU_IGNORE_INPUT)
			{	
				MenuState = S1NextState;
			}
		}
		else if(S2Pressed)
		{
			S2Pressed = FALSE;
			if(S2NextState != MENU_IGNORE_INPUT)
			{	
				MenuState = S2NextState;
			}
		}
		else if(PotValue != PotValuePrevious)
		{
			PotValuePrevious = PotValue;
			if(PotNextState != MENU_IGNORE_INPUT)
			{	
				MenuState = PotNextState;
			} 
		}
		else if(TimerValue >= TimerThreshold)
		{
			TimerValue = 0;
			if(TimerNextState != MENU_IGNORE_INPUT)
			{	
				MenuState = TimerNextState;
			} 
		}
		
		
		
		switch(MenuState)
		{
			/* Placeholder case to use afer executing a do once menu case */
			case MENU_IDLE:
				break;
				
			/* Initial Menu page and Running page */
			case MENU_READY:
				UI_Cmd.Run = 0;
				LCD_Position(0,0);
            	LCD_PrintString("Microstepper    ");
				LCD_Position(1,0);
            	LCD_PrintString("S1=Menu  S2=Run ");
				MenuState = MENU_IDLE;
				S1NextState = MENU_DIRECTION;
				S2NextState = MENU_RUN;
				PotNextState = MENU_IGNORE_INPUT;
				TimerNextState = MENU_IGNORE_INPUT;
				break;
			case MENU_RUN:
				UI_Cmd.Run = 1;
				LCD_Position(0,0);
				LCD_PrintString("CMD=     Running");
				LCD_Position(1,0);
				LCD_PrintString("RPS=     S2=Stop");
				MenuState = MENU_RUN_UPDATE;
				S1NextState = MENU_IDLE;
				S2NextState = MENU_READY;
				TimerThreshold = 8;
				TimerNextState = MENU_RUN_UPDATE;
				break;
			case MENU_RUN_UPDATE:
				SpeedFrq = (PotValue + 25);//;
				SpdRPS = (100000/(uint16)SpeedFrq) / (((uint16)128 * 200) / (uint16)UI_Data.MicroStepPace);
				LCD_Position(0,4);
				LCD_PrintString("     ");
				LCD_Position(0,4);
				LCD_PrintNumber(SpeedFrq);
				LCD_Position(1,4);
				LCD_PrintString("     ");
				LCD_Position(1,4);
				LCD_PrintNumber(SpdRPS);
				break;
			
			/* Rotation Direction menu page */
			case MENU_DIRECTION:
				LCD_Position(0,0);
				LCD_PrintString("Direction       ");
				LCD_Position(1,0);
				LCD_PrintString("S1=Next  S2=Sel ");
				MenuState = MENU_IDLE;
				S1NextState = MENU_CURRENT;
				S2NextState = MENU_DIRECTION_SELECT;
				PotNextState = MENU_IGNORE_INPUT;
				break;
			case MENU_DIRECTION_SELECT:
				LCD_Position(0,0);
				LCD_PrintString("Dir=            ");
				LCD_Position(1,0);
				LCD_PrintString("S1=Exit  S2=Set ");
				MenuState = MENU_DIRECTION_UPDATE;
				S2NextState = MENU_DIRECTION_SET;
				PotNextState = MENU_DIRECTION_UPDATE;
				break;	
			case MENU_DIRECTION_UPDATE:
				LCD_Position(0,4);
				if(PotValue > 100)
				{
					LCD_PrintString("Forward");
					TempSetting = TRUE;
				}
				else
				{
					LCD_PrintString("Reverse");
					TempSetting = FALSE;
				}
				MenuState = MENU_IDLE;
				break;
			case MENU_DIRECTION_SET:
					UI_Data.Dir = TempSetting;
					MenuState = MENU_CURRENT;
				break;
			
			/* Max Current menu page */
			case MENU_CURRENT:
				LCD_Position(0,0);
				LCD_PrintString("Phase Current   ");
				LCD_Position(1,0);
				LCD_PrintString("S1=Next  S2=Sel ");
				MenuState = MENU_IDLE;
				S1NextState = MENU_STEPS;
				S2NextState = MENU_CURRENT_SELECT;
				PotNextState = MENU_IGNORE_INPUT;
				break;
			case MENU_CURRENT_SELECT:
				LCD_Position(0,0);
				LCD_PrintString("Iphase=         ");
				LCD_Position(1,0);
				LCD_PrintString("S1=Exit  S2=Set ");
				MenuState = MENU_CURRENT_UPDATE;
				S2NextState = MENU_CURRENT_SET;
				PotNextState = MENU_CURRENT_UPDATE;
				break;	
			case MENU_CURRENT_UPDATE:
				LCD_Position(0,7);
				if(PotValue < 66)
				{
					LCD_PrintString("1.0A");
					TempSetting = CURR_LOW;
				}
				else if(PotValue > 133)
				{
					LCD_PrintString("2.0A");
					TempSetting = CURR_HIGH;
				}
				else
				{
					LCD_PrintString("1.5A");
					TempSetting = CURR_MEDIUM;
				}
				MenuState = MENU_IDLE;
				break;
			case MENU_CURRENT_SET:
				UI_Data.PhaseCurr = TempSetting;
				SetMaxCurr(TempSetting);
				MenuState = MENU_STEPS;
				break;
		
			/* Microsteppes menu page */
			case MENU_STEPS:
				LCD_Position(0,0);
				LCD_PrintString("Micro Steps     ");
				LCD_Position(1,0);
				LCD_PrintString("S1=Next  S2=Sel ");
				MenuState = MENU_IDLE;
				S1NextState = MENU_DEFAULT;
				S2NextState = MENU_STEPS_SELECT;
				PotNextState = MENU_IGNORE_INPUT;
				break;
			case MENU_STEPS_SELECT:
				LCD_Position(0,0);
				LCD_PrintString("uSteps=         ");
				LCD_Position(1,0);
				LCD_PrintString("S1=Exit  S2=Set ");
				MenuState = MENU_STEPS_UPDATE;
				S2NextState = MENU_STEPS_SET;
				PotNextState = MENU_STEPS_UPDATE;
				break;	
			case MENU_STEPS_UPDATE:
				LCD_Position(0,7);
				if(PotValue < 40)
				{
					LCD_PrintString("Full");
					TempSetting = 128;
					Sm.MicroStepDivider = 1;
				}
				else if(PotValue < 80)
				{
					LCD_PrintString("4   ");
					TempSetting = 32;
					Sm.MicroStepDivider = 8;
				}
				else if(PotValue < 120)
				{
					LCD_PrintString("16  ");
					TempSetting = 8;
					Sm.MicroStepDivider = 32;
				}
				else if(PotValue < 160)
				{
					LCD_PrintString("32  ");
					TempSetting = 4;
					Sm.MicroStepDivider = 64;
				}
				else
				{
					LCD_PrintString("128 ");
					TempSetting = 1;
					Sm.MicroStepDivider = 129;
				}
				MenuState = MENU_IDLE;
				break;
			case MENU_STEPS_SET:
				UI_Data.MicroStepPace = TempSetting;
				SpeedFrq = 150;		/* Prepare for re-starting with slow speed */
                Timer_1_WritePeriod(SpeedFrq);
				MenuState = MENU_DEFAULT;
				break;

					
			/* Reset Defaults menu page */
			case MENU_DEFAULT:
				LCD_Position(0,0);
				LCD_PrintString("Reset Defaults  ");
				LCD_Position(1,0);
				LCD_PrintString("S1=Next S2=RESET");
				MenuState = MENU_IDLE;
				S1NextState = MENU_READY;
				S2NextState = MENU_DEFAULT_SET;
				PotNextState = MENU_IGNORE_INPUT;
				break;
			case MENU_DEFAULT_SET:
				/* reset all variables */
				UI_Data.Dir = TRUE;
				UI_Data.MicroStepPace =16;
				UI_Data.PhaseCurr = 16;
 				PGA_1_SetGain(PGA_1_GAIN_32); /*Medium current level*/
    			PGA_2_SetGain(PGA_1_GAIN_32);
				MenuState = MENU_READY;
				break;
		 
			default: 
				break;
		}   //End switch
}








