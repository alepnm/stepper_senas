/******************************************************************************
* Project Name		: Stepper Motor Control
* File Name			: systick.h
* Version 			: 1.0
* Device Used		: CY8C4245AXI-483
* Software Used		: PSoC Creator 4.0
* Compiler    		: ARM GCC 4.9-2015-q1-update
* Related Hardware	: 
*
* Owner             : Ventmatika Inc.
*******************************************************************************/
#if !defined(SYSTICK_H)
#define SYSTICK_H

/* --------------------------- System includes ------------------------------*/
#include <stdint.h>
#include <stdbool.h>
    
    
typedef struct{
    uint32_t    WTime;
    uint32_t    Timestamp_seconds;
    
    uint8_t     t100ms  :1;
    uint8_t     t500ms  :1;
    uint8_t     t1s     :1;
    uint8_t     t60s    :1;    
}TIME_HandleTypeDef;
    
    
    
/* --------------------------- Global functions -----------------------------*/    
void SysTickInit(void);
uint32_t GetTicks(void);
    
#endif /* SYSTICK_H */ 
/* [] END OF FILE */
