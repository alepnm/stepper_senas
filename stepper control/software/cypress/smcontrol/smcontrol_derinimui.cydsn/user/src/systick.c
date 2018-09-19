/******************************************************************************
* Project Name		: Stepper Motor Control
* File Name			: systick.c
* Version 			: 1.0
* Device Used		: CY8C4245AXI-483
* Software Used		: PSoC Creator 4.0
* Compiler    		: ARM GCC 4.9-2015-q1-update
* Related Hardware	: 
*
* Owner             : Ventmatika Inc.
*******************************************************************************/
#include <project.h>
#include "systick.h"

/* ------------------------------- Defines ----------------------------------*/
#define SYSTICK_EACH_10_HZ  (1000u)
#define SYSTICK_RELOAD      (CYDEV_BCLK__SYSCLK__HZ / SYSTICK_EACH_10_HZ)


__IO TIME_HandleTypeDef SystemTimer;

/* -------------------------------- Statics ---------------------------------*/
static volatile uint32_t ticks;
//register uint32_t ticks asm("r0");

static struct{    
    uint8_t     Seconds     :6;
    uint8_t     Minutes     :6;
    uint8_t     Hours       :5;    
}Counters = { 0, 0, 0 };


/* Functions prototypes */
static void UpdateTimers( void );
CY_ISR_PROTO(SysTickHandler);


/* ------------------------- Start implementation ---------------------------*/
void SysTickInit( void )
{
    /* Setup SysTick Timer */
    CySysTickStart();
    CySysTickSetReload(SYSTICK_RELOAD);
    CyIntSetSysVector( (SysTick_IRQn + 16u), SysTickHandler ); 
}

uint32_t GetTicks( void )
{
    uint8_t isr;
    volatile uint32_t _ticks;
    
    isr = CyEnterCriticalSection();
    _ticks = ticks;
    CyExitCriticalSection(isr);
    
    return _ticks;
}


void SetTicks(uint32_t _ticks)
{
    uint8_t isr;

    isr = CyEnterCriticalSection();
    ticks = _ticks;
    CyExitCriticalSection(isr);
}


static void UpdateTimers( void ){
    
    SystemTimer.Timestamp_seconds++;
    
    if( Counters.Seconds++ >= (60u) ){
        Counters.Seconds = (0u);
        SystemTimer.t1s = ~SystemTimer.t1s;        
        SystemTimer.WTime++;
        
        if( Counters.Minutes++ >= (60u) ){
            Counters.Minutes = (0u);
            SystemTimer.t60s = ~SystemTimer.t60s;
            
            if( Counters.Hours++ >= (24u) ){
                Counters.Hours = (0u);
            }            
        }        
    } 
}


/*****************************************************************************/
/*                          SysTick Interrupt Handler                        */
/*****************************************************************************/
CY_ISR(SysTickHandler)
{
    static uint32_t delay_100ms = (0u);
    static uint8_t delay_500ms = (0u);
    
    ticks++;
    
    if( delay_100ms < ticks ){
        
        delay_100ms = ticks + (100u);
        SystemTimer.t100ms = ~SystemTimer.t100ms;
        
        if( delay_500ms++ >= (5u) ){
            delay_500ms = (0u);
            
            SystemTimer.t500ms = ~SystemTimer.t500ms;
            
            if( SystemTimer.t500ms == true ) UpdateTimers();
        }        
    }
}


/* [] END OF FILE */
