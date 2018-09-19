/******************************************************************************
* Project Name		: Stepper Motor Control
* File Name			: user_mb_app.h
* Version 			: 1.0
* Device Used		: CY8C4245AXI-483
* Software Used		: PSoC Creator 4.0
* Compiler    		: ARM GCC 4.9-2015-q1-update
* Related Hardware	: 
*
* Owner             : Ventmatika Inc.
*******************************************************************************/
    
#ifndef	USER_APP
#define USER_APP
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"

/* -----------------------Slave Defines -------------------------------------*/
#define     DISCRETE_INPUT_START        0
#define     DISCRETE_INPUT_NDISCRETES   16
#define     COIL_START                  0
#define     COIL_NCOILS                 64
#define     REG_INPUT_START             0
#define     REG_INPUT_NREGS             16
#define     REG_HOLDING_START           0
#define     REG_HOLDING_NREGS           20
    
    
 /* DEFAULTS */
#define     MBADDR_DEF          ( 21u )   //15H
#define     MBPARITY_DEF        SCB_UART_PARITY_NONE
#define     MBBAURATE_DEF       ( 19200u )
#define     MBSTOPBITS_DEF      SCB_UART_STOP_BITS_1

#define     ROTSPEED_DEF        ( 10u )
    
    
/* COILS */
#define     CO_START            ( 0u )  // start(true)/stop(false)
#define     CO_DIRECTION        ( 1u )  // sukimosi kriptis: CW(true)/CCW(false)
#define     CO_FAULT_FLAG       ( 2u )  // sistemos klaidos pozymis: nera klaidos(false)/klaida(true)
//#define     CO_SCROLLING        ( 3u )  // SCROLL funkcija yra(true)/nera(false)
//#define     CO_HALL_SENS        ( 4u )  // HALL sensorius yra(true)/nera(false)
#define     CO_SMC_CONTROL      ( 5u )  // kontrolerio valdymas per Modbus(true)/autonominis(false)
#define     CO_DRV_REBOOT       ( 8u )  // draiverio reinicializacija(true)
    
/* DESCREETS */
#define     DI_HALL_STATE       ( 0u )  // dubliuoja HALL daviklio iejimo busena
#define     DI_HALL_FAULT       ( 1u )  // Hall daviklio avarijos flagas
    
/* INPUTS */
#define     IR_SP_REQ_VALUE     ( 0u )  // sukimosi greicio potenciometro reiksme
#define     IR_VBUS_VALUE       ( 1u )  // VBUS itampos reiksme
#define     IR_CURR_RPM         ( 2u )  // variklio apsukos ( apskaiciuota SP_REQ_VALUE )
#define     IR_FAULT_CODE       ( 3u )  // sistemos klaidos kodas
#define     IR_SMC_STATE        ( 4u )  // SMC statusas (veikia/stop/scrolling ir t.t.)
#define     IR_HS_TO_COUNTER    ( 5u )  // Hall sensorio timeout counteris
#define     IR_WTIMEHI          ( 6u )  // WTIME hi registras
#define     IR_WTIMELO          ( 7u )  // WTIME lo registras

    
/* HOLDINGS */
#define     HR_WTIME            ( 0u )  // bendras veikimo laikas minutemis
#define     HR_ROT_SPEED        ( 1u )  // variklio apsukos nustatymui
#define     HR_MAX_RPM          ( 2u )  // maksimalus variklio RPMai
#define     HR_HS_TO_VALUE      ( 5u )  // Hall sensorio timeout reiksme ( 0 - Hall Sensor isjungtas )
    
#define     HR_UNI_STEPS_PER_REV    ( 6u )  // UNI variklio rezoliucija stepais per apsisukima
#define     HR_UNI_CURRENT      ( 7u )  // UNI variklio maksimali srove amperais ( formatas: Reksme(A) * 100 )
    
    
#define     HR_MICROSTEPS       ( 8u )  // darbo rezimas mikrostepais
#define     HR_SCROLL_RPM       ( 9u )  // SCROLLING rezimo RPMai ( 0 - Scrolling isjungtas )
#define     HR_SCROLL_OFF_CYCLE_TIME    ( 10u )  // SCROLLING rezimo OFF ciklo laikas sekundemis
#define     HR_SCROLL_ON_CYCLE_TIME     ( 11u )  // SCROLLING rezimo ON ciklo laikas sekundemis
#define     HR_SCROLL_SYNC      ( 12u ) // ScrollSync apsisukimu skaicius ( kas kelinta apsisukima keiciam sukimosi kripti. 0 - funkcija isjungta )

    
#define     HR_MBADDR           ( 13u ) // Modbus adresas
#define     HR_MBPARITY         ( 14u ) // Parity
#define     HR_MBSTOPBITS       ( 15u ) // Stopbitai
#define     HR_MBBAUDRATE       ( 16u ) // Bodreitas
    
#define     HR_TEST_MODE        ( 18u ) // testinis rezimas ( jai irasom TEST_MODE_CODE (9118u/239Eh) - aktyvuojasi testinis rezimas, kita - darbinis rezimas )
   
    
#endif
