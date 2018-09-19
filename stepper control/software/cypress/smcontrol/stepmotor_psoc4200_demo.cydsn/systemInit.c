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
#include <systemInit.h>
#include <DIR_INPUT.h>
#include <TCPWM_1.h>
#include <TCPWM_2.h>
//#include <PWM1_Sine.h>
//#include <PWM2_Sine.h>
#include <INTERNAL_STEP.h>

void systemInit (){
    currentStepA = 0;
    currentStepB = 16;
    divider = 16;
    previousDivider = 16;
    FullScale = 16;
    step = 1;
    pwmA = sineTable[0];
    pwmB = sineTable[FullScale];
    
    if (dirReg == 0 ){
        phase = 0;
    }
    else{
        phase = 2;
    }
    
    TCPWM_1_WriteCompare(pwmA);
    TCPWM_2_WriteCompare(pwmB);
    Clock_PWM_MAIN_Start(); 
    //PWM_Sine_Start();
	TCPWM_1_Start();
	TCPWM_2_Start();
	TCPWM_1_TriggerCommand (TCPWM_1_MASK, TCPWM_1_CMD_START);
	TCPWM_2_TriggerCommand (TCPWM_2_MASK, TCPWM_2_CMD_START);	
	//PWM2_Sine_Start();
    INTERNAL_STEP_Start();
}

/* [] END OF FILE */