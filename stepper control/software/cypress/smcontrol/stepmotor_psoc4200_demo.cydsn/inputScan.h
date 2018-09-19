/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
//[] END OF FILE
#include <CYlIB.h>
#include <DIR_INPUT.h>
#include <DIR_INPUT_SOURCE.h>
#include <INTERNAL_STEP_OUT.h>
#include <STEP_INPUT_SOURCE.h>
#include <STEP_INPUT.h>



extern uint8 stepPulse;
extern uint8 previousStepPulse;
uint8 dirReg;

void inputScan();
