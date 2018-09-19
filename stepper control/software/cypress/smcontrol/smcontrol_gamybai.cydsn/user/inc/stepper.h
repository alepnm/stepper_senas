/******************************************************************************
* Project Name		: Stepper Motor Control
* File Name			: stepper.h
* Version 			: 1.0
* Device Used		: CY8C4245AXI-483
* Software Used		: PSoC Creator 4.0
* Compiler    		: ARM GCC 4.9-2015-q1-update
* Related Hardware	: 
*
* Owner             : Ventmatika Inc.
*******************************************************************************/
#if !defined(STEPPER_H)
#define  STEPPER_H

#include <cytypes.h>  
#include <stdbool.h>   
    
    
#define COEF_MIN    0.4
#define COEF_MAX    1.0
    
/* makrosas apskaiciuoja variklio srove is duoto diapazono UNI_MIN_CURRENT_DEF ... UNI_MAX_CURRENT_DEF  koeficientu */
#define CURR2COEF(X) ( COEF_MIN + ( (double)X - UNI_MIN_CURRENT_DEF ) * \
                    ( ( COEF_MAX - COEF_MIN )/( UNI_MAX_CURRENT_DEF - UNI_MIN_CURRENT_DEF ) ) )
    
    
#define TEST_MODE_CODE      ( 9118u ) // 239Eh
    
#define TABLETOP            ( 127u ) // sinusoid lenteles dydis

#define MICROSTEPCLK        ( 8000000u )    // mikrostepo taktinis daznis    
#define MAX_MICROSTEPS      ( 128u )        // maksimalus mikrozingsniu kiekis
    
// defaultai
#define MICROSTEPS_DEF      ( 64u )         // 
#define MICROSTEP_PACE_DEF  (uint8_t)( MAX_MICROSTEPS/MICROSTEPS_DEF )
#define SCROLL_RPM_DEF      ( 10u )             // Scrolling rezimo RPMu defaultas
#define SCROLL_OFF_CYCLE_TIME_DEF   ( 10u )     // SCROLL rezimo OFF ciklo laikas sekundemis (600s)
#define SCROLL_ON_CYCLE_TIME_DEF    ( 10u )     // SCROLL rezimo ON ciklo laikas sekundemis (30s)
#define SCROLL_SYNC_DEF     ( 5u )              // ScrollSync apsisukimu skaicius
#define MAX_RPM_DEF         ( 200u )            // maksimalus sukimosi greitis
#define HALL_SENS_TO_DEF    ( 15u )             // HALL sensorio taimautas sekundemis ( baze 15s )
    
/* UNI tipo variklio defainai */
#define UNI_STEPS_PER_REV_DEF   ( 200u )        // UNI variklio Stepu per apsisukima default
#define UNI_CURRENT_DEF         ( 1.0L )          // UNI variklio srove Amperais default
#define UNI_MIN_CURRENT_DEF     ( 1L )        // UNI tipo variklio MIN srove amperais
#define UNI_MAX_CURRENT_DEF     ( 5L )          // UNI tipo variklio MAX srove amperais

    
/* ADC: 1835 --> 18.10V arba 1.50V po daliklio 22k/2k*/
#define NOVLTHRESHOLD       ( 500u )
#define LVTHRESHOLD         ( 1230u )    /* 12V */
                                         /* 24V -> 2460 */
#define HVTHRESHOLD         ( 3690u )    /* 36V  <-- dar tikslinti! */ 
    
/* ADC kanalai */
#define REQ_SPEED_CHANNEL   ( 0u )      // sukimosi greicio reguliavimas
#define VBUS_CHANNEL        ( 1u )      // VBUS lygio kontrole
    
#define MOTOR_HOLD          false     // ENABLE funkcija galima/negalima

/* Sistemos defainai */
    
/* klaidu tipai */
typedef enum{
    noError = CYRET_SUCCESS,    // operacija sekminga
    NoPower,                    // nera VBus
	LowVolt,                    // zemas VBus
	HighVolt,                   // aukstas VBus
    OverCurrent                 // sroves perkrova (rezervas)
}ERROR_TypeDef;
 
/* darbo rezimas: autonominis(Modbus)/isorinis */
typedef enum {
    UI_INT = 0,             // autonominis/Modbus rezimas
    UI_EXT                  // darbo rezimas per isorini valdyma
} MODE_TypeDef;

/* kontrollerio busena: NORMAL/FAULT/SCROLL/STOP/TEST */
typedef enum { 
    ST_NORMAL = 0,          // vyksta normalus variklio sukimas
    ST_FAULT,               // sistemos avarija
    ST_SCROLL,              // vyksta Scrolling prasukimas
    ST_STOP,                // variklis nesisuka
    ST_TEST                 // testinis rezimas (rezervas)
} STATE_TypeDef;
    

typedef struct{
    uint8_t ID;                         // ID
    uint8_t StepsPerRevolution;         // zingsniu per apsisukima
    union{
        uint8_t data[4];
        double  val;        // darbines sroves koefficientas (0.1-1/1A-5A)
    }CurrentCoef;
    
}MOTOR_TYPE_TypeDef;


typedef struct{ 
    struct{
        uint16_t*       pMbAddr;        // Modbus adresas
        uint16_t*       pMbBaudrate;    // baudrate
        uint16_t*       pMbParity;      // parity
        uint16_t*       pMbStopBits;    // stopbits
    }Modbus;    
    struct{
        const MOTOR_TYPE_TypeDef* pMotor;   // aktyvaus zingsninio variklio tipas
        uint16_t*       pMicroSteps;    // mikrostepu kiekis
        uint16_t*       pScrollingRPM;  // Scrolling rezimo RPMai
        uint16_t*       pScrollSync;    // Scrolling rezimo prasukimas i viena/abi puses
    }Settings;
    struct{    
        MODE_TypeDef    Mode;           // valdiklio darbas autonominis arba su isoriniu valdymu (STEP, DIR)
        uint8_t         RotDirection;   // sukimosi kriptis
        uint8_t         ScrollingEna;   // Scrolling galimas/negalimas
        uint16_t*       pRotSpeed;      // sukimosi greitis
        uint16_t*       pScrollCycleOff;// Scrolling ciklo pauze
        uint16_t*       pScrollCycleOn; // Scrolling ciklo prasukimo laikas
    }Control;
    struct{
        uint8_t         IsRun;          // variklis sukasi/stovi
        uint16_t*       pVbusValue;     // VBus maitinimo reiksme ADC vienetais
        uint16_t*       pPotValue;      // greicio reguliatoriaus pozicijos reiksme ADC vienetais
        uint16_t*       pCurrRPM;       // apskaiciuoti is greicio reguliatoriaus arba uzduoti per Modbus variklio RPMai
        STATE_TypeDef*  pContrState;    // kontrollerio busena
        ERROR_TypeDef*  pFaultCode;     // kontrollerio klaidos kodas
    }Status;
    
    bool  Home;                         // zingsninio variklio Home padeties flagas
    uint8 stageIndex;                   // zingsninio variklio darbine faze
    uint8 microStepPointer;             // pointeris i sinusoid lenteles reiksme 
    uint8 microStepPace;                //
    uint8 sinTblPointer;                // sinusoid lenteles microStepPointer sin reiksme
    uint8 cosTblPointer;                // sinusoid lenteles microStepPointer cos reiksme (pastumtas sinusas)
} SMC_HandleTypeDef;

	
/* Funkciju prototipai */
void StepperInit( void );
void Stepping( void );
void MotorHandler ( void );
void ScrollingHandler( void );
void HallSensorHandler( void );
void CheckVB_Value( void );

#endif
/* End STEPPER_H */

