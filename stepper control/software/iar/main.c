/******************************************************************************
*   Module :  main.h
*          
*   Created:        
*   Author:   Alex(@lex)  
******************************************************************************/
/****************************    INCLUDES    *********************************/
#include "main.h"
#include "mcport.h"
#include "systick.h"
#include "stepper.h"
#include "version.h"

#include "mbutils.h"

//#if defined (USE_EEPROM)
	#include "eeprom.h"
//#endif

/***********************    DEFINES,  TYPEDEFS     ***************************/
#if defined (USE_MODBUS)
	#include "mb.h"
	#include "mbport.h"
#endif

/****************************    EXTERNS     *********************************/
extern STEPPER_CLOCK_T Clock;
extern MOTOR_CONTROL_T MotorControl;

extern USHORT usSRegHoldBuf[];
extern UCHAR  ucSCoilBuf[];
extern UCHAR  ucSDiscInBuf[];
extern USHORT usSRegInBuf[];

extern bool ScrollActive;
extern bool NewMbEvent;

/****************************    GLOBALS     *********************************/
#if defined (USE_MODBUS)
	static bool MbInitOk = false;
#endif
	
//__root __eeprom char ee_data_ok@EE_ADDR_INIT = 0xFF;

	
__root uint32_t qqq;
/****************************   PROTOTYPES   *********************************/
static void MCU_Init(void);

#if defined (USE_EEPROM)
	static void EE_DataInit(void);
#endif
	
static void HandleManualControl(void);

#if defined (USE_MODBUS)
	static void HandleModbusCommands(void);
#endif

extern void WDT_on(void);
extern void WDT_off(void);

/* ======================= Start implementation ============================ */
__C_task main( void )
{
    static uint32_t delay = 0;
	
	Adc2Rpm(1023);
	
//	EEPROM_PutChar(10, 0xAA);
//	qqq = EEPROM_GetChar(10);
//	
//	EEPROM_PutWord(10, 0x1234);
//	qqq = EEPROM_GetWord(10);
//	
//	EEPROM_PutDWord(10, 0x12345678);
//	qqq = EEPROM_GetDWord(10);
	
	
    MCU_Init();	
    SysTickInit();  /* Timer0 - SysTick(T = 1ms) */    

#if defined (USE_EEPROM)
	
	//EEPROM_PutChar(EE_ADDR_INIT, 0xFF);
	
	/* jai EEPROM neinicializuotas, inicializuojam ji */
	if( EEPROM_GetChar( EE_ADDR_INIT ) != EE_DATA_OK ) EE_DataInit();	
	//EE_DataInit();	
#endif
	
    StepperInit();  /* Timer1 - stepper clock */
    
#if defined (USE_MODBUS)
    uint8_t mbaddr = *MotorControl.Modbus.pMbAddress;
    uint16_t port_baud = *MotorControl.Modbus.pPortBaudrate;
    eMBParity port_parity = *MotorControl.Modbus.pPortParity;
    
	(void)eMBSetSlaveID(MB_UNIT_ID, TRUE, ucID_buf, sizeof(ucID_buf));    
#endif		
	
//	eMBInit( MB_RTU, 4, 0, 19200, MB_PAR_NONE );
//	(void)eMBEnable();	
	
    while(GetTicks() < 2000) __watchdog_reset();    // pauze 2 sekundes	
	
	//pSysTickTimer()->WTime.val = 0x12345678;
	
	while(1){		
		__watchdog_reset();  
        
        if(delay < GetTicks()){
            delay = GetTicks() + 100;
            
			usSRegInBuf[IR_MOTOR_WTIME_LO] = pSysTickTimer()->WTime.Lo;
			usSRegInBuf[IR_MOTOR_WTIME_HI] = pSysTickTimer()->WTime.Hi;
			
#if defined (USE_EEPROM)
			if(pSysTickTimer()->WTime.val != EEPROM_GetDWord(EE_ADDR_MOTOR_WTIME_COUNTER)){
				EEPROM_PutDWord(EE_ADDR_MOTOR_WTIME_COUNTER, pSysTickTimer()->WTime.val);				
			}
#endif	
			
			AffBit(PORTB, PB5, pSysTickTimer()->TimeBits.HalfSec);
			
			DipSwitchHandle();
			ReadAnalogPins();
			HallSensorHandle();			
            ScrollingHandle();            
            LedsHandle();
			
#if defined (USE_MODBUS) 			
            if(xMBUtilGetBits( ucSDiscInBuf, DI_MBUS_DIP, 1 ) == true){
		        // dirbame per Modbus
                /* jai parametrai Modbus pakeisti, darom reinicializacija */
                if( mbaddr != *MotorControl.Modbus.pMbAddress || 
				   	port_baud != *MotorControl.Modbus.pPortBaudrate || 
					port_parity != *MotorControl.Modbus.pPortParity ){
						
                    if( MB_ENOERR == eMBDisable()){
                        (void)eMBClose();
                        
                        mbaddr = *MotorControl.Modbus.pMbAddress;
                        port_baud = *MotorControl.Modbus.pPortBaudrate;
                        port_parity = *MotorControl.Modbus.pPortParity;
						
						#if defined (USE_EEPROM)
							if( EEPROM_GetChar( EE_ADDR_MBADDR ) != mbaddr ) EEPROM_PutChar( EE_ADDR_MBADDR, mbaddr );
							if( EEPROM_GetWord( EE_ADDR_MBBAUD ) != port_baud ) EEPROM_PutWord( EE_ADDR_MBBAUD, port_baud );
							if( EEPROM_GetChar( EE_ADDR_MBPARITY ) != port_parity ) EEPROM_PutChar( EE_ADDR_MBPARITY, port_parity );
						#endif
						
                        MbInitOk = false;
                    }                    
                }
                
                if( MbInitOk == false ){
                    if( MB_ENOERR == eMBInit( MB_RTU, mbaddr, 0, port_baud, port_parity )){
                        (void)eMBEnable();                        
                        MbInitOk = true;
						
						xMBUtilSetBits( ucSDiscInBuf, DI_DIR_DIP, 1, false );
						
						usSRegHoldBuf[HR_ROT_TO_RUN] = 0;
						
						usSRegHoldBuf[HR_PRC_SET] = 100;			
						//Prc2Rpm(usSRegHoldBuf[HR_PRC_SET]);
						
						MotorStop();
                    }
                }				
				
                HandleModbusCommands();
    		}else{
                // dirbame autonomiskai
                if( MbInitOk == true ){
                    if( MB_ENOERR == eMBDisable()){
                        (void)eMBClose();
                        MbInitOk = false;
						
						MotorStop();
                    }
                }
                
                HandleManualControl();				
			}
#else
            HandleManualControl();
#endif
			          
        }       
		       
#if defined (USE_MODBUS)
        eMBPoll();
#endif
        
	}
}




void HandleModbusCommands(){	
	
	//if( NewMbEvent == false) return;	
	if( NewMbEvent == true){
		//jai gavom nauja komanda, ziurim ar stabdyti skrolinga, ar ne
		
		NewMbEvent = false;
	}	
	
    if( xMBUtilGetBits( ucSCoilBuf, CO_CONTROL_RST, 1 ) == true ){        
        xMBUtilSetBits( ucSCoilBuf, CO_CONTROL_RST, 1, false );        
		delay_ms(500);
		((void (*)())0x0000)();
    }    
	
#if defined (USE_EEPROM)
	if( xMBUtilGetBits( ucSCoilBuf, CO_EEPROM_REINIT, 1 ) == true ){        
        xMBUtilSetBits( ucSCoilBuf, CO_EEPROM_REINIT, 1, false );        
		delay_ms(100);		
		EEPROM_PutChar( EE_ADDR_INIT, 0xFF );
		delay_ms(100);		
		((void (*)())0x0000)();
    } 
#endif
    
	if(MotorControl.MotorState == STOP){
		AffBit(PORTD, CCW_PIN, xMBUtilGetBits( ucSCoilBuf, CO_DIR, 1 ));
			
		
		if(usSRegHoldBuf[HR_ROT_TO_RUN] > 0){			
			
			AffBit(PORTD, CCW_PIN, xMBUtilGetBits( ucSDiscInBuf, DI_DIR_DIP, 1 ));			
			
			if( usSRegHoldBuf[HR_PRC_SET] > 100 ) usSRegHoldBuf[HR_PRC_SET] = 100;			
			Prc2Rpm( usSRegHoldBuf[HR_PRC_SET] );		
			
        	Clock.OcrRegister = Rpm2Ocr( usSRegInBuf[IR_RPM_SET] );
			
			MotorStart();
		}
	}	
}


void HandleManualControl(){

    if(usSRegInBuf[IR_RPM_SET] > 1) ScrollActive = false;    
    if(ScrollActive == true) return;
    
    uint8_t dir = CW;    
    
//    if( xMBUtilGetBits( ucSDiscInBuf, DI_DIR_DIP, 1 ) == true ){
//        if( BitIsClr(PINC, DIRECT_PIN) == true ) dir = CW;
//        else dir = CCW; 
//		dir = BitIsClr(PINC, DIRECT_PIN);
//    }else{
//        if( BitIsClr(PINC, DIRECT_PIN) == true ) dir = CCW;
//        else dir = CW;
//		dir = BitIsSet(PINC, DIRECT_PIN);
//    } 	
	dir = ( xMBUtilGetBits( ucSDiscInBuf, DI_DIR_DIP, 1 ) ) ? BitIsClr(PINC, DIRECT_PIN) : BitIsSet(PINC, DIRECT_PIN);
    xMBUtilSetBits( ucSCoilBuf, CO_DIR, 1, dir );    

    if(usSRegInBuf[IR_RPM_SET] > 1){
		
		if( usSRegInBuf[IR_EXTERN_SET] < ADC_ON_RAMP ) return;
		
        usSRegHoldBuf[HR_ROT_TO_RUN] = CONTINOUS_RUN;
        Clock.OcrRegister = Rpm2Ocr(usSRegInBuf[IR_RPM_SET]);
        MotorStart();
		
    }else{
        
		if( usSRegInBuf[IR_EXTERN_SET] > ADC_OFF_RAMP ) return;
		
        MotorStop();
		
		AffBit(PORTD, CCW_PIN, dir);
    }
}



void MCU_Init()
{        
    DDRB = DDRC = DDRD = 0x00;
    DDRB = (1<<DE_PIN)|(1<<CLOCK_PIN)|(1<<PB5);
    DDRC = (1<<RELAY_PIN)|(1<<LED3_PIN)|(1<<LED2_PIN);
    DDRD = (1<<LED1_PIN)|(1<<ENABLE_PIN)|(1<<CCW_PIN)|(1<<HALF_PIN)|(1<<TX_PIN);
	
	/* Konfiguruojam Pull-up */
	PORTB = PORTC = PORTD = 0x00;
	PORTB = (1<<MAX_SPEED_PIN)|(1<<ROT_PIN);//PORTB = (1<<HSENS_PIN)|(1<<MAX_SPEED_PIN)|(1<<ROT_PIN);	
	PORTC = (1<<SCROLL_PIN)|(1<<DIRECT_PIN);
	PORTD = (1<<DIR_PIN)|(1<<MBUS_PIN)|(1<<TX_PIN);
    
    MCUCR = TIMSK = 0x00;
    
    __enable_interrupt();    
    
    WDT_on();
}


#if defined (USE_EEPROM)
static void EE_DataInit(){
	
	while( EEPROM_GetChar( EE_ADDR_MBADDR ) != MB_ADDRESS_DEF ){
		EEPROM_PutChar( EE_ADDR_MBADDR, MB_ADDRESS_DEF );
		delay_ms(1);
	}
	
	while( EEPROM_GetWord( EE_ADDR_MBBAUD ) != PORT_BAUDRATE_DEF ){
		EEPROM_PutWord( EE_ADDR_MBBAUD, PORT_BAUDRATE_DEF );
		delay_ms(1);
	}
	
	while( EEPROM_GetChar( EE_ADDR_MBPARITY ) != PORT_PARITY_DEF ){
		EEPROM_PutChar( EE_ADDR_MBPARITY, PORT_PARITY_DEF );
		delay_ms(1);
	}
	
	while( EEPROM_GetChar( EE_ADDR_MAX_RPM ) != MAX_RPM_HIGH_DEF ){
		EEPROM_PutChar( EE_ADDR_MAX_RPM, MAX_RPM_HIGH_DEF );
		delay_ms(1);
	}
	
	while( EEPROM_GetChar( EE_ADDR_MIN_RPM ) != MIN_RPM_DEF ){
		EEPROM_PutChar( EE_ADDR_MIN_RPM, MIN_RPM_DEF );
		delay_ms(1);
	}
	
	while( EEPROM_GetChar( EE_ADDR_SCROLL_TIMEOUT ) != SCROLL_TO_MINS_DEF ){
		EEPROM_PutChar( EE_ADDR_SCROLL_TIMEOUT, SCROLL_TO_MINS_DEF );
		delay_ms(1);
	}
	
	while( EEPROM_GetChar( EE_ADDR_SCROLL_PRC ) != SCROLL_PRC_DEF ){
		EEPROM_PutChar( EE_ADDR_SCROLL_PRC, SCROLL_PRC_DEF );
		delay_ms(1);
	}
	
	while( EEPROM_GetChar( EE_ADDR_SCROLL_ROTATIONS ) != SCROLL_ROTATIONS_DEF ){
		EEPROM_PutWord( EE_ADDR_SCROLL_ROTATIONS, SCROLL_ROTATIONS_DEF );
		delay_ms(1);
	}
	
	while( EEPROM_GetChar( EE_ADDR_HALL_SENSOR_TIMEOUT ) != HALL_SENSOR_FAULT_TO_DEF ){
		EEPROM_PutWord( EE_ADDR_HALL_SENSOR_TIMEOUT, HALL_SENSOR_FAULT_TO_DEF );
		delay_ms(1);
	}
	
	while( EEPROM_GetDWord( EE_ADDR_MOTOR_WTIME_COUNTER ) != 0x00000000 ){
		EEPROM_PutDWord( EE_ADDR_MOTOR_WTIME_COUNTER, 0x00000000 );
		delay_ms(1);
	}
	
	EEPROM_PutChar( EE_ADDR_INIT, EE_DATA_OK );
}
#endif