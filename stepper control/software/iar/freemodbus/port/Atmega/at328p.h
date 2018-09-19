
#if !defined(AT328_H)
#define AT328_H

/* Modbus port Timer -> Timer2 */
#define SIG_OUTPUT_COMPARE    	TIMER2_COMPA_vect

#define MB_TCNTx				TCNT2
#define MB_OCRx					OCR2A

#define MB_TIMSKx				TIMSK2
#define MB_OCIEx				OCIE2A

#define MB_TCCRxB				TCCR2B
#define MB_CSx0					CS20
#define MB_CSx1					CS21
#define MB_CSx2					CS22

#define MB_TIFRx				TIFR2
#define MB_OCFx					OCF2A


/* Modbus port UART */
#define SIG_USART_DATA          USART_UDRE_vect
#define SIG_USART_TRANS         USART_TX_vect
#define SIG_USART_RECV          USART_RX_vect

#define MB_UBRRn				UBRR0L

#define	MB_UCSRxA				UCSR0A
#define MB_RXCn					RXC0
#define MB_TXCn					TXC0
#define MB_UDREn				UDRE0

#define	MB_UCSRxB				UCSR0B
#define	MB_TXENx				TXEN0
#define	MB_TXCIEx				TXCIE0
#define	MB_RXENx				RXEN0
#define	MB_RXCIEx				RXCIE0
#define MB_UDRIEn				UDRIE0

#define MB_UCSRxC				UCSR0C
#define MB_UPMx0				UPM00
#define MB_UPMx1				UPM01
#define MB_UCSZx0				UCSZ00
#define MB_UCSZx1				UCSZ01

#define MB_UDRx					UDR0

#endif	/* AT328_H */