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

#include <DIR_OUT_A.h>
#include <DIR_OUT_B.h>
#include <PHASE_REG.h>


extern uint8 phase;
extern uint8 currentStepA;
extern uint8 currentStepB;
extern uint8 dirReg;

void phaseControl (uint8 currentphase);
void phaseMove();

/* [] END OF FILE */