/*******************************************************************************
* File Name: userinterface.h  
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
#ifndef  _USERINTERFACE_H_
#define  _USERINTERFACE_H_


/* UI menu and display service API */
void InterfaceProcess(void);

extern void   SetMaxCurr(uint8);

/* UI menu structure defines */
#define MENU_IDLE				0
#define MENU_IGNORE_INPUT		1
#define MENU_READY				2
#define MENU_RUN				3
#define MENU_RUN_UPDATE			4
#define MENU_DIRECTION			5
#define MENU_DIRECTION_SELECT	6
#define MENU_DIRECTION_UPDATE	7
#define MENU_DIRECTION_SET		8
#define MENU_CURRENT			9
#define MENU_CURRENT_SELECT		10
#define MENU_CURRENT_UPDATE		11
#define MENU_CURRENT_SET		12
#define MENU_STEPS				13
#define MENU_STEPS_SELECT		14
#define MENU_STEPS_UPDATE		15
#define MENU_STEPS_SET			16
#define MENU_DEFAULT			17
#define MENU_DEFAULT_SET		18

#endif	/* End _USERINTERFACE_H_ */