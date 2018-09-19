/* ========================================
 *
 * The following firmware was developed by Cypress Semiconductor
 * This work is licensed under a Creative Commons Attribution 3.0 Unported License.
 * 
 * http://creativecommons.org/licenses/by/3.0/deed.en_US
 * 
 * You are free to:
 * -To Share — to copy, distribute and transmit the work 
 * -To Remix — to adapt the work 
 * -To make commercial use of the work
 *
*/
#include <device.h>
#include <global.h>
#include <systemInit.h>
#include <inputScan.h>
#include <Move.h>
#include <internal_step.h>
#define PH1 3204 //run
#define PH2 4800 //p
#define PH3 8000 //run
#define PH4 9600 //p
#define PH5 16000 //run
#define PH6 17600 //p
#define PH7 24000 //run
#define PH8 27200 //p
#define PH9 28800 //run
#define PH10 32000 //p
#define PH11 33600 //run
#define PH12 36800 //p

int main()
{
    uint16 i = 0;

    inputScan();
    
    systemInit();
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    /* CyGlobalIntEnable; */ /* Uncomment this line to enable global interrupts. */
    while(1)
    {
        inputScan();
        if (previousStepPulse == 0 && stepPulse == 1){
  			if (i < PH1){
   				dirReg = 0;
          		Move();
          		i++;
  			}
  			else if (PH1 <= i && i < PH2){
   				i++;
  			}
  			else if (PH2 <= i && i < PH3){
   				dirReg = 1;
   				Move();
   				i++;
  			}
			else if (PH3 <= i && i < PH4){
				i++;
			}
  			else if (PH4 <= i && i < PH5){
				if (i == PH4){
    				INTERNAL_STEP_WritePeriod(2000);
    				INTERNAL_STEP_WriteCompare(1000);
                }
   				dirReg = 0;
   				Move();
   				i++;
  			}
  			else if (PH5 <= i && i < PH6){

   				i++;
  			}
  			else if (PH6 <= i && i < PH7){
			   	dirReg = 1;
   				Move();		
   				i++;
  			}
  			else if (PH7 <= i && i < PH8){
   				i++;
  			}
  			else if (PH8 <= i && i < PH9){
				if (i == PH8){
    				INTERNAL_STEP_WritePeriod(4000);
    				INTERNAL_STEP_WriteCompare(2000);
                }
				dirReg = 0;
   				Move();
   				i++;
  			}
  			else if (PH9 <= i && i < PH10){
   				i++;
  			}
  			else if (PH10 <= i && i < PH11){
   				dirReg = 1;
				Move();
				i++;
  			}
  			else if (PH11 <= i && i < PH12){
   				i++;
  			}

  			else{

   				dirReg = 1;
   				Move();
   
  			}
		}
		else
        {
		}
	}
    
    return 1;
}
