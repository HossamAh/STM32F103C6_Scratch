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
#ifndef _CAN_INTERFACE_H
#define _CAN_INTERFACE_H

/*---------------------------------------------------------------------------------------------------------------------
 *  INCLUDES
---------------------------------------------------------------------------------------------------------------------*/
#include "../../LIB//Std_Types.h"
#include "../../LIB//Bit_Math.h"

#include "CAN_private.h"
#include "CAN_config.h"
/*---------------------------------------------------------------------------------------------------------------------
 *  GLOBAL MACROS
---------------------------------------------------------------------------------------------------------------------*/
/** @defgroup CAN_identifier_type CAN Identifier Type **/
#define CAN_ID_STD                  (0x0)  /*!< Standard Id */
#define CAN_ID_EXT                  (0x1)  /*!< Extended Id */

/** @defgroup CAN_remote_transmission_request CAN Remote Transmission Request **/
#define CAN_RTR_DATA                (0x0)  /*!< Data frame   */
#define CAN_RTR_REMOTE              (0x1)  /*!< Remote frame */

/** @defgroup CAN_receive_FIFO_number CAN Receive FIFO Number **/
#define CAN_RX_FIFO0                (0x0)  /*!< CAN receive FIFO 0 */
#define CAN_RX_FIFO1                (0x1)  /*!< CAN receive FIFO 1 */


/*---------------------------------------------------------------------------------------------------------------------
 *  GLOBAL DATA TYPES AND STRUCTURES
---------------------------------------------------------------------------------------------------------------------*/
typedef enum
{
    CAN_MASK_MODE,
    CAN_IDENTIFIERLIST_MODE,
}CAN_filterMode_t;
typedef enum
{
    CAN_ONE_32BIT_FILTER,
    CAN_TWO16BIT_FILTER,
}CAN_SCALABLEWIDTH_t;

typedef struct
{
CAN_filterMode_t filterMode;
CAN_SCALABLEWIDTH_t filterScalableWidth;

uint32 FilterIdHigh;          /*!< Specifies the filter identification number :
                                    1- ID in one 32-bit identifier mask FxR1 0-32
                                    2- first ID in two 32-bit identifer list FxR1 0-32
                                    3- ID FxR1 0-15 in two 16-bit filters identifier mask
                                    4- ID FxR1 0-15 in two 16-bit filters identifier list
                                       This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF. */

uint32 FilterIdLow;           /*!< Specifies the filter identification number:                                     
                                    1- second ID in two 32-bit identifer list FxR2 0-32
                                    2- ID 0-15 in FxR2 in two 16-bit filters identifier mask
                                    3- 2nd ID FxR1 16-31 in two 16-bit filters identifier list
                                    This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF. */

uint32 FilterMaskIdHigh;      /*!< Specifies the filter mask number or identification number,
                                    according to the mode:
                                    1- Mask in one 32-bit identifier mask
                                    2- first mask FxR1 16-31 in two 16-bit filters identifier mask
                                    3- 3rd ID FxR2 0-15 in two 16-bit filters identifier list
                                    This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF. */

uint32 FilterMaskIdLow;       /*!< Specifies the filter mask number or identification number,
                                    according to the mode
                                    1- second mask FxR2 16-31 in two 16-bit filters identifier mask
                                    2- 4th ID FxR2 16-31 in two 16-bit filters identifier listt
                                    This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF. */ 

uint32 FilterFIFOAssignment;  /*!< Specifies the Recevier FIFO (0 or 1U) which will be assigned to the filter.*/
                                       
uint32 FilterBank;            /*!< Specifies the filter bank which will be initialized.*/
  
}CAN_Filter_t;


typedef struct 
{
  uint32 StdId;    
  uint32 ExtId;    
  uint32 IDE;      
  uint32 RTR;      
  uint32 DLC;      
  uint8 TransmitGlobalTime; 	/*!< Specifies whether the time stamp counter value captured on start
									of frame transmission*/
}CAN_TX_Frame_t;

typedef struct 
{
  uint32 StdId;
  uint32 ExtId;    
  uint32 IDE;      
  uint32 RTR;      
  uint32 DLC;      
  uint32 TimeStamp; 
  uint32 FilterMatchIndex; /*!< Specifies the index of matching acceptance filter element.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 0xFF. */

}CAN_RX_Frame_t;

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
void MCAN_VoidInit();

/******************************************************************************
* \Syntax          : void MCAN_VoidConfigureIDFilter(CAN_Filter_t *pFilterConfig)                                      
* \Description     : initialize CAN filter banks and filers modes based on configuraion parameters                                                                          
* \Sync\Async      : Synchronous                                               
* \Reentrancy      : Non Reentrant                                             
* \Parameters (in) : CAN_Filter_t *pFilterConfig                   
* \Parameters (out): None                                                      
* \Return value:   : None
*******************************************************************************/
void MCAN_VoidConfigureIDFilter(CAN_Filter_t *pFilterConfig);

/******************************************************************************
* \Syntax          : void MCAN_VoidTransmission(CAN_TX_Frame_t * TXframe,uint8 Data[])                                      
* \Description     : initialize CAN Frame to be send and put it in Tx mailbox and                                                                           
* \Sync\Async      : ASynchronous (when tx is transmitted interrupt notify the app)                                               
* \Reentrancy      : Non Reentrant                                             
* \Parameters (in) : CAN_TX_Frame * TXframe , Data array represent the data bytes                  
* \Parameters (out): None                                                      
* \Return value:   : None
*******************************************************************************/
void MCAN_VoidTransmission(CAN_TX_Frame_t * TXframe,uint8 Data[]);

/******************************************************************************
* \Syntax          : void MCAN_VoidReception(uint8 RX_FIFO,CAN_RX_Frame_t * RXframe,uint8 Data[])                                      
* \Description     : Read CAN frame from RX FIFO and store its data in Data Array.                                                                           
* \Sync\Async      : Synchronous                                                
* \Reentrancy      : Non Reentrant                                             
* \Parameters (in) :  RX_ FIFO number of fifo to read from,CAN_RX_Frame * RXframe , Data array represent the data bytes                  
* \Parameters (out): CAN_RX_Frame * RXframe , Data array represent the data bytes                                                      
* \Return value:   : None
*******************************************************************************/
void MCAN_VoidReception(uint8 RX_FIFO,CAN_RX_Frame_t * RXframe,uint8 Data[]);
#endif
