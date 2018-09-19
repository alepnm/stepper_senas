// This file has been prepared for Doxygen automatic documentation generation.   
/*! \file ********************************************************************  
*  
* Atmel Corporation  
*  
* \li File:               eeprom.c  
* \li Compiler:           IAR EWAAVR 3.10c  
* \li Support mail:       avr@atmel.com  
*  
* \li Supported devices:  All devices with split EEPROM erase/write  
*                         capabilities can be used.  
*                         The example is written for ATmega48.  
*  
* \li AppNote:            AVR103 - Using the EEPROM Programming Modes.  
*  
* \li Description:        Example on how to use the split EEPROM erase/write  
*                         capabilities in e.g. ATmega48. All EEPROM  
*                         programming modes are tested, i.e. Erase+Write,  
*                         Erase-only and Write-only.  
*  
*                         $Revision: 1.6 $  
*                         $Date: Friday, February 11, 2005 07:16:44 UTC $  
****************************************************************************/  
#include "eeprom.h"
/*============================================================================
Typedefs
============================================================================*/

/*============================================================================
Defines, Macros
============================================================================*/
#if defined (__ATmega8__)
    #define EEPE		EEWE   
    #define EEMPE 		EEMWE
	#define EEPM0 		4u
	#define EEPM1 		5u
#elif defined (__ATmega16__)
	#define EEPE		EEWE
	#define EEMPE		EEMWE
	#define EEPM0 		4u
	#define EEPM1 		5u
#elif defined (__ATmega328P__)
	#define SPMCR		SPMCSR
	#define SPMEN		SELFPRGEN
#endif

   
/* Define to reduce code size. */   
#define EEPROM_IGNORE_SELFPROG //!< Remove SPM flag polling.

/*============================================================================
Static variables
============================================================================*/ 
/*============================================================================
Global variables
============================================================================*/ 
uint8_t EeResult = NO_ERROR;	// EEPROM result: false - no errors, true - error
/*============================================================================
Global function prototypes
============================================================================*/
void EEPROM_GetBlock(char *, uint16_t, uint8_t);
void EEPROM_PutBlock(char *, uint16_t, uint8_t);

static char SecureEeGet(uint16_t);
static void SecureEePut(uint16_t, uint8_t);

/*============================================================================
Function definitions
============================================================================*/
/*! \brief  Read byte from EEPROM.  
 *  
 *  This function reads one byte from a given EEPROM address.  
 *  
 *  \note  The CPU is halted for 4 clock cycles during EEPROM read.  
 *  
 *  \param  addr  EEPROM address to read from.  
 *  \return  The byte read from the EEPROM address.  
 */   
char EEPROM_GetChar( uint16_t addr )   
{   
	EeResult = NO_ERROR;
	if(addr > E2END) EeResult = IS_ERROR;
	
    do {} while( EECR & (1<<EEPE) ); // Wait for completion of previous write.   
    EEAR = addr; // Set EEPROM address register.   
    EECR = (1<<EERE); // Start EEPROM read operation.   
    return EEDR; // Return the byte read from EEPROM.   
}//---------------------------------------------------------------------------
   
   
   
/*! \brief  Write byte to EEPROM.  
 *  
 *  This function writes one byte to a given EEPROM address.  
 *  The differences between the existing byte and the new value is used  
 *  to select the most efficient EEPROM programming mode.  
 *  
 *  \note  The CPU is halted for 2 clock cycles during EEPROM programming.  
 *  
 *  \note  When this function returns, the new EEPROM value is not available  
 *         until the EEPROM programming time has passed. The EEPE bit in EECR  
 *         should be polled to check whether the programming is finished.  
 *  
 *  \note  The EEPROM_GetChar() function checks the EEPE bit automatically.  
 *  
 *  \param  addr  EEPROM address to write to.  
 *  \param  new_value  New EEPROM value.  
 */   
void EEPROM_PutChar( uint16_t addr, uint8_t new_value )   
{   
	if(addr > E2END){
		EeResult = IS_ERROR;
		return;
	}
	
    char old_value; // Old EEPROM value.   
    char diff_mask; // Difference mask, i.e. old value XOR new value.   
	
    char old_interrupt = __save_interrupt(); // Save interrupt flag state.   
    
	__disable_interrupt(); // Ensure atomic operation for the write operation.   
       
    do {} while( EECR & (1<<EEPE) ); // Wait for completion of previous write. 
	
#ifndef EEPROM_IGNORE_SELFPROG   
    do {} while(SPMCR & (1<<SPMEN)); // Wait for completion of SPM.   
#endif   
       
    EEAR = addr; // Set EEPROM address register.   
    EECR = (1<<EERE); // Start EEPROM read operation.   
    old_value = EEDR; // Get old EEPROM value.   
    diff_mask = old_value ^ new_value; // Get bit differences.   
       
    // Check if any bits are changed to '1' in the new value.   
    if( diff_mask & new_value ) {   
        // Now we know that _some_ bits need to be erased to '1'.   
           
        // Check if any bits in the new value are '0'.   
        if( new_value != 0xff ) {   
            // Now we know that some bits need to be programmed to '0' also.   
               
            EEDR = new_value; // Set EEPROM data register.   
            EECR = (1<<EEMPE) | // Set Master Write Enable bit...   
                   (0<<EEPM1) | (0<<EEPM0); // ...and Erase+Write mode.   
            EECR |= (1<<EEPE);  // Start Erase+Write operation.   
        } else {   
            // Now we know that all bits should be erased.   
   
            EECR = (1<<EEMPE) | // Set Master Write Enable bit...   
                   (1<<EEPM0);  // ...and Erase-only mode.   
            EECR |= (1<<EEPE);  // Start Erase-only operation.   
        }   
    } else {   
        // Now we know that _no_ bits need to be erased to '1'.   
           
        // Check if any bits are changed from '1' in the old value.   
        if( diff_mask ) {   
            // Now we know that _some_ bits need to the programmed to '0'.   
               
            EEDR = new_value;   // Set EEPROM data register.   
            EECR = (1<<EEMPE) | // Set Master Write Enable bit...   
                   (1<<EEPM1);  // ...and Write-only mode.   
            EECR |= (1<<EEPE);  // Start Write-only operation.   
        }   
    } 
	
    __restore_interrupt( old_interrupt ); // Restore interrupt flag state.   
}//---------------------------------------------------------------------------


uint16_t EEPROM_GetWord(uint16_t addr)
{
	reg16_t d;
	
	d.data[0] = SecureEeGet(addr++);
	d.data[1] = SecureEeGet(addr++);
	
	return d.val;
}//---------------------------------------------------------------------------


void EEPROM_PutWord(uint16_t addr, uint16_t dat)
{
	reg16_t d = {
		.val = dat
	};

  	SecureEePut(addr++, d.data[0]); 
  	SecureEePut(addr, d.data[1]);
}//---------------------------------------------------------------------------


uint32_t EEPROM_GetDWord(uint16_t addr)
{
	reg32_t d;
	
	d.data[0] = SecureEeGet(addr++);
	d.data[1] = SecureEeGet(addr++);
	d.data[2] = SecureEeGet(addr++);
	d.data[3] = SecureEeGet(addr);
	
	return d.val;
}//---------------------------------------------------------------------------


void EEPROM_PutDWord(uint16_t addr, uint32_t dat)
{
	reg32_t d = {
		.val = dat
	};
	
  	SecureEePut(addr++, d.data[0]); 
  	SecureEePut(addr++, d.data[1]);  
  	SecureEePut(addr++, d.data[2]); 
  	SecureEePut(addr, d.data[3]);
}//---------------------------------------------------------------------------


void EEPROM_GetBlock(char *block, uint16_t addr, uint8_t len)
{
  	for (int i = addr; len--; i++) *(block++) = EEPROM_GetChar(i); 
}//---------------------------------------------------------------------------


void EEPROM_PutBlock(char *block, uint16_t addr, uint8_t len)
{
  	for (int i = addr; len--; i++) EEPROM_PutChar(i, *(block++));  
}//---------------------------------------------------------------------------



static void SecureEePut(uint16_t addr, uint8_t cValue)
{
	EeResult = NO_ERROR;
	if(addr > E2END){
		EeResult = IS_ERROR;
		return;
	}
	
   	char c = SREG;         /* Need local variable to hold status register */
                 /* While it is -> do nothing                   	*/	
   	while(SPMCR & (1<<SPMEN));
   	while(EECR & 0x02);     /* Check if EEPROM write in progress          */
   	__disable_interrupt(); /* Disable interrupts                          */
   	EEAR = addr;      /* Set up EEPROM address registers             */
   	EEDR = cValue;         /* Set up EEPROM data register                 */
   	EECR |= EEPM0;             /* Set master EEPROM write bit                 */
   	EECR |= EEMPE;             /* Set EEPROM write bit                        */
                /* restore status register                     	*/
   	//while (EECR & (1<<EEWE));
   	SREG = c;  
   	__enable_interrupt();    
}


static char SecureEeGet(uint16_t addr)
{
	EeResult = NO_ERROR;
	if(addr > E2END) EeResult = IS_ERROR;
	
    char cValue;
    char c = SREG;        /* Need local variable to hold status register */
    while(EECR & 0x02);    /* Check if EEPROM write in progress           */
                          /* While it is -> do nothing                   */
    __disable_interrupt();/* Disable interrupts                          */
    EEAR = addr;     /* Set up EEPROM address registers             */
    EECR |= 0x01;         /* Set EEPROM read bit                         */
    cValue = EEDR;        /* Read EEPROM data register                   */
    SREG = c;             /* restore status register                     */   
    __enable_interrupt();
    return(cValue);       /* Return result                               */    
}

