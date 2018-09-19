#if !defined(USER_APP)
#define USER_APP

#include "mb.h"
#include "mbconfig.h"

	#if(MB_FUNC_READ_DISCRETE_INPUTS_ENABLED  > 0)
		#define S_DISCRETE_INPUT_START          0
		#define S_DISCRETE_INPUT_NDISCRETES     32
	#endif
	#if(MB_FUNC_READ_COILS_ENABLED > 0||MB_FUNC_WRITE_COIL_ENABLED > 0||MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0)
		#define S_COIL_START                    0
		#define S_COIL_NCOILS                   32
	#endif
	#if(MB_FUNC_READ_INPUT_ENABLED > 0) 
		#define S_REG_INPUT_START               0
		#define S_REG_INPUT_NREGS               16
	#endif
	#if(MB_FUNC_READ_HOLDING_ENABLED > 0||MB_FUNC_WRITE_HOLDING_ENABLED > 0||MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0) 
		#define S_REG_HOLDING_START             0
		#define S_REG_HOLDING_NREGS             32
	#endif


//	#if(MB_FUNC_READ_INPUT_ENABLED > 0)    
//		eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs );
//	#endif
//	
//	#if(MB_FUNC_READ_HOLDING_ENABLED > 0 || MB_FUNC_WRITE_HOLDING_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0)     
//		eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode);
//	#endif
//	
//	#if(MB_FUNC_READ_COILS_ENABLED > 0 || MB_FUNC_WRITE_COIL_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0)
//		eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode);
//	#endif
//		
//	#if(MB_FUNC_READ_DISCRETE_INPUTS_ENABLED  > 0)
//		eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete );
//	#endif
	
#endif

	
