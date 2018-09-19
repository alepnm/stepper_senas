#include "mcport.h"

#pragma optimize =0
__monitor void WDT_on(void)
{
        // Start timed equence
        WDTCR |= (1<<WDCE)|(1<<WDE);
        WDTCR = (1<<WDE)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0);
}//--------------------------------------------------------------

#pragma optimize =0
__monitor void WDT_off(void)
{
        __watchdog_reset();
        WDTCR |= (1<<WDCE)|(1<<WDE);
        WDTCR = 0;
}


#pragma optimize =0
uint16_t ReadADC( uint8_t channel ){
	
	ADMUX |= channel;	// Select pin ADC	
    ADCSR |= (1<<ADSC);	//Start conversion	    
    while(ADCSR & (1<<ADSC));	// wait until converstion completed
    
    return ADC;
}


/*
!<QWERTY
*/
char ReverseBits(char byte)
{  
    char result = 0;
    char i = 8;
    
    while (i-- > 0)
    {
        result <<= 1;         
        result |= (byte & 1);         
        byte = (byte >> 1);
    }  
    return result;
}/*QWERTY*/