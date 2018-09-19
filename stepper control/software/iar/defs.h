/****************************************************************
// defs.h
//
// Created: 11/27/2014 1:54:37 PM
//  Author: Alex
//
****************************************************************/
#if !defined (DEFS_H)
#define DEFS_H

//#define OLD_VERSION

#if defined (OLD_VERSION)
	/* Inputs */
	#define HSENS_PIN	    PB2
	#define DIR_PIN			PD4
	#define DIRECT_PIN		PC3
	#define MAX_SPEED_PIN	PB6
	#define ROT_PIN			PB7
	#define SCROLL_PIN		PC0
	#define MBUS_PIN		PD3
	#define RX_PIN			PD0
	
	/* Outputs */
	#define LED1_PIN		PD2
	#define LED2_PIN		PC5
	#define LED3_PIN		PC4
	#define RELAY_PIN		PC2
	#define ENABLE_PIN		PD5
	#define DE_PIN			PB0
	#define CCW_PIN			PD6
	#define HALF_PIN		PD7
	#define CLOCK_PIN		PB1
	#define TX_PIN			PD1
#else
	/* Inputs */
	#define HSENS_PIN	    PD2
	#define DIR_PIN			PD4
	#define DIRECT_PIN		PC3
	#define MAX_SPEED_PIN	PB6
	#define ROT_PIN			PB7
	#define SCROLL_PIN		PC0
	#define MBUS_PIN		PD3
	#define RX_PIN			PD0
	
	/* Outputs */
	#define LED1_PIN		PB2
	#define LED2_PIN		PC5
	#define LED3_PIN		PC4
	#define RELAY_PIN		PC2
	#define ENABLE_PIN		PD5
	#define DE_PIN			PB0
	#define CCW_PIN			PD6
	#define HALF_PIN		PD7
	#define CLOCK_PIN		PB1
	#define TX_PIN			PD1
#endif

#endif /* DEFS_H */
/*============================================================================
  END OF FILE 
============================================================================*/