/******************************************************************************
*   Module :  systick.c
*          
*   Created:        
*   Author:   Alex(@lex)  
******************************************************************************/
/****************************    INCLUDES    *********************************/
#include "systick.h"

/***********************    DEFINES,  TYPEDEFS     ***************************/
/* Used Timer defines -> Timer0 */
#if defined(__ATmega8__)
	#define SYSTICK_TIM_TCCR			TCCR0			/* timer control register */
	#define SYSTICK_TIM_TCNT			TCNT0			/* timer counter */
	#define SYSTICK_TIM_TIMSK			TIMSK			/* timer interrupt mask register */
	#define SYSTICK_TIM_IE				TOIE0			/* timer overflow interrupt enable bit */
	#define SYSTICK_TIM_ISR_HANDLER		TIMER0_OVF_vect	/* timer interrupt handler */
#elif defined(__ATmega16__)
	#define SYSTICK_TIM_TCCR			TCCR0			/* timer control register */
	#define SYSTICK_TIM_TCNT			TCNT0			/* timer counter */
	#define SYSTICK_TIM_TIMSK			TIMSK			/* timer interrupt mask register */
	#define SYSTICK_TIM_IE				TOIE0			/* timer overflow interrupt enable bit */
	#define SYSTICK_TIM_ISR_HANDLER		TIMER0_OVF_vect	/* timer interrupt handler */
#elif defined(__ATmega328P__)
	#define SYSTICK_TIM_TCCR			TCCR0B			/* timer control register */
	#define SYSTICK_TIM_TCNT			TCNT0			/* timer counter */
	#define SYSTICK_TIM_TIMSK			TIMSK0			/* timer interrupt mask register */
	#define SYSTICK_TIM_IE				TOIE0			/* timer overflow interrupt enable bit */
	#define SYSTICK_TIM_ISR_HANDLER		TIMER0_OVF_vect	/* timer interrupt handler */
#endif

#define SYSTICK_TIM_PRESCALER_MASK_DEF	0x03
/****************************    EXTERNS     *********************************/
/****************************    GLOBALS     *********************************/
static uint32_t tick = 0;

static volatile SYSTICK_T SysTickTimer = {
    .Seconds 	= ZERO,
    .Minutes 	= ZERO,
	.Hours 		= ZERO,
	.WTime = {
		{
			.Lo = ZERO,
			.Hi = ZERO,
		},
		.val = ZERO,
	},
    .TimeBits = {
        .Msec = 	false,
		.HalfSec = 	false,
        .Sec = 		false,
        .Min = 		false
    }  
};
/****************************   PROTOTYPES   *********************************/

/* ======================= Start implementation ============================ */
/******************************************************************************
Function name: SysTickInit
Function description:
Parameters: none

Output: none
******************************************************************************/
void SysTickInit(){
	SYSTICK_TIM_TCCR = SYSTICK_TIM_PRESCALER_MASK_DEF;
	SYSTICK_TIM_TIMSK |= (1<<SYSTICK_TIM_IE);
	
	SYSTICK_TIM_TCNT = 0x83;//FOSC/PRESCALER/PERIOD
}
/*****************************************************************************/


/******************************************************************************
Function name: GetTicks
Function description:
Parameters: none

Output: uint32_t systick registro reiksme
******************************************************************************/
inline __monitor uint32_t GetTicks(){   
    return tick;
}
/*****************************************************************************/


/******************************************************************************
Function name: pSysTickTimer
Function description: grazina pointeri i SysTickTimer'i
Parameters: nera

Output: pointeris i SysTickTimer kintamaji
******************************************************************************/
__monitor SYSTICK_T* pSysTickTimer(){
	return &SysTickTimer;
}



#pragma vector=SYSTICK_TIM_ISR_HANDLER
__interrupt void TMR_OVF_Handler()
{ 
    static struct{
        uint32_t    delay;
		uint8_t		halfsec_counter;
    };
	
    if(delay <= ++tick){
        delay = tick + 100;
        
        SysTickTimer.TimeBits.Msec = !SysTickTimer.TimeBits.Msec;
        
		if(++halfsec_counter >= 5){
			halfsec_counter = ZERO;
			SysTickTimer.TimeBits.HalfSec = !SysTickTimer.TimeBits.HalfSec;
			if(SysTickTimer.TimeBits.HalfSec == false){
				SysTickTimer.TimeBits.Sec = !SysTickTimer.TimeBits.Sec;				
				if(++SysTickTimer.Seconds >= 60){
					SysTickTimer.Seconds = ZERO; 
					SysTickTimer.WTime.val++;
					SysTickTimer.TimeBits.Min = !SysTickTimer.TimeBits.Min;					
					if(++SysTickTimer.Minutes >= 60){
						SysTickTimer.Minutes = ZERO;
						SysTickTimer.Hours++;
					}
				}
			}  
		}
    }
	
    SYSTICK_TIM_TCNT = 0x83;
}
/*****************************************************************************/

/*=============================================================================
END OF FILE 
=============================================================================*/

