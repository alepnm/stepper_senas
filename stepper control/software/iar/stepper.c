/******************************************************************************
*   Module :  stepper.c
*          
*   Created:        
*   Author:   Alex(@lex)  
******************************************************************************/
/****************************    INCLUDES    *********************************/
#include "stepper.h"
#include "stepper_port.h"
#include "mcport.h"
#include "mbport.h"
#include "systick.h"
#include "mbutils.h"
#include "eeprom.h"

/***********************    DEFINES,  TYPEDEFS     ***************************/
#define STEPS_PER_ROTATION_DEF			(200u)
#define ALWAYS_ENABLE_DEF       		FALSE
#define HALF_STEP_DEF           		TRUE

#define STEP_PULSE()		    		ClrBit(PORTB,CLOCK_PIN);	\
							    		delay_us(1);	            \
							    		SetBit(PORTB,CLOCK_PIN);	\
      
#define STEPPER_TIM_PRESCALER_MASK		0x03
#define STEPPER_TIM_PRESCALER           (64u)

#define ADC_AVGFACTOR					(4u)



/****************************    EXTERNS     *********************************/
extern USHORT usSRegHoldBuf[];
extern UCHAR  ucSCoilBuf[];
extern UCHAR  ucSDiscInBuf[];
extern USHORT usSRegInBuf[];

extern bool led;
/****************************    GLOBALS     *********************************/
STEPPER_CLOCK_T Clock, *pClock = &Clock;

MOTOR_CONTROL_T MotorControl = {
    .pSW_Version = &usSRegInBuf[IR_SW_VERSION],
    .pHW_Version = &usSRegInBuf[IR_HW_VERSION],
    .Alarm = false,
    .MotorState = STOP,
	.RunUp = false,
	.RunDown = false,
    .Modbus = {
		.pMbAddress = &usSRegHoldBuf[HR_MB_ADDRESS],
		.pPortBaudrate = &usSRegHoldBuf[HR_PORT_BAUDRATE],
		.pPortParity = (eMBParity*)&usSRegHoldBuf[HR_PORT_PARITY],
        .pCoils = ucSCoilBuf,
		.pDescreetes = ucSDiscInBuf,
        .pHoldings = usSRegHoldBuf,
		.pInputs = usSRegInBuf,
		.pSlaveIdBuffer = NULL       
    }
};
MOTOR_CONTROL_T *pMotorControl = &MotorControl;


__flash uint8_t ucRpm150[100] = {
     0,   0,   0,   1,   1,   1,   1,   1,   2,   2,
     2,   2,   3,   3,   3,   3,   4,   4,   4,   4,
     5,   5,   5,   5,   6,   6,   6,   7,   7,   8,
     8,   8,   9,   9,  10,  10,  11,  12,  12,  13,
    14,  14,  15,  16,  16,  17,  18,  19,  20,  20,
    21,  22,  23,  24,  25,  26,  27,  28,  29,  30,
    31,  32,  33,  35,  36,  37,  38,  40,  41,  42,
    44,  45,  47,  48,  50,  52,  54,  56,  58,  60,
    62,  65,  67,  70,  73,  76,  79,  83,  87,  91,
    95, 100, 105, 110, 115, 121, 128, 135, 142, 150
};
__flash uint8_t ucRpm200[100] = {
      0,   1,   1,   1,   1,   2,   2,   2,   3,   3,
      3,   3,   4,   4,   4,   4,   5,   5,   5,   6,
      6,   6,   7,   7,   8,   8,   9,   9,  10,  10,
     11,  11,  12,  13,  13,  14,  15,  15,  16,  17,
     18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
     28,  29,  31,  32,  33,  35,  36,  37,  39,  40,
     41,  43,  44,  46,  48,  49,  51,  53,  55,  56,
     58,  60,  62,  65,  67,  69,  72,  74,  77,  80,
     83,  86,  90,  93,  97, 101, 106, 111, 116, 121,
    127, 133, 139, 146, 154, 162, 171, 180, 189, 200
};


bool ScrollActive = false;	// SCROLL rezimo aktyvumo flagas
bool HallShot = false;	// holo daviklio suveikimo flagas
uint16_t StepCounter = STEPS_PER_ROTATION_DEF;

/****************************   PROTOTYPES   *********************************/
static void StopClockTimer(void);
static void StartClockTimer(void);

extern uint16_t ReadADC( uint8_t channel );
extern int16_t calc_temperature(uint16_t adcsum);
/* ======================= Start implementation ============================ */


/******************************************************************************
Function name:          StepperInit
Function description:
Parameters:

Output:
******************************************************************************/
void StepperInit(){

    STEPPER_TIM_TCCRA = STEPPER_TIM_TCCRB = 0x00;
    STEPPER_TIM_TCCRB = (1<<STEPPER_TIM_WGM2); // Configure Timer 1 for CTC mode
	STEPPER_TIM_TIMSK |= (1<<STEPPER_TIM_OCIE);	// Timer 1 Counter Compare interrupts enable	

    StopClockTimer();
    
    Clock.OcrRegister = 100;	
    Clock.PrescalerMask = STEPPER_TIM_PRESCALER_MASK;
    Clock.Prescaler = STEPPER_TIM_PRESCALER;
    
	/* Konfiguruojam ADC */
	STEPPER_ADCSR = 0x00;
	STEPPER_ADCSR = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS0); // enabling ADC, set division factor 32
	STEPPER_ADMUX = (1<<REFS0);   
    
    usSRegInBuf[IR_SW_VERSION] = SW_VERSION;
    usSRegInBuf[IR_HW_VERSION] = HW_VERSION;
   
#if defined (USE_MODBUS)
	#if defined (USE_EEPROM)
		usSRegHoldBuf[HR_MB_ADDRESS] = EEPROM_GetChar( EE_ADDR_MBADDR );
		usSRegHoldBuf[HR_PORT_BAUDRATE] = EEPROM_GetWord( EE_ADDR_MBBAUD );
		usSRegHoldBuf[HR_PORT_PARITY] = EEPROM_GetChar( EE_ADDR_MBPARITY );
		usSRegHoldBuf[HR_PORT_STOPBITS] = PORT_STOPBITS_DEF;
		usSRegHoldBuf[HR_PORT_DATABITS] = PORT_DATABITS_DEF; 
	#else
		usSRegHoldBuf[HR_MB_ADDRESS] = MB_ADDRESS_DEF;
		usSRegHoldBuf[HR_PORT_BAUDRATE] = PORT_BAUDRATE_DEF;
		usSRegHoldBuf[HR_PORT_PARITY] = PORT_PARITY_DEF;
		usSRegHoldBuf[HR_PORT_STOPBITS] = PORT_STOPBITS_DEF;
		usSRegHoldBuf[HR_PORT_DATABITS] = PORT_DATABITS_DEF; 
	#endif	/* USE_EEPROM */
#endif	/* USE_MODBUS */	
		
		
#if defined (USE_EEPROM)
    usSRegHoldBuf[HR_MAX_RPM_HIGH] = EEPROM_GetChar( EE_ADDR_MAX_RPM );
    usSRegHoldBuf[HR_MAX_RPM_LOW] = EEPROM_GetChar( EE_ADDR_MIN_RPM );
	usSRegHoldBuf[HR_MIN_RPM] = EEPROM_GetChar( EE_ADDR_RPM );
	
	usSRegHoldBuf[HR_SCROLL_TIMEOUT] = EEPROM_GetChar( EE_ADDR_SCROLL_TIMEOUT );
	usSRegHoldBuf[HR_SCROLL_ROTATIONS] = EEPROM_GetWord( EE_ADDR_SCROLL_ROTATIONS );
	usSRegHoldBuf[HR_HALL_SENSOR_TIMEOUT] = EEPROM_GetWord( EE_ADDR_HALL_SENSOR_TIMEOUT );
	
	usSRegInBuf[IR_MOTOR_WTIME_HI] = EEPROM_GetWord( EE_ADDR_MOTOR_WTIME_COUNTER_HI );
	usSRegInBuf[IR_MOTOR_WTIME_LO] = EEPROM_GetWord( EE_ADDR_MOTOR_WTIME_COUNTER_LO );
	
	usSRegInBuf[HR_STEPS_PER_ROTATION] = EEPROM_GetWord( EE_ADDR_STEPS_PER_ROT );
	usSRegHoldBuf[HR_SCROLL_PRC] = EEPROM_GetChar( EE_ADDR_SCROLL_PRC );
#else
    usSRegHoldBuf[HR_MAX_RPM_HIGH] = MAX_RPM_HIGH_DEF;
    usSRegHoldBuf[HR_MAX_RPM_LOW] = MAX_RPM_LOW_DEF;
	usSRegHoldBuf[HR_MIN_RPM] = MIN_RPM_DEF;	

	usSRegHoldBuf[HR_SCROLL_TIMEOUT] = SCROLL_TO_MINS_DEF;	
	usSRegHoldBuf[HR_SCROLL_ROTATIONS] = SCROLL_ROTATIONS_DEF;
	usSRegHoldBuf[HR_HALL_SENSOR_TIMEOUT] = HALL_SENSOR_FAULT_TO_DEF;
	
	usSRegInBuf[IR_MOTOR_WTIME_LO] = usSRegInBuf[IR_MOTOR_WTIME_HI] = (0u);
	
	usSRegHoldBuf[HR_STEPS_PER_ROTATION] = STEPS_PER_ROTATION_DEF;
	usSRegHoldBuf[HR_SCROLL_PRC] = SCROLL_PRC_DEF;
#endif	/* USE_EEPROM */
		
	usSRegHoldBuf[HR_MAX_RPM] = ( BitIsClr(PINB, MAX_SPEED_PIN) ) ? usSRegHoldBuf[HR_MAX_RPM_HIGH] : usSRegHoldBuf[HR_MAX_RPM_LOW];
    
	pSysTickTimer()->WTime.Hi = usSRegInBuf[IR_MOTOR_WTIME_HI];
	pSysTickTimer()->WTime.Lo = usSRegInBuf[IR_MOTOR_WTIME_LO];

    Prc2Rpm( (uint8_t)usSRegHoldBuf[HR_SCROLL_PRC] );
    
	usSRegInBuf[IR_SCROLL_COUNTER] = usSRegHoldBuf[HR_SCROLL_TIMEOUT];
	usSRegInBuf[IR_HALL_TO_COUNTER] = usSRegHoldBuf[HR_HALL_SENSOR_TIMEOUT];
	
	usSRegInBuf[IR_EXTERN_SET] = ReadADC(SC_010_ADC_CHANNEL);
    usSRegInBuf[IR_VVX_TYPE] = ReadADC(SC_VVX_TYPE_CHANNEL);
    usSRegInBuf[IR_EXTERN_SET] = ReadADC(SC_010_ADC_CHANNEL);
    
    xMBUtilSetBits( ucSCoilBuf, CO_ALWAYS_ENABLE, 1, ALWAYS_ENABLE_DEF );
    xMBUtilSetBits( ucSCoilBuf, CO_HALF_STEP, 1, HALF_STEP_DEF );
    
    usSRegHoldBuf[HR_ROT_TO_RUN] = 0;//CONTINOUS_RUN;	
	usSRegInBuf[IR_MOTOR_WTIME_LO] = usSRegInBuf[IR_MOTOR_WTIME_HI] = 0;
	
    MotorControl.Alarm = LOW;    
    
	AffBit(PORTD, HALF_PIN, xMBUtilGetBits( ucSCoilBuf, CO_HALF_STEP, 1 ));
	AffBit(PORTD, ENABLE_PIN, xMBUtilGetBits( ucSCoilBuf, CO_ALWAYS_ENABLE, 1 ));
    
    SetBit(PORTC, RELAY_PIN);
    MotorStop();
}


/******************************************************************************
Function name: StopClockTimer
Function description:
Parameters:

Output:
******************************************************************************/
static void StopClockTimer(){    
    STEPPER_TIM_TCCRB &= 0xF8;  					// stabdom taimeri    
    ClrBit(STEPPER_TIM_TIMSK, STEPPER_TIM_OCIE);  	// isjungiam interaptus 
}


/******************************************************************************
Function name: StartClockTimer
Function description:
Parameters:

Output:
******************************************************************************/
static void StartClockTimer(){  
	STEPPER_TIM_TCNT = 0x00;
    STEPPER_TIM_OCR = Clock.OcrRegister;    
    STEPPER_TIM_TCCRB |= Clock.PrescalerMask;  		// startuojam taimeri
    SetBit(STEPPER_TIM_TIMSK, STEPPER_TIM_OCIE);  	// ijungiam interaptus 
}



void MotorStart(){

	SetBit(PORTD, ENABLE_PIN);
	
	StepCounter = usSRegHoldBuf[HR_STEPS_PER_ROTATION];	
	if( xMBUtilGetBits( ucSCoilBuf, CO_HALF_STEP, 1 ) == TRUE ) StepCounter *= 2;
	//if( HALF_STEP_DEF == TRUE ) StepCounter *= 2;
		
    StartClockTimer();
	
	xMBUtilSetBits( ucSDiscInBuf, DI_MOTOR_RUN, 1, true );
    MotorControl.MotorState = RUN;
}


void MotorStop(){ 
	
	StepCounter = 0;
	
    StopClockTimer();
    
    if( xMBUtilGetBits( ucSCoilBuf, CO_ALWAYS_ENABLE, 1 ) == false ){
		delay_ms(1);
        ClrBit(PORTD, ENABLE_PIN);
    }
	
	xMBUtilSetBits( ucSDiscInBuf, DI_MOTOR_RUN, 1, false );	
    MotorControl.MotorState = STOP;
}


/*******************************************************************************
Function name: Adc2Rpm
Function description: Konvertuoja ADC reiksme i RPM. Konvertavimui naudoja RPM
presetu lenteles. Rezultatus krauna tiesiai i Modbus registrus:
    procentus i registra usSRegInBuf[IR_PROC_SET],
    RPM i regista usSRegInBuf[IR_RPM_SET]
Parameters: uint16_t adc - ADC reiksme

Output: nera
*******************************************************************************/
void Adc2Rpm(uint16_t adc){
    uint16_t xmin = 50, xmax = 100;  //-> procentai
    uint16_t ymin = 1, ymax = 1012; //-> ADC. ymax = ADC max - ADC_AVGFACTOR (8)
    
    float f = 0;
    
    if(adc < 5) adc = 1;
    else if(adc > ymax) adc = ymax;   
    
    /* paskaiciuojam is ADC reiksmes koeficienta */
	f = ((float)adc-ymin)/(ymax-ymin);

	/* paskaiciuojam procentus panaudodami koeficienta */
	usSRegInBuf[IR_PROC_SET] = (uint8_t)(xmin + (xmax-xmin) * f);

	Prc2Rpm( usSRegInBuf[IR_PROC_SET] );
}


/*******************************************************************************
Function name: Prc2Rpm
Function description: Konvertuoja procentu reiksme i RPM. Konvertavimui naudoja RPM
presetu lenteles. Rezultata krauna tiesiai i Modbus registra usSRegInBuf[IR_RPM_SET]
Parameters: uint8_t prc - procentu reiksme

Output: nera
*******************************************************************************/
void Prc2Rpm(uint8_t prc){
	
	/* is procentu gaunam is lenteles RPM'us */	
	usSRegInBuf[IR_RPM_SET] = (usSRegHoldBuf[HR_MAX_RPM] == MAX_RPM_LOW_DEF) ? ucRpm150[prc-1] : ucRpm200[prc-1];
	
	/* tikrinam ar apsukos nevirsyja maksimaliu nustatytu RPM'u */
	//if( usSRegInBuf[IR_RPM_SET] > usSRegHoldBuf[HR_MAX_RPM] ) usSRegInBuf[IR_RPM_SET] = usSRegHoldBuf[HR_MAX_RPM];
	
	/* tikrinam ar apsukos nezemesnes uz minimalius nustatytus RPM'us */
	//if( usSRegInBuf[IR_RPM_SET] < usSRegHoldBuf[HR_MIN_RPM] ) usSRegInBuf[IR_RPM_SET] = usSRegHoldBuf[HR_MIN_RPM];
}


/*******************************************************************************
Function name: Rpm2Ocr
Function description: Konvertuoja RPM reikme i OCR reiksme taimeriui
Parameters: uint16_t rpm - apsisukimu per minute reiksme

Output: uint16_t OCR reiksme
*******************************************************************************/
uint16_t Rpm2Ocr(uint16_t rpm){ 
    
    if( xMBUtilGetBits( ucSCoilBuf, CO_HALF_STEP, 1 ) == TRUE ) rpm *= 2;
	//if( HALF_STEP_DEF == TRUE ) rpm *= 2;
    
    float freq = (rpm * usSRegHoldBuf[HR_STEPS_PER_ROTATION])/60; 
    return (uint16_t)(F_CPU/(Clock.Prescaler * freq));
}



/******************************************************************************
Function name: ReadAnalogPins
Function description:
Parameters: nera

Output: nera
******************************************************************************/
void ReadAnalogPins(){

	static uint8_t n = T_NUM_OF_MEASURMENTS;
	static uint8_t channel = SC_HSINK_TEMP_CHANNEL;
	static int16_t hsink_tmp, vvx_tmp, pwm_tmp;	
	
	switch( channel ){
		case SC_HSINK_TEMP_CHANNEL:
			if(n-- > 0){
				hsink_tmp += ReadADC(SC_HSINK_TEMP_CHANNEL);
			}else{
				n = T_NUM_OF_MEASURMENTS;
				usSRegInBuf[IR_INT_TEMPERATURE] = calc_temperature( hsink_tmp );
				hsink_tmp = 0;
			}
			channel = SC_VVX_TYPE_CHANNEL;
			break;
		case SC_VVX_TYPE_CHANNEL:
			vvx_tmp = vvx_tmp + ((int16_t)( ReadADC(SC_VVX_TYPE_CHANNEL) - vvx_tmp )>>ADC_AVGFACTOR);
    		usSRegInBuf[IR_VVX_TYPE] = vvx_tmp;			

			channel = SC_010_ADC_CHANNEL;
			break;
		case SC_010_ADC_CHANNEL:
			
			usSRegInBuf[IR_EXTERN_SET] = ReadADC(SC_010_ADC_CHANNEL);

			pwm_tmp = pwm_tmp + ((int16_t)( ReadADC(SC_010_ADC_CHANNEL) - pwm_tmp )>>ADC_AVGFACTOR);
    		usSRegInBuf[IR_EXTERN_SET] = pwm_tmp;

            Adc2Rpm(usSRegInBuf[IR_EXTERN_SET]);

			channel = SC_HSINK_TEMP_CHANNEL;
			break;
	}

	ADMUX &= 0xF8;	//
	delay_ms(1);
}


/******************************************************************************
Function name: DipSwitchHandle
Function description: DIP jungikliu tvarkykle
Parameters: nera

Output: nera
******************************************************************************/
void DipSwitchHandle(){
			
//	if( BitIsSet(PINB, MAX_SPEED_PIN) == true ){
//		usSRegHoldBuf[HR_MAX_RPM] = MAX_RPM_LOW_DEF;
//		xMBUtilSetBits( ucSDiscInBuf, DI_SPEED_DIP, 1, true );
//	}else{
//		usSRegHoldBuf[HR_MAX_RPM] = MAX_RPM_HIGH_DEF;
//		xMBUtilSetBits( ucSDiscInBuf, DI_SPEED_DIP, 1, false );
//	}

	usSRegHoldBuf[HR_MAX_RPM] = ( BitIsSet(PINB, MAX_SPEED_PIN) == true ) ? MAX_RPM_LOW_DEF : MAX_RPM_HIGH_DEF;
	xMBUtilSetBits( ucSDiscInBuf, DI_SPEED_DIP, 1, BitIsSet(PINB, MAX_SPEED_PIN) );
	
	if( BitIsClr(PIND, MBUS_PIN) ){
		/* dirbame per Modbusa */
		xMBUtilSetBits( ucSDiscInBuf, DI_MBUS_DIP, 1, true );
		
	}else{
		/* dirbame tiesiogiai */
		xMBUtilSetBits( ucSDiscInBuf, DI_MBUS_DIP, 1, false );		
		xMBUtilSetBits( ucSDiscInBuf, DI_HALL_DIP, 1, BitIsSet(PINB, ROT_PIN) );		
		xMBUtilSetBits( ucSDiscInBuf, DI_DIR_DIP, 1, BitIsSet(PIND, DIR_PIN) );
		xMBUtilSetBits( ucSDiscInBuf, DI_SCROLL_DIP, 1, BitIsSet(PINC, SCROLL_PIN) );
	}
}


/******************************************************************************
Function name: LedsHandle
Function description: LED indikacijos tvarkykle
Parameters: nera

Output: nera
******************************************************************************/
void LedsHandle(){
    
    if(BitIsClr(PORTC, RELAY_PIN)){
		AffBit(PORTD, LED1_PIN, pSysTickTimer()->TimeBits.HalfSec);		
    }else{        
		AffBit(PORTD, LED1_PIN, HallShot);
		HallShot = false;
    }    
    
    if(ScrollActive == true){				
		AffBit(PORTC, LED2_PIN, pSysTickTimer()->TimeBits.HalfSec);		
    }else{
		AffBit(PORTC, LED2_PIN, MotorControl.MotorState);
    }    
}


/*******************************************************************************
Function name: HallSensorHandle
Function description: Handle Hall sensor input. Period = 100ms
Parameters: nera

Output: nera
*******************************************************************************/
void HallSensorHandle(){

    uint16_t *pTimeout = &usSRegHoldBuf[HR_HALL_SENSOR_TIMEOUT];
	uint16_t *pCounter = &usSRegInBuf[IR_HALL_TO_COUNTER];
    static uint8_t dly = 0;    
	static bool lstate = LOW;	

	bool state = GetBit(PINB, HSENS_PIN);		
	
    /* indikuojam Modbase holo daviklio busena */
	xMBUtilSetBits( ucSDiscInBuf, DI_HALL_S_STATE, 1, state );
    
    /* jai variklis nesisuka nuolat nuresetinam holo daviklio kounteri */
    if(MotorControl.MotorState == STOP){
		*pCounter = *pTimeout;
        dly = 0;
        return; 
    }
    
    if( state == HIGH && lstate == LOW ){
        
        /* indikuojam Modbase holo daviklio suveikima */
		xMBUtilSetBits( ucSDiscInBuf, DI_HALL_S_PULSE, 1, !xMBUtilGetBits( ucSDiscInBuf, DI_HALL_S_PULSE, 1 ) );

        SetBit(PORTC, RELAY_PIN);
		
		*pCounter = *pTimeout;
        dly = 0;
		
		HallShot = true;
    }
    
    lstate = state;
    
    /* vykdom reles isjungimo uzdelsima */
    if(++dly < 10) return;  // laukiam sekundes
    dly = 0;
    
    if( *pCounter > 0) {
		*pCounter--;
		return;  // laukiam taimauto
	}
    
    ClrBit(PORTC, RELAY_PIN);
}


/*******************************************************************************
Function name: ScrollingHandle
Function description: Scroll handler. Period = 100ms
Parameters: nera

Output: nera
*******************************************************************************/

#warning yra negerai: jai skrolingo metu pareis modbus komanda motorui paleisti,
#warning si komanda nebus ivykdyta!!! 

void ScrollingHandle(){
	
    STEPPER_CLOCK_T clock;
    uint16_t *pTimeout = &usSRegHoldBuf[HR_SCROLL_TIMEOUT];
	uint16_t *pCounter = &usSRegInBuf[IR_SCROLL_COUNTER];
    static uint16_t dly = 0;
    static bool dir = CW;
	
	
    if( xMBUtilGetBits( ucSDiscInBuf, DI_SCROLL_DIP, 1 ) == true || MotorControl.MotorState == RUN ){
		*pCounter = *pTimeout;
        dly = 0;
        return;
    }	
	
    if(dly++ < 600) return;	
    dly = 0;
	
    if( *pCounter > 0 ) {
		*pCounter--;
		return;  // laukiam taimauto  
	}
	
	//ChangeBit(PORTC, LED3_PIN);
	
    // prasukam rotoriu    
    ScrollActive = true;
    
	dir = !dir;
	AffBit(PORTD, CCW_PIN, dir);
	
	xMBUtilSetBits( ucSCoilBuf, CO_DIR, 1, dir );
	
	clock = Clock;	//issaugojam taimerio nustatymus
	
    usSRegHoldBuf[HR_ROT_TO_RUN] = usSRegHoldBuf[HR_SCROLL_ROTATIONS];    
    
	/* greiti imam is lenteles */
    //Prc2Rpm(SCROLL_PRC_DEF);    
    //usSRegHoldBuf[HR_SCROLL_RPM] = usSRegInBuf[IR_RPM_SET];	
	//Clock.OcrRegister = Rpm2Ocr(usSRegHoldBuf[HR_SCROLL_RPM]);
	
	/* arba perskaiciuojame procentaliai*/
	uint8_t rpm = (usSRegHoldBuf[HR_MAX_RPM] * SCROLL_PRC_DEF)/100;
	
    Clock.OcrRegister = Rpm2Ocr(rpm);	
    MotorStart();
	
	Clock = clock;	//atstatom taimerio nustatymus
    
    //ChangeBit(PORTC, LED3_PIN);
}


/*****************************************************************************/
#pragma vector=STEPPER_TIM_ISR_HANDLER
__interrupt void TMR_COMPA_Handler()
{ 	   	
	uint16_t *pCounter = &usSRegHoldBuf[HR_ROT_TO_RUN];
	
	if(*pCounter > 0){		
		if(*pCounter != CONTINOUS_RUN && StepCounter == 0){
			*pCounter--; 
			StepCounter = usSRegHoldBuf[HR_STEPS_PER_ROTATION];
			if( xMBUtilGetBits( ucSCoilBuf, CO_HALF_STEP, 1 ) == TRUE ) StepCounter *= 2;
			//if( HALF_STEP_DEF == TRUE ) StepCounter *= 2;
			
			//usSRegHoldBuf[HR_DBG1] = StepCounter;
		}else{
			STEP_PULSE();
			
			//SetBit(PORTC, LED3_PIN);
			//delay_us(100);
			//ClrBit(PORTC, LED3_PIN); 
			
			StepCounter--;
		}
   
		return;
	}
	
    ScrollActive = false;
    
	MotorStop();	
}
/*=============================================================================
END OF FILE 
=============================================================================*/
