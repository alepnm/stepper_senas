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

#include <phaseControl.h>

void phaseControl (uint8 currentphase){
/*    uint8 RA0;
    uint8 RA1;
	switch(currentphase)
	{
		case 0:
		    RA0 = 1;
		    RA1 = 0;
		break;
		case 1:
			RA0 = 0;
			RA1 = 0;
		break;
		case 2:
			RA0 = 1;
			RA1 = 1;
		break;
		case 3:
			RA0 = 0;
            RA1 = 1;
		break;
		default:
		break;
	}
    DIR_OUT_A_Write(RA0);
    DIR_OUT_B_Write(RA1);*/
    
    PHASE_REG_Write(currentphase);
}

void phaseMove(){
	if((currentStepA==0)||(currentStepB==0))
	{
		if(dirReg==0)	//Clockwise
		{
			switch(phase)
			{
				case 0:
					phase = 2;
				break;
				case 1:
					phase = 0;
				break;
				case 2:
					phase = 3;
				break;
				case 3:
					phase = 1;
				break;
				default:
				break;
			}
		}
		else	//Anti-Clockwise
		{
			switch(phase)
			{
				case 0:
					phase = 1;
				break;
				case 1:
					phase = 3;
				break;
				case 2:
					phase = 0;
				break;
				case 3:
					phase = 2;
				break;
				default:
				break;
			}
		}
	} 
}

/* [] END OF FILE */