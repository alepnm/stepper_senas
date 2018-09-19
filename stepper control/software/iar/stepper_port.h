#if !defined (STEPPER_PORT_H)
#define STEPPER_PORT_H

#define STEPPER_TIM_TCCRA				TCCR1A
#define STEPPER_TIM_TCCRB				TCCR1B
#define STEPPER_TIM_TCNT				TCNT1
#define STEPPER_TIM_OCR					OCR1A
#define STEPPER_TIM_WGM0				WGM10
#define STEPPER_TIM_WGM1				WGM11
#define STEPPER_TIM_WGM2				WGM12
#define STEPPER_TIM_OCIE				OCIE1A
#define STEPPER_TIM_ISR_HANDLER			TIMER1_COMPA_vect

#define STEPPER_ADMUX					ADMUX

#if defined (__ATmega8__)
	#define STEPPER_TIM_TIMSK			TIMSK
	#define STEPPER_ADCSR				ADCSR
#elif defined (__ATmega16__)
	#define STEPPER_TIM_TIMSK			TIMSK
	#define STEPPER_ADCSR				ADCSRA
#elif defined (__ATmega328P__)
	#define STEPPER_TIM_TIMSK			TIMSK
	#define STEPPER_ADCSR				ADCSRA
#endif

#endif	/* STEPPERPORT_H */