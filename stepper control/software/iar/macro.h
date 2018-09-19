#ifndef MACRO_H
#define MACRO_H

/*============================================================================
Include files
============================================================================*/


/*============================================================================
Defines
============================================================================*/
#define NULL	((void*)0)

#define nop()	asm ("nop")
#define die()	while(1);

#define HI(_int) (char)(_int>>8)
#define LO(_int) (char)(_int)
#define Int(hi,low) (int)((hi << 8) | low);

#define BIT(x) (1 << (x))
#define SetBit(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define ClrBit(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define CheckBit(ADDRESS,BIT) (ADDRESS & (1<<BIT))
#define ChangeBit(ADDRESS,BIT) (ADDRESS ^= (1<<BIT))
#define InvBit(ADDRESS,BIT) (ADDRESS ^= (1<<BIT))

#define ValBit(VAR,BIT) (VAR & (1<<BIT))
#define GetBit(VAR,BIT) (VAR & (1<<BIT)) ? true : false;
#define BitIsClr(VAR,BIT) (0 == ValBit(VAR,BIT) )
#define BitIsSet(VAR,BIT) (!BitIsClr(VAR,BIT) )
#define BitSet(VAR,BIT) ((VAR) |= (1<<(BIT)))
#define BitClr(VAR,BIT) ((VAR) &= ((1<<(BIT))^0xFF))
#define AffBit(VAR,Place,Value)   ((Value) ? \
                                  (VAR |= (1<<Place)) : \
                                  (VAR &= ((1<<Place)^255)))
#define MskBit(Dest,Msk,Src)      ( Dest = (Msk & Src) | ((~Msk) & Dest) )

/*============================================================================
Global variables
============================================================================*/

/*============================================================================
Global function prototypes
============================================================================*/

#endif/* MACRO_H_ */
/*============================================================================
  END OF FILE 
============================================================================*/