/******************************************************************************
* Project Name		: Stepper Motor Control
* File Name			: uart.c
* Version 			: 1.0
* Device Used		: CY8C4245AXI-483
* Software Used		: PSoC Creator 4.0
* Compiler    		: ARM GCC 4.9-2015-q1-update
* Related Hardware	: 
*
* Owner             : Ventmatika Inc.
*******************************************************************************/
#include "uart.h"
#include "mbport.h"


/*******************************************************************************
* Common Definitions
*******************************************************************************/
#define COMMON_BUFFER_SIZE     (127u)

static uint8 bufferTx[COMMON_BUFFER_SIZE];
static uint8 bufferRx[COMMON_BUFFER_SIZE + 1u];

/*******************************************************************************
* UART Configuration
*******************************************************************************/
#define INTR_NONE               0x00

#define UART_OVERSAMPLING       (16u)
//#define UART_DATA_WIDTH         (8u)
#define UART_RX_INTR_MASK       SCB_INTR_RX_NOT_EMPTY
#define UART_TX_INTR_MASK       SCB_INTR_TX_UART_DONE

#define UART_RX_BUFFER_SIZE     COMMON_BUFFER_SIZE
#define UART_TX_BUFFER_SIZE     COMMON_BUFFER_SIZE
#define UART_RX_BUFER_PTR       bufferRx
#define UART_TX_BUFER_PTR       bufferTx


UART_HandleTypeDef MbPort;

static const uint32_t baudrates[6u] = {4800u, 9600u, 19200u, 38400u, 57600u, 115200u};

static SCB_UART_INIT_STRUCT configUart =
{
    .mode = SCB_UART_MODE_STD,              /* mode: Standard */
    .direction = SCB_UART_TX_RX,            /* direction: RX + TX */
    .dataBits = UART_DATA_WIDTH,            /* dataBits: 8 bits */
    .parity = SCB_UART_PARITY_NONE,         /* parity: None */
    .stopBits = SCB_UART_STOP_BITS_1,       /* stopBits: 1 bit */
    .oversample = UART_OVERSAMPLING,        /* oversample: 16 */
    .enableIrdaLowPower = DISABLED,         /* enableIrdaLowPower: disabled */
    .enableMedianFilter = DISABLED,         /* enableMedianFilter: disabled */
    .enableRetryNack = DISABLED,            /* enableRetryNack: disabled */
    .enableInvertedRx = DISABLED,           /* enableInvertedRx: disabled */
    .dropOnParityErr = DISABLED,            /* dropOnParityErr: disabled */
    .dropOnFrameErr = DISABLED,             /* dropOnFrameErr: disabled */
    .enableWake = NON_APPLICABLE,           /* enableWake: disabled */
    .rxBufferSize = UART_RX_BUFFER_SIZE,    /* rxBufferSize: TX software buffer size */
    .rxBuffer = UART_RX_BUFER_PTR,          /* rxBuffer: pointer to RX software buffer */
    .txBufferSize = UART_TX_BUFFER_SIZE,    /* txBufferSize: TX software buffer size */
    .txBuffer = UART_TX_BUFER_PTR,          /* txBuffer: pointer to TX software buffer */
    .enableMultiproc = DISABLED,            /* enableMultiproc: disabled */
    .multiprocAcceptAddr = DISABLED,        /* multiprocAcceptAddr: disabled */
    .multiprocAddr = NON_APPLICABLE,        /* multiprocAddr: N/A */
    .multiprocAddrMask = NON_APPLICABLE,    /* multiprocAddrMask: N/A */
    .enableInterrupt = ENABLED,             /* enableInterrupt: enable internal interrupt
                                             * handler for the software buffer */
    .rxInterruptMask = UART_RX_INTR_MASK,   /* rxInterruptMask: enable INTR_RX.NOT_EMPTY to
                                             * handle RX software buffer operations */
    .rxTriggerLevel = NON_APPLICABLE,       /* rxTriggerLevel: N/A */
    .txInterruptMask = UART_TX_INTR_MASK,   /* txInterruptMask: enable INTR_TX.UART_DONE to
                                             * handle TX software buffer operations */
    .txTriggerLevel = NON_APPLICABLE,       /* txTriggerLevel: N/A */
    .enableByteMode = DISABLED,              /* enableByteMode: enabled */
    .enableCts = DISABLED,                  /* enableCts: disabled */
    .ctsPolarity = DISABLED,                /* ctsPolarity: disabled */
    .rtsRxFifoLevel = DISABLED,             /* rtsRxFifoLevel: disabled */
    .rtsPolarity = DISABLED                 /* rtsPolarity: disabled */
};


/* Functions prototypes */
CY_ISR_PROTO(SCB_IsrHandler);


/*****************************************************************************************
* Function Name: UartConfig
******************************************************************************************
* Summary:
*  Si funkcija konfiguruoja UART nustatymus BLE rezimui. Patikrina parametra, ar jie
*  yra leistini ar ne. Jai parametras klaidingas, jis pakeiciamas standartiniu
*
* Parametrai:
*   uint32_t    baudrate :  (uint32_t) baudraitas is standartiniu (4800 - 115200)
*   uint8_t     parity  :   SCB_UART_PARITY_EVEN    /0x00
*                           SCB_UART_PARITY_ODD     /0x01
*                           SCB_UART_PARITY_NONE    /0x02
*   uint8_t     stopbits :  SCB_UART_STOP_BITS_1    /0x02
*                           SCB_UART_STOP_BITS_1_5  /0x03
*                           SCB_UART_STOP_BITS_2    /0x04
*               databits :  (uint8_t) 7/8
*
* Grazina: Nera
*****************************************************************************************/
void UartConfig( uint32_t baudrate, uint8_t parity, uint8_t stopbits, uint8_t databits )
{   
    uint8_t i = 0;
    
    while(baudrate != baudrates[i++]){
        if( i >= ( sizeof(baudrates)/sizeof(baudrate) ) ){
            baudrate = (19200u);
            break;
        }
    }
    
    if( parity != SCB_UART_PARITY_EVEN && parity != SCB_UART_PARITY_ODD && 
        parity != SCB_UART_PARITY_NONE ){
        parity = SCB_UART_PARITY_NONE;
    }
    
    if( stopbits != SCB_UART_STOP_BITS_1 && stopbits != SCB_UART_STOP_BITS_1_5 && 
        stopbits != SCB_UART_STOP_BITS_2 ){
        stopbits = SCB_UART_STOP_BITS_1;
    }
    
    if( databits != 7 && databits != 8 ){
        databits = (8u);
    }
    
    MbPort.Settings.Baudrate.val = baudrate;
    MbPort.Settings.Parity.val = parity;
    MbPort.Settings.StopBits.val = stopbits;
    MbPort.Settings.DataBits.val = databits;
}


/*******************************************************************************
* Function Name: UartStart
********************************************************************************
* Summary:
* Funkcija konfiguruoja UART irengini. Parametrus ima is MbPort handlerio, kuris
* turi buti pries tai sukonfiguruotas su UartConfig(...) funkcija
*
* Parameters: Nera
*
* Return: Nera
*
*******************************************************************************/
void UartStart()
{
    static uint8_t isr;
    
    isr = CyEnterCriticalSection();
    
    SCB_Stop();

    CommCLK_SetFractionalDividerRegister((( CYDEV_BCLK__HFCLK__HZ /
        ( MbPort.Settings.Baudrate.val * UART_OVERSAMPLING )) - 1 ), 0u);

    configUart.dataBits = MbPort.Settings.DataBits.val;
    configUart.parity = MbPort.Settings.Parity.val;
    configUart.stopBits = MbPort.Settings.StopBits.val;
    
    SCB_UartInit(&configUart);
    
    SCB_SpiUartClearTxBuffer();
    SCB_SpiUartClearRxBuffer();
    
    SCB_ClearRxInterruptSource( SCB_INTR_RX_ALL );
    SCB_ClearTxInterruptSource( SCB_INTR_TX_ALL );

    SCB_SetCustomInterruptHandler( SCB_IsrHandler );
    
    SCB_Start();
    
    CyExitCriticalSection(isr);
}


/*****************************************************************************************
* Function Name: UartStop
******************************************************************************************
* Summary:
*  Si funkcija stabdo SCB moduli.
*
* Parametrai: Nera.
*
* Grazina: Nera.
*****************************************************************************************/
void UartStop()
{
    SCB_Stop();
    
    SCB_SpiUartClearTxBuffer();
    SCB_SpiUartClearRxBuffer();
    
    SCB_ClearRxInterruptSource( SCB_INTR_RX_ALL );
    SCB_ClearTxInterruptSource( SCB_INTR_TX_ALL );
    
    SCB_DISABLE_INTR_RX( SCB_INTR_TX_ALL );
    SCB_DISABLE_INTR_TX( SCB_INTR_TX_ALL );
}



/* UART ISR Handler */
CY_ISR(SCB_IsrHandler){  
    switch(SCB_GetInterruptCause())
    {        
        case SCB_INTR_CAUSE_TX: 
            if( SCB_GetTxInterruptSourceMasked() & SCB_INTR_TX_UART_DONE )
            {
                //prvvUARTTxReadyISR();
                pxMBFrameCBTransmitterEmpty(  );
            }
            
            SCB_ClearTxInterruptSource( SCB_GetTxInterruptSourceMasked() );
            break;
        case SCB_INTR_CAUSE_RX:         
            if( SCB_GetRxInterruptSourceMasked() & SCB_INTR_RX_NOT_EMPTY )
            {
                //prvvUARTRxISR();
                pxMBFrameCBByteReceived(  );
            }
            
            SCB_ClearRxInterruptSource( SCB_GetRxInterruptSourceMasked() ); 
            break;
    }    
}

/* [] END OF FILE */
