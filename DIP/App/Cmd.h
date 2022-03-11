#ifndef __Cmd_H
#define __Cmd_H

#include "main.h"
//#include "stm32f10x_flash.h"
#include "time.h"
#include "usb_conf.h"


#define random(max,min)	((rand() % ((max)-(min)+1)) + (min))

//#define usart1_dr_address ((u32)0x40013804)
#define usart3_dr_address ((u32)0x40004804)
#define uart5_dr_address ((u32)0x40005004)
//#define USART1_BRR *(u32 *)(0x40013808)
#define USART3_BRR *(u32 *)(0x40004808)

#define UART5_BRR *(u32 *)(0x40005008)

#define SOH 0x01
#define STX 0x02
#define ETX 0x03
#define EOT 0x04
#define ENQ 0x05
#define ACK 0x06
#define NAK 0x15
//#define CAN 0x18

#define ComandNotDefine					1
#define NoCard							2
#define CardFail						3
#define CardUnLockFail					4
#define DataFail						5
#define TimeOut							6
#define WrongCommandData				7
#define MSBlankError					8
#define MSPreamlieError					9
#define MSParityError					10
#define MSpostAmbleError				11
#define MSLRCError						12
#define IFMCommunicationError			13
#define ICCardContactError				14
#define ICCardWrongCommandlength		15
#define ICCardShortCircuiting			16
#define ICCardATRLong					17
#define ICCardAPDULengthWrong			18
#define ICCardBufferOverflow			19
#define ICCardParityError				20
#define ICCardDetectsExcessiveCurrent	21
#define ICCardDetectsDefectiveVoltage	22
#define ICCardCommandNotAllowed			23
#define CommandCancel					24
#define WrongDesKeyError				25
#define RFCommError 					26
#define CardLockFail					27

// bit bending	address

#ifdef Hanmega_USA

	#define BACKWARD	1//0
	#define FORWARD		0///1

	#define STAT_B (*(volatile unsigned char *)(0x20000000))

	#define IS_SAM1 (*(volatile unsigned long *)(0x22000000)) = 1
	#define Non_SAM1 (*(volatile unsigned long *)(0x22000000)) = 0
	#define IS_SAM1_IN (*(volatile unsigned long *)(0x22000000))

	#define IS_SAM2 (*(volatile unsigned long *)(0x22000004)) = 1
	#define Non_SAM2 (*(volatile unsigned long *)(0x22000004)) = 0
	#define IS_SAM2_IN (*(volatile unsigned long *)(0x22000004))

	#define IS_SAM3 (*(volatile unsigned long *)(0x22000008)) = 1
	#define Non_SAM3 (*(volatile unsigned long *)(0x22000008)) = 0
	#define IS_SAM3_IN (*(volatile unsigned long *)(0x22000008))

	/// 20150107 hyesun: 삭제
	//#define IS_SAM4 (*(volatile unsigned long *)(0x2200000C)) = 1
	//#define Non_SAM4 (*(volatile unsigned long *)(0x2200000C)) = 0
	//#define IS_SAM4_IN (*(volatile unsigned long *)(0x2200000C))

	/// 20150107 hyesun: 추가
	#define MS_F_Read_ON (*(volatile unsigned long *)(0x2200000C)) = 0///1
	#define MS_R_Read_ON (*(volatile unsigned long *)(0x2200000C)) = 1///0
	#define MS_Read_IN (*(volatile unsigned long *)(0x2200000C))

	#define IS_MS_DATA (*(volatile unsigned long *)(0x22000010)) = 1
	#define Non_MS_DATA (*(volatile unsigned long *)(0x22000010)) = 0
	#define MS_DATA_IN (*(volatile unsigned long *)(0x22000010))

	#define IC_Reset_ON (*(volatile unsigned long *)(0x22000014)) = 1
	#define NO_ICC_Reset (*(volatile unsigned long *)(0x22000014)) = 0
	#define ICC_Reset_IN (*(volatile unsigned long *)(0x22000014))

	#define Front_Detect_ON (*(volatile unsigned long *)(0x22000018)) = 1
	#define Front_Detect_Non (*(volatile unsigned long *)(0x22000018)) = 0
	#define Front_Detect_IN (*(volatile unsigned long *)(0x22000018))

	#if 1
	#define LockDetectON (*(volatile unsigned long *)(0x2200001C)) = 1
	#define LockDetectOFF (*(volatile unsigned long *)(0x2200001C)) = 0
	#define StateLock (*(volatile unsigned long *)(0x2200001C))
	#else // 20160415 : 한메가 요청으로 STAT Bit7 Lock Sensor -> Rear Sensor 번경
	#define Rear_Detect_ON (*(volatile unsigned long *)(0x2200001C)) = 1
	#define Rear_Detect_Non (*(volatile unsigned long *)(0x2200001C)) = 0
	#define Rear_Detect_IN (*(volatile unsigned long *)(0x2200001C))
	#endif

	#define STAT2_B (*(volatile unsigned char *)(0x20000001))

	/// always [2011/1/27] DES 사용 유무 
	#define UseDesON (*(volatile unsigned long *)(0x22000024)) = 1
	#define UseDesOFF (*(volatile unsigned long *)(0x22000024)) = 0
	#define StatDes (*(volatile unsigned long *)(0x22000024))

	#if 1
	#define Rear_Detect_ON (*(volatile unsigned long *)(0x22000028)) = 1
	#define Rear_Detect_Non (*(volatile unsigned long *)(0x22000028)) = 0
	#define Rear_Detect_IN (*(volatile unsigned long *)(0x22000028))
	#else // 20160415 : 한메가 요청으로 STAT Bit7 Lock Sensor -> Rear Sensor 번경
	#define LockDetectON (*(volatile unsigned long *)(0x22000028)) = 1
	#define LockDetectOFF (*(volatile unsigned long *)(0x22000028)) = 0
	#define StateLock (*(volatile unsigned long *)(0x22000028))
	#endif

	#define Connect_RF_Module (*(volatile unsigned long *)(0x2200002c)) = 1
	#define Disconnect_RF_Module (*(volatile unsigned long *)(0x2200002c)) = 0
	#define StateRF (*(volatile unsigned long *)(0x2200002c))
	#if 1		//pbbch 180524 기존에 FW1.7효성향에서 추가 된 filed로 현재 효성도 사용 안함. 단 배포가 되어 field만 유지 함. 따라서 inner sensor bit를 4->5로 변경.
	// bit 4
    #define Anti_Skimming_Detect_ON (*(volatile unsigned long *)(0x22000030)) = 1
    #define Anti_Skimming_Detect_Non (*(volatile unsigned long *)(0x22000030)) = 0
    #define Anti_Skimming_Detect_IN (*(volatile unsigned long *)(0x22000030))
	// bit 5
    #define Inner_Detect_ON (*(volatile unsigned long *)(0x22000034)) = 1
    #define Inner_Detect_Non (*(volatile unsigned long *)(0x22000034)) = 0
    #define Inner_Detect_IN (*(volatile unsigned long *)(0x22000034))
	#else//pbbch 180517 inner sensor add
	// bit 4
    #define Inner_Detect_ON (*(volatile unsigned long *)(0x22000030)) = 1
    #define Inner_Detect_Non (*(volatile unsigned long *)(0x22000030)) = 0
    #define Inner_Detect_IN (*(volatile unsigned long *)(0x22000030))
	#endif

#else
// normal
#define BACKWARD 0
#define FORWARD 1

  #define STAT_B        (*(volatile unsigned char *)(0x20000000))
    // bit 0
    // #define IS_SAM1		        gSTAT_B |= 0x01                 //(*(volatile unsigned long *)(0x22000000)) = 1
    // #define Non_SAM1	        gSTAT_B &= ~0x01                //(*(volatile unsigned long *)(0x22000000)) = 0
    // #define IS_SAM1_IN	        (gSTAT_B&0x01)                  //(*(volatile unsigned long *)(0x22000000))
    #define IS_SAM1             (*(volatile unsigned long *)(0x22000000)) = 1
    #define Non_SAM1            (*(volatile unsigned long *)(0x22000000)) = 0
    #define IS_SAM1_IN          (*(volatile unsigned long *)(0x22000000))

    // bit 1
    // #define IS_SAM2		        gSTAT_B |= 0x02                 //(*(volatile unsigned long *)(0x22000004)) = 1
    // #define Non_SAM2	        gSTAT_B &= ~0x02                //(*(volatile unsigned long *)(0x22000004)) = 0
    // #define IS_SAM2_IN	        ((gSTAT_B>>1)&0x01)             //(*(volatile unsigned long *)(0x22000004))   
    #define IS_SAM2             (*(volatile unsigned long *)(0x22000004)) = 1
    #define Non_SAM2            (*(volatile unsigned long *)(0x22000004)) = 0
    #define IS_SAM2_IN          (*(volatile unsigned long *)(0x22000004))

    // bit 2
    // #define IS_SAM3		        gSTAT_B |= 0x04                 //(*(volatile unsigned long *)(0x22000008)) = 1
    // #define Non_SAM3	        gSTAT_B &= ~0x04                //(*(volatile unsigned long *)(0x22000008)) = 0
    // #define IS_SAM3_IN	        ((gSTAT_B>>2)&0x01)             //(*(volatile unsigned long *)(0x22000008))
    #define IS_SAM3             (*(volatile unsigned long *)(0x22000008)) = 1
    #define Non_SAM3            (*(volatile unsigned long *)(0x22000008)) = 0
    #define IS_SAM3_IN          (*(volatile unsigned long *)(0x22000008))

    // bit 3
    // #define IS_SAM4 	        gSTAT_B |= 0x08                 //(*(volatile unsigned long *)(0x2200000C)) = 1
    // #define Non_SAM4 	        gSTAT_B &= ~0x08                //(*(volatile unsigned long *)(0x2200000C)) = 0
    // #define IS_SAM4_IN 	        ((gSTAT_B>>3)&0x01)             //(*(volatile unsigned long *)(0x2200000C))
    #define IS_SAM4             (*(volatile unsigned long *)(0x2200000C)) = 1
    #define Non_SAM4            (*(volatile unsigned long *)(0x2200000C)) = 0
    #define IS_SAM4_IN          (*(volatile unsigned long *)(0x2200000C))

    // bit 4
    // #define IS_MS_DATA 			gSTAT_B |= 0x10                 //(*(volatile unsigned long *)(0x22000010)) = 1
    // #define Non_MS_DATA 		gSTAT_B &= ~0x10                //(*(volatile unsigned long *)(0x22000010)) = 0
    // #define MS_DATA_IN 			((gSTAT_B>>4)&0x01)             //(*(volatile unsigned long *)(0x22000010))
    #define IS_MS_DATA          (*(volatile unsigned long *)(0x22000010)) = 1
    #define Non_MS_DATA         (*(volatile unsigned long *)(0x22000010)) = 0
    #define MS_DATA_IN          (*(volatile unsigned long *)(0x22000010))

    // bit 5
    // #define IC_Reset_ON         gSTAT_B |= 0x20                 //(*(volatile unsigned long *)(0x22000014)) = 1
    // #define NO_ICC_Reset        gSTAT_B &= ~0x20                //(*(volatile unsigned long *)(0x22000014)) = 0
    // #define ICC_Reset_IN        ((gSTAT_B>>5)&0x01)             //(*(volatile unsigned long *)(0x22000014))
    #define IC_Reset_ON         (*(volatile unsigned long *)(0x22000014)) = 1
    #define NO_ICC_Reset        (*(volatile unsigned long *)(0x22000014)) = 0
    #define ICC_Reset_IN        (*(volatile unsigned long *)(0x22000014))

    // bit 6
    // #define Front_Detect_ON     gSTAT_B |= 0x40                 //(*(volatile unsigned long *)(0x22000018)) = 1
    // #define Front_Detect_Non    gSTAT_B &= ~0x40                //(*(volatile unsigned long *)(0x22000018)) = 0
    // #define Front_Detect_IN     ((gSTAT_B>>7)&0x01)             //(*(volatile unsigned long *)(0x22000018))
    #define Front_Detect_ON     (*(volatile unsigned long *)(0x22000018)) = 1
    #define Front_Detect_Non    (*(volatile unsigned long *)(0x22000018)) = 0
    #define Front_Detect_IN     (*(volatile unsigned long *)(0x22000018))

    // bit 7
    // #define Rear_Detect_ON      gSTAT_B |= 0x80                 //(*(volatile unsigned long *)(0x2200001C)) = 1
    // #define Rear_Detect_Non     gSTAT_B &= ~0x80                //(*(volatile unsigned long *)(0x2200001C)) = 0
    // #define Rear_Detect_IN      ((gSTAT_B>>7)&0x01)             //(*(volatile unsigned long *)(0x2200001C))
    #define Rear_Detect_ON      (*(volatile unsigned long *)(0x2200001C)) = 1
    #define Rear_Detect_Non     (*(volatile unsigned long *)(0x2200001C)) = 0
    #define Rear_Detect_IN      (*(volatile unsigned long *)(0x2200001C))

  #define STAT2_B       (*(volatile unsigned char *)(0x20000001))
    // bit 0
    // #define MS_F_Read_ON        gSTAT2_B |= 0x01                //(*(volatile unsigned long *)(0x22000020)) = 1
    // #define MS_R_Read_ON        gSTAT2_B &= ~0x01               //(*(volatile unsigned long *)(0x22000020)) = 0
    // #define MS_Read_IN          (gSTAT2_B&0x01)                 //(*(volatile unsigned long *)(0x22000020))
    #define MS_F_Read_ON        (*(volatile unsigned long *)(0x22000020)) = 1
    #define MS_R_Read_ON        (*(volatile unsigned long *)(0x22000020)) = 0
    #define MS_Read_IN          (*(volatile unsigned long *)(0x22000020))

    // bit 1
    /// always [2011/1/27] DES 사용 유무
    #define UseDesON (*(volatile unsigned long *)(0x22000024)) = 1
    #define UseDesOFF (*(volatile unsigned long *)(0x22000024)) = 0
    #define StatDes (*(volatile unsigned long *)(0x22000024))
    // bit 2
    #define LockDetectON (*(volatile unsigned long *)(0x22000028)) = 1
    #define LockDetectOFF (*(volatile unsigned long *)(0x22000028)) = 0
    #define StateLock (*(volatile unsigned long *)(0x22000028))
    // bit 3
    #define Connect_RF_Module (*(volatile unsigned long *)(0x2200002c)) = 1
    #define Disconnect_RF_Module (*(volatile unsigned long *)(0x2200002c)) = 0
    #define StateRF (*(volatile unsigned long *)(0x2200002c))
	#if 1		//pbbch 180524 기존에 FW1.7효성향에서 추가 된 filed로 현재 효성도 사용 안함. 단 배포가 되어 field만 유지 함. 따라서 inner sensor bit를 4->5로 변경.
	// bit 4
    #define Anti_Skimming_Detect_ON (*(volatile unsigned long *)(0x22000030)) = 1
    #define Anti_Skimming_Detect_Non (*(volatile unsigned long *)(0x22000030)) = 0
    #define Anti_Skimming_Detect_IN (*(volatile unsigned long *)(0x22000030))
	// bit 5
    // #define Inner_Detect_ON (*(volatile unsigned long *)(0x22000034)) = 1
    // #define Inner_Detect_Non (*(volatile unsigned long *)(0x22000034)) = 0
    // #define Inner_Detect_IN (*(volatile unsigned long *)(0x22000034))
    // nxp에서는 bit4로 처리하고 있음.
    #define Inner_Detect_ON		gSTAT2_B |= 0x10//(*(volatile unsigned long *)(0x22000018)) = 1
    #define Inner_Detect_Non	gSTAT2_B &= ~0x10//(*(volatile unsigned long *)(0x22000018)) = 0
    #define Inner_Detect_IN 	((gSTAT2_B>>4)&0x01) //Inner_Detect_ON//(*(volatile unsigned long *)(0x22000018))

	#else//pbbch 180517 inner sensor add
	// bit 4
    #define Inner_Detect_ON (*(volatile unsigned long *)(0x22000030)) = 1
    #define Inner_Detect_Non (*(volatile unsigned long *)(0x22000030)) = 0
    #define Inner_Detect_IN (*(volatile unsigned long *)(0x22000030))
	#endif
#endif


#define UART5_OFF (*(volatile unsigned long *)(0x420A01B4)) = RESET

#define TIM2_CNT *(u32 *)(0x40000024)
#define TIM2_PRS *(u32 *)(0x40000028)

#define TIM6_CNT *(u32 *)(0x40001024)
#define TIM6_PRS *(u32 *)(0x40001028)

#define brr_9600		0xea6
#define brr_19200	0x753
#define brr_38400	0x3a9
#define brr_57600	0x271

// jshsin : 2015.09.03
//#define LastFlashPage 0x0803FC00
//domyst #define LastFlashPage 0x0803F800		// Page 127(2k bytes)
#define LastFlashPage		0x1030000U
#define MasterKeyStorage LastFlashPage

#define DMABufferSize 	518
#define DMA_CNDTR5_INT 	*(u32 *)(0x4002005c) = DMABufferSize
#define DMA_CNDTR5 			*(u32 *)(0x4002005c)
#define DMA_CNDTR3_INT		*(u32 *)(0x40020034) = DMABufferSize
#define DMA_CNDTR3 			*(u32 *)(0x40020034)

#define PRespons	0xff

#define LED_Default '0'
#define LED_Off		'1'
#define LED_On		'2'
#define LED_Toggle	'3'

#define Toggle_slow		20
#define Toggle_1sec	 	10
#define Toggle_fast		2
#if 1	//pbbch 181015 system tick time is 100ms
#define TIMEOUT_10SEC	100
#endif

#define SamVCC_MUX1_ALL_OFF SamVCC_MUX1_A_OFF;SamVCC_MUX1_B_OFF;SamVCC_MUX1_C_OFF;SamVCC_MUX1_D_OFF

#define SamVCC_MUX1_A_ON	pPE15_1
#define SamVCC_MUX1_A_OFF	pPE15_0
#define SamVCC_MUX1_B_ON	pPE14_1
#define SamVCC_MUX1_B_OFF	pPE14_0
#define SamVCC_MUX1_C_ON	pPE13_1
#define SamVCC_MUX1_C_OFF	pPE13_0
#define SamVCC_MUX1_D_ON	pPE12_1
#define SamVCC_MUX1_D_OFF	pPE12_0

#define SamCLK_SAM1 	SamCLK_MUX2_B_0;SamCLK_MUX2_A_0
#define SamCLK_SAM2		SamCLK_MUX2_B_0;SamCLK_MUX2_A_1
#define SamCLK_SAM3		SamCLK_MUX2_B_1;SamCLK_MUX2_A_0
#define SamCLK_SAM4		SamCLK_MUX2_B_1;SamCLK_MUX2_A_1
#define SamCLK_MUX2_A_1		pPD7_1;pPC2_1
#define SamCLK_MUX2_A_0		pPD7_0;pPC2_0
#define SamCLK_MUX2_B_1		pPD6_1;pPC3_1
#define SamCLK_MUX2_B_0		pPD6_0;pPC3_0

#define SamRST_SAM1		SamRST_MUX3_B_0;SamRST_MUX3_A_0
#define SamRST_SAM2		SamRST_MUX3_B_0;SamRST_MUX3_A_1
#define SamRST_SAM3		SamRST_MUX3_B_1;SamRST_MUX3_A_0
#define SamRST_SAM4		SamRST_MUX3_B_1;SamRST_MUX3_A_1
#define SamRST_MUX3_A_1		pPD5_1;pPC4_1
#define SamRST_MUX3_A_0		pPD5_0;pPC4_0
#define SamRST_MUX3_B_1		pPD4_1;pPC5_1
#define SamRST_MUX3_B_0		pPD4_0;pPC5_0

#define SamIO_SAM1		SamIO_MUX4_B_0;SamIO_MUX4_A_0
#define SamIO_SAM2		SamIO_MUX4_B_0;SamIO_MUX4_A_1
#define SamIO_SAM3		SamIO_MUX4_B_1;SamIO_MUX4_A_0
#define SamIO_SAM4		SamIO_MUX4_B_1;SamIO_MUX4_A_1
#define SamIO_MUX4_A_1		pPC6_1
#define SamIO_MUX4_A_0		pPC6_0
#define SamIO_MUX4_B_1		pPC7_1
#define SamIO_MUX4_B_0		pPC7_0


/*
#define sync_9600_min	100
#define sync_9600_max	110

#define sync_19200_min	48
#define sync_19200_max	60#define UART5_ON (*(volatile unsigned long *)(0x420A01B4)) = SET

#define sync_38400_min 20
#define sync_38400_max 30
*/
#define Positive 'P'
#define Negative 'N'

#define Nomal_data_len 3
#define LEN				512

#define	USBEndpoint	1

#define USEMS	'M'
#define USEIC	'I'
#define USEALL	'A'

#if 0
typedef struct{
	u8 Len_H;
	u8 Len_L;
	u8 CMD;
	uchar DATA[LEN];	
}Cmd_Temp;

typedef struct {
	u8 Len_H;
	u8 Len_L;
	uchar STAT;
	uchar STAT2;
	uchar DATA[LEN];	// jsshin 2015.09.07 : 1024 --> 512 buffer size down.
}Response_P_Temp;

typedef struct {
	u8 Len_H;
	u8 Len_L;
	u8 ST1;
	u8 ST2;
}Response_N_Temp;
#else
typedef struct{
	uchar Len_H;
	uchar Len_L;
	uchar CMD;
	uchar PM;
	uchar DATA[LEN];		
}Cmd_Temp;

typedef struct {
	u8 Len_H;
	u8 Len_L;
	uchar Pos;
	uchar STAT;
	uchar STAT2;
	uchar DATA[LEN];	// jsshin 2015.09.07 : 1024 --> 512 buffer size down.
}Response_P_Temp;

typedef struct {
	u8 Len_H;
	u8 Len_L;
	uchar Neg;
	u8 ST1;
	u8 ST2;
	uchar etx;
}Response_N_Temp;
#endif

#define FlashDataSize 43//41

typedef struct {
	u8 MasterDesKey[24];
	u16 IFM_WatingTime_Respons;
	u16 IFM_WatingTime_INComm;
	u8 IFM_RetryCnt_Respons;
	u8 IFM_RetryCnt_INComm;
	u16 Main_WatingTime_INComm;
	u8 ModuleType;
	u8 SerialNumber[7];
	u8 BoardCheckFlag;
	u8 AutoLocktime;
} FlashData;

#if 0	// domyst
#define FlashDataSize 42//43
typedef struct {
	uint8_t MasterDesKey[24];
	uint16_t IFM_WatingTime_Respons;
	uint16_t IFM_WatingTime_INComm;
	uint8_t IFM_RetryCnt_Respons;
	uint8_t IFM_RetryCnt_INComm;
	uint16_t Main_WatingTime_INComm;
	uint8_t ModuleType;
	uint8_t SerialNumber[7];
	uint8_t BoardCheckFlag;
//	uint8_t AutoLocktime;
	uint8_t rsvd[FLASH_PAGE_SIZE-FlashDataSize];
} FlashData;
#endif	// end of domyst
//

typedef struct{
	uchar bmFindexDindex;
	uchar bmTCCKST1;
	uchar bGuardTimeT1;
	uchar bWaitingIntegerT1;
	uchar bClockStop;
	uchar bIFSC;
	uchar bNadValue;
}SetParameter;

typedef struct{
	uchar bCommandEscape;
	uchar bTypeLevel;
}Escape;

#if 1		//pbbch 180220 when protocol length error or etc erro occure,  timeout flg add
extern unsigned char lprotocol_timeout_flg;
#endif

extern void Check_Sensor();

extern uchar MS_Bit_to_Byte(uchar *MS_bit_data,uchar *MS_byte_data,uchar *Byte_data_cnt,ushort length,uchar track,uchar time_use);
extern void Send_u5_data(uchar data);
extern void Send_u4_data(uchar data);
/// always [2010/10/18] 펌웨어 다운로드 플래그 ON
extern void FW_Download_flag_ON();
/// always [2010/10/18] 펌웨어 다운로드 플래그 OFF
extern void FW_Download_flag_OFF();
/// always [2010/10/20] test
extern void NVIC_SystemReset(void);

/// always [2010/10/28] icc로 CMD 전송
extern void Send_cmd_icc(uint Message, uchar *data,ushort size,uchar Slot, uchar resend,uchar T_Protocol);

/// always [2011/1/26] Test
extern void testDes(void);

extern void ICC_buffer_init();
extern FLASH_Status FLASH_WaitForLastOperation(u32 Timeout);
extern void UserToPMABufferCopy(uint8_t *pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes);
extern void SetEPTxCount(uint8_t bEpNum, uint16_t wCount);
extern void SetEPTxValid(uint8_t bEpNum);

#if 1		//pbbch 171212 warning across
extern void DMAInit();
extern u8 SAM_initial(void);	//pbbch 180321 void add.
extern uchar E_CmdProc(uchar WaitTime);
extern void Response(uchar status_position);		//pbbch 180220 add

extern void IFM_reset_excution(unsigned char ldevice_sel);		//pbbch 180321 ifm reset function add
#endif

#if 1		//pbbch 180425 CR30 deactivation direct command send function add
extern unsigned char CR30_Deactivation_direct_execution(void);
#endif

#endif  //__Cmd_H