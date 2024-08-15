/*********************************************************************************/
/* Author    : Hossam Ahmed                                                     */
/* Version   : V01                                                               */
/* Data      : 26 OCT 2023                                                       */
/*********************************************************************************/
/*---------------------------------------------------------------------------------------------------------------------
 *  *  FILE DESCRIPTION
 *  --------------------
 *         File:  CAN_interface.h
 *       Module:  CAN Module
 *  Description:  Interface header file for CAN Driver
---------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------------------
 *  INCLUDES
---------------------------------------------------------------------------------------------------------------------*/

#include "../../LIB//Std_Types.h"
#include "../../LIB//Bit_Math.h"

#include "CAN_config.h"
#include "CAN_private.h"
#include "CAN_interface.h"


#include "../GPIO/GPIO_interface.h"
#include "../RCC/RCC_interface.h"
#include "../AFIO/AFIO_interface.h"
/*---------------------------------------------------------------------------------------------------------------------
 *  GLOBAL FUNCTIONS
---------------------------------------------------------------------------------------------------------------------*/
/******************************************************************************
* \Syntax          : void MCAN_VoidInit()                                      
* \Description     : initialize CAN and check the default configuation supported                                                                             
* \Sync\Async      : Synchronous                                               
* \Reentrancy      : Non Reentrant                                             
* \Parameters (in) : ConfigPtr  pointer to port initalization configurations.                   
* \Parameters (out): None                                                      
* \Return value:   : None
*******************************************************************************/
void MCAN_VoidInit()
{
    /*Enable CAN clock and setup the AFIO configuarions*/
    MRCC_voidEnableClock(RCC_APB1,PERIPHERAL_EN_CAN1);
    MRCC_voidEnableClock(RCC_APB2,PERIPHERAL_EN_AFIO);
    MRCC_voidEnableClock(RCC_APB2,PERIPHERAL_EN_IOPB);

    /*set AFIO pins*/
    MAFIO_voidRemapPeripheralPins(CAN_REMAP);//Rx pinB8, TX pinB9
    /*setup AFIO pins for CAN Tx,Rx */
    MGPIO_VoidSetPinMode_TYPE(_GPIOB_PORT,pin8,INPUT_FLOATING);//RX
    MGPIO_VoidSetPinMode_TYPE(_GPIOB_PORT,pin9,OUTPUT_SPEED_2MHZ_AFPUSHPULL);//RX

    /* After Reset CAN boot up in sleep mode
    first we need to exit from it*/
    //clear SLEEP bit
    CLEAR_BIT(CAN_Control->MCR,1);
		
    /* wait to exit sleep mode SLAK bit ack of sleep  */
    while (!(READ_BIT(CAN_Control->MSR,1) == 0));
    /*Switch to initialization mode for init*/
    /*set INRQ bit */
    SET_BIT(CAN_Control->MCR,0);
    /*wait until hardware set the INAK bit ack of init*/
    while ((READ_BIT(CAN_Control->MSR,0)==0));
    
    /*from bits of control register*/

    /** CHECK Transmit FIFO priority **/
    /** CHECK Receive FIFO locked mode **/
    /** CHECK automatic retransmission **/
    /** CHECK automatic wakeup mode **/
    /** CHECK automatic bus-off management **/
    /** CHECK Time triggered communication mode **/
    
    /** CHECK Transmit FIFO priority **/
    #if TransmitFifoPriority == ENABLE
    /*by identifer*/
    CLEAR_BIT(CAN_Control->MCR,2);//TXFP Transmit FIFO priority
    #else
    /*by transmit request order*/
    SET_BIT(CAN_Control->MCR,2);
    #endif

    /** CHECK Receive FIFO locked mode **/
    /*RFLM: Receive FIFO locked mode
    0: Receive FIFO not locked on overrun. Once a receive FIFO is full the next incoming
    message will overwrite the previous one.
    1: Receive FIFO locked against overrun. Once a receive FIFO is full the next incoming
    message will be discarded.*/
    #if ReceiveFIFOLockedMode == Enabled
    SET_BIT(CAN_Control->MCR,3);
    #else
    CLEAR_BIT(CAN_Control->MCR,3);
    #endif

    /*NART: No automatic retransmission
    0: The CAN hardware will automatically retransmit the message until it has been
    successfully transmitted according to the CAN standard.
    1: A message will be transmitted only once, independently of the transmission result
    (successful, error or arbitration lost).*/
    #if NoAutomaticRetransmission == Enabled
    SET_BIT(CAN_Control->MCR,4);
    #else
    CLEAR_BIT(CAN_Control->MCR,4);
    #endif

    /*AWUM: Automatic wakeup mode
    This bit controls the behavior of the CAN hardware on message reception during Sleep
    mode.
    0: The Sleep mode is left on software request by clearing the SLEEP bit of the CAN_MCR
    register.
    1: The Sleep mode is left automatically by hardware on CAN message detection.
    */
    #if AutomaticWakeupMode == Enabled
    SET_BIT(CAN_Control->MCR,5);
    #else
    CLEAR_BIT(CAN_Control->MCR,5);
    #endif

    /*ABOM: Automatic bus-off management
    This bit controls the behavior of the CAN hardware on leaving the Bus-Off state.
    0: The Bus-Off state is left on software request, once 128 occurrences of 11 recessive bits
    have been monitored and the software has first set and cleared the INRQ bit of the
    CAN_MCR register.
    1: The Bus-Off state is left automatically by hardware once 128 occurrences of 11 recessive
    bits have been monitored.*/
    #if AutomaticBus_off == Enabled
    SET_BIT(CAN_Control->MCR,6);
    #else
    CLEAR_BIT(CAN_Control->>MCR,6);
    #endif

    /*Time stamp in TX and RX mailbox*/
    #if TimeTriggeredCommunicationMode == Enabled
    SET_BIT(CAN_Control->MCR,7);
    #else
    CLEAR_BIT(CAN_Control->MCR,7);
    #endif
    
    /** Set the bit timing register **/
    CAN_Control->BTR= (uint32) (MODE |
		  ((CAN_bitRateConfig[BAUDRATE].TS2-1) << 20) |
		  ((CAN_bitRateConfig[BAUDRATE].TS1-1) << 16) |
		  (CAN_bitRateConfig[BAUDRATE].BRP-1));
}

/******************************************************************************
* \Syntax          : void MCAN_VoidConfigureIDFilter(CAN_Filter_t *pFilterConfig)                                      
* \Description     : initialize CAN filter banks and filers modes based on configuraion parameters                                                                          
* \Sync\Async      : Synchronous                                               
* \Reentrancy      : Non Reentrant                                             
* \Parameters (in) : CAN_Filter_t *pFilterConfig                   
* \Parameters (out): None                                                      
* \Return value:   : None
*******************************************************************************/
void MCAN_VoidConfigureIDFilter(CAN_Filter_t *pFilterConfig)
{
    /*at first we enter initalization mode for filter banks FINIT=1 in CAN_FMR*/
    SET_BIT(CAN_Filter->FMR,0);
    /*deactivate the filter bank to start the init  in CAN_FA1R filter number bit*/
    CLEAR_BIT(CAN_Filter->FA1R,pFilterConfig->FilterBank);

    /*configure filter bank according to mode and scalable width*/
    if(pFilterConfig->filterScalableWidth == CAN_ONE_32BIT_FILTER)
    {
        /*set scale width*/
        SET_BIT(CAN_Filter->FS1R,pFilterConfig->FilterBank);
        /*set mode of filter and configure registers*/
        switch (pFilterConfig->filterMode)
        {
        case CAN_IDENTIFIERLIST_MODE:
            SET_BIT(CAN_Filter->FM1R,pFilterConfig->FilterBank);
            CAN_Filter->FiRx[pFilterConfig->FilterBank].FxR1.r = pFilterConfig->FilterIdHigh;
            CAN_Filter->FiRx[pFilterConfig->FilterBank].FxR2.r = pFilterConfig->FilterIdLow;
            break;
        case CAN_MASK_MODE:
            CLEAR_BIT(CAN_Filter->FM1R,pFilterConfig->FilterBank);
            CAN_Filter->FiRx[pFilterConfig->FilterBank].FxR1.r = pFilterConfig->FilterIdHigh;
            CAN_Filter->FiRx[pFilterConfig->FilterBank].FxR2.r = pFilterConfig->FilterMaskIdHigh;
            break;
        default:
            break;
        }
    }
    else if(pFilterConfig->filterScalableWidth==CAN_TWO16BIT_FILTER)
    {
        /*set scale width*/
        CLEAR_BIT(CAN_Filter->FS1R,pFilterConfig->FilterBank);
				
        /*set mode of filter and configure registers*/
        switch (pFilterConfig->filterMode)
        {
        case CAN_IDENTIFIERLIST_MODE:
            SET_BIT(CAN_Filter->FM1R,pFilterConfig->FilterBank);
            CAN_Filter->FiRx[pFilterConfig->FilterBank].FxR1.B.LowerNibble.r = pFilterConfig->FilterIdHigh;
            CAN_Filter->FiRx[pFilterConfig->FilterBank].FxR1.B.HigherNibble.r = pFilterConfig->FilterIdLow;
            CAN_Filter->FiRx[pFilterConfig->FilterBank].FxR2.B.LowerNibble.r = pFilterConfig->FilterMaskIdHigh;
            CAN_Filter->FiRx[pFilterConfig->FilterBank].FxR2.B.HigherNibble.r = pFilterConfig->FilterMaskIdLow;
            break;
        case CAN_MASK_MODE:
            CLEAR_BIT(CAN_Filter->FM1R,pFilterConfig->FilterBank);
            CAN_Filter->FiRx[pFilterConfig->FilterBank].FxR1.B.LowerNibble.r = pFilterConfig->FilterIdHigh;
            CAN_Filter->FiRx[pFilterConfig->FilterBank].FxR1.B.HigherNibble.r = pFilterConfig->FilterMaskIdHigh;
            CAN_Filter->FiRx[pFilterConfig->FilterBank].FxR2.B.LowerNibble.r = pFilterConfig->FilterIdLow;
            CAN_Filter->FiRx[pFilterConfig->FilterBank].FxR2.B.HigherNibble.r = pFilterConfig->FilterMaskIdLow;
            break;
        
        default:
            break;
        }
    }
    else{}

    /*after configure filter bank registers add it to specific FIFO*/
    if(pFilterConfig->FilterFIFOAssignment==0)
    {
        /*assign this filter bank to fifo 0*/
        CLEAR_BIT(CAN_Filter->FFA1R,pFilterConfig->FilterBank);
    }
    else
    {
        /*assign this filter bank to fifo 1*/
        SET_BIT(CAN_Filter->FFA1R,pFilterConfig->FilterBank);
    }

    /*filter Acivate*/
    SET_BIT(CAN_Filter->FA1R,pFilterConfig->FilterBank);

}

/******************************************************************************
* \Syntax          : void MCAN_VoidTransmission(CAN_TX_Frame_t * TXframe,uint8 Data[])                                      
* \Description     : initialize CAN Frame to be send and put it in Tx mailbox and                                                                           
* \Sync\Async      : ASynchronous (when tx is transmitted interrupt notify the app)                                               
* \Reentrancy      : Non Reentrant                                             
* \Parameters (in) : CAN_TX_Frame * TXframe , Data array represent the data bytes                  
* \Parameters (out): None                                                      
* \Return value:   : None
*******************************************************************************/
void MCAN_VoidTransmission(CAN_TX_Frame_t * TXframe,uint8 Data[])
{
    /*get the empty mailbox number (CODE in CAN TSR)*/
    uint8 Local_u8EmptyMailboxNumber = (uint8)CAN_Control->TSR.CODE;

    /*check if there is any empty mailbox  */
    if(CAN_Control->TSR.TME0==1||CAN_Control->TSR.TME1==1||CAN_Control->TSR.TME2==1)
    {
        CAN_Mailbox->Txmailbox[Local_u8EmptyMailboxNumber].TIR.IDE=TXframe->IDE;
        if(TXframe->IDE==CAN_ID_STD)
        {
            CAN_Mailbox->Txmailbox[Local_u8EmptyMailboxNumber].TIR.STD_EXID18_28 = TXframe->StdId;
        }
        else if(TXframe->IDE==CAN_ID_EXT)
        {
            CAN_Mailbox->Txmailbox[Local_u8EmptyMailboxNumber].TIR.EXID = TXframe->ExtId;
            CAN_Mailbox->Txmailbox[Local_u8EmptyMailboxNumber].TIR.STD_EXID18_28 = (TXframe->ExtId>>18);
        }
        CAN_Mailbox->Txmailbox[Local_u8EmptyMailboxNumber].TIR.RTR = TXframe->RTR;
        CAN_Mailbox->Txmailbox[Local_u8EmptyMailboxNumber].TDTR.DLC = TXframe->DLC;
        /*Enable or Disable sending time stamp*/
        CAN_Mailbox->Txmailbox[Local_u8EmptyMailboxNumber].TDTR.TGT = TXframe->TransmitGlobalTime;

        /*Get the number of bytes to send*/
        uint8 Local_u8DataLenght = sizeof(&Data)/sizeof(uint8);
        uint8 Local_u8itr=0;
        for(Local_u8itr=0;Local_u8itr<Local_u8DataLenght;Local_u8itr++)
        {
            if(Local_u8itr<=3)
            {
                CAN_Mailbox->Txmailbox[Local_u8EmptyMailboxNumber].TDLR.DATA[Local_u8itr] = Data[Local_u8itr];
            }
            else
            {
                CAN_Mailbox->Txmailbox[Local_u8EmptyMailboxNumber].TDHR.DATA[Local_u8itr] = Data[Local_u8itr];
            }
        }

        /*Request to send*/
        CAN_Mailbox->Txmailbox[Local_u8EmptyMailboxNumber].TIR.TXRQ=1;
    }
}


/******************************************************************************
* \Syntax          : void MCAN_VoidReception(uint8 RX_FIFO,CAN_RX_Frame_t * RXframe,uint8 Data[])                                      
* \Description     : Read CAN frame from RX FIFO and store its data in Data Array.                                                                           
* \Sync\Async      : Synchronous                                                
* \Reentrancy      : Non Reentrant                                             
* \Parameters (in) :  RX_ FIFO number of fifo to read from,CAN_RX_Frame * RXframe , Data array represent the data bytes                  
* \Parameters (out): CAN_RX_Frame * RXframe , Data array represent the data bytes                                                      
* \Return value:   : None
*******************************************************************************/
void MCAN_VoidReception(uint8 RX_FIFO,CAN_RX_Frame_t * RXframe,uint8 Data[])
{
    /*we enter this function after RX interrupt and the callback of specific FIFO call this function
    * so the Frame is ready in the FIFO mailbox*/
    /*FIFO Registers Contain the frame to be read (the first received one)*/
    
    /*Read the IDE from received frame*/
    RXframe->IDE = CAN_Mailbox->RXFIFO[RX_FIFO].RIR.IDE;
    if(CAN_Mailbox->RXFIFO[RX_FIFO].RIR.IDE==CAN_ID_STD)
    {
        RXframe->StdId = CAN_Mailbox->RXFIFO[RX_FIFO].RIR.STD_EXID18_28;
    }
    else if(CAN_Mailbox->RXFIFO[RX_FIFO].RIR.IDE==CAN_ID_EXT)
    {
        RXframe->ExtId = (CAN_Mailbox->RXFIFO[RX_FIFO].RIR.STD_EXID18_28<<18)|CAN_Mailbox->RXFIFO[RX_FIFO].RIR.EXID;
    }
    else{}
    RXframe->RTR = CAN_Mailbox->RXFIFO[RX_FIFO].RIR.RTR;
    RXframe->DLC = CAN_Mailbox->RXFIFO[RX_FIFO].RDTR.DLC;
    RXframe->FilterMatchIndex = CAN_Mailbox->RXFIFO[RX_FIFO].RDTR.FMI;
    RXframe->TimeStamp = CAN_Mailbox->RXFIFO[RX_FIFO].RDTR.TIME;
    uint8 Local_u8Itr;
    for(Local_u8Itr=0;Local_u8Itr<RXframe->DLC;Local_u8Itr++)
    {
        if(Local_u8Itr<=3)
            Data[Local_u8Itr] = CAN_Mailbox->RXFIFO[RX_FIFO].RDLR.DATA[Local_u8Itr];
        else
            Data[Local_u8Itr] = CAN_Mailbox->RXFIFO[RX_FIFO].RDHR.DATA[Local_u8Itr];
    }
    
    /*After reading the frame ,Release the FIFO to reduce the msgs count and receive another one*/
    CAN_Control->RFR[RX_FIFO].RFOM=1;   
}