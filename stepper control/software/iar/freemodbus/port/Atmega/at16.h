#if !defined(AT16_H)
#define AT16_H

/* Modbus port Timer -> Timer2 */
#define SIG_OUTPUT_COMPARE    	TIMER2_COMP_vect

#define MB_TCNTx				TCNT2
#define MB_OCRx					OCR2

#define MB_TIMSKx				TIMSK
#define MB_OCIEx				OCIE2

#define MB_TCCRxB				TCCR2
#define MB_CSx0					CS20
#define MB_CSx1					CS21
#define MB_CSx2					CS22

#define MB_TIFRx				TIFR
#define MB_OCFx					OCF2


/* Modbus port UART */
#define SIG_USART_DATA          USART_UDRE_vect
#define SIG_USART_TRANS         USART_TXC_vect
#define SIG_USART_RECV          USART_RXC_vect

#define MB_UBRRn				UBRRL

#define	MB_UCSRxA				UCSRA
#define MB_RXCn					RXC
#define MB_TXCn					TXC
#define MB_UDREn				UDRE

#define	MB_UCSRxB				UCSRB
#define	MB_TXENx				TXEN
#define	MB_TXCIEx				TXCIE
#define	MB_RXENx				RXEN
#define	MB_RXCIEx				RXCIE
#define MB_UDRIEn				UDRIE

#define MB_UCSRxC				UCSRC
#define MB_URSEL				URSEL
#define MB_UPMx0				UPM0
#define MB_UPMx1				UPM1
#define MB_UCSZx0				UCSZ0
#define MB_UCSZx1				UCSZ1

#define MB_UDRx					UDR

#endif	/* AT16_H */