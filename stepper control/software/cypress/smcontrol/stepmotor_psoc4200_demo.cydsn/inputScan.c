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

/* [] END OF FILE */
#include <inputScan.h>

void inputScan()
{
    previousStepPulse = stepPulse;
    
    /*  */
    if ( STEP_INPUT_SOURCE_Read() == 0 ){
        // STEP impulsu saltinis - STEP_INPUT kanopa
        
        //previousStepPulse  = stepPulse;
        stepPulse = STEP_INPUT_Read(); // rising edge, reserved for potential glith
    }
    else
    {
        /* STEP impulsu saltinis - vidinis taimeris */
        
        //previousStepPulse = stepPulse;
        stepPulse = INTERNAL_STEP_OUT_Read();
    }
    
    if ( DIR_INPUT_SOURCE_Read()== 0 ){
        /* DIR signalo saltinis - DIR_INPUT kanopa */
        
        dirReg = DIR_INPUT_Read();        
    }
    else{
        /* DIR signalas statinis - 0 */
        
        dirReg = 0;
    }
}