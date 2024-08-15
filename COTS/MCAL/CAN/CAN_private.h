/*********************************************************************************/
/* Author    : Hossam Ahmed                                                     */
/* Version   : V01                                                               */
/* Data      : 26 OCT 2023                                                       */
/*********************************************************************************/
/*---------------------------------------------------------------------------------------------------------------------
 *  *  FILE DESCRIPTION
 *  --------------------
 *         File:  CAN_Private.h
 *       Module:  CAN Module
 *  Description:  Private header file for CAN Driver
---------------------------------------------------------------------------------------------------------------------*/
#ifndef _CAN_PRIVATE_H
#define _CAN_PRIVATE_H

/*---------------------------------------------------------------------------------------------------------------------
 *  GLOBAL MACROS
---------------------------------------------------------------------------------------------------------------------*/
#define CAN_MODE_NORMAL           			  ((uint32)(0x0))                        			       /*!< Normal mode   */
#define CAN_MODE_LOOPBACK        		 	  ((uint32)(0x1<<30))                					   /*!< Loopback mode BIT 30 < 0x40000000 > */
#define CAN_MODE_SILENT            			  ((uint32)(0x1<<31))                					   /*!< Silent mode  BIT 31 < 0x80000000 > */
#define CAN_MODE_SILENT_LOOPBACK  			  ((uint32)(CAN_MODE_LOOPBACK | CAN_MODE_SILENT)) 	 	   /*!< Loopback combined with silent mode */

/*---------------------------------------------------------------------------------------------------------------------
 *  GLOBAL DATA TYPES AND STRUCTURES
---------------------------------------------------------------------------------------------------------------------*/
/*Bit rate*/
enum BITRATE{CAN_50Kbps, CAN_100Kbps, CAN_125Kbps, CAN_250Kbps, CAN_500Kbps, CAN_800Kbps, CAN_1Mbps};

typedef const struct
{
	uint8 TS2;
	uint8 TS1;
	uint8 BRP;
} CAN_BitTimingConfig;

/******** BoudRate value rating to table at web site {http://www.bittiming.can-wiki.info/}
 * 1. select ST Microelectronics bxCAN
 * 2. put freq with 8Mhz (APB1 freq)
 * 3. click Request Table						        ***********************************/
CAN_BitTimingConfig CAN_bitRateConfig[7] = {{2, 13, 10}, {2, 13, 5}, {2, 13, 4}, {2, 13, 2}, {2, 13, 1},{1, 8, 1}, {1, 6, 1}};


/*CAN registers definitions*/
typedef struct
{
    uint32 LowerNibble:8;
    uint32 HigherNibble:8;
}TwoBytes_TAG_t;
typedef union
{
    uint16 r;
    TwoBytes_TAG_t B;
}TwoBytes_t;

typedef struct
{
    TwoBytes_t LowerNibble;
    TwoBytes_t HigherNibble;
}FourBytes_TAG_t;
typedef union
{
    uint32 r;
    FourBytes_TAG_t B;
}FourBytes_t;

typedef struct
{
    FourBytes_t FxR1;
    FourBytes_t FxR2;
}FilterBank_t;

typedef struct
{
    volatile uint32 RQCP0:1;
    volatile uint32 TXOK0:1;
    volatile uint32 ALST0:1;
    volatile uint32 TERR0:1;
    volatile uint32      :3;
    volatile uint32 ABRQ0:1;
    volatile uint32 RQCP1:1;
    volatile uint32 TXOK1:1;
    volatile uint32 ALST1:1;
    volatile uint32 TERR1:1;
    volatile uint32      :3;
    volatile uint32 ABRQ1:1;
    volatile uint32 RQCP2:1;
    volatile uint32 TXOK2:1;
    volatile uint32 ALST2:1;
    volatile uint32 TERR2:1;
    volatile uint32      :3;
    volatile uint32 ABRQ2:1;
    volatile uint32 CODE :2;
    volatile uint32 TME0 :1;
    volatile uint32 TME1 :1;
    volatile uint32 TME2 :1;
    volatile uint32 LOW0 :1;
    volatile uint32 LOW1 :1;
    volatile uint32 LOW2 :1;
}TSR_Reg_t;

typedef struct
{
    volatile uint32 FMP    :2;
    volatile uint32        :1;
    volatile uint32 FULL   :1;
    volatile uint32 FOVR   :1;
    volatile uint32 RFOM   :1;
}RFR_Reg_t;

typedef struct
{
    volatile uint32 MCR;
    volatile uint32 MSR;
    volatile TSR_Reg_t TSR;
    volatile RFR_Reg_t RFR[2];
    volatile uint32 IER;
    volatile uint32 ESR;
    volatile uint32 BTR;
}CAN_CONTROL_STATUS_t;

/*******************************TX Mailbox Registers********************************************** */
typedef struct
{
    volatile uint32 TXRQ:1;
    volatile uint32 RTR:1;
    volatile uint32 IDE:1;
    volatile uint32 EXID:18;
    volatile uint32 STD_EXID18_28:11;
}TIR_Reg_t;
typedef struct
{
    volatile uint32 DLC:4;
    volatile uint32    :4;
    volatile uint32 TGT:1;/*this bit determine if the time stamp is send in the last 2 bytes of 8-bytes data or not
    DLC must be 8 for timestamp transfer*/
    volatile uint32    :7;
    volatile uint32 TIME:16;
}TDTR_Reg_t;
typedef struct
{
    volatile uint8 DATA[4];
}TDLR_Reg_t;
typedef struct
{
    volatile uint8 DATA[4];
}TDHR_Reg_t;

typedef struct
{
    volatile TIR_Reg_t TIR;
    volatile TDTR_Reg_t TDTR;
    volatile TDLR_Reg_t TDLR;
    volatile TDHR_Reg_t TDHR;
}CAN_TXMailBoxes_t;

/****************************************************************************************************************** */
/***********************************RX FIFO Registers**************************************************** */
typedef struct
{
    volatile uint32    :1;
    volatile uint32 RTR:1;
    volatile uint32 IDE:1;
    volatile uint32 EXID:18;
    volatile uint32 STD_EXID18_28:11;
}RIR_Reg_t;
typedef struct
{
    volatile uint32 DLC:4;
    volatile uint32    :4;
    volatile uint32 FMI:8;/*The Filter Match index can be used in two ways:
                            • Compare the Filter Match index with a list of expected values.
                            • Use the Filter Match Index as an index on an array to access the data destination
                            location.*/
                            /*each filter bank according to its configuration there is a filter number with it , 
                            when receiving frame and pass through this filter ,filter number is the filter match index.
                            we defines filters to pass frame with needed Data , then the data with specific filter index match 
                            we gonna store it in related data variable in the app */
    volatile uint32    :7;
    volatile uint32 TIME:16;
}RDTR_Reg_t;
typedef struct
{
    volatile RIR_Reg_t RIR;
    volatile RDTR_Reg_t RDTR;
    volatile TDLR_Reg_t RDLR;
    volatile TDHR_Reg_t RDHR;
}CAN_RXFIFO_t;
typedef struct
{
    volatile CAN_TXMailBoxes_t Txmailbox[3];
    volatile CAN_RXFIFO_t RXFIFO[2];
}CAN_MAILBOX_REGISTERS_t;

typedef struct
{
    volatile uint32 FMR;
    volatile uint32 FM1R;
    uint32 ReservedReg;
    volatile uint32 FS1R;
    uint32 ReservedReg1;
    volatile uint32 FFA1R;
    uint32 ReservedReg2;
    volatile uint32 FA1R;
     uint32 reserved3[8];
    volatile FilterBank_t FiRx[27];
}CAN_FILTER_REGISTERS_t;


/*---------------------------------------------------------------------------------------------------------------------
 *  GLOBAL MACROS
---------------------------------------------------------------------------------------------------------------------*/
#define 	CAN_Base_Address        0x40006400          // Base address of bxCAN1

#define 	CAN_Control 		((volatile CAN_CONTROL_STATUS_t *) CAN_Base_Address)
#define 	CAN_Mailbox 		((volatile CAN_MAILBOX_REGISTERS_t *) (CAN_Base_Address +0x180 ) )
#define 	CAN_Filter 		((volatile CAN_FILTER_REGISTERS_t *) (CAN_Base_Address +0x200 ) )

#endif
