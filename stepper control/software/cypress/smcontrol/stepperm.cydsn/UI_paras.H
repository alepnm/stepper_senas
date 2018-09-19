/*******************************************************************************
* File Name: UI_paras.h  
* Version 1.1
*
*  Description:
*    This file contains the function prototypes and constants used in
*    the  User Interface
*
*   Note:
*     
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


/* LED specific */
#define LED_ON  				1 
#define LED_OFF 				0 
/* Status specific*/
#define GUI_CONTROL             1 
#define UI_CONTROL        		0 

/*Command from UI*/
typedef struct
{
    uint8  Run; /* Set motor running*/
    
} UI_CMD;


/*Data passed between UI and MC controller*/
typedef struct
{ 
   uint8   MotorType; 		/*Define Motor Type*/
   uint8   Dir; 			/*Direction*/
  
   uint16  SpeedRpm; 		/* Actural motor speed*/
   uint16  SpeedRpmRef; 	/* motor speed command value*/
   
   uint16  Kp;   			/* Proportional coeffient of PID*/
   uint16  Ki;   			/* Integral coeffient of PID*/
   
   uint16  MaxSpeedRpm;  	/* Motor parameters*/
   uint16  MinSpeedRpm;  	/* Motor parameters*/ 
   uint8   Poles;        	/*POLE PAIRS*/
   uint8   MaxCurr;     	/*Over current threshold*/
   
   uint8   K_RampUp;     	/*Coeffieient of RampUp*/
   uint8   PWM_RampUp;   	/*PWM duty in init RamUP*/
   uint8   CloseLoopWait; 	/* wait cycles from openloop to closeloop*/
   uint8   BlankCnt;      	/*Blank count number*/
   int8    RisingCompensation; /*Compensation in rise edge*/
   int8    FallingCompensation; /*Compensation in falling edge*/
   
   uint8   MicroStepPace;  	/*For Stepper Motor*/
   uint8   PhaseCurr;      	/*Max Phase Curr of stepper motor*/  
   
   uint16  LineStart[10];   /*Start Period*/
   uint16  LineEnd[10];     /*End Period*/
   uint16  LineCnt[10];     /* Total steps in SPeed-up/Speed-down*/
   uint16  LineWaitCnt[10]; /* Constant speed Steps*/
   uint8   LineIndex;       /* Indicate current line number*/
   uint8   LineCmdIndex;    /* Indicate line number of UART command*/
   uint8   TotalLineNumber; /* How many lines in this pakage*/
    
}UI_DATA;

