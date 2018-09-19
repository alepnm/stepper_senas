/******************************************************************************
* Project Name		: Stepper Motor Control
* File Name			: stepper.c
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
#include "stepper.h"
#include "common.h"
#include "user_mb_app.h"
#include "mbutils.h"


/* Extern */
__IO TIME_HandleTypeDef SystemTimer;

extern uint8  stepEnable;
extern uint8  firstRun;

extern UCHAR    ucCoilBuf[];
extern UCHAR    ucDiscInputBuf[];
extern USHORT   usRegInputBuf[];
extern USHORT   usRegHoldingBuf[];


const uint8_t SinTable[] = {
     127, 127, 127, 127, 127, 127, 127, 127, 126, 126,
     126, 126, 126, 125, 125, 125, 125, 124, 124, 124,
     123, 123, 122, 122, 121, 121, 120, 120, 119, 119,
     118, 118, 117, 117, 116, 115, 115, 114, 113, 113,
     112, 111, 110, 109, 109, 108, 107, 106, 105, 104,
     103, 103, 102, 101, 100,  99,  98,  97,  96,  95,
      94,  93,  91,  90,  89,  88,  87,  86,  85,  83,
      82,  81,  80,  79,  77,  76,  75,  74,  72,  71,
      70,  68,  67,  66,  64,  63,  62,  60,  59,  58,
      56,  55,  54,  52,  50,  49,  48,  46,  45,  43,
      42,  40,  39,  37,  36,  34,  33,  31,  30,  28,
      27,  25,  23,  22,  20,  19,  17,  16,  14,  13,
      11,   9,   8,   6,   5,   3,   2,   1
 };


/* universalus variklio tipas (is Modbus HR_STEPS_PER_REV ir HR_MOTOR_CURRENT ) */
static MOTOR_TYPE_TypeDef UNIVERSAL = {
    .ID = 0,
    .StepsPerRevolution = UNI_STEPS_PER_REV_DEF,
    .CurrentCoef.val = CURR2COEF(UNI_CURRENT_DEF)
};

/* variklis Microcon SX34-2740N */
const MOTOR_TYPE_TypeDef SX34_2740N = {
    .ID = 1,
    .StepsPerRevolution = 200,
    .CurrentCoef.val = 1
};

/* variklis Matsushita KP39HM4-016 */
const MOTOR_TYPE_TypeDef KP39HM4_016 = {
    .ID = 2,
    .StepsPerRevolution = 100,
    .CurrentCoef.val = 0.1
};





__IO SMC_HandleTypeDef SM = {
    .Modbus = {
        .pMbAddr = &usRegHoldingBuf[HR_MBADDR],
        .pMbBaudrate = &usRegHoldingBuf[HR_MBBAUDRATE],
        .pMbParity = &usRegHoldingBuf[HR_MBPARITY],
        .pMbStopBits = &usRegHoldingBuf[HR_MBSTOPBITS]   
    },
    .Settings = {
        .pMotor = &UNIVERSAL,
        .pMicroSteps = &usRegHoldingBuf[HR_MICROSTEPS],
        .pScrollingRPM = &usRegHoldingBuf[HR_SCROLL_RPM],
        .pScrollSync = &usRegHoldingBuf[HR_SCROLL_SYNC]
    },
    .Control = {
        .Mode = UI_EXT,
        .RotDirection = CW,
        .ScrollingEna = DISABLED,
        .pRotSpeed = &usRegHoldingBuf[HR_ROT_SPEED],
        .pScrollCycleOff = &usRegHoldingBuf[HR_SCROLL_OFF_CYCLE_TIME],
        .pScrollCycleOn = &usRegHoldingBuf[HR_SCROLL_ON_CYCLE_TIME]
    },
    .Status = {
        .IsRun = 0,
        .pVbusValue = &usRegInputBuf[IR_VBUS_VALUE],
        .pPotValue = &usRegInputBuf[IR_SP_REQ_VALUE],
        .pCurrRPM = &usRegInputBuf[IR_CURR_RPM],
        .pContrState = ( STATE_TypeDef* )&usRegInputBuf[IR_SMC_STATE],
        .pFaultCode = ( ERROR_TypeDef* )&usRegInputBuf[IR_FAULT_CODE]
    }    
};

uint32_t RpmCoef = 0;

/* Statics */
static uint8  currRef_A = (0u);
static uint8  currRef_B = (0u);
static uint16_t CurrentStep = (0u);



/* Function prototypes */
static void MotorRun( void );
static void MotorStop( void );

static void CalcRPM( void );
static double CalcCoef( double current );

CY_ISR_PROTO(MicrostepIsrHandler);


/*******************************************************************************
* Function Name: StepperInit
********************************************************************************
* Summary:
* Sistemos inicializavimas
*
* Parameters: Nera
*
* Return: Nera
*
*******************************************************************************/
void StepperInit( void ){
    
    Microstep_ISR_StartEx(MicrostepIsrHandler);
	Microstep_ISR_Enable();
    Microstep_ISR_ClearPending();   
    
    /* 1 - inicializuojam darbinius registrus */    
    /* inicializuojam MODBUS darbinius registrus */
    usRegHoldingBuf[HR_MBADDR] = MBADDR_DEF;
    usRegHoldingBuf[HR_MBBAUDRATE] = MBBAURATE_DEF;
    usRegHoldingBuf[HR_MBSTOPBITS] = MBSTOPBITS_DEF;
    usRegHoldingBuf[HR_MBPARITY] = MBPARITY_DEF;    

    /* inicializuojam SMC kontroleriaus darbinius registrus */
    xMBUtilSetBits( ucCoilBuf, CO_START, 1, false );
    xMBUtilSetBits( ucCoilBuf, CO_DIRECTION, 1, true );
    xMBUtilSetBits( ucCoilBuf, CO_FAULT_FLAG, 1, false );
    xMBUtilSetBits( ucCoilBuf, CO_DRV_REBOOT, 1, false );
    xMBUtilSetBits( ucCoilBuf, CO_SMC_CONTROL, 1, true );
    
    usRegHoldingBuf[HR_TEST_MODE] = ( 0u );
    usRegHoldingBuf[HR_MICROSTEPS] = MICROSTEPS_DEF; 
    usRegHoldingBuf[HR_ROT_SPEED] = ( 0u );
    usRegHoldingBuf[HR_MAX_RPM] = MAX_RPM_DEF;
    
    SystemTimer.WTime = 183428201;    
    usRegInputBuf[IR_WTIMELO] = LO16(SystemTimer.WTime);
    usRegInputBuf[IR_WTIMEHI] = HI16(SystemTimer.WTime);
    
    usRegInputBuf[IR_SMC_STATE] = ST_STOP; 
    usRegInputBuf[IR_VBUS_VALUE] = ( 0u );
    usRegInputBuf[IR_SP_REQ_VALUE] = ( 0u );
    usRegInputBuf[IR_CURR_RPM] = ( 0u );
    usRegInputBuf[IR_FAULT_CODE] = noError;    
    
    /* inicializuojam UNI tipo variklio darbinius registrus */
    usRegHoldingBuf[HR_UNI_STEPS_PER_REV] = UNI_STEPS_PER_REV_DEF;
    usRegHoldingBuf[HR_UNI_CURRENT] = UNI_CURRENT_DEF*100;
    UNIVERSAL.StepsPerRevolution = usRegHoldingBuf[HR_UNI_STEPS_PER_REV];
    //UNIVERSAL.CurrentCoef.val = CalcCoef( usRegHoldingBuf[HR_UNI_CURRENT]/100 );

    
    /* inicializuojam SCROLL rezimo darbinius registrus */
    usRegHoldingBuf[HR_SCROLL_RPM] = SCROLL_RPM_DEF; 
    usRegHoldingBuf[HR_SCROLL_OFF_CYCLE_TIME] = SCROLL_OFF_CYCLE_TIME_DEF;
    usRegHoldingBuf[HR_SCROLL_ON_CYCLE_TIME] = SCROLL_ON_CYCLE_TIME_DEF;
    usRegHoldingBuf[HR_SCROLL_SYNC] = SCROLL_SYNC_DEF;
    
    /* inicializuojam HALL daviklio darbinius registrus */
    usRegHoldingBuf[HR_HS_TO_VALUE] = HALL_SENS_TO_DEF;
    usRegInputBuf[IR_HS_TO_COUNTER] = HALL_SENS_TO_DEF; 
    
    ucDiscInputBuf[DI_HALL_STATE] = false;
    ucDiscInputBuf[DI_HALL_FAULT] = false;
    
    /* 2 - inicializuojam pointerius i darbinius registrus ( jai reikia ) */
    
    /* Variklio inicializacija */
    MotorStop();
    
    
    RpmCoef = (uint32_t)( MICROSTEPCLK * 60 )/( MAX_MICROSTEPS * SM.Settings.pMotor->StepsPerRevolution );     
    SM.microStepPace = (uint8_t)( MAX_MICROSTEPS / (*SM.Settings.pMicroSteps) );
    
    SM.Control.RotDirection = CW;  /* nustatom sukimosi kripti */	
    SM.Status.IsRun = false;
    SM.Control.Mode = UI_INT;
    
}



/*******************************************************************************
* Function Name: Stepping
********************************************************************************
* Summary:
* Funkcija prasuka varikli per viena mikrostepa. Suzadinama is uStepTimer_ISR
*
* Parameters: Nera
*
* Return: Nera
*
*******************************************************************************/
void Stepping( void )
{
	/* When 1/4 Sinusoidal wave end, move change the stage of run-on phase*/
    if( SM.microStepPointer == (0u) )		 
    {    
        SM.stageIndex += SM.Control.RotDirection;   /* stumiam faze pagal sukimosi kripti */
        SM.stageIndex &= 0x03;          /* numetam fazes indeksa po 4-os fazes */
	
        ControlReg_Write(SM.stageIndex);/* formuojam isejimose signalus is LUT */
        
        /* formuojam signala Home, jai padareme pilna apsisukima */
        if( SM.stageIndex == (0u) ){
            
            if( --CurrentStep == 0 ){                
                CurrentStep = SM.Settings.pMotor->StepsPerRevolution;
                SM.Home = true;
            }
        }        
    }
    else
    {
        SM.Home = false;
    }

    /* paimam is refpointu lenteles referensu reiksmes fazem A ir B esamam mikrostepui */
    if( SM.Control.RotDirection == CCW )									
    {
        if( (SM.stageIndex & 0x01) == (0u) )		
        /* Faze 0 ir 2: Current Down */				 
            SM.sinTblPointer = SM.microStepPointer;
        else  									    
        /* Fazes 1 ir 3: Current Up */
            SM.sinTblPointer = TABLETOP - SM.microStepPointer ; 
    }
    else
    {
        if( (SM.stageIndex & 0x01) == (1u) )		
            SM.sinTblPointer = SM.microStepPointer;
        else  						
            SM.sinTblPointer = TABLETOP - SM.microStepPointer;
    }
            
    SM.cosTblPointer = TABLETOP - SM.sinTblPointer;

    currRef_A = SinTable[SM.sinTblPointer];    
    currRef_B = SinTable[SM.cosTblPointer];
    
    // pirmam variantui
    //IDAC_A_SetValue(currRef_A);
    //IDAC_B_SetValue(currRef_B);
    
    // antram variantui
	IDAC_A_SetValue( currRef_B * SM.Settings.pMotor->CurrentCoef.val );
    IDAC_B_SetValue( currRef_A * SM.Settings.pMotor->CurrentCoef.val );
    
    SM.microStepPointer += SM.microStepPace;    /* stumiam mikrostepo lenteles pointeri per SM.microStepPace zingsniu */
    SM.microStepPointer &= 0x7F;                /* numetam pointeri, jai virsyjam lenteles dydi (0x7F) */
    
}




/*******************************************************************************
* Function Name: MotorStop
********************************************************************************
* Summary:
* 
*
* Parameters: Nera
*
* Return: Nera
*
*******************************************************************************/
static void MotorStop( void ){    

   	stepEnable = FALSE;
    
    uStepTimer_Stop();
    
    /* numetam fazes indeksa i 0 */
    SM.stageIndex = (0u);

    currRef_A = SinTable[SM.sinTblPointer];
    currRef_A = currRef_A * SM.Settings.pMotor->CurrentCoef.val;
    currRef_A >>= (1u);		/* perpus sumazinam uzlaikimo srove apvijos A */
    
    currRef_B = SinTable[SM.cosTblPointer];
    currRef_B = currRef_B * SM.Settings.pMotor->CurrentCoef.val;
    currRef_B >>= (1u);		/* perpus sumazinam uzlaikimo srove apvijos B */
    
	IDAC_A_SetValue( currRef_A );
    IDAC_B_SetValue( currRef_B );	
    
#if(MOTOR_HOLD == false )
    ControlEnable_Write(0u); 
#endif    
    
    firstRun = TRUE;
}


/*******************************************************************************
* Function Name: MotorRun
********************************************************************************
* Summary:
* 
*
* Parameters: Nera
*
* Return: Nera
*
*******************************************************************************/
static void MotorRun( void ){
    
    /* Start Motor */
  	ControlEnable_Write(1u);	

  	stepEnable = TRUE;
	
	/* startuojam mikrostepa */            
	if(firstRun == TRUE)
	{  
		IDAC_A_Start();
		IDAC_A_SetValue(0u);
        
		IDAC_B_Start();
		IDAC_B_SetValue(0u);
        
		uStepTimer_Start();

		firstRun = FALSE;
	}
    
}



/*******************************************************************************
* Function Name: MotorHandler
********************************************************************************
*
* Summary:
* Variklio darbo kontroleris
*
* Parameters: Nera
*
* Return: Nera
*
*******************************************************************************/
void MotorHandler ( void ){
    
    switch( *SM.Status.pContrState ){
        case ST_STOP:

        break;
        case ST_NORMAL:
        
            CalcRPM();
            
            /* sukimosi kripties kontrole */
            SM.Control.RotDirection = ( xMBUtilGetBits( ucCoilBuf, CO_DIRECTION, 1 ) == true ) ? CCW : CW;
            
            /* Start/Stop kontrole */
            SM.Status.IsRun = ( xMBUtilGetBits( ucCoilBuf, CO_START, 1 ) == true )? TRUE : FALSE;
            
        break;
        case ST_SCROLL:
        
        break;
        case ST_FAULT:
            MotorStop();
            ControlEnable_Write(0u);
        break; 
        case ST_TEST:
            
        break;
    }    
    
	/* sukam varikli jai nera avariju */
    if( SM.Status.IsRun == TRUE ) MotorRun();	
    else MotorStop();        
}




/*******************************************************************************
* Function Name: CalcRPM
********************************************************************************
*
* Summary:
* Apskaiciuoja RPMus ir Taimerio Period registra is ADC reiksmes
*
* Parameters: Nera
*
* Return: Nera
*
*******************************************************************************/
static void CalcRPM( void ){
    
    uint32 rpmReq = (0u);    
    
	/* skaitom sukimosi greicio ADC */
	(*SM.Status.pPotValue) = ( ADC_SAR_Seq_1_GetResult16( REQ_SPEED_CHANNEL )& 0x0FFF );  /* atfiltruojam zemiausius 12 bitus */     
    
    if( SM.Control.Mode == UI_INT ){    // 
        
        rpmReq = ( ( *SM.Status.pPotValue ) ^= 0x0FFF );
        
		/* nustatom variklio sukimosi ribos */
		if( rpmReq < (80u) ) rpmReq = (80u);
		else if( rpmReq > (5200u) ) rpmReq = (5200u);
	
		/* mazinam greiti, kai naudojamas zemas mikrostepas */
        switch( SM.microStepPace )
        {
            case 8:
                /* apeinam mechaninio rezonanso taskus, reiksmes nustatom
                skirtingems varikliams real-time rezime */
                //if( rpmReq > (1300u) && rpmReq < (1700u) ) rpmReq = (1700u);
                rpmReq <<= (1u);
                break;                    
            case 16:
                rpmReq <<= (2u);
                break;
            case 32:
                rpmReq <<= (3u);
                break;
            case 64:
                rpmReq <<= (4u);
                break;
            case 128:                      
                //if( rpmReq > (2000u) && rpmReq < (2400u) ) rpmReq = (2400u);
                //if( rpmReq > (1000u) && rpmReq < (1400u) ) rpmReq = (1400u);
                        
                rpmReq <<= (5u);
                break;
            default:
                rpmReq <<= (1u);
        }               
        
        /* Calculate the real time motor speed value */              
        (*SM.Control.pRotSpeed) = (*SM.Status.pCurrRPM) = (uint16)( (RpmCoef * SM.microStepPace) / rpmReq );
    
    }else{  // valdymas per Modbus
        rpmReq = (uint32)(RpmCoef * SM.microStepPace) / (*SM.Control.pRotSpeed);                
    }            
    
	uStepTimer_WritePeriod( rpmReq );    /* atnaujinam variklio sukimosi greiti */     
}


/*******************************************************************************
* Function Name: CalcCoef
********************************************************************************
*
* Summary:
* 
*
* Parameters: double
*
* Return: double
*
*******************************************************************************/
static double CalcCoef( double current ){
    
    if( current < UNI_MIN_CURRENT_DEF ) current = UNI_MIN_CURRENT_DEF;
    if( current > UNI_MAX_CURRENT_DEF ) current = UNI_MAX_CURRENT_DEF;    
    
    return CURR2COEF(current);
}


/*******************************************************************************
* Function Name: ScrollingHandler
********************************************************************************
*
* Summary:
* Funkcija nustato kontrollerio busena
*
* Parameters: Nera
*
* Return: Nera
*
*******************************************************************************/
void ScrollingHandler( void ){
    
    static uint32_t delay_s;
    static bool stage = STOP;  // SCROLLING busena: laukimas(false)/sukimas(true)
    static uint8_t scroll_dir = CW;
    static uint8_t scroll_sync_counter = 0u;
    
    
    /* cia patikrinam ar sukasi variklis ne SCROLLING rezime. Jai sukasi, iseinam
    */
    if( xMBUtilGetBits( ucCoilBuf, CO_START, 1 ) == true ){
            
        (*SM.Status.pContrState) = ST_NORMAL;
        
        stage = STOP;
        
        return;
    }     
    
    SM.Control.ScrollingEna = ( usRegHoldingBuf[HR_SCROLL_RPM] > ( 0u ) ) ? ENABLED : DISABLED;
    
    if( delay_s > SystemTimer.Timestamp_seconds || SM.Control.ScrollingEna == DISABLED ) return;
   
    if( stage == STOP ) {  
        /* laukiam skrollingo. Busena ST_STOP. */        
        (*SM.Status.pContrState) = ST_STOP;
        
        SM.Status.IsRun = FALSE;
        
        delay_s = SystemTimer.Timestamp_seconds + (*SM.Control.pScrollCycleOff);        
        
    }else{  
        /* vykdom skrollinga. Busena ST_SCROLL. */ 
        
        if(usRegHoldingBuf[HR_SCROLL_SYNC] > 0){
//            if(--scroll_sync_counter == 0u){
//                scroll_sync_counter = usRegHoldingBuf[HR_SCROLL_SYNC];
//                
                if(scroll_dir == CW){
                    SM.Control.RotDirection = CW;
                    scroll_dir = CCW;
                }else{
                    SM.Control.RotDirection = CCW;
                    scroll_dir = CW;
                }
//            }            
        }          

        (*SM.Status.pContrState) = ST_SCROLL;
        
        uint32_t rpm = (uint32)(RpmCoef * SM.microStepPace) / (*SM.Settings.pScrollingRPM);    
	    uStepTimer_WritePeriod( rpm );
        
        SM.Status.IsRun = TRUE;
        
        delay_s = SystemTimer.Timestamp_seconds + (*SM.Control.pScrollCycleOn);        
    }
    
    stage = !stage;
}


/*******************************************************************************
* Function Name: HallSensorHandler
********************************************************************************
*
* Summary:
* Hall sensor kontroleris
*
* Parameters: Nera
*
* Return: Nera
*
*******************************************************************************/
void HallSensorHandler( void ){
    
    uint16_t *pTimeout = &usRegHoldingBuf[HR_HS_TO_VALUE];
	uint16_t *pCounter = &usRegInputBuf[IR_HS_TO_COUNTER];
    uint16_t *pHallSensorTO = &usRegHoldingBuf[HR_HS_TO_VALUE];
    //static uint8_t delay = ( 0u );    
	static bool lstate = LOW;	

    if( (*pHallSensorTO) == ( 0u ) ) return;  // 0 - Hall sensorius nenaudojamas
    
    /* skaitome holo daviklio busena */
	bool state = DI2_Read();		
	
    /* indikuojam Modbase holo daviklio busena */
    xMBUtilSetBits( ucDiscInputBuf, DI_HALL_STATE, 1u, state );
    
    /* jai variklis nesisuka nuolat nuresetinam holo daviklio kounteri */
    if(SM.Status.IsRun == STOP){
		*pCounter = *pTimeout;
//        delay = ( 0u );
        return; 
    }
    
    if( state == HIGH && lstate == LOW ){
        
        /* indikuojam Modbase holo daviklio suveikima - avarija */
		xMBUtilSetBits( ucDiscInputBuf, DI_HALL_FAULT, 1, true );

        //SetBit(PORTC, RELAY_PIN); // aktyvuojam avarijos rele
		
		*pCounter = *pTimeout;
        
        //delay = ( 0u );
		
		//HallShot = true;
    }
    
    lstate = state;
    
    /* vykdom reles isjungimo uzdelsima */
    //if(++delay < 10) return;  // laukiam sekundes
    
    //delay = ( 0u );
    
    if( (*pCounter) > 0) {
        
        (*pCounter)--;
        
		return;  // laukiam taimauto
	}
    
    //ClrBit(PORTC, RELAY_PIN); // isjungiam avarijos rele
    
    
    //if( delay > SystemTimer.Timestamp_seconds ) return;    
    //delay = SystemTimer.Timestamp_seconds + HALL_SENS_TO_DEF;    
    
    
    
}


/*******************************************************************************
* Function Name: CheckVB_Value
********************************************************************************
* Summary:
* Funkcija nuolat skanuoja Vbus ADC iejima kad nustatyti nukrypima maitinimo
* itampos VB nuo nominalo ir, jai toks buvo, aktyvuoti Avarijos flaga errorFlag.
* Sis flagas kontroliojamas pagrindiniame main cikle
*
* Parameters: Nera 
*
* Return: Nera
*
*******************************************************************************/

void CheckVB_Value( void )
{
    static uint8 VoltageFaultCounter = (0u);
    
	(*SM.Status.pVbusValue) = ( ADC_SAR_Seq_1_GetResult16( VBUS_CHANNEL ) & 0x0FFF ); /* atfiltruojam zemiausius 12 bitus ADC */
    
    /* tikrinam reiksme */
    if( (*SM.Status.pVbusValue) < LVTHRESHOLD )   /* jai itampa zema */
    {        
        if( VoltageFaultCounter < 100 ) VoltageFaultCounter++;
        else 
        {
            VoltageFaultCounter = (50u);    // periodinis patikrinimas vyksta dazniau
            
            *SM.Status.pContrState = ST_FAULT;
            
            if( (*SM.Status.pVbusValue) < NOVLTHRESHOLD )
            {
                *SM.Status.pFaultCode = NoPower;
            }else{
                *SM.Status.pFaultCode = LowVolt;
            }
            xMBUtilSetBits( ucCoilBuf, CO_FAULT_FLAG, 1, true );
        }
    }
    else if( (*SM.Status.pVbusValue) > HVTHRESHOLD ) /* jai itampa auksta */
    {
        if( VoltageFaultCounter < 100 ) VoltageFaultCounter++;
        else 
        {
            VoltageFaultCounter = (50u);    // periodinis patikrinimas vyksta dazniau
            
            *SM.Status.pFaultCode = HighVolt;
            *SM.Status.pContrState = ST_FAULT;
            xMBUtilSetBits( ucCoilBuf, CO_FAULT_FLAG, 1, true );
        }
    }
    else  /* jai itampa normoje */
    {
        if( VoltageFaultCounter > 0 ) VoltageFaultCounter--;
        else 
        {
            *SM.Status.pFaultCode = noError; 
            //*SM.Status.pContrState = ST_NORMAL;
            xMBUtilSetBits( ucCoilBuf, CO_FAULT_FLAG, 1, false );
        }
    }
}


/*******************************************************************************
* Function Name: microstep_isr
********************************************************************************
*
* Summary:
* Funkcija, kuri atlieka mikrostep rezimo kontrole
*
* Parameters: Nera
*
* Return: Nera
*
*******************************************************************************/
CY_ISR(MicrostepIsrHandler)
{
    if(stepEnable)
    {
        Stepping(); /* atliekam viena mikrostepa */ 
    }	
    
    uStepTimer_ReadStatusRegister();    /* isvalom interupta ( jai naudojam UDB Timer v2.70 ) */
}

/* End of File */
