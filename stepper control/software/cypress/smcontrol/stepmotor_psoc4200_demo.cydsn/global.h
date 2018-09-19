/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 
 This header file is for global definition
 
 *
 * ========================================
*/
#define OUTPUTLEVEL 127
#include <CYLIB.H>
uint8 currentStepA; // current step level of channel A
uint8 currentStepB; // current step level of channel B
uint8 divider; // divider value of step moter driver 2, 4, 8 ,16
uint8 step; //step length
uint8 previousDivider; // in case have various divider in the future
uint8 FullScale; // step from 0 to 1/4 waveform
uint8 phase; //current phase of driver 0, 1, 2, 3
uint8 stepPulse;
uint8 previousStepPulse;
uint8 dirReg;// direction
uint8 pwmA; // determine the duty cycle (comes from sine table value here)
uint8 pwmB; // determine the duty cycle (comes from sine table value here)
//#if (OUTPUTLEVEL == 255)
//const uint8 sineTable [17] = {0, 25, 50, 74, 98, 120, 142, 162, 180, 197, 212, 225, 236, 244, 250, 254, 255}; // sine table
//#elif (OUTPUTLEVEL == 127)      
//const uint8 sineTable [17] = {0, 12, 25, 37, 49, 60, 71, 81, 90, 98, 106, 112, 118, 122, 125, 126, 127}; // sine table
//const uint8 sineTable [17] = {0, 18, 35, 52, 69, 85, 100, 114, 127, 139, 150, 159, 166, 172, 177, 179, 180}; // sine table
//#else
//const uint8 sineTable [17] = {0, 25, 50, 74, 98, 120, 142, 162, 180, 197, 212, 225, 236, 244, 250, 254, 255}; // sine table
const uint8 sineTable [17] = {100, 115, 130, 145, 159, 173, 186, 198, 210, 220, 229, 237, 243, 248, 252, 254, 255};
//#endif
