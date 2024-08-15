/*********************************************************************************/
/* Author    : Hossam Ahmed                                                     */
/* Version   : V01                                                               */
/* Data      : 26 OCT 2023                                                       */
/*********************************************************************************/
/*---------------------------------------------------------------------------------------------------------------------
 *  *  FILE DESCRIPTION
 *  --------------------
 *         File:  CAN_config.h
 *       Module:  CAN Module
 *  Description:  Configuration header file for CAN Driver
---------------------------------------------------------------------------------------------------------------------*/
#ifndef _CAN_CONFIG_H
#define _CAN_CONFIG_H

/*---------------------------------------------------------------------------------------------------------------------
 *  INCLUDES
---------------------------------------------------------------------------------------------------------------------*/
#include "CAN_interface.h"

/*---------------------------------------------------------------------------------------------------------------------
 *  GLOBAL MACROS
---------------------------------------------------------------------------------------------------------------------*/
/** CAN  configuration structure definition **/
#define NoAutomaticRetransmission           ENABLE
#define AutomaticBus_off                  ENABLE
#define AutomaticWakeupMode                   DISABLE
#define TimeTriggeredCommunicationMode            DISABLE
#define ReceiveFIFOLockedMode            DISABLE
#define TransmitFifoPriority         DISABLE

/********** option : 
 * 					 CAN_MODE_NORMAL
 * 					 CAN_MODE_LOOPBACK
 * 					 CAN_MODE_SILENT
 * 					 CAN_MODE_SILENT_LOOPBACK
 	 	 	 	 	 	 	 	 	 	 	 	 **********************************/
#define MODE			 CAN_MODE_NORMAL
/********** option : CAN_50KBPS
 * 					 CAN_100KBPS
 * 					 CAN_125KBPS
 * 					 CAN_250KBPS
 * 					 CAN_500KBPS
 * 					 CAN_800KBPS,
 * 					 CAN_1MBPS
 	 	 	 	 	 	 	 	 	 	 	 	 *************************/
#define BAUDRATE		CAN_500Kbps
/*---------------------------------------------------------------------------------------------------------------------
 *  GLOBAL DATA
---------------------------------------------------------------------------------------------------------------------*/







#endif
