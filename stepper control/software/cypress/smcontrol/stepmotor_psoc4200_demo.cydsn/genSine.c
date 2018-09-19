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
#include <genSine.h>
uint8 genSine(uint8 position)
{
	uint8 index;
	if(position > FullScale)
	{
		index = (FullScale - position + FullScale);			
	}
	else
	{	
			index = position;
	}
    return sineTable[index];
}