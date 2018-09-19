#if !defined (MCPORT_H)
#define MCPORT_H

#if defined (__ATmega8__)
#elif defined (__ATmega16__)
	#define WDCE			WDE
	#define ADCSR 			ADCSRA
#elif defined (__ATmega328P__)
	#define TIMSK			TIMSK1
	#define WDTCR			WDTCSR
	#define ADCSR			ADCSRA
#endif

#endif	/* MCPORT_H */