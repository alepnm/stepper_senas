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
#include <project.h>
#include <Move.h>
#include <genSine.h>


void Move()
{
    if(divider != previousDivider)
	{
    //reset the step is the divider number is changed
		switch(divider)
		{
			case 2:
			    step = 8;
			    FullScale = 16;
			break;
		    case 4:
			    step = 4;
			    FullScale = 16;
			break;
			case 8:
				step = 2;
				FullScale = 16;
			break;
			case 16:
				step = 1;
				FullScale = 16;
			break;
			default:
			break;
        }
        currentStepA = 0;
        // CHA and CHB have 90 degree phase delay
	    if(currentStepA < FullScale)
	    {
		    currentStepB = currentStepA + FullScale;
	    }
	    else if(currentStepA >= FullScale)
	    {
			currentStepB = currentStepA - FullScale;
	    }
    }    
    else
    {// nomal operation
    //The total step is 2 times to the fullscale
	    phaseMove();
        
    	phaseControl(phase);
        
    	if(dirReg == 1)	//Clockwise
		{
			if (currentStepA < (FullScale - step + FullScale))
			{
				currentStepA = currentStepA + step;
			}
			else
			{
				currentStepA = 0;
			}
		}
        else		//Anti-Clockwise
		{
			if (currentStepA >= step)
			{
				currentStepA = currentStepA - step;
			}
			else
			{
				currentStepA = FullScale - step + FullScale;
			}
		}
    }
	
	if(currentStepA < FullScale)
	{
		currentStepB = currentStepA + FullScale;
	}
	else if(currentStepA >= FullScale)
	{
		currentStepB = currentStepA - FullScale;
	}
    TCPWM_1_WriteCompare( genSine(currentStepA) );
    TCPWM_2_WriteCompare( genSine(currentStepB) );
//    phaseMove();
//    phaseControl(phase);
}