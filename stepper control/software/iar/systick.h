/******************************************************************************
*   Module :  systick.h
*          
*   Created:        
*   Author:   Alex(@lex)  
******************************************************************************/
/****************************    INCLUDES    *********************************/

/***********************    DEFINES,  TYPEDEFS     ***************************/
typedef volatile struct{	
    uint8_t   Seconds;
    uint8_t   Minutes;
	uint16_t	Hours;
	union{
		struct{
			uint16_t	Lo;
			uint16_t	Hi;
		};
		uint32_t	val;
	}WTime;					// darbo laikas minutemis	
    struct{
        uint8_t    Msec;			// 0.1 sekundes flagas
		uint8_t	HalfSec;		// 0.5 sekundes flagas
        uint8_t    Sec;			// sekundziu flagas
        uint8_t    Min;			// minuciu flagas
    }TimeBits;
}SYSTICK_T;
/****************************    EXTERNS     *********************************/
/****************************    GLOBALS     *********************************/
/****************************   PROTOTYPES   *********************************/
void SysTickInit(void);
uint32_t GetTicks(void);
SYSTICK_T* pSysTickTimer(void);




