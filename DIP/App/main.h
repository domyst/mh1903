/************************** (C) COPYRIGHT 2010 TITENG Co., Ltd Inc. *********************************/
/* project 	  : ky-7911 ms/r         		    				    */
/* processor 	  : CORETEX-M3(STM32F10X)         		    				    */
/* compiler       : IAR EWARM Compiler								    */
/* program by	  : jongil yun								    */
/* History:											    */
/* 		09/27/2010 - Version 0.1 Started						    */
/* copy right	  : TITENG Co., Ltd								    */
/****************************************************************************************************/
/* Includes ------------------------------------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include "Gpio.h"
#include "bit_define.h"
//#include "stm32f10x_lib.h" // jsshin 2015.08.18
#include "stm32f10x_init.h"
#include "stm32f10x_it.h"
#include "Cmd.h"
#include "Download.h"
#include "icc.h"
#include "msr.h"
#include "Des_AllHead.H"

//#define Sol_Locktime 10//2??
//#define Sol_Locktime 120//2??
//#define Sol_Locktime 300//5??
#define ADC_Reference	2000		//about 1.6V

#define PCB_GEM_POS	0x10
#define PCB_CR30_R1	0x01
#define PCB_CR30_R2	0x02
#define PCB_CR30_R3	0x04

#if 1 //pbbch 180129 usb protect structure add
typedef struct {
	unsigned char detect;
	unsigned char configured_flag;
	unsigned char packet_complete;
}usb_protect;
extern usb_protect gusb_protect;
extern unsigned char gmain_process_status;
#endif

extern u8 g_pcb_version;


#define LOCK_SEN	0	//0			SENSOR_1_ADC	// lock sensor
#define REAR_SEN	1	//1//		SENSOR_2_ADC	// rear sensor
#define FRONT_SEN	2	//2//		SENSOR_3_ADC// inner sensor
//#define SENSOR_4_ADC	3
#if 1		//pbbch 180503 sen5 real sensor add
#define FRONT_REAL_SEN		3		//front sensor
#endif

#define SwtichState		pPB15_IN
#define rCardIN			(*(volatile unsigned char *)(0x20000002))
#define rCardIN_ON		rCardIN = 1
#define rCardIN_OFF		rCardIN = 0

#define ErrorFlag		(*(volatile unsigned char *)(0x20000003))
//#define ErrorFlag_OFF ErrorFlag = 0

//#define bShortCard (*(volatile unsigned long *)(0x22000060))
//#define bShortCard_ON bShortCard= 1
//#define bShortCard_OFF bShortCard = 0

//#define bJamCard (*(volatile unsigned long *)(0x22000064))
//#define bJamCard_ON bJamCard = 1
//#define bJamCard_OFF bJamCard = 0

#define bLockState (*(volatile unsigned long *)(0x22000068))
#define bLockFail bLockState = 1
#define bLockSuc bLockState = 0

#define testp *(u32 *)(0x40004800)

#define USB_Con_ON pPA7_0
#define USB_Con_OFF pPA7_1

//#define CheckFrontCnt 3///2

/*
#define USB_Con_ON pPA7_1
#define USB_Con_OFF pPA7_0
*/
#define USBPowerStat (*(volatile unsigned long *)(0x4221019C))
#define USBConStat pPE9_IN
//#define _STX			0
//#define _SN			1
//#define _LENGTH_H		2
//#define _LENGTH_L		3
//#define _COMMAND		4
//#define _DATA			5
//#define _ETX			6
//#define _BCC			7
//#define _CR			8
//#define _PACKET_END	9

	#define _STX			0
	#define _LENGTH_H	1
	#define _LENGTH_L	2
	#define _COMMAND	3
	#define _PARAMETER	4
	#define _DATA		5
	#define _CRC			6
	#define _ETX			7
	#define _BCC			8
	#define _PACKET_END	9

typedef struct{
	volatile unsigned char Messsage_type;
	volatile unsigned int Lenth_ICC_DATA;
	volatile unsigned char Slot_Num;
	volatile unsigned int Seqence_Num;
	volatile unsigned char Resp_status;
	volatile unsigned int Error_code;
	volatile unsigned int RFU_ProtocolNum;
	unsigned char icc_data_buffer[512];
}ICC_Rx_Temp;


void delay_ms(volatile u32 msCount);

extern void Send_cmd_icc(uint Message, uchar *data,ushort size,uchar Slot, uchar resend,uchar T_Protocol);
extern uchar  icc_process(uchar MaxTimeCnt,ICC_Rx_Temp *ICC_Rx);
//extern uchar check_icc();
extern uchar check_NAK(ICC_Rx_Temp *ICC_Rx);
extern void Send_data_U3(uchar data);
extern void MS_Bit_DATA_R();
extern void Check_Download_ON();
extern void USBCmdProc();
extern void Make_Cmd_data();
extern uchar SolOn(ushort Time, ushort Duty);

#if 1 		//pbbch 180129 we need to know usb connect or not connect. so function add
extern void protect_usb_detect(void);
#endif

#endif
