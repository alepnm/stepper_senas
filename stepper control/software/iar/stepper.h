/****************************************************************
// stepper.h
//
// Created: 11/27/2014 1:54:37 PM
//  Author: Alex
//
****************************************************************/
#if !defined (STEPPER_H)
#define STEPPER_H
/****************************    INCLUDES    *********************************/
#include "defs.h"
#include "mbport.h"


#define USE_MODBUS // ijungiam/isjungiam USE_MODBUS ir "mb.h" faile!

/***********************    DEFINES,  TYPEDEFS     ***************************/
#define MB_UNIT_ID				    (41u)
#define SW_VERSION				    (100u)	// versija - 1.00
#define HW_VERSION				    (101u)	// versija - 1.00

#if defined (USE_MODBUS)
	#define MB_ADDRESS_DEF			(4u)
	#define PORT_PARITY_DEF			MB_PAR_NONE
	#define PORT_STOPBITS_DEF		(1u)
	#define PORT_BAUDRATE_DEF		(19200u)
	#define PORT_DATABITS_DEF		(8u)
#endif

#define ADC_ON_RAMP					(1023*1.1)/5	// variklio ijungimo slenkstis (ADC) 	=225
#define ADC_OFF_RAMP				(1023*0.6)/5	// variklio isjungimo slenkstis (ADC)	=123

#define SC_010_ADC_CHANNEL		    (1u)
#define SC_VVX_TYPE_CHANNEL		    (6u)
#define SC_HSINK_TEMP_CHANNEL	    (7u)

#define MAX_RPM_LOW_DEF		    	(120u)
#define MAX_RPM_HIGH_DEF	    	(160u)//(200u)
#define MIN_RPM_DEF			    	(1u)

#define SCROLL_TO_MINS_DEF      	(10u)	//skrolingo taimautas (10 minuciu)
#define SCROLL_PRC_DEF	        	(10u)	//skrolingo greitis (10% nuo nustatytos DIP jungikliu MAX_RPM)
#define SCROLL_ROTATIONS_DEF    	(8u)	//apsisukimu skaicius skrolingo metu
#define HALL_SENSOR_FAULT_TO_DEF    (30u)   // sec

#define CONTINOUS_RUN               0xFFFF

#define T_NUM_OF_MEASURMENTS        (64u)

/*****   HOLDING REGISTERS     	REG_ADDR    ************************/
#define HR_STEPS_PER_ROTATION   	(1u)	// zingsniu per apsisukima
#define HR_START_TIME           	(2u)    // variklio isibegejimo(startavimo) laikas
#define HR_MB_ADDRESS				(3u)	// modbus adresas
#define HR_PORT_PARITY				(4u)	// porto nustatymai: parity
#define HR_PORT_STOPBITS			(5u)	// porto nustatymai: stopbitai
#define HR_PORT_BAUDRATE			(6u)	// porto nustatymai: baudrate
#define HR_PORT_DATABITS			(7u)	// porto nustatymai: databitai

#define HR_ROT_TO_RUN				(10u)	// apsisukimu skaicius prasukimui
#define HR_PRC_SET					(11u)	// greitis procentais
#define HR_MAX_RPM_LOW          	(13u)   // maksimalus RPM low
#define HR_MAX_RPM_HIGH         	(14u)   // maksimalus RPM high
#define HR_MIN_RPM              	(15u)	// minimalus greitis RPM'ais
#define HR_MAX_RPM              	(16u)	// aktyvus maksimalus RPM: HR_MAX_RPM_LOW/HR_MAX_RPM_HIGH

#define HR_SCROLL_TIMEOUT       	(23u)	// scrollingo periodas: minutes
#define HR_SCROLL_PRC        		(24u)	// scrollingo apsukos (procentai nuo maksimaliu HR_MAX_RPM)
#define HR_SCROLL_ROTATIONS     	(25u)	// scrollingo apsisukimu skaicius
#define HR_HALL_SENSOR_TIMEOUT  	(26u)	// HALL sensorio tikrinimo periodas: sekundes

#define HR_DBG1						(30u)
#define HR_DBG2						(31u)

/*****    COILS     				COIL ADDR		*******************/
//#define CO_MOTOR_ON				(0u)	// variklio paleidimas/stabdymas
//#define CO_RESET_ALARM			(1u)	
#define CO_DIR						(3u)	// variklio sukimosi kriptis
#define CO_EEPROM_REINIT			(7u)	// restartas su EEPROM inicializacijos aktyvavimu
#define CO_CONTROL_RST  			(8u)	// kontrollerio soft-resetas
#define CO_HALF_STEP            	(9u)    // full/half step
#define CO_ALWAYS_ENABLE        	(10u)   // stepperio kontrolerio ENABLE rezimas: false - isjungtas, true - ijungtas

/*****    INPUTS   				INPUT ADDR		***********************/
#define IR_VVX_TYPE					(0u)	// kontrolerio nustatytas VREF
#define IR_INT_TEMPERATURE			(1u)	// kontrolerio radiatoriaus temperatura (grad * 10)
#define IR_EXTERN_SET				(5u)	// 0-10 iejimo reiksme: 0=0%(U<=1.1V), 10000=100%(U>=9.5V)
#define IR_PROC_SET					(6u)	// apskaiciuoti % pagal IR_EXTERN_SET
#define IR_RPM_SET					(7u)	// apskaiciuoti RPM pagal IR_EXTERN_SET
#define IR_MOTOR_WTIME_LO			(8u)	// variklio veikimo bendras laikas LOW int
#define IR_MOTOR_WTIME_HI			(9u)	// variklio veikimo bendras laikas HIGH int
#define IR_MOTOR_WTIME_DAYS			(10u)	// variklio veikimo bendras laikas (dienom)
#define IR_HALL_TO_COUNTER			(11u)	// holo daviklio tikrinimo periodo kounteris
#define IR_SCROLL_COUNTER			(12u)	// scrollingo periodo kounteris
#define IR_SW_VERSION				(14u)	// sw versija
#define IR_HW_VERSION				(15u)	// hw versija

/*****    DESCREETS   			DESCREET ADDR		*******************/
#define DI_MOTOR_RUN				(0u)	// rodo ar sukasi variklis, ar stovi
#define DI_HALL_SENS_FAULT			(2u)
#define DI_MOTOR_FAULT				(3u)
#define DI_OVERHEAT					(4u)	// 
#define DI_HALL_DIP					(5u)	// rodo HALL sensorio dipuko busena
#define DI_SPEED_DIP				(6u)	// rodo SPEED dipuko busena
#define DI_DIR_DIP					(7u)	// rodo DIR dipuko busena
#define DI_SCROLL_DIP				(8u)	// rodo SCROLL dipuko busena
#define DI_MBUS_DIP					(9u)	// rodo Modbus dipuko busena
#define DI_HALL_S_PULSE				(10u)	// holo sensorio signalo metu invertuojasi
#define DI_HALL_S_STATE				(11u)   // holo daviklio busena

enum { CW = false, CCW = true };

typedef struct{	
    uint8_t 	PrescalerMask	:3;
    uint8_t     Prescaler;
    uint16_t 	OcrRegister;
}STEPPER_CLOCK_T;


typedef struct{
	USHORT			*pSW_Version;
	USHORT			*pHW_Version;
    uint16_t        *pMotorRpm;
    uint8_t         Alarm           :1;
    uint8_t         MotorState      :1;	// variklio busenos flagas: false - nesisuka, true - sukasi
	uint8_t 		RunUp			:1;	// isibegejimo flagas: false - nesisuka/sukasi, true - isibegeja
	uint8_t 		RunDown			:1;	// stabdymo flagas: false - nesisuka/sukasi, true - stabdosi
	struct{
		USHORT		*pMbAddress;
		USHORT		*pPortBaudrate;
		eMBParity	*pPortParity;
        UCHAR		*pCoils;
		UCHAR		*pDescreetes;
        USHORT      *pHoldings;
		USHORT      *pInputs;
		UCHAR		*pSlaveIdBuffer;		
	}Modbus;
}MOTOR_CONTROL_T;

/****************************    EXTERNS     *********************************/
/****************************    GLOBALS     *********************************/
/****************************   PROTOTYPES   *********************************/
void StepperInit(void);
void ReadAnalogPins(void);
void Adc2Rpm(uint16_t adc);
void Prc2Rpm(uint8_t prc);
uint16_t Rpm2Ocr(uint16_t rpm);

void MotorStart(void);
void MotorStop(void);

void HallSensorHandle(void);
void ScrollingHandle(void);
void DipSwitchHandle(void);
void LedsHandle(void);
#endif /* STEPPER_H */
/*============================================================================
  END OF FILE 
============================================================================*/