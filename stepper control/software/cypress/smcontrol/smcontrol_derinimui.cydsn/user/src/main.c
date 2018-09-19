/******************************************************************************
* Project Name		: Stepper Motor Control
* File Name			: main.c
* Version 			: 1.0
* Device Used		: CY8C4245AXI-483
* Software Used		: PSoC Creator 4.0
* Compiler    		: ARM GCC 4.9-2015-q1-update
* Related Hardware	: 
*
* Owner             : Ventmatika Inc.
*******************************************************************************/

#include <project.h>
#include "common.h"
#include "stepper.h"
#include "systick.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "user_mb_app.h"
#include "mbutils.h"


/* Extern variables */
extern __IO SMC_HandleTypeDef SM;
extern __IO TIME_HandleTypeDef SystemTimer;

extern UCHAR    ucCoilBuf[];
extern UCHAR    ucDiscInputBuf[];
extern USHORT   usRegInputBuf[];
extern USHORT   usRegHoldingBuf[];

extern uint32_t RpmCoef;

/* Globals */
uint8  stepEnable = FALSE;
uint8  firstRun = FALSE;


/* Function prototypes */
void PwmTimerIsrConfiguration( void );
void IN0_IsrConfiguration( void );

CY_ISR_PROTO(PwmIsrHandler);
CY_ISR_PROTO(IN0_IsrHandler);

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary: Main Function,Parameter is initialized and main loop is included.
*  
* Parameters: None  
*
* Return: None
*  
*******************************************************************************/
int main()
{
    static uint32 delay = (0u);
    
    SysTickInit();
    
    /* PWM inicializacija */
	PWM_B_Start();
	PWM_A_Start();
    
    PWM_B_TriggerCommand(0x03, PWM_B_CMD_START);

    PWM_A_WriteCounter(0u);
	PWM_B_WriteCounter(0u);
	
    /* SAR ir Opamp inicializacija */	
	ADC_SAR_Seq_1_Start();
	//Opamp_A_Start();
	//Opamp_B_Start();
	
    /* ISR inicializacija */	
    PwmTimerIsrConfiguration();
    IN0_IsrConfiguration();
	
    StepperInit();
    
    /* Modbus inicializavimas */
    (void)eMBInit( MB_RTU, *SM.Modbus.pMbAddr, 0u, *SM.Modbus.pMbBaudrate, *SM.Modbus.pMbParity );
    (void)eMBEnable( );
       
	CyGlobalIntEnable;	 
    
    
    Status_LED_Write( TRUE );
    CyDelay(1000);
    Status_LED_Write( FALSE );
    
    
    for(;;)
    {			
        ( void )eMBPoll( );
        
		if( delay < GetTicks() )
		{  
			delay = GetTicks() + (20u);            
            
            if( usRegHoldingBuf[HR_TEST_MODE] == TEST_MODE_CODE ){
                // dirbame testo rezimu
                
            }else{
                // dirbame nornaliu rezimu
                ScrollingHandler();  
                
                HallSensorHandler();
                
                MotorHandler();
            }         
            
            CheckVB_Value();   /* skaitome VB maitinimo reiksme ir, jai ji netinka, aktyvuojam avarijos rezima */   
            
            
            /* Reboot kontrole */
            if( xMBUtilGetBits( ucCoilBuf, CO_DRV_REBOOT, 1 ) == TRUE ){

                
                //SaveSettings() -> issaugom EEPROME nustatymus
                //.....
                
                Status_LED_Write( true );
                CyDelay(50u);
                Status_LED_Write( false );
                CyDelay(50u);
                Status_LED_Write( true );
                CyDelay(50u);
                Status_LED_Write( false );
                CyDelay(50u);
                Status_LED_Write( true );
                CyDelay(50u);
                Status_LED_Write( false );
                CyDelay(1000u);
                
                CySoftwareReset();
                while(1);
            }
            
            usRegInputBuf[IR_WTIMELO] = LO16(SystemTimer.WTime);
            usRegInputBuf[IR_WTIMEHI] = HI16(SystemTimer.WTime);
        }
    }
    
    return (1u);
}


/*  */
void PwmTimerIsrConfiguration()
{
    PwmTimer_ISR_StartEx(PwmIsrHandler);
	PwmTimer_ISR_Enable();
    PwmTimer_ISR_ClearPending();   
}


/*  */
void IN0_IsrConfiguration()
{
    IN0_ISR_StartEx(IN0_IsrHandler);
    IN0_ISR_Enable();
    IN0_ISR_ClearPending();   
}


/*******************************************************************************
* Function Name: PwmIsrHandler
********************************************************************************
*
* Summary:
* This function is PWM ISR when TC happens, it increases PWM ticker and clear flag.
*
* Parameters: Nera
*
* Return: Nera
*
*******************************************************************************/
CY_ISR(PwmIsrHandler)
{
    //pwmCnt++;
    
    PWM_B_ClearInterrupt(PWM_B_INTR_MASK_TC);
    // arba taip:
    // PwmTimer_ISR_ClearPending(); //<-- neveikia
}


/*  */
CY_ISR(IN0_IsrHandler){   
    
    DI0_ClearInterrupt();
    //IN0_ISR_ClearPending();
}

/* [] END OF FILE */
