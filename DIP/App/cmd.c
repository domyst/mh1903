/************************** (C) COPYRIGHT 2010 TITENG Co., Ltd Inc. *********************************/
/* project 	  : ky-7911 ms/r         		    				    */
/* processor 	  : CORETEX-M3(STM32F103vc)         		    				    */
/* compiler       : IAR EWARM Compiler								    */
/* file name      : cmd.c								    */
/* purpose       : usart1 �� ���� ���� cmd  ó��					    */
/* program by	  : jongil yun								    */
/* History:											    */
/* 		09/27/2010 - Version 0.1 Started						    */
/* copy right	  : TITENG Co., Ltd								    */
/****************************************************************************************************/
#include "cmd.h"

#if 1		//pbbch 171212 warning across
#include "stm32f10x_init.h"
#include "usb_regs.h"

#include "t_if_ic_sub.h"
#endif
#if defined(KTC_MODEL)
extern void DLK_MemoryRead32(void *pDst, void *pSrc, u32 len);
extern void DLK_MemoryWrite32(void *pDst, void *pSrc, u32 len);
#else
#define  DLK_MemoryRead32 memcpy
#define  DLK_MemoryWrite32 memcpy
#endif

#if defined(NICE_PROTOCOL)
#define FROM_BCD(n)     ((((n) >> 4) * 10) + ((n) & 0xf))
#define TO_BCD(n)       ((((n) / 10) << 4) | ((n) % 10))

#define PROTOCOL_TIT_MCRW	0
#define PROTOCOL_TIT_DIP	1
#define PROTOCOL_NICE		2

u8 gProtocol;
#endif

uchar DMATEst[DMABufferSize];
ushort CmdDMACnt =0 ;
ushort CmdCnt= 0 ;
uchar CmdLRC = 0;
ushort DataCnt = 0;
uchar ReceiveStat= _STX;
uchar if_bcc_count;
ushort if_bcc_data;
#if 1		//pbbch 180220 when protocol length error or etc erro occure,  timeout flg add
unsigned char lprotocol_timeout_flg=0;
#endif

/// always [2010/10/1] host ��� ����
///unsigned char usart1_Cmd_buffer[512];
/// always [2010/10/1] ���� ��ȣ�� ������ ����
// jsshin no use //unsigned short Cmd_buffer_cnt = 0;
/// always [2010/10/1] Len_H, Len_L ��
unsigned short Cmd_data_len;
/// always [2010/10/1] ��� ��ȣ�� bcc ��
// jsshin no use //unsigned char Cmd_bcc = 0;
/// always [2010/10/1] ��� ��ȣ�� bcc ���� �񱳸� ���� �ӽ� ��
unsigned char data_bcc = 0;
/// always [2010/10/1] ���� �迭 �� ������ ī��Ʈ
// jsshin no use //unsigned short buffer_cnt;
/// always [2010/10/22] ����� Data size
// jsshin no use //unsigned short Data_size;
/// always [2010/10/1] ��ɿ� ���� ���� data ����
unsigned short Resp_data_len;

/// always [2011/8/18] Lock ���� �� Ÿ�� �ƿ� ����
//uchar CheckLock = 0;
//uchar TimeCnt = 0;
/// always [2010/10/28] ī�� �Ǵ� sam ����
unsigned char card_select = 0;
//unsigned char card_select_dum = 0;// 20150302 hyesun : CR30�� SAM 1ea�̹Ƿ� MUX ������� �߰� ���� �߰�
	
/// always [2010/10/1] host ��� ����ü ����
Cmd_Temp cmd_format;
Response_P_Temp Response_P_format;
Response_N_Temp Response_N_format;

extern volatile unsigned int Lenth_ICC_DATA;
extern volatile unsigned int ICC_com_cnt;
//extern unsigned char icc_Send_buffer[512];
extern volatile unsigned short Send_cmd_cnt;

//extern unsigned char Short_card_flag;
//extern unsigned char NO_card_flag;
//extern unsigned char card_contact_flag;

/// always [2010/12/30] �ӽ÷� sam4�� ���¸� ����
extern uchar status_sam4;

/// always [2010/11/9] ms data
extern uchar gMS_DATA_T1[DATA_MAX_T1];
extern uchar gMS_DATA_T2[DATA_MAX_T2];
extern uchar gMS_DATA_T3[DATA_MAX_T3];

// jsshin 2015.10.12 extern uchar MS_DATA_ALL[DATA_MAX_ALL];
extern uchar Byte_DATA_cnt_T1;/********************///130425hs: Command �߰�~
extern uchar Byte_DATA_cnt_T2;/********************///130425hs: Command �߰�~
extern uchar Byte_DATA_cnt_T3;/********************///130425hs: Command �߰�~
extern uchar Byte_DATA_cnt_All;

extern uchar Status_T1;
extern uchar Status_T2;
extern uchar Status_T3;
extern uchar Status_ALL;

/// always [2010/12/28] ���巹��Ʈ ����
// jsshin no use //extern ushort usart_T_test[20];

/// always [2011/1/27] Des Test
uchar MasterKey[DES3_USER_KEY_LEN];
uchar SubKey[DES3_USER_KEY_LEN];
uchar MasterkeyUse;
extern uchar DesErrorFlag;
extern uchar IV[8];
extern const uchar Defaultkey[24];

//extern unsigned char icc_data_buffer[257];
u8 SAM_initial(void);		//pbbch 180321 void add.

uchar DesKeyBuffer[(DES3_USER_KEY_LEN*2)];
/// always [2010/10/8] �߿��� ���� ǥ��
/// always [2012/3/27] 1.12 - > 1.13 IC ī�� T1 ī�� ȣȯ ���� EMV ���� ����, RF Ŀ�ǵ� �߰�
/// always [2012/3/27] ûȣ�� ���� ��
/// always [2013/2/5] 1.13->1.14 ��� ��� �߻� �� icc com stat ���� �ʱ�ȭ ���з� ���� ���� �߻� ����, ���� ���ͷ�Ʈ���� ó���ϴ� ������ ��ġ ����
/// always [2013/2/5] ���� ���� �� ��Ŷ�� �޾ƾ� ó�� �ϴ����� ����Ʈ �ϳ��� ó��
#ifdef Hanmega_USA
/// 20150112 hyesun : �Ѹް� ��û���� ����ǥ�� ����
//uchar version[4] = {'V','T','0','2'};//�Ѹް� ������
//uchar version[4] = {'V','T','0','3'};//�Ѹް� ������ 20160415 : STAT Bit7 Lock Sensor -> Rear Sensor �� ����
//uchar version[4] = {'V','T','0','3'};//�Ѹް� ������ 20160524 : PCB 1/2/3�� merge, �Ѹް� ������ 20160713 : STAT Bit7(Lock Sensor), �Ѹް� ��û���� VT02 ���� ������Ʈ�� �߿���� VT03�� ����
//uchar version[4] = {'V','T','0','4'};// ������ ������ ��û�� ���� �������� �������� ���� , �Ѹް���û���� forward read MS data Clear!!
//uchar version[4] = {'V','T','0','5'};// 20160921 hyesun : �Ѹް� ������ ���� �� �̽����� �ǽ� �κ� ������ �߿���, MS ����Ÿ �Ľ��ϴ� �κп��� 
//uchar version[4] = {'V','T','0','6'};// 20170512 hyesun : �Ѹް� ������ IC Ĩ ���� ������ 261����Ʈ ���� ��� ���� ���� ���� ���� (�Ѹް��� �䱸�� ����,K�� ���� ������)// 20170531 hyesun : �Ѹް� ������ 61xx & 6cxx ó���� �߰�
uchar version[4] = {'V','T','0','7'};// 20170908 hyesun : �Ѹް� ������ IC ���� �ӵ� ���� ��û���� CR30 Ĩ ���� �ӵ� 9600bps -> 115200bps ��� �ӵ� ���� ����
									// 20170908 hyesun : �Ѹް� ������ ī�� ���� �� MS Read Block ��Ŵ, ������ ��������û �� ���� ���� ������ ������.
//***�Ѹް� Defined sumbols : Hanmega_USA, USE_ICC_115200	***//
#else
//uchar version[5] = {'V','1','.','1','5'};
//uchar version[5] = {'V','1','.','1','6'};//Reset time out ����


/*
KTC : 2016�� 01�� 04�� ������ V1.00 
  - IC�ŷ��� PAN �Ķ���� NULL ó�� �Ǵ� ���� 
*/
/* 
KTC : 2016�� 01�� 29�� ������ V1.01
 - DMA ���� �����÷ο� ���� ó��.
*/
/* 
KTC : 2016�� 02�� 01�� ������ V1.02
 - CR30 �ٿ�����. ī�� ���� �� Ÿ�Ӿƿ� �� CR30 �ʱ�ȭ.
 - DIK ���� �� TIT �������� ���� �ϵ��� ����.
 - MS ������ �б� �õ�.. 
*/

/* 
KTC : 2016�� 02�� 26�� ��Ÿ V1.03
- EMV Ŀ�� ���� : ������� M3/T3 ī�� ó��. ���� ������� üũī��.

*/

/* 
KTC : 2016�� 03�� 31��  V1.04A
- MS Read ���� ����. nCLS wait timeout ����. RCP buffering ���� ����.

*/
//uchar version[5] = {'V','1','.','0','6'};
//uchar version[5] = {'V','1','.','0','7'}; //���ǽý��� NFC ��� �߰�
//uchar version[5] = {'V','1','.','0','8'}; //ûȣ �̼��� ��� �߰� ����
//uchar version[5] = {'V','1','.','0','9'}; //���ǽý��� NFC
//uchar version[5] = {'V','1','.','1','0'}; //USB ��� Ÿ��, 160621
//uchar version[5] = {'V','1','.','1','1'}; //LGCNS ��û LED ������ �߰�
//uchar version[5] = {'V','1','.','1','2'}; //USB ��� Ÿ�� & RF option 
//uchar version[5] = {'V','1','.','1','3'}; //USB ��� Ÿ��
//uchar version[5] = {'V','1','.','1','4'}; // 20161005 ; T0,T1ī�� IC control ���� ���� ���� ISO mode Power on �� Set Parameter �� EMV mode Change�ϵ��� ������. 
//uchar version[5] = {'V','1','.','1','5'}; // 20161024: Manual Locking ON command �߰�
#if 1		//pbbch 171213 test//pbbch 180129 Hyosung dip-type...HID test cde and usb protect code add.
//uchar version[5] = {'V','1','.','1','7'}; // 20161214: USB&RS232, DualiRF// 20180208 neoicp ���� �׽�Ʈ ����
#else
//uchar version[5] = {'V','1','.','1','6'}; // 20161214: USB&RS232, DualiRF
uchar version[5] = {'T','1','.','1','6'}; // 20180116: Only RS232, ������A ICP		//pbbch 180123 neicp or save zoon code add
#endif
//uchar version[5] = {'V','t','e','s','t'}; // 20170118: ���� ��ŷ �׽�Ʈ��
//uchar version[5] = {'V','1','.','0','3'};
//uchar version[5] = {'V','1','.','0','2'};
//uchar version[5] = {'V','1','.','1','1'};

//uchar version[5] = {'V','1','.','1','8'}; 	// 20180212 neoicp ���� �׽�Ʈ ���� + ms cls 'h'��(card eject �� card�� ������ �� �� 2�ʵ��� retry�Ͽ� ms reading���� ��Ŵ.

//pbbch 180306 Neoicp&EB ��� ������ �����Ͽ�
//1) MS Read Function���� Timer1,4,5 timer_cnt�� CLS Low ���� ��Ȳ���� counter�� �����ڷ� �������� ��찡 �߻��Ͽ�, loop timeout counter�� �߰��Ͽ� 
//   Max about 1.9sec ���Ŀ��� �������� ����.
//2) loop timeout �߻� �� ms chip reset code �߰�.
//3) Neoicp & EB �׽�Ʈ ���� ����.
//uchar version[5] = {'V','1','.','1','9'}; 	

//pbbch 180322 version update
//1) V1.17���� ������ �ξ��� Contact IC,RF,USB Device ��� �ٽ� �߰�.
//2) Contact IC, RF �ʱ�ȭ ��, booting�ӵ� ������ ���ؼ� Ĩ�� ��� Timeout�ð��� short, normal�� �����Ͽ� ����. ���� �� 6�ʿ��� 1�� ���Ϸ� ������.
//3) USB connect pin ��ȣ�� �����Ͽ�, USB ���� �� �� �� �ʱ�ȭ ���� ����.
//4) USB �Ǵ� �ø��� ����� ���� �� ���� ���� �ŷ��� �Ǵ� Port���� ��� �� �� �ֵ��� ��� �߰�.
//5) IC Card Detect�� ���� �׻� enable ���ÿ��� Lock ������ �����Ͽ� MCU�� cotrol �� �� �ֵ��� ��� �߰�.
//ȿ�� ���� USB HID ReportDescriptor LOGICAL_MAXIMUM ���� ���� 0x25, 0xFf, ���� 0x25, 0x7f�� ����.
//LOGICAL_MAXMUM ���� ��� 0~127, 128���Ĵ� ���� (�� ���� bit 1)�� �ν��Ͽ� ���� �� ������ ������ �־���. ������ �� REPORT_ID���� 7���̸�, ������� ���� ���� Ȯ��. 
//uchar version[5] = {'V','1','.','2','0'}; 
//pbbch 181015
//1)ûȣ kiosk power on �� host uart ��� ��ְ� �߻��Ͽ�, 10�� ���� data�� ���� �ȵǸ�, uart �ʱ�ȭ�� ���� �ϵ��� ��� �߰� ��. ���� uart data�� ������ ���̻� �ʱ�ȭ ����.
//2)uart re-init�� led�� red�� toggle...���� packet ���Ž� green���� ���� toggle ��Ŵ.
//uchar version[5] = {'V','1','.','2','1'}; 
//pbbch 190620
//1)booting�� ms reset �κ� �߰�
//2)CR30 hanmega ��ȣ ���� �߰�, rx pullup ���� �߰�(���� ������)
//3)ic retry power on�ø� 1ȸ �߰�.
//4)CR30 �ʱ�ȭ ��, reset�� �ӵ������ϴ� �߸��� flow ����. 
//5)pwer reset�� ���� 1ȸ�� ���� �ϵ��� fix ��Ŵ.
//6)STM32 Uart �ʱ�ȭ �� ���� 100ms delay�� �߰�.
//7)reset rising time 500ns�� ����. spec�� 100ns �̻��̿��� ��.
//8)eeprom ���� �ڵ� Ȯ�� ����.
//9)ȿ������ ���� ���� ������Ʈ ������ V1.21���� V1.22�� ������Ʈ ���� ���� ��.
//uchar version[5] = {'V','1','.','2','2'}; 
//200508 : DualiRF option FW
uchar version[5] = {'V','1','.','2','4'}; 
#endif

extern uchar RX_Buffer[512];
extern ushort  Length;
extern uchar USBCmd;
extern uchar ReportID;

//uchar check_SAM=0;

/// always [2011/6/10] �ӵ� ����
extern ushort SpeedColl[500];
extern ushort SpeedCnt ;


extern FlashData TempFlashData;

#ifdef USE_RF
extern uchar RFData[256];
extern ushort RFData_CNT;
#endif

extern uint SolCnt ;
extern uchar StatSol;
extern uchar InDeal;
extern uchar LockUse;
uchar LED_Control = LED_Default;
uchar LEDTime_Control = Toggle_1sec;

uchar IC_Init = RESET;
#ifdef Hanmega_Russia
uchar SolDuty = 10;
#else
uchar SolDuty = 15;
#endif
extern ushort ADC_DATA[Number_of_ADC];
//extern uchar Delay_test_cnt;
//uchar Device_Response_cnt=0;

//extern uchar Test_flag;
//#ifdef SamsungPay
extern uchar SamsungPayMode;
//#endif

//extern TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
#define OLD	0
#define NEW	1
uchar gProtocol = NEW;

/// always [2010/10/8] Negative ���信 ���� ���� �ڵ� ����Ʈ
const uchar Negative_Respond_List[31][2] =
{
	{'0','0'},
	{'0','1'}, ///1,comand Not Define
	{'0','2'}, ///2No Card
	{'0','3'}, ///3Card Fail
	{'0','4'}, ///4Card Jam
	{'0','5'}, ///5Data Fail
	{'0','6'}, ///6Time Out
	{'0','7'}, ///7Wrong Command Data
	{'0','8'}, ///8M/S Blank Error
	{'0','9'}, ///9M/S preamlie Error
	{'1','0'}, //10M/S Parity Error
	{'1','1'},///11M/S post amble Error
	{'1','2'},///12M/S LRC Error
	{'1','3'},///13IFM Communication Error
	{'1','4'},////14IC Card Contact Error
	{'1','5'},///15IC Card Wrong Command length
	{'1','6'},///16IC Card is short circuiting
	{'1','7'},///17IC Card ATR is too long
	{'1','8'},///18IC Card APDU command length is wrong
	{'1','9'},///19IC Card buffer overflow
	{'2','0'},///20IC Card Parity Error
	{'2','1'},///21IC Card detects an excessive current
	{'2','2'},///22IC Card detects a defective voltage
	{'2','3'},///23IC Card command is not allowed
	{'2','4'},///24Command Cancel
	{'2','5'},///25Wrong Des Key Error
	{'2','6'},//Read Data Format Error(Charecter - Error)
	{'2','7'},//RF Initial Error
	{'2','8'},//The Carrier wave not emitted in antenna
	{'2','9'},//RF Card Contact Error
	{'3','0'},//Block Error
};

uchar Escape_IC(uchar *Data,ushort Datasize,uchar SubCmd,uchar TypeLevel);

#if 1		//pbbch 171212 warning across
extern u8 Check_Sensor_State();
extern void Send_u1_data(uchar data);
#endif

///////////////////////////////////////////////////////
const ushort Crc_Table[256] = {
//volatile INT16U  Crc_Table[256] = {	
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0 
};

#if 1
unsigned short Crc16Calc(ushort crc_data, uchar *buf_ptr, unsigned short len, uchar type)
{
	unsigned short int dt = 0;
	unsigned short int crc16 = 0;
	
	len *= 8;
	for(crc16 = 0x0000; len >= 8; len -= 8, buf_ptr++)
		{crc16 = Crc_Table[(crc16>>8) ^ *buf_ptr] ^ (crc16<<8);}
	if(len != 0)
	{
		dt = ((unsigned short int)(*buf_ptr)) << 8;
		while(len-- != 0)
		{
			if(((crc16^dt) & ((unsigned short int)1 << 15)) != 0)
			{
				crc16 <<= 1;crc16 ^= 0x1021;
			}
			else crc16 <<= 1;
			dt <<= 1;
		}
	}
//	#if DEBUG_TIU
//	Uart_Printf4("crc = %x,%x \n",crc_data,crc16);
//	#endif	
	if((!crc_data) && (type == SET))
	{
		return(crc16);
	}
	else
	{
		if(crc_data == crc16) return 0;//GOOD;
		else return 1;//FAIL;
	}	
	
}
//#else
uint getCRC(uchar *buf_ptr,int offset,int len)
{
      int value = 0;
      for(int i = offset; len > 0; --len, ++i) {
        int x = (value >> 8);
        int y = 0xffff & (value << 8);
        int b = buf_ptr[i];
        if (b < 0) b += 256;
        int z = (x ^ b);
        value =  (Crc_Table[z] ^ y);
      }
      return value;
}

#endif
/*******************************************************************************
* Function Name  : Make_data_Len
* Description    : data len_L, H �� ���
			   ushort data_len : 16��Ʈ ������ data len
*******************************************************************************/
void Make_data_Len(ushort data_len,uchar UseUSB)
{
	{
		if(UseUSB == RESET)
		{
			data_len += Nomal_data_len;
		}
		Response_P_format.Len_H = (data_len >> 8) & (u8)0xff;
		Response_P_format.Len_L= data_len & (u8)0xff;
	}
}

void Send_data_U3(uchar data)
{
	/// always [2010/12/22] test
	USART3->DR = (data & (u16)0x01FF);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
}
/*******************************************************************************
* Function Name  : Send_Response_data(uchar data)
* Description    : ��� ���� ��� �� ���� �� bcc �ۼ�
*******************************************************************************/
void Send_data(uchar data)
{
#ifdef DBG
	UART5->DR = (data & (u16)0x01FF);
	while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
#else
	if(g_pcb_version == PCB_CR30_R3)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART1->DR = (data & (u16)0x01FF);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
	else
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
		USART3->DR = (data & (u16)0x01FF);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	}
#endif
}

// domyst
void Send_data1(uchar data)
{
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) data);			// debug port : uart2, main : uart0
	while(!UART_IsTXEmpty(UART0));
}

void Uart_SendByte(int data)
{
	//Send_u5_data(data);
	Send_data(data);
}
/// always [2010/11/23] string ����Ʈ
void Uart_SendString(char *pt)
{
    while(*pt) Uart_SendByte(*pt++);
}

/// always [2010/11/23] printf �Լ�
void Uart_DBG_Printf(const char *fmt,...)
{
    va_list ap;
    char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    Uart_SendString(string);
    va_end(ap);
}
/*******************************************************************************
* Function Name  : Make_Cmd_data_Len()
* Description    : ������ �ΰ��� len ���� �ϳ��� ��ģ��.
*******************************************************************************/
void Make_Cmd_data_Len(uchar high_len,uchar low_len)
{
	Cmd_data_len = ((high_len & (u16)0x00ff) << 8);
	Cmd_data_len |= low_len & 0xff; 
}

/*******************************************************************************
* Function Name  : Make_resp_data_bcc(uchar resp_bcc)
* Description    : ������ bcc���� ���� ���� �񱳸� ���� data�� bcc�� ���
			    uchar resp_bcc : xor �� ������ ��
*******************************************************************************/
void Make_resp_data_bcc(uchar resp_bcc)
{
	data_bcc ^= resp_bcc;
}



/*******************************************************************************
* Function Name  : Send_Response_data(uchar data)
* Description    : ��� ���� ��� �� ���� �� bcc �ۼ�
*******************************************************************************/
#if 0 //org
void Send_Response_data(uchar data)
{
#ifdef DBG
  	Make_resp_data_bcc(data);
	UART5->DR = (data & (u16)0x01FF);
	
	while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
#else
	Make_resp_data_bcc(data);
	if(g_pcb_version == PCB_CR30_R3)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART1->DR = (data & (u16)0x01FF);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
	else
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET); 
		USART3->DR = (data & (u16)0x01FF);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	}
#endif
}
#endif
void Send_Response_data(uchar data)
{
#ifdef DBG
  	Make_resp_data_bcc(data);
	UART5->DR = (data & (u16)0x01FF);
	
	while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
#else
	Make_resp_data_bcc(data);
	// if(g_pcb_version == PCB_CR30_R3)
	// {
	// 	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	// 	USART1->DR = (data & (u16)0x01FF);
	// 	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	// }
	// else
	// {
	// 	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET); 
	// 	USART3->DR = (data & (u16)0x01FF);
	// 	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	// }
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) data);			// debug port : uart2, main : uart0
	while(!UART_IsTXEmpty(UART0));
#endif
}

/*******************************************************************************
* Function Name  : Make_data_bcc()
* Description    : ������ bcc���� ���� ���� �񱳸� ���� data�� bcc�� ���
*******************************************************************************/
/*
void Make_data_bcc()
{
	data_bcc ^= usart1_Cmd_buffer[buffer_cnt++];
}
*/

/*******************************************************************************
* Function Name  : compare_bcc()
* Description    : ������ bcc����?���� ���� ��
*******************************************************************************/
/*
// jsshin : no use delete
u8 compare_bcc()
{
	data_bcc ^= ETX;
	if(Cmd_bcc != data_bcc)
	{
		return 0;
	}
	return 1;
}
*/

void send_response(uchar *rsp, uchar length)
{
	uchar	i=0;

	for(i=0 ; i<length ; i++) Send_data(rsp[i]);
}

/*******************************************************************************
* Function Name  : Response()
* Description    : ��� ���� ��� �� ����
			   uchar status_position : ���� ���� P,N, NAK
*******************************************************************************/
void Response(uchar status_position)
{
	unsigned short resp_data_buffer_cnt;
	ushort crc16 =0 ;
	data_bcc = 0;

	if(gProtocol == NEW) 
	{
		switch (status_position)
		{
		case 'K' :
			//LED_1_OFF;
			//LED_2_ON;
			Send_data(NAK);
			
			break;
		case 'P' :
			//LED_1_ON;
			//LED_2_OFF;
			Response_P_format.Pos = Positive;
			Response_P_format.STAT = STAT_B & (u8)0xFF;
			/// always [2011/1/10] protocol revision 1.2 �߰�
			Response_P_format.STAT2 = STAT2_B & (u8)0xFF;

			//while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
			//delay_ms(100);
			Send_data(STX);
			
			Send_data(Response_P_format.Len_H);
			
			Send_data(Response_P_format.Len_L);
			
			Send_data(Response_P_format.Pos);

			Send_data(Response_P_format.STAT);

			/// always [2011/1/10] protocol revision 1.2 �߰�
			Send_data(Response_P_format.STAT2);
			
			for(resp_data_buffer_cnt = 0 ; resp_data_buffer_cnt < Resp_data_len ;resp_data_buffer_cnt++)
			{
				Send_data(Response_P_format.DATA[resp_data_buffer_cnt]);
			}

			#if 1
			Response_P_format.DATA[Resp_data_len]=ETX;
			crc16 = Crc16Calc(data_bcc,&Response_P_format.Len_H,Resp_data_len+6,SET);//Len_H ���� ETX Field���� CRC16�� ��
			
			Send_data(crc16 >> 8);//Crc_High
			Send_data(crc16 & 0x00FF);//Crc_Low
			#endif

			Send_Response_data(ETX);

	//		Send_data(data_bcc);

			memset(Response_P_format.DATA,0,sizeof(Response_P_format.DATA));

			break;
		case 'N' :
			//LED_1_OFF;
			//LED_2_ON;
			Response_N_format.Neg = Negative;

			Send_data(STX);
			
			Send_Response_data(Response_N_format.Len_H);
			
			Send_Response_data(Response_N_format.Len_L);
			
			Send_Response_data(Response_N_format.Neg);
			
			Send_Response_data(Response_N_format.ST1);

			Send_Response_data(Response_N_format.ST2);

			#if 1
			Response_N_format.etx=ETX;
			crc16 = Crc16Calc(data_bcc,&Response_N_format.Len_H,6,SET);//Len_H ���� ETX Field���� CRC16�� ��
			
			Send_data(crc16 >> 8);//Crc_High
			Send_data(crc16 & 0x00FF);//Crc_Low
			#endif
			
			Send_Response_data(ETX);
			
	//		Send_data(data_bcc);
			break;
		}
	}
	else
	{
		switch (status_position)
		{
		case 'K' :
			//LED_1_OFF;
			//LED_2_ON;
			Send_data(NAK);
			
			break;
		case 'P' :
			//LED_1_ON;
			//LED_2_OFF;
			Response_P_format.STAT = STAT_B & (u8)0xFF;
			/// always [2011/1/10] protocol revision 1.2 �߰�
			Response_P_format.STAT2 = STAT2_B & (u8)0xFF;

			//while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
			//delay_ms(100);
			Send_data(STX);

					#ifdef Hanmega_Russia
					Send_Response_data(Response_P_format.SN);
					#endif
			
			Send_Response_data(Response_P_format.Len_H);
			
			Send_Response_data(Response_P_format.Len_L);
			
			Send_Response_data(Positive);

			Send_Response_data(Response_P_format.STAT);

			#ifdef Hanmega_USA
			if(cmd_format.CMD == 'Z')Send_Response_data(0x00);
			#else
			/// always [2011/1/10] protocol revision 1.2 �߰�
			Send_Response_data(Response_P_format.STAT2);
			#endif
			
			for(resp_data_buffer_cnt = 0 ; resp_data_buffer_cnt < Resp_data_len ;resp_data_buffer_cnt++)
			{
				Send_Response_data(Response_P_format.DATA[resp_data_buffer_cnt]);
			}

			Send_Response_data(ETX);

			Send_data(data_bcc);

			memset(Response_P_format.DATA,0,sizeof(Response_P_format.DATA));

			break;
		case 'N' :
			//LED_1_OFF;
			//LED_2_ON;
			Response_P_format.Pos = Negative;

			Send_data(STX);
			
			Send_Response_data(Response_N_format.Len_H);
			
			Send_Response_data(Response_N_format.Len_L);
			
			Send_Response_data(Response_N_format.Neg);
			
			Send_Response_data(Response_N_format.ST1);

			Send_Response_data(Response_N_format.ST2);
			
			Send_Response_data(ETX);
			
			Send_data(data_bcc);
			break;
		}
	}
}
/*******************************************************************************
* Function Name  : Resp_Version_proc()
* Description    : F/W ���� ���� ����
*******************************************************************************/
void FWdownload_Resp_DATA_proc(uchar *Data,ushort size)
{
	Resp_data_len = size;

	//Make_data_Len(size,RESET);
	size += Nomal_data_len;
	Response_P_format.Len_L= size & (u8)0xff;
	
	memcpy(Response_P_format.DATA,Data,0);
	
	Response('P');
}

/*******************************************************************************
* Function Name  : Resp_Version_proc()
* Description    : F/W ���� ���� ����
*******************************************************************************/
void Resp_DATA_proc(uchar *Data,ushort size)
{
	Resp_data_len = size;

	Make_data_Len(size,RESET);

	Response('P');
}
/*******************************************************************************
* Function Name  : Resp_normal_proc()
* Description    : data ���� �븻 ���� ����
*******************************************************************************/
void Resp_normal_proc()
{
	Resp_data_len = 0;
	
	#ifdef Hanmega_Russia
	Response_P_format.SN = cmd_format.SN;
	#endif

	Response_P_format.Len_H=0;
	#ifdef Hanmega_USA
	Response_P_format.Len_L=Nomal_data_len-1;
	#else
	Response_P_format.Len_L=Nomal_data_len;
	#endif

	Response('P');
}
/*******************************************************************************
* Function Name  : Resp_N_proc()
* Description    : negative ����
			    st_num : ���� ����Ʈ �ѹ�
*******************************************************************************/
void Resp_N_proc(uchar st_num)
{

	#ifdef Hanmega_Russia
	Response_N_format.SN = cmd_format.SN;
	#endif

	Response_N_format.Len_H=0;
	Response_N_format.Len_L=Nomal_data_len;

	Response_N_format.ST1 = Negative_Respond_List[st_num][0];
	Response_N_format.ST2 = Negative_Respond_List[st_num][1];
	
	Response('N');
}

/*******************************************************************************
* Function Name  : SolOn(ushort Time,ushort Duty)
* Description    : Card�� ��� ���� �ַ����̵� �۵�
*******************************************************************************/
uchar SolOn(ushort Time,ushort Duty)
{
	ushort Differ;
	ushort TimeCnt;
	uchar RetryCnt= 0;

	//Differ = (((uint)(TIM3_ARR3)*(uint)Duty)/100);
	Differ = (TIM3_ARR3/100) *Duty;
	/// always [2011/5/11] �ַ����̵��� Ǯ ���� ���� �帣�� �ϱ� ���� ��
	SOL_CON = TIM3_ARR3;
	/// always [2011/5/11] �Էµ� Time �� ��ŭ �ִ� ���� �����Ѵ�.

	for(RetryCnt = 0;RetryCnt < 2;RetryCnt++)
	{
		for(TimeCnt = 0;TimeCnt <Time;TimeCnt++)
		{
			delay_ms(1);
		}
		//delay_ms(10);
		if(Check_Sensor_State(LOCK_SEN) == SET)
		//if(LockStat == SET)
		{
			RetryCnt = 0;
			break;
		}
	}
	/// always [2011/5/11] �Էµ� ��Ƽ ����Ŭ�� ����
	SOL_CON = Differ;
	return RetryCnt;
}
#if 0 //domyst
// nxp �ҽ� sub.c
void SolOn(uint16_t Time,uint32_t Duty)
{
	uint16_t TimeCnt;
	
	#if 0
	/* Initialize PWM submodule SM0 main configuration */
  PWM_Init(PWM2_PERIPHERAL, PWM2_SM0, &PWM2_SM0_config);
  /* Initialize fault input filter configuration */
  PWM_SetupFaultInputFilter(PWM2_PERIPHERAL, &PWM2_faultInputFilter_config);
  /* Initialize fault channel 0 fault Fault0 configuration */
  PWM_SetupFaults(PWM2_PERIPHERAL, PWM2_F0_FAULT0, &PWM2_Fault0_fault_config);
  /* Initialize fault channel 0 fault Fault1 configuration */
  PWM_SetupFaults(PWM2_PERIPHERAL, PWM2_F0_FAULT1, &PWM2_Fault1_fault_config);
  /* Initialize fault channel 0 fault Fault2 configuration */
  PWM_SetupFaults(PWM2_PERIPHERAL, PWM2_F0_FAULT2, &PWM2_Fault2_fault_config);
  /* Initialize fault channel 0 fault Fault3 configuration */
  PWM_SetupFaults(PWM2_PERIPHERAL, PWM2_F0_FAULT3, &PWM2_Fault3_fault_config);
  /* Initialize submodule SM0 channel B output disable mapping to the selected faults */
  PWM_SetupFaultDisableMap(PWM2_PERIPHERAL, PWM2_SM0, PWM2_SM0_B, kPWM_faultchannel_0, (kPWM_FaultDisable_0));
  /* Enable faults interrupts */
  PWM_EnableInterrupts(PWM2_PERIPHERAL, PWM2_SM0, (kPWM_Fault0InterruptEnable));
  /* Initialize deadtime logic input for the channel B */
  PWM_SetupForceSignal(PWM2_PERIPHERAL, PWM2_SM0, PWM2_SM0_B, kPWM_UsePwm);
  /* Setup PWM output setting for submodule SM0 */
  PWM_SetupPwm(PWM2_PERIPHERAL, PWM2_SM0, PWM2_SM0_pwm_function_config, 1U, kPWM_SignedCenterAligned, PWM2_SM0_COUNTER_FREQ_HZ, PWM2_SM0_SM_CLK_SOURCE_FREQ_HZ);
  #endif

	
	__disable_irq();
	XBARA_Init(XBARA);
	XBARA_SetSignalsConnection(XBARA, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm2Fault0);
	/* Call the init function with demo configuration */
	// PWM_DRV_Init3PhPwm();
	/* Update duty cycles for PWM signals */
    PWM_UpdatePwmDutycycle(PWM2_PERIPHERAL, kPWM_Module_0, kPWM_PwmB, kPWM_SignedCenterAligned, 100/*Duty*/);
    /* Set the load okay bit for all submodules to load registers from their buffer */
	PWM_SetPwmLdok(PWM2_PERIPHERAL, kPWM_Control_Module_0, true);
    /* Start the PWM generation from Submodules 0, 1 and 2 */
    PWM_StartTimer(PWM2_PERIPHERAL, kPWM_Control_Module_0 );


	for(TimeCnt = 0;TimeCnt <Time;TimeCnt++)
	{
		delay_ms(1);
	}

	/* Update duty cycles for PWM signals */
    PWM_UpdatePwmDutycycle(PWM2_PERIPHERAL, kPWM_Module_0, kPWM_PwmB, kPWM_SignedCenterAligned, Duty);
    /* Set the load okay bit for all submodules to load registers from their buffer */
	PWM_SetPwmLdok(PWM2_PERIPHERAL, kPWM_Control_Module_0, true);
	__enable_irq();
	
}

void SolOff(void)
{
	#if 0
	__disable_irq();
	XBARA_Init(XBARA);
	XBARA_SetSignalsConnection(XBARA, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm2Fault0);

	/* Update duty cycles for PWM signals */
    PWM_UpdatePwmDutycycle(PWM2_PERIPHERAL, kPWM_Module_0, kPWM_PwmB, kPWM_SignedCenterAligned, 0);
    /* Set the load okay bit for all submodules to load registers from their buffer */
	PWM_SetPwmLdok(PWM2_PERIPHERAL, kPWM_Control_Module_0, true);

	PWM_ClearStatusFlags(PWM2_PERIPHERAL, kPWM_Control_Module_0, 0);
	__enable_irq();
//	StatSol = 0;
//	SolCnt = 0;	
	#else
	__disable_irq();	
	XBARA_Init(XBARA);
	XBARA_SetSignalsConnection(XBARA, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm2Fault0);

	/* Update duty cycles for PWM signals */
    PWM_UpdatePwmDutycycle(PWM2_PERIPHERAL, kPWM_Module_0, kPWM_PwmB, kPWM_SignedCenterAligned, 0);
    /* Set the load okay bit for all submodules to load registers from their buffer */
	PWM_SetPwmLdok(PWM2_PERIPHERAL, kPWM_Control_Module_0, true);

	PWM_StartTimer(PWM2_PERIPHERAL, kPWM_Control_Module_0 );
	PWM_StopTimer(PWM2_PERIPHERAL, kPWM_Control_Module_0);
	PWM_ClearStatusFlags(PWM2_PERIPHERAL, kPWM_Module_0, 0);
	__enable_irq();
	#endif

}
// 
#endif
/*******************************************************************************
* Function Name  : L_CmdProc(uchar SlotNum)
* Description    : IC ����
*******************************************************************************/
uchar L_CmdProc(uchar SlotNum)			//domyst ������ ��
{
	if(g_pcb_version == PCB_GEM_POS)
	{
		if(SlotNum == 0x30)
		{
			card_select = ICC_0;
		}
		else if(SlotNum == 0x31)
		{
			card_select = SAM_1;
		}
		else if(SlotNum == 0x32)
		{
			card_select = SAM_2;
		}
		else if(SlotNum == 0x33)
		{
			card_select = SAM_3;
		}
		else if(SlotNum == 0x34)
		{
			card_select = SAM_4;
		}
		else
		{
			return ERROR;
		}
		return PRespons;
	}
	else
	{
		if(SlotNum == 0x30)
		{
			card_select = ICC_0;
			//card_select_dum = ICC_0;//SlotNum;
		}
		#ifndef Hanmega_USA
		else if(SlotNum == 0x31)
		{
			card_select = SAM_1;
			//card_select_dum = SAM_1;
			SamVCC_MUX1_ALL_OFF;
			SamCLK_SAM1;
			SamRST_SAM1;
			SamIO_SAM1;
			SamVCC_MUX1_A_ON;
		}
		else if(SlotNum == 0x32)
		{
			card_select = SAM_2;
			//card_select_dum = SAM_2;
			SamVCC_MUX1_ALL_OFF;
			SamCLK_SAM2;
			SamRST_SAM2;
			SamIO_SAM2;
			SamVCC_MUX1_B_ON;
		}
		else if(SlotNum == 0x33)
		{
			card_select = SAM_3;
			//card_select_dum = SAM_3;
			SamVCC_MUX1_ALL_OFF;
			SamCLK_SAM3;
			SamRST_SAM3;
			SamIO_SAM3;
			SamVCC_MUX1_C_ON;
		}
		else if(SlotNum == 0x34)
		{
			card_select = SAM_4;
			//card_select_dum = SAM_4;
			SamVCC_MUX1_ALL_OFF;
			SamCLK_SAM4;
			SamRST_SAM4;
			SamIO_SAM4;
			SamVCC_MUX1_D_ON;
		}
		#endif
		else
		{
			return ERROR;
		}
		return PRespons;
	}
}


/*******************************************************************************
* Function Name  : M_CmdProc(uchar *Data, ushort *DataSize)
* Description    : MS data�� ���� Ŀ�ǵ� ���� ��
*******************************************************************************/
uchar M_CmdProc(uchar *Data, ushort *DataSize)
{
	//	ulong DesLength;
	//	uchar DesBuffer[300];
	uchar ErrorState = 0;

	DesErrorFlag =RESET;

	if(MS_DATA_IN != SET)
	{
#ifdef Hanmega_USA
		if(Status_T2 >= 8)
#else
		if(Status_T2 != 0)
#endif
		{
			if(Status_T2 & 0xf0)
			{
				ErrorState = 10 + (Status_T2 & 0x0f);
			}
			else
			{
				ErrorState = Status_T2;
			}
		}
		else
		{
			ErrorState = MSBlankError;
		}
	}
	else
	{
		//++ jsshin 2015.10.12
		ulong DesLength;
		uchar MS_DATA_ALL[DATA_MAX_ALL];
		memset(MS_DATA_ALL, 0, sizeof(MS_DATA_ALL));
		Status_ALL = Make_All_data(Status_T1,Status_T2,Status_T3, MS_DATA_ALL );		// domyst msr ó��??
		//-- jsshin 2015.10.12
		if(Status_ALL)
		{
			/// always [2011/5/11]  Des�� ��� ����
			if(StatDes == SET)
			{
				DesLength=AppUeseCryption(ENCRYPTIONVALU,Byte_DATA_cnt_All,IV,SubKey,MS_DATA_ALL,Data);

				if(DesErrorFlag != SET)
				{
					//memcpy(Data,DesBuffer,DesLength);
					*DataSize = DesLength;
					ErrorState =  PRespons;
				}
				else
				{
					DesErrorFlag = RESET;
					ErrorState =  WrongDesKeyError;
				}
			}
			else
			{
				memcpy(Data,MS_DATA_ALL,Byte_DATA_cnt_All);
				*DataSize = Byte_DATA_cnt_All;
				ErrorState =  PRespons;
			}
		}
#ifdef Hanmega_USA
		/// 20150107 hyesun : �߰�
		Status_T1 = READ_ERROR_BLANK;
		Status_T2 = READ_ERROR_BLANK;
		Status_T3 = READ_ERROR_BLANK;
		Status_ALL = 0;
		Non_MS_DATA;/// 20150107 hyesun : �߰�
#endif

		//++ jsshin 2015.10.12
		memset(MS_DATA_ALL, 0, sizeof(MS_DATA_ALL));
		//-- jsshin 2015.10.12
	}
	return ErrorState;
}

/****************************************************************************************************************************130425hs: Command �߰�~
* Function Name  : m_CmdProc(uchar *Data, ushort *DataSize)
* Description    : MS Single data�� ���� Ŀ�ǵ� ���� ��
*******************************************************************************/
uchar m_CmdProc(uchar *Data, ushort *DataSize, uchar *CmdData)
{
//	ulong DesLength;
//	uchar DesBuffer[128];
	uchar ErrorState = MSBlankError;
	
	DesErrorFlag =RESET;

	if(MS_DATA_IN != SET)
	{
		// single track Read ����̹Ƿ� ��ȿ�� ������ ����.
		//
		/*
		if(Status_T2 != 0)
		{
			if(Status_T2 & 0xf0)
			{
				ErrorState = 10 + (Status_T2 & 0x0f);
			}
			else
			{
				ErrorState = Status_T2;
			}
		}
		else
		*/
		{
			ErrorState = MSBlankError;
		}
	}
	else
	{
		//++ jsshin 2015.10.12
		u16 Length = 0;
		ulong DesLength;
		u8 MS_DATA_SINGLE[DATA_MAX_SINGLE];
		memset(MS_DATA_SINGLE, 0, sizeof(MS_DATA_SINGLE));
    uchar MS_DATA_T1[DATA_MAX_T1];
    uchar MS_DATA_T2[DATA_MAX_T2];
    uchar MS_DATA_T3[DATA_MAX_T3];
    memset(MS_DATA_T1, 0x00, sizeof(MS_DATA_T1));
    memset(MS_DATA_T2, 0x00, sizeof(MS_DATA_T2));
    memset(MS_DATA_T3, 0x00, sizeof(MS_DATA_T3));
    
		/// always [2011/5/11]  Des�� ��� ����
		switch(CmdData[0])
		{
#ifdef T1
		//MS 1Track
		case 0x31:
			if(Status_T1 == SUCCESS && Byte_DATA_cnt_T1) 
			{
				DLK_MemoryRead32(MS_DATA_T1, gMS_DATA_T1, sizeof(gMS_DATA_T1));
				memcpy(MS_DATA_SINGLE,MS_DATA_T1,Byte_DATA_cnt_T1-1);//// 20141016 hyesun : ������ �ѹ���Ʈ �� ����, 1/2/3 Track ��ü ����(�߸��� �����Ͱ��̹Ƿ�)
				Length = Byte_DATA_cnt_T1-1;
				ErrorState =  PRespons;
			}
			else 
			{
 				if(Status_T1 & 0xf0)ErrorState = 10 + (Status_T1 & 0x0f);
				else ErrorState = Status_T1;
			}
			break;
#endif
#ifdef T2
		//MS 2Track
		case 0x32:
			if(Status_T2 == SUCCESS && Byte_DATA_cnt_T2) 
			{
				DLK_MemoryRead32(MS_DATA_T2, gMS_DATA_T2, sizeof(gMS_DATA_T2));
				memcpy(MS_DATA_SINGLE,MS_DATA_T2,Byte_DATA_cnt_T2-1);
				Length = Byte_DATA_cnt_T2-1;
				ErrorState =  PRespons;
			}
			else 
			{
				if(Status_T2 & 0xf0)ErrorState = 10 + (Status_T2 & 0x0f);
				else ErrorState = Status_T2;
			}
			break;
#endif
#ifdef T3
		//MS 3Track
		case 0x33:
			if(Status_T3 == SUCCESS && Byte_DATA_cnt_T3)
			{
				DLK_MemoryRead32(MS_DATA_T3, gMS_DATA_T3, sizeof(gMS_DATA_T3));
				memcpy(MS_DATA_SINGLE,MS_DATA_T3,Byte_DATA_cnt_T3-1);
				Length = Byte_DATA_cnt_T3-1;
				ErrorState =  PRespons;
			}
			else 
			{
				if(Status_T3 & 0xf0)ErrorState = 10 + (Status_T3 & 0x0f);
				else ErrorState = Status_T3;
			}
			break;
#endif
		default:
			ErrorState = WrongCommandData;
			break;
		}
			memset(MS_DATA_T1,0,sizeof(MS_DATA_T1));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
			memset(MS_DATA_T2,0,sizeof(MS_DATA_T2));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
			memset(MS_DATA_T3,0,sizeof(MS_DATA_T3));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
		//++ jsshin 2015.10.12
		// single track encrypt ��Ű���� �ڵ� ���� �ʿ�!!!!
		if(ErrorState == PRespons)
		{
			if(StatDes == SET)
			{
				DesLength=AppUeseCryption(ENCRYPTIONVALU,Length,IV,SubKey,MS_DATA_SINGLE,Data);
	
				if(DesErrorFlag != SET)
				{
					*DataSize = DesLength;
					ErrorState =  PRespons;
				}
				else
				{							
					DesErrorFlag = RESET;
					ErrorState =  WrongDesKeyError;
				}
			}
			else
			{
				memcpy(Data,MS_DATA_SINGLE,Length);
				*DataSize = Length;
				ErrorState =  PRespons;
			}
		}
		// stack clear!
		memset(MS_DATA_SINGLE, 0, sizeof(MS_DATA_SINGLE));
		memset(MS_DATA_T1, 0x00, sizeof(MS_DATA_T1));
		memset(MS_DATA_T2, 0x00, sizeof(MS_DATA_T2));
		memset(MS_DATA_T3, 0x00, sizeof(MS_DATA_T3));
		//-- jsshin 2015.10.12
	}
	return ErrorState;
}

/****************************************************************************************************************************///130425hs: ~Command �߰�


/*******************************************************************************
* Function Name  : CmpKey(uchar *OldKey,uchar *NewKey)
* Description    : Des�� Ű ���� ��ü�ϱ� ���� �ʿ��� ���� ������ Ű ���� ���Ѵ�.
*******************************************************************************/
uchar CmpKey(uchar *OldKey,uchar *NewKey)
{
	uchar MasterKeyStat;
	
	MasterKeyStat = memcmp(OldKey,NewKey,DES3_USER_KEY_LEN);
	
	if(!MasterKeyStat)
	{
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}
/*******************************************************************************
* Function Name  : FlashDataWrite()
* Description    : �÷��ÿ� ������ ������ ������ �������� �����Ѵ�.
*******************************************************************************/
/// always [2011/6/17] �߰�
void FlashDataWrite()
{
	uchar WordDataCnt;
	u32 *pKey;

	pKey = (u32 *)(&TempFlashData.MasterDesKey);
	
	FLASH_Unlock();
	FLASH_ErasePage((u32)MasterKeyStorage);
	for(WordDataCnt = 0; WordDataCnt <11;WordDataCnt ++ )
	{
		FLASH_ProgramWord((u32)(MasterKeyStorage+(WordDataCnt*4)),*(pKey+WordDataCnt));
	}
	FLASH_Lock();
}
/*******************************************************************************
* Function Name  : MasterKeySet(uchar *KeyData,uchar KeyLeng)
* Description    : ������ Ű ���� ����� ���� �Ѵٸ�, Ű ���� �÷��ÿ� �����Ѵ�.
*******************************************************************************/
uchar MasterKeySet(uchar *KeyData,uchar KeyLeng)
{
	uchar DesKeyLeng = 0;
	uchar *NewDesKey;
	memset(DesKeyBuffer,0,sizeof(uchar)*(DES3_USER_KEY_LEN*2));
	
	DesKeyLeng = AppUeseCryption(DECRYPTIONVALU,KeyLeng,IV,MasterKey,KeyData,DesKeyBuffer);
	/*
	for(cnt = 0; cnt <DesKeyLeng;cnt++ )
		Uart_DBG_Printf("%x",DesKeyBuffer[cnt]);
	Uart_DBG_Printf("\n");
	*/
	NewDesKey = &(DesKeyBuffer[24]);
	if((DesErrorFlag != SET)||(DesKeyLeng == (DES3_USER_KEY_LEN*2)))
	{
		if(CmpKey(MasterKey,DesKeyBuffer))
		{
			memcpy(TempFlashData.MasterDesKey,NewDesKey,sizeof(uchar)*DES3_USER_KEY_LEN);
			memcpy(MasterKey,TempFlashData.MasterDesKey,(sizeof(uchar)*DES3_USER_KEY_LEN));
			FlashDataWrite();

			MasterkeyUse = SET;
	
			return SUCCESS;
		}
		else
		{
			//DesErrorFlag = RESET;
			return ERROR;
		}
	}
	else
	{
		DesErrorFlag = RESET;
		return ERROR;
	}
}

/*******************************************************************************
* Function Name  : SubKeySet(uchar *KeyData,uchar KeyLeng)
* Description    : Des ����� ���� ���� Ű�� ����
*******************************************************************************/
uchar SubKeySet(uchar *KeyData,uchar KeyLeng)
{
	uchar DesKeyLeng = 0;
	uchar Cnt;
	//uchar DesKeyBuffer[(DES3_USER_KEY_LEN*2)];
	//u32 *pKey;
	
	memset(DesKeyBuffer,0,sizeof(uchar)*(DES3_USER_KEY_LEN*2));

	//ReadMasterKey(DES_USER_KEY_LEN);

	//if(MasterkeyUse == SET)
	//	{
	DesKeyLeng = AppUeseCryption(DECRYPTIONVALU,KeyLeng,IV,MasterKey,KeyData,DesKeyBuffer);
	///	}
	/*
	else
	{
		DesKeyLeng = AppUeseCryption(DECRYPTIONVALU,KeyLeng,IV,Defaultkey,KeyData,SubKey);
	}
	*/
	if((DesErrorFlag != SET)||(DesKeyLeng == (DES3_USER_KEY_LEN*2)))
	{
		//SubkeyUse = SET;	
		if(CmpKey(MasterKey,DesKeyBuffer))
		{
			for(Cnt = 0; Cnt <DES3_USER_KEY_LEN;Cnt ++ )
			{
				SubKey[Cnt] =DesKeyBuffer[DES3_USER_KEY_LEN+Cnt ];
			}

			UseDesON;
			return SUCCESS;
		}
		else
		{
			return ERROR;
		}
	}
	else
	{
		DesErrorFlag = RESET;
		return ERROR;
	}
}

/*******************************************************************************
* Function Name  : K_CmdProc(uchar *KeyData,uchar KeyLeng)
* Description    : Des Ű ���� ��� ���� ��
*******************************************************************************/
uchar K_CmdProc(uchar *KeyData,uchar KeyLeng)
{
	char Stat;
	uchar SubCmd;

	SubCmd = *(KeyData);
	
	switch(SubCmd)
	{
	/// always [2011/1/27] ������Ű ���� �� DES ON
	case 'M' :
		Stat = MasterKeySet((KeyData+1),KeyLeng);
		break;

	case 'S' :
		Stat = SubKeySet((KeyData+1),KeyLeng);
		break;
	}
	if(Stat == SUCCESS)
	{
		return PRespons;
	}
	else
	{
		return WrongDesKeyError;
	}
}
/*******************************************************************************
* Function Name  :WriteOptionByte(uchar OptionByte)
* Description    : �ɼ� ����Ʈ�� ���� �����Ѵ�.
*******************************************************************************/
void WriteOptionByte(uchar OptionByte)
{
	FLASH_Unlock();
	FLASH_EraseOptionBytes();
	FLASH_ProgramOptionByteData(0x1ffff804,OptionByte);
}
// domyst ���߿� ��������...ȣ���ϴµ� �ֽ�ó���Ǿ� ����
void stm32_reset_configure(void) {
	//USB_PowerOff();	
	//BEEP_Failure();
	
	
	/// always [2011/5/17] DMA ī���͸� �ʱ�ȭ �ϱ� ���ؼ��� DMA ��ü�� �� ���� �Ͽ��� �Ѵ�.
	if(g_pcb_version == PCB_CR30_R3)
	{
		DMA_Cmd(DMA1_Channel5, DISABLE);
	}
	else
	{
		DMA_Cmd(DMA1_Channel3, DISABLE);
	}
	
	USART_Cmd(USART1, DISABLE);
	USART_Cmd(USART3, DISABLE);
	USART_Cmd(UART4, DISABLE);
	USART_Cmd(UART5, DISABLE);
	
	delay_ms(100);	
	
	TIM_DeInit(TIM1);
	TIM_DeInit(TIM2);
	TIM_DeInit(TIM3);
	TIM_DeInit(TIM4);
	TIM_DeInit(TIM5);
	TIM_DeInit(TIM6);
	//__MSR_MSP(*(vu32*)(BOOT_START_ADDR));
	//Jump_to_Application = (pFunction)(*(vu32*)(BOOT_START_ADDR+4));
	//Jump_to_Application();	
}
/*******************************************************************************
* Function Name  : K_CmdProc(uchar *KeyData,uchar KeyLeng)
* Description    : Des Ű ���� ��� ���� ��
*******************************************************************************/
uchar Q_CmdProc(uchar SubCmd)
{
	uchar OptionByte;
#if defined(USE_ICC_115200)		//pbbch 180321 cr30 reset retry cnt
	char lretry_cnt=0;
#endif
	
	switch(SubCmd)
	{
	/// always [2011/5/16] ��ġ ����
	case 'D' :
		ICC_buffer_init();
		OptionByte = (uchar)bOptionByte & 0xff  ;
		OptionByte |= (0x1 << 1);
		WriteOptionByte(OptionByte);
		//stm32_reset_configure();		//pbbch 180201 add
		NVIC_SystemReset();
		//ICC_buffer_init();
		break;

	/// always [2011/5/16] IFM ����
	case 'I' :
		#if 0		//pbbch 180320 eject ���������� CR30�� Card detect signal�� high�� �־� card�� ������ �˷� detective�� ������ �ؾ� �Ѵ�.
		IFM_ICC_PRESENT;
		#endif
		NO_ICC_Reset;
		Non_SAM1;
		Non_SAM2;
		Non_SAM3;
		#ifndef Hanmega_USA
		Non_SAM4;
		#endif
#if defined(USE_ICC_115200)
	#if 1		//pbbch 180321 CR30 reset code revision
		for(lretry_cnt=0; lretry_cnt<3; lretry_cnt++)
		{
			if(SAM_initial()) break;
			delay_ms(10);
		}
	#else
		//IFM_reset_excution(0); //pbbch 180321 ifm reset add
		SAM_initial(); 
	#endif
#else
		// if(g_pcb_version == PCB_GEM_POS)		// domyst
		// {
			IFM_OFF;
			delay_ms(50); // 50ms ����.
			IFM_ON;	
			delay_ms(300); // 300ms ����.
		// }
		// else
		// {
		// 	CR30_RESET_ON;//CR30 reset active nRESET
		// 	delay_ms(1);
		// 	CR30_RESET_OFF;//CR30 reset
		// 	delay_ms(50);
		// }
#endif
		break;
	}
	return PRespons;
}


/*******************************************************************************
* Function Name  : IC_CmdProc(uchar IC_Cmd,uchar *Data,ushort *DataSize,uchar *CmdData, ushort CmdDataSize)
* Description    : IC Ŀ�ǵ� ó�� ���� ��, Reset, Direct, OFF ���
*******************************************************************************/
//extern unsigned char tim_resend_cnt;
#if 0		//pbbch 190329 not unsing variable. so across
static u8 gAPDU_Fail = 0;
#endif
uchar IC_CmdProc(uchar IC_Cmd,uchar *Data,ushort *DataSize,uchar *CmdData, ushort CmdDataSize,uchar RFU_0)
{
	uchar Card_Stat = 0; //,ResendCnt = 0;
	ICC_Rx_Temp ICC_Rx;
	//uchar OptionByte;
	//uchar ResetFlag;
	//OptionByte = (uchar)bOptionByte & 0xff  ;
	//ResetFlag = (OptionByte >> 1) & (0x1);
#if 0//defined(USE_ICC_115200)		//pbbch 180321 cr30 reset retry cnt//pbbch 190401 ic retry�� �ܺ� �Լ����� 1ȸ �����Ͽ� ����.
	char lretry_cnt=0;
#endif
	
	TIM_Cmd(TIM6, DISABLE);//TIM_Cmd(TIM2, DISABLE);
  
  ICC_buffer_init();
  
#if defined(USE_IWDG_RESET)
    /* Reload IWDG counter */
    IWDG_ReloadCounter();
#endif
  
	// jsshin 2016.03.27 : IFM initialize���� IC_CmdProc �Լ� ȣ������ �ʰ� ���� üũ�Ѵ�.
////////  if(IC_Init == SET) // ���ý� �ʱ� ���� ���� ���� �÷���.
////////  {
////////    Send_cmd_icc(IC_Cmd, CmdData,(CmdDataSize-1), card_select, RESET,RFU_0);
////////    Card_Stat = icc_process(3,&ICC_Rx);
//////////    if(Card_Stat != SUCCESS)
//////////    {
//////////      ICC_buffer_init();
//////////      Send_cmd_icc(IC_Cmd, CmdData,(CmdDataSize-1), card_select, RESET,RFU_0);
//////////      Card_Stat = icc_process(3,&ICC_Rx);
//////////    }
////////  }
////////  else
  {
#if 1	//pbbch 180517 real front sensor change
 	if((Check_Sensor_State(REAR_SEN) == SET && Check_Sensor_State(FRONT_REAL_SEN) == SET)||(card_select!=ICC_0)) // 2016.01.26 jsshin : card present check
#else
	if((Check_Sensor_State(REAR_SEN) == SET && Check_Sensor_State(FRONT_SEN) == SET)||(card_select!=ICC_0)) // 2016.01.26 jsshin : card present check
#endif
    //if((Front_Sensor_Status == SET)) // 2016.01.26 jsshin : card present check
    {
	#if 1
	Send_cmd_icc(IC_Cmd, CmdData,(CmdDataSize-1), card_select, RESET,RFU_0);// (CmdDataSize-1) => CMD 1����Ʈ ����
	#else
	Send_cmd_icc(IC_Cmd, CmdData,(CmdDataSize-2), card_select, RESET,RFU_0);// (CmdDataSize-2) => CMD PM 2����Ʈ ����
	#endif
      Card_Stat = icc_process(6,&ICC_Rx); // jsshin 2016.01.26 30�� -> 6�� ����
#if 1		//pbbch 190319 activaion exception add	//pbbch 190329 update
 	 if(IC_Cmd ==Power_ON) 
 	 {
 	 	if(((ICC_Rx.Resp_status >= 0x40) && (ICC_Rx.Resp_status <= 0x42))|| (Card_Stat == IFMCommunicationError))
 	 	{
	 	 	#if defined(USE_ICC_115200)
				SAM_initial(); 
			#else
			if(g_pcb_version == PCB_GEM_POS)
			{
				IFM_OFF;
				delay_ms(50); // 50ms ����.
				IFM_ON;	
				delay_ms(300); // 300ms ����.
			}
			else
			{
				CR30_RESET_ON;//CR30 reset active nRESET
				delay_ms(1);
				CR30_RESET_OFF;//CR30 reset
				delay_ms(50);
			}
			#endif
 	 	}
 	 }
	  #if 0		//pbbch 190329 not unsing variable. so across
	 else gAPDU_Fail = 0;
	 #endif
 #else      
      if((Card_Stat == NAK)||(Card_Stat == IFMCommunicationError))//20130904_hyesun: ��ſ��� ���� �߰�
      {
		// 2016.02.01 : ��� ���� �� IFM ���±�� �߰�! 
		//SAM_initial();
		 if(gAPDU_Fail >= 1)
		 {
#if defined(USE_ICC_115200)
	#if 1		//pbbch 180321 CR30 reset code revision
		for(lretry_cnt=0; lretry_cnt<3; lretry_cnt++)
		{
			if(SAM_initial()) break;
			delay_ms(10);
		}
	#else
			SAM_initial(); 
	#endif
#else
			if(g_pcb_version == PCB_GEM_POS)
			{
				IFM_OFF;
				delay_ms(50); // 50ms ����.
				IFM_ON;	
				delay_ms(300); // 300ms ����.
			}
			else
			{
				CR30_RESET_ON;//CR30 reset active nRESET
				delay_ms(1);
				CR30_RESET_OFF;//CR30 reset
				delay_ms(50);
			}
#endif
			gAPDU_Fail = 0;
			// power on ó��..
		 } else gAPDU_Fail++;
      } else gAPDU_Fail = 0;
#endif
// jsshin 2016.03.27 retry ����.
//      if((Card_Stat == NAK)||(Card_Stat == IFMCommunicationError))//20130904_hyesun: ��ſ��� ���� �߰�
//      {
//        ICC_buffer_init();
//        Send_cmd_icc(IC_Cmd, CmdData,(CmdDataSize-1), card_select, RESET,RFU_0);
//        Card_Stat = icc_process(6,&ICC_Rx); // jsshin 2016.01.26 30�� -> 6�� ����
//      }
    }
    else Card_Stat = ICCardContactError;
  }
  
	if(Card_Stat == SUCCESS)
	{
		memcpy(Data,ICC_Rx.icc_data_buffer,ICC_Rx.Lenth_ICC_DATA);
		*DataSize= ICC_Rx.Lenth_ICC_DATA;
		Card_Stat = PRespons;
	}
	//delay_us(1);//[20131107_hyesun]:�߰�
	//ICC_buffer_init();//[20140722_hyesun]:���� SAM ����
  
	return Card_Stat;
}

#ifdef UseUsb
void TxUSBData(uchar ResponseType,uchar NCord,uchar *Data,ushort DataSize)
{
	uchar USBDataBuffer[63];
	uchar ChainNum= 0;
	uchar ChiainCnt=0;
	//uchar TxSize;
	uchar DataCnt ;
	uchar USBTxStat = 0;
	uchar RestDataCnt = 0;
	//uchar DataTogle = 1;
	//DataSize += 2;
	ChainNum = DataSize/57;
	RestDataCnt = DataSize%57;
	memset(USBDataBuffer,0,sizeof(uchar)*63);
	/// always [2011/4/13] ���ε��� Data�ɰ��� �ۼ� ��
	/// always [2011/4/14] ������ �ۼ� ���
	switch(ResponseType)
	{
	case 'P':	
		USBDataBuffer[0] = 'P';
		/// always [2011/4/22] len_h, len_l ������ +2 ������
		Make_data_Len(DataSize,SET);
		USBDataBuffer[2] = Response_P_format.Len_H;
		USBDataBuffer[3] = Response_P_format.Len_L;
		
		USBDataBuffer[4] = STAT_B & (u8)0xFF;
		/// always [2011/1/10] protocol revision 1.2 �߰�
		USBDataBuffer[5] = STAT2_B & (u8)0xFF;

				
		for(ChiainCnt = 0;ChiainCnt <= ChainNum;ChiainCnt++)
		{
			memset(&USBDataBuffer[6],0,sizeof(uchar)*57);
			USBDataBuffer[1] = ChiainCnt;
			if(ChiainCnt < ChainNum)
			{
				for(DataCnt = 0;DataCnt < 57;)
				{
					USBDataBuffer[DataCnt+6] = Data[DataCnt+ChiainCnt*57];
					DataCnt++;
				}
			}
			/// always [2011/12/27] ������ ��Ŷ
			else
			{
				for(DataCnt = 0;DataCnt < RestDataCnt;)
				{
					USBDataBuffer[DataCnt+6] = Data[DataCnt+ChiainCnt*57];
					DataCnt++;
				}
			}
			UserToPMABufferCopy(USBDataBuffer, ENDP1_TXADDR, 63);
			SetEPTxCount(USBEndpoint, 63);
			SetEPTxValid(USBEndpoint);
				
			/// always [2011/7/12] ��Ŷ ���� �߰� tx �Ϸ� Ȯ��

			while(1)
			{
				/// always [2011/7/12] endp 1
				USBTxStat=GetEPTxStatus(1);//��� �����Ͱ� ���ۿϷ����� üũ�մϴ�.
				USBTxStat = USBTxStat & 0x30;
				if(USBTxStat==0x20) // ���ۿϷ� ���¶�� breakó��!     �ƴϸ� ��� ��ٸ��ϴ�.
				break;
				/// always [2011/11/18] usbcon�� ��� ����Ǹ� high, power�� low�϶� ����
				/// always [2011/11/18] ���� �����ų� USB�� �����ִٸ� �������´�.
				if((bDeviceState != 4)||(USBConStat == RESET)||(USBPowerStat == SET))
				{
					break;
				}
			}
			delay_ms(40);/// 20151124 hyesun : win8.1���� ���� �ذ�
		}
		 #if 1 		//pbbch 180129 usb normal packet detect. so fucture usb only use.
		 gusb_protect.packet_complete=1;
		 #endif
		break;
	case 'N':	
		USBDataBuffer[0] = 'N';
		USBDataBuffer[1] = Negative_Respond_List[NCord][0];
		USBDataBuffer[2] = Negative_Respond_List[NCord][1];
		UserToPMABufferCopy(USBDataBuffer, ENDP1_TXADDR, 63);
		SetEPTxCount(USBEndpoint, 63);
		SetEPTxValid(USBEndpoint);
		break;
	}

}
#endif
/*******************************************************************************
* Function Name  : Baudrate_Setting
* Description    :
			   ushort data_len : 16��Ʈ ������ data len
*******************************************************************************/
uchar  Baudrate_Setting(uchar cmd)
{
//	uchar  ErrorStat;
	uchar BaudrateStat;
	uint Baudrate;
	uchar HardwareFlowStat;
	ushort HardwareFlow;

	uchar OptionByte;

	#ifdef Hanmega_USA
	/// 20150108 hyesun : �߰�, ����
	cmd -= 0x30;
	#endif
	
	BaudrateStat = cmd & (0x03);
	HardwareFlowStat = cmd & (0x0c);
	
	Baudrate = SetBaudrate(BaudrateStat);
	HardwareFlow = SetHardwareFlow(HardwareFlowStat);

	// if(g_pcb_version == PCB_CR30_R3)
	// {
	// 	UART1_initial(Baudrate,HardwareFlow);	
	// }
	// else
	// {
	// 	UART3_initial(Baudrate,HardwareFlow);	
	// }

	UART0_initial(Baudrate,HardwareFlow);		// mh1903 uart0�� �Ҵ�, ȸ�ε��� �帧���� �Ǿ� ���� ����

	OptionByte = (uchar)bOptionByte & 0x0f  ;
	OptionByte |= cmd<<4;
	WriteOptionByte(OptionByte);
	/// always [2011/6/7] usart �ӵ� ���� �ڿ� �� ����Ʈ�� ��Դ� ���� �߻�
	//Send_data(STX);
	return PRespons;
}

/*******************************************************************************
* Function Name  : LockUse_Setting
* Description    :
			   ushort data_len : 16��Ʈ ������ data len
*******************************************************************************/
uchar  LockUse_Setting(uchar cmd)
{
	uchar OptionByte;
	
	OptionByte = (uchar)bOptionByte & 0xfb;
	OptionByte |= cmd<<2;
	WriteOptionByte(OptionByte);
	/// always [2011/6/7] usart �ӵ� ���� �ڿ� �� ����Ʈ�� ��Դ� ���� �߻�
	//Send_data(STX);
	return PRespons;
}


const u8 IFM_GetVersion[14] = {0x03, 0x06, 0x6B, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x6D};
const u8 CR30_57600[16] = 	{ 0x03, 0x06, 0x6B, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x10, 0x6C};
const u8 CR30_115200[16] = 	{ 0x03, 0x06, 0x6B, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x20, 0x5C};
//const u8 CR30_115200[] = 	{ 0x03, 0x06, 0x6B, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x20, 0x4d};
#if 1		//pbbch 180321 test ic protocol and ifm reset function add
u8 CR30_transaction_test(void)
{
	unsigned int i=0;
	unsigned int Card_Stat;
	unsigned char lcr30_test_cmd[16]={0x03, 0x06, 0x6B, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x6C};
	
	
	ICC_Rx_Temp ICC_Rx;
	memset(&ICC_Rx, 0, sizeof(ICC_Rx));
	
	TIM_Cmd(TIM6, DISABLE); // CR30 �� New Hw �̹Ƿ� TIM6 ���.
	ICC_buffer_init();
	
	for(i=0; i< sizeof(lcr30_test_cmd); i++)
	{
		Send_ICC_byte(lcr30_test_cmd[i]);
	}
	Card_Stat = icc_process(1,&ICC_Rx);
	if(Card_Stat == SUCCESS)
	{
		if(0x20 != ICC_Rx.icc_data_buffer[0]) Card_Stat = ERROR;
#if defined(DEBUG)
		u32 len = ICC_Rx.Lenth_ICC_DATA;
		printf("Transaction_test : ");
		for(i=0; i< ICC_Rx.Lenth_ICC_DATA; i++)
		{
			printf("%02X ", ICC_Rx.icc_data_buffer[i]);
		}
		printf("\r\n");
#endif
	}
	
	return Card_Stat;
}

void IFM_reset_excution(unsigned char ldevice_sel)
{
	CR30_RESET_ON;//CR30 reset active nRESET
	delay_ms(1);
	CR30_RESET_OFF;//CR30 reset
	delay_ms(50);
}
#endif

u8 Set_CR30_baudrate_115200(unsigned char wait)		//pbbch 180321 wait variable add
{
	u32 i, Card_Stat;
	
	
	ICC_Rx_Temp ICC_Rx;
	memset(&ICC_Rx, 0, sizeof(ICC_Rx));
	
	TIM_Cmd(TIM6, DISABLE); // CR30 �� New Hw �̹Ƿ� TIM6 ���.
	ICC_buffer_init();
#if defined(DEBUG)//pbbch 180321 debug add
		printf("CR30_115200 Send [%d] : ",sizeof(CR30_115200));
		for(i=0; i< sizeof(CR30_115200); i++)
		{
			printf("%02X ", CR30_115200[i]);
		}
		printf("\r\n");
#endif			
	for(i=0; i< sizeof(CR30_115200); i++)
	{
		Send_ICC_byte(CR30_115200[i]);
	}
	#if 1		//pbbch 180321 change wait time setting
	Card_Stat = icc_process(wait,&ICC_Rx);
	#else
	Card_Stat = icc_process(1,&ICC_Rx);
	#endif
	if(Card_Stat == SUCCESS)
	{
		if(0x20 != ICC_Rx.icc_data_buffer[0]) Card_Stat = ERROR;
	}
#if defined(DEBUG)//pbbch 180321 debug add
		printf("BAUDRATE Change Res[%d] : ",ICC_Rx.Lenth_ICC_DATA);
		if(ICC_Rx.Lenth_ICC_DATA>sizeof(ICC_Rx.icc_data_buffer))
		{
			printf("\r\nBAUDRATE Change Res  Length Error so change length[%d]->[%d]: ",ICC_Rx.Lenth_ICC_DATA,sizeof(ICC_Rx.icc_data_buffer));
			ICC_Rx.Lenth_ICC_DATA=sizeof(ICC_Rx.icc_data_buffer);
		}
		for(i=0; i< ICC_Rx.Lenth_ICC_DATA; i++)
		{
			printf("%02X ", ICC_Rx.icc_data_buffer[i]);
		}
		printf("\r\n");
		printf("Card_Stat[%d] \r\n",Card_Stat);
#endif	
	return Card_Stat;
}

const u8 verGEMPOSP[] = {"GemPosP-V1.00-GB08"};
const u8 verGEMCORE[] = {"GemCore SimPro 3.1"};

u8 Set_CR30_GetVersion(u8 wait)
{
	u8 i;
	u8 Card_Stat;
	
	ICC_Rx_Temp ICC_Rx;
	memset(&ICC_Rx, 0, sizeof(ICC_Rx));
	
	TIM_Cmd(TIM6, DISABLE); // CR30 �� New Hw �̹Ƿ� TIM6 ���.
	ICC_buffer_init();
#if defined(DEBUG)//pbbch 180321 debug add
		printf("IFM Version Send [%d] : ",sizeof(IFM_GetVersion));
		for(i=0; i< sizeof(IFM_GetVersion); i++)
		{
			printf("%02X ", IFM_GetVersion[i]);
		}
		printf("\r\n");
#endif		
	for(i=0; i< sizeof(IFM_GetVersion); i++)
	{
		Send_ICC_byte(IFM_GetVersion[i]);
	}
	#if 1		//pbbch 180321 change wait time setting
	Card_Stat = icc_process(wait,&ICC_Rx);
	#else
	Card_Stat = icc_process(1,&ICC_Rx);
	#endif
	if(Card_Stat == SUCCESS)
	{
		if(0 == memcmp(ICC_Rx.icc_data_buffer, verGEMCORE, 7)) //ICC_Rx.Lenth_ICC_DATA))
		{
			Card_Stat = SUCCESS; // return PCB_CR30_R1;			
		}
		else
		if(0 == memcmp(ICC_Rx.icc_data_buffer, verGEMPOSP, 7)) //ICC_Rx.Lenth_ICC_DATA))
		{
			Card_Stat = SUCCESS; //return PCB_GEM_POS;
		}
		else Card_Stat = ERROR;
	} else Card_Stat = ERROR;
#if defined(DEBUG)//pbbch 180321 debug add
		printf("IFM Version Res[%d] : ",ICC_Rx.Lenth_ICC_DATA);
		if(ICC_Rx.Lenth_ICC_DATA>sizeof(ICC_Rx.icc_data_buffer))
		{
			printf("\r\nBAUDRATE Change Res  Length Error so change length[%d]->[%d]: ",ICC_Rx.Lenth_ICC_DATA,sizeof(ICC_Rx.icc_data_buffer));
			ICC_Rx.Lenth_ICC_DATA=sizeof(ICC_Rx.icc_data_buffer);
		}
		for(i=0; i< ICC_Rx.Lenth_ICC_DATA; i++)
		{
			printf("%02X ", ICC_Rx.icc_data_buffer[i]);
		}
		printf("\r\n");
		printf("Card_Stat[%d]\r\n",Card_Stat);
#endif	
	return Card_Stat;
}

#if 1		//pbbch 180425 CR30 deactivation direct command send function add
unsigned char CR30_Deactivation_direct_execution(void)
{
	unsigned int i=0;
	unsigned int Card_Stat;
	unsigned char lcr30_deactivation_cmd[13]={0x03, 0x06, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66};
	
	
	ICC_Rx_Temp ICC_Rx;
	memset(&ICC_Rx, 0, sizeof(ICC_Rx));
	
	TIM_Cmd(TIM6, DISABLE); // CR30 �� New Hw �̹Ƿ� TIM6 ���.
	ICC_buffer_init();
#if defined(DEBUG)
		printf("Direct Deactivation Req: ");
		for(i=0; i< 13; i++)
		{
			printf("%02X ",lcr30_deactivation_cmd[i]);
		}
		printf("\r\n");
#endif	
	for(i=0; i< 13; i++)
	{
		Send_ICC_byte(lcr30_deactivation_cmd[i]);
	}
	Card_Stat = icc_process(0,&ICC_Rx);		//50ms apply..500cnt
	if(Card_Stat == SUCCESS)
	{
		if(0x20 != ICC_Rx.icc_data_buffer[0]) Card_Stat = ERROR;
#if defined(DEBUG)
		
		printf("Direct Deactivation Res: ");
		for(i=0; i< ICC_Rx.Lenth_ICC_DATA; i++)
		{
			printf("%02X ", ICC_Rx.icc_data_buffer[i]);
		}
		printf("\r\n");
#endif
	}
	
	return Card_Stat;
}
#endif

extern uchar SAM_OFF;
extern volatile unsigned char Slot_Num;
/******************************************************************************************************/
/* STAT_initial	:	SAM 1~4 �ʱ�ȭ  						      */												
/******************************************************************************************************/

//u8 SAM_initial(uchar SamNum)
//{
//	uchar Escape_Data[10];
//	uchar Escape_Data_leng=0;
//	uchar Esape_state = 0;
//	
//	Esape_state = Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToISO);
//
//}


/*
// IFM module check(GEMPOS or CR30)
*/
// IFM_nRESET
#if 1		//pbbch 190329 cr30 chip �ļ����� ���Ͽ� ��ȣ ���� ����.
u8 SAM_initial()
{
	uchar Esape_state = 0;
#if 0		//pbbch 181116 CR30 init�� ù TX byte ���� �κ� ���� �ڵ�
	unsigned int i=0;
#endif
	IC_Init = SET;////20140721 hyesun �߰�
	#if 0		//pbbch 190319 cr30 �ʱ�ȭ ��ġ  ����.//pbbch 190329 update//pbbch 190401 power on reset �κ� SAM_initial �Լ��� merge//pbbch 190412 power reset only 1 excute
		#if 1		//pbbch 190412 power on reset�� low, high �����ð��� ���� 300ms ��  fix
		CR30_RESET_ON;//CR30 reset active nRESET
		delay_ms(300);
		IFM_OFF;
		delay_ms(300); // 50ms ����.
		IFM_ON; 
		delay_ms(300); // 300ms ����.
		#else
		if(g_pcb_version == PCB_GEM_POS)
		{
			CR30_RESET_ON;//CR30 reset active nRESET
		}
		IFM_OFF;
		delay_ms(50); // 50ms ����.
		IFM_ON; 
		delay_ms(150); // 300ms ����.
		#endif
	#endif

#if !defined(USE_ICC_115200)
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4, DISABLE);
		if(g_pcb_version == PCB_GEM_POS)
		{
			IFM_OFF;
			delay_ms(50); // 50ms ����.
			IFM_ON; 
			delay_ms(300); // 300ms ����.
		}
		else
		{
			CR30_RESET_ON;//CR30 reset active nRESET
			delay_ms(300);//(1);		//pbbch 190412 power on reset�� low, high �����ð��� ���� 300ms ��  fix
			CR30_RESET_OFF;//CR30 reset
			delay_ms(300);//(50);		//pbbch 190412 power on reset�� low, high �����ð��� ���� 300ms ��  fix
                        // ������Ʈ ���� �ϸ� ó�� ����� ���� �ȴ�!!!!!
		Esape_state = Set_CR30_GetVersion(0);
		}
	
		//UART4_initial(BAUDRATE_9600); //IFM
	
	#if 0
	// ������Ʈ 115200 ���� �õ� ��ƾ...
	if(SUCCESS == Esape_state)
	{
		if(g_pcb_version != PCB_GEM_POS)
		{
			Esape_state = Set_CR30_baudrate_115200();	
			if(Esape_state != SUCCESS)
			{
				CR30_RESET_ON;//CR30 reset active nRESET
				delay_ms(1);
				CR30_RESET_OFF;//CR30 reset
				delay_ms(50);
			
				Esape_state = Set_CR30_baudrate_115200();	
			
			}
			if(Esape_state == SUCCESS)
			{
				UART4_initial(BAUDRATE_115200);//ICC
				delay_ms(5);
			
				// ������Ʈ ���� �ϸ� ó�� ����� ���� �ȴ�!!!!!
				Esape_state = Set_CR30_GetVersion(0);	
				if(Esape_state != SUCCESS)
				{
					// CR30 check & EMV Mode set & baudrate config...
					Esape_state = Set_CR30_GetVersion(1);	
				}
			}
		}
		else
		{
		{
	}
	#endif
#else
// ������Ʈ 115200 ���� �õ� ��ƾ...
//pbbch 180321 CR30 bardrate ���� ����.
//1. ������ �� �� ������, ù��° ����� bardrate ���� ���ο� ��� ���� ���� �Ѵ�.
//2. 1�� ������ 
//	1)115200�ӵ� ���� CMD ����(timeout ª�� ��. �ʱ�ȭ �ð��� ���̱� ���� )
//	2)���� �� reset
//	3)115200�ӵ� ���� CMD ����  
//	4)���� ���� �� ���� device bardrate�� default 9600���� 115200���� ����.
//	5)���� read ��� ����
//	6)���� �� �ٽ� ���� read ��� ����.
//	7)���� ���� �� ����.

#if 1	//pbbch 181114 baudreat ���� �� reset�� ���� ���� �Ǿ�� ��.
	//RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4, DISABLE);
	if(g_pcb_version == PCB_GEM_POS)
	{
		IFM_OFF;
		delay_ms(50); // 50ms ����.
		IFM_ON; 
		delay_ms(300); // 300ms ����.
	}
	else
	{
		#if 1		//pbbch 190319 delay change	//pbbch 190412 power on reset�� low, high �����ð��� ���� 300ms ��  fix
		CR30_RESET_ON;//CR30 reset active nRESET
		delay_ms(300);
		CR30_RESET_OFF;//CR30 reset
		delay_ms(300);		//CR30 rx ready time is about 31ms
		#else
		CR30_RESET_ON;//CR30 reset active nRESET
		delay_ms(1);
		CR30_RESET_OFF;//CR30 reset
		delay_ms(50);		//CR30 rx ready time is about 31ms
		#endif
	}

#endif
// ������Ʈ 115200 ���� �õ� ��ƾ...
	
	UART4_initial(BAUDRATE_9600); //
	#if 1		//pbbch 190412 baudrete setting�� 100ms delay add
	delay_ms(100);
	#endif

	//if(SUCCESS == Esape_state)
	{
		Esape_state = Set_CR30_baudrate_115200(0);	
		if(Esape_state != SUCCESS)
		{
			#if 1		//pbbch 190412 power on reset�� low, high �����ð��� ���� 300ms ��  fix
			CR30_RESET_ON;//CR30 reset active nRESET
			delay_ms(300);
			CR30_RESET_OFF;//CR30 reset
			delay_ms(300);		//CR30 rx ready time is about 31ms
			#else
			CR30_RESET_ON;//CR30 reset active nRESET
			delay_ms(10);
			CR30_RESET_OFF;//CR30 reset
			delay_ms(100);
			#endif
			
			Esape_state = Set_CR30_baudrate_115200(1);	
			
		}
		if(Esape_state == SUCCESS)
		{
			UART4_initial(BAUDRATE_115200);//ICC
			#if 1		//pbbch 190412 baudrete setting�� 100ms delay add
			delay_ms(100);
			#else
			delay_ms(5);//5
			#endif
			
			// ������Ʈ ���� �ϸ� ó�� ����� ���� �ȴ�!!!!!
			Esape_state = Set_CR30_GetVersion(0);	
			if(Esape_state != SUCCESS)
			{
				// CR30 check & EMV Mode set & baudrate config...
				Esape_state = Set_CR30_GetVersion(1);	
			}
		}
	}
#endif
	IC_Init = RESET;////20140721 hyesun �߰�
	return Esape_state;
}
#else
u8 SAM_initial(void)
{
	uchar Esape_state = 0;
	
	IC_Init = SET;////20140721 hyesun �߰�

#if !defined(USE_ICC_115200)
	#if 1		//pbbch 180129 ifm ������� �ݺ� Ȯ���ڵ带 ���� �ص� ȿ���� ����, delay �ð��� ��� sideimpact�� �߻� �Ͽ�, �ٽ� �ڵ带 ����ȭ ����. KHS code.
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4, DISABLE);
	if(g_pcb_version == PCB_GEM_POS)
	{
		IFM_OFF;
		delay_ms(50); // 50ms Ao����.
		IFM_ON;	
		delay_ms(300); // 300ms Ao����.
	}
	else
	{
		CR30_RESET_ON;//CR30 reset active nRESET
		delay_ms(1);
		CR30_RESET_OFF;//CR30 reset
		delay_ms(50);
	}

	//UART4_initial(BAUDRATE_9600); //IFM
	
	#else
	if(g_pcb_version == PCB_GEM_POS)
	{
		// GEMPOS�� RESET ��ȣ�� �����Ƿ� IFM_ON/OFF �Ѵ�...
		// ������Ʈ ���� �ϸ� ó�� ����� ���� �ȴ�!!!!!
		Esape_state = Set_CR30_GetVersion(0);
		if(Esape_state != SUCCESS) 
		{
			IFM_OFF;
			delay_ms(50); // 50ms ����.
			IFM_ON;	
			delay_ms(300); //300ms ����.
			Esape_state = Set_CR30_GetVersion(0);
		}
	}
	else
	{
		// CR30_nRESET(PB1)
		CR30_RESET_ON; //  pPB1_0 set LOW
		delay_ms(50); // 50ms ����.
		CR30_RESET_OFF;	//CR30 activate
		delay_ms(200); // 200ms ����.
		// ������Ʈ ���� �ϸ� ó�� ����� ���� �ȴ�!!!!!
		Esape_state = Set_CR30_GetVersion(0);
	}
	#endif
	if(Esape_state != SUCCESS)
	{
		//---
		if(g_pcb_version == PCB_GEM_POS)
		{
			IFM_OFF;
			delay_ms(50); // 50ms ����.
			IFM_ON;	
			delay_ms(300); // 300ms ����.
		}
		else
		{
			CR30_RESET_ON;//CR30 reset active nRESET
			delay_ms(1);
			CR30_RESET_OFF;//CR30 reset
			delay_ms(50);
		}
		Esape_state = Set_CR30_GetVersion(1);
	}
	#if 0
	// ������Ʈ 115200 ���� �õ� ��ƾ...
	if(SUCCESS == Esape_state)
	{
		if(g_pcb_version != PCB_GEM_POS)
		{
			Esape_state = Set_CR30_baudrate_115200();	
			if(Esape_state != SUCCESS)
			{
				CR30_RESET_ON;//CR30 reset active nRESET
				delay_ms(1);
				CR30_RESET_OFF;//CR30 reset
				delay_ms(50);
			
				Esape_state = Set_CR30_baudrate_115200();	
			
			}
			if(Esape_state == SUCCESS)
			{
				UART4_initial(BAUDRATE_115200);//ICC
				delay_ms(5);
			
				// ������Ʈ ���� �ϸ� ó�� ����� ���� �ȴ�!!!!!
				Esape_state = Set_CR30_GetVersion(0);	
				if(Esape_state != SUCCESS)
				{
					// CR30 check & EMV Mode set & baudrate config...
					Esape_state = Set_CR30_GetVersion(1);	
				}
			}
		}
		else
		{
		{
	}
	#endif
#else
// ������Ʈ 115200 ���� �õ� ��ƾ...
//pbbch 180321 CR30 bardrate ���� ����.
//1. ������ �� �� ������, ù��° ����� bardrate ���� ���ο� ��� ���� ���� �Ѵ�.
//2. 1�� ������ 
//	1)115200�ӵ� ���� CMD ����(timeout ª�� ��. �ʱ�ȭ �ð��� ���̱� ���� )
//	2)���� �� reset
//	3)115200�ӵ� ���� CMD ����  
//	4)���� ���� �� ���� device bardrate�� default 9600���� 115200���� ����.
//	5)���� read ��� ����
//	6)���� �� �ٽ� ���� read ��� ����.
//	7)���� ���� �� ����.

	//CR30_RESET_ON;//CR30 reset active nRESET
	//delay_ms(50);
	//CR30_RESET_OFF;//CR30 reset
	//delay_ms(500);
	
	UART4_initial(BAUDRATE_9600); //

	//Esape_state = CR30_transaction_test();
	//if(SUCCESS == Esape_state)
	{
		Esape_state = Set_CR30_baudrate_115200(0);	//500 timeout
		if(Esape_state != SUCCESS)
		{
			CR30_RESET_ON;//CR30 reset active nRESET
			delay_ms(1);
			CR30_RESET_OFF;//CR30 reset
			delay_ms(50);
			
			Esape_state = Set_CR30_baudrate_115200(1);	//10000 timeout...���� 712���� ����
			
		}
		if(Esape_state == SUCCESS)
		{
			UART4_initial(BAUDRATE_115200);//ICC
			delay_ms(5);
			
			// ������Ʈ ���� �ϸ� ó�� ����� ���� �ȴ�!!!!!
			Esape_state = Set_CR30_GetVersion(0);		//500 timeout
			if(Esape_state != SUCCESS)
			{
				// CR30 check & EMV Mode set & baudrate config...
				Esape_state = Set_CR30_GetVersion(1);		//10000 timeout...���� 712���� ����
			}
		}
	}
#endif
	IC_Init = RESET;////20140721 hyesun �߰�
	return Esape_state;
}
#endif

/*******************************************************************************
* Function Name  : Check_Pass(uchar *Pass)
* Description    : T cmd�� ���� �н����带 �˻��Ѵ�.
*******************************************************************************/
/// always [2011/6/17] �߰�
uchar Check_Pass(uchar *Pass)
{
	uchar Passward[6] = {'T','I','T','E','N','G'};
	uchar Cmp;

	Cmp = memcmp(Passward,Pass,6);

	return Cmp;
}

/*******************************************************************************
* Function Name  : T_CmdProc(uchar *CmdData,uchar SubCmd,uchar *DataBuffer,ushort *DataBufferSize)
* Description    : �׽�Ʈ�� Ŀ�ǵ�� �ӵ��� ��Ʈ�� ����
*******************************************************************************/
/// always [2011/6/17] �߰�
uchar T_CmdProc(uchar *CmdData,uchar SubCmd,uchar *DataBuffer,ushort *DataBufferSize)
{
	uchar ErrorState = 0;
	ushort TestCnt =0;
	ushort DataCnt = 0;

	/*
	uchar CheckPass;
	CheckPass = Check_Pass((CmdData+1));
	
	if(CheckPass != 0)
		{
			ErrorState = WrongCommandData;
		}
		
	else
		{*/
			switch(SubCmd)
				{
					/// always [2011/8/25] ms ���� �ӵ�
					case 's':
						*DataBufferSize = SpeedCnt * 2;
						for(DataCnt = 0; DataCnt < SpeedCnt ;)
						{
							*(DataBuffer + TestCnt++) = (SpeedColl[DataCnt] >> 8)& 0xff; 
							*(DataBuffer + TestCnt++) = (SpeedColl[DataCnt++] & 0xff);
						}
						ErrorState =PRespons;
						break;

					/// always [2011/8/25] ms bit data
					case 'b':
						//*DataBufferSize = Bit_Time_cnt_T2;
						//memcpy(DataBuffer,MS_Bit_Temp_T2,sizeof(uchar)*Bit_Time_cnt_T2);
						ErrorState =PRespons;
						break;
						
					case 'i':
						FlashSettingInit(SET);//(RESET);
						ErrorState =PRespons;
						break;
						
					case 'm':
						DataCnt = 1;
						if((*(CmdData + DataCnt) == 'M')||(*(CmdData + DataCnt) == 'I')||(*(CmdData + DataCnt) == 'A'))
							{
								TempFlashData.ModuleType=*(CmdData + DataCnt++);
								*DataBufferSize = 0;
								ErrorState =PRespons;
							}
						else
							{	
								ErrorState =	WrongCommandData;
							}	
						for(TestCnt =0; TestCnt < 7 ; TestCnt++)
							{
								if((*(CmdData + DataCnt) >= '0') && (*(CmdData + DataCnt) <= '9'))
									{
										TempFlashData.SerialNumber[TestCnt] = *(CmdData + DataCnt++);
										*DataBufferSize = 0;
										ErrorState =PRespons;
									}
								else
									{
										ErrorState = WrongCommandData;
									}
							}
						FlashDataWrite();
						break;
				}
			
		//}
	return ErrorState;
}

/*******************************************************************************
* Function Name  :  F_CmdProc(uchar *CmdData,uchar SubCmd,uchar *DataBuffer,ushort *DataBufferSize)
* Description    : ����� ������� ������ ��Ű��� Ÿ�� �ƿ� �� ��õ� �ð��� �����Ѵ�.
*******************************************************************************/
/// always [2011/6/17] �߰�
/*
uchar F_CmdProc(uchar *CmdData,uchar SubCmd,uchar *DataBuffer,ushort *DataBufferSize)
{
	ushort TempTime;
	uchar Datacnt = 0;
	uchar *pFlashData;
	ushort TimeMax = 60000;
	uchar CntMax = 10;
	uchar CmdResult;

 	switch(SubCmd)
		{
			/// always [2011/6/17] ����Ʈ Ŀ�ǵ�
			case 'W':
				Datacnt++;
				TempFlashData.Main_WatingTime_INComm = CmdData[Datacnt++];
				TempFlashData.Main_WatingTime_INComm = TempFlashData.Main_WatingTime_INComm << 8;
				TempFlashData.Main_WatingTime_INComm |= CmdData[Datacnt++];
				TempFlashData.Main_WatingTime_INComm *= 2;

				TempFlashData.IFM_WatingTime_Respons = CmdData[Datacnt++];
				TempFlashData.IFM_WatingTime_Respons = TempFlashData.IFM_WatingTime_Respons << 8;
				TempFlashData.IFM_WatingTime_Respons |= CmdData[Datacnt++];
				TempFlashData.IFM_WatingTime_Respons *= 2;

				TempFlashData.IFM_WatingTime_INComm = CmdData[Datacnt++];
				TempFlashData.IFM_WatingTime_INComm = TempFlashData.IFM_WatingTime_INComm << 8;
				TempFlashData.IFM_WatingTime_INComm |= CmdData[Datacnt++];
				TempFlashData.IFM_WatingTime_INComm *= 2;

				TempFlashData.IFM_RetryCnt_Respons = CmdData[Datacnt++];

				TempFlashData.IFM_RetryCnt_INComm = CmdData[Datacnt];

				if((TempFlashData.Main_WatingTime_INComm > TimeMax)|(TempFlashData.IFM_WatingTime_Respons > TimeMax)|(TempFlashData.IFM_WatingTime_INComm > TimeMax)|
					(TempFlashData.IFM_RetryCnt_Respons > CntMax)|(TempFlashData.IFM_RetryCnt_INComm > CntMax))
					{
						CmdResult = 1;
					}
				else
					{
						CmdResult = PRespons;
						FlashDataWrite();
					}
				break;

			/// always [2011/6/17] ���� Ŀ�ǵ�
			case 'R':
				TempTime = TempFlashData.Main_WatingTime_INComm >> 1;
				DataBuffer[Datacnt++] = (TempTime >> 8) & 0xff;
				DataBuffer[Datacnt++] = TempTime  & 0xff;
				TempTime = TempFlashData.IFM_WatingTime_Respons >> 1;
				DataBuffer[Datacnt++] = (TempTime >> 8) & 0xff;
				DataBuffer[Datacnt++] = TempTime  & 0xff;
				TempTime = TempFlashData.IFM_WatingTime_INComm >> 1;
				DataBuffer[Datacnt++] = (TempTime >> 8) & 0xff;
				DataBuffer[Datacnt++] = TempTime  & 0xff;

				DataBuffer[Datacnt++] = TempFlashData.IFM_RetryCnt_Respons;
				DataBuffer[Datacnt] = TempFlashData.IFM_RetryCnt_INComm;
				*DataBufferSize = Datacnt;
				CmdResult = PRespons; 
		}
	return CmdResult;
		
}

*/

uchar E_CmdProc(uchar WaitTime)
{
	uchar CmdResult; //, i = 0;

#if 1	//pbbch 180425 ic deactivation add
	//Send_cmd_icc(Power_OFF, ltemp_buf,0, ICC_0, RESET,0);// (CmdDataSize-1) => CMD 1����Ʈ ����
	CR30_Deactivation_direct_execution();
#endif
#if 1		//pbbch 180320 eject ���������� CR30�� Card detect signal�� high�� �־� card�� ������ �˷� detective�� ������ �ؾ� �Ѵ�.
	//IFM_ICC_ABSENT;
	GPIO_SetBits(GPIOB, GPIO_Pin_9);
#endif
	/////////////////////////
	// 2016.06.02 Solenoid ���� ��  CR30 ����!
#if defined(USE_ICC_115200)
	//M_initial(); // IFM ������ �߻����� ��� �ϴ��� SAM_initial�� ����... 2016.09.27
#else
	if(g_pcb_version == PCB_GEM_POS)
	{
		IFM_OFF;
		delay_ms(50); // 50ms ����.
		IFM_ON;	
		//delay_ms(300); // 300ms ����. // �ð� ������ ���� off ���� ���ð� ����.
	}
	else
	{
		CR30_RESET_ON;//CR30 reset active nRESET
		delay_ms(1);
		CR30_RESET_OFF;//CR30 reset
		//delay_ms(50); // �ð� ������ ���� off ���� ���ð� ����.
	}
#endif
	/////////////////////////
	// jsshin 2016.05.30 : add unlock retry
	if(Check_Sensor_State(LOCK_SEN) == SET)
	{
		__disable_irq();
		SolOn(50,SolDuty);				
		__enable_irq();
		delay_ms(50);
	}
	/////////////////////////
	/// always [2011/8/18] ���� s
	SOL_CON_OFF;
	
	while(WaitTime)
	{
		if(Check_Sensor_State(LOCK_SEN) != SET)
		//if(LockStat != SET)
		{
			/// 20150112 hyesun : �߰� 
			LockDetectOFF;
			NO_ICC_Reset;

			SolCnt = TempFlashData.AutoLocktime*60;//Sol_Locktime;
			StatSol = RESET;
			InDeal = RESET;
			SOL_CON_OFF;
			//bJamCard_ON;
			rCardIN_ON;
			CmdResult = PRespons;
			bLockSuc;//20140116_hyesun:Ǫ����� ���� ���� �߰� 

			break;
		}
		SOL_CON_OFF;
		delay_ms(1);
		WaitTime--;
	}

	if(WaitTime == 0)
	{
		InDeal = RESET;
		bLockFail;
		CmdResult = CardUnLockFail;
	}
	return CmdResult;
}

uchar l_CmdProc()//���� Card Locking On 20161024
{
	uchar CmdResult; //, i = 0;
	uchar WaitTime = 250;

	/// ī�尡 ���� ���� ��
#if 1	//pbbch 180517 real front sensor change
 	if((Check_Sensor_State(REAR_SEN) == SET) && (Check_Sensor_State(FRONT_REAL_SEN) == SET))//&&(rCardIN == RESET))
#else	
	if((Check_Sensor_State(REAR_SEN) == SET) && (Check_Sensor_State(FRONT_SEN) == SET))//&&(rCardIN == RESET))
#endif			
		{
			//if(StatSol == RESET)
			//{
				__disable_irq(); //__SETPRIMASK(); // jsshin 2015.08.18 change with CMSIS core func
				SolOn(100,SolDuty);
				rCardIN_ON;
				StatSol=SET;
				InDeal = SET;
				__enable_irq(); //__RESETPRIMASK(); // jsshin 2015.08.18 change with CMSIS core func
				
				while(WaitTime)
				{
					if(Check_Sensor_State(LOCK_SEN) == SET)
					{
						bLockSuc;//20140116_hyesun:Ǫ����� ���� ���� �߰� 
						CmdResult = PRespons;
						LockDetectON;
						break;
					}
					delay_ms(1);
					WaitTime--;
				}
				
				if(WaitTime == 0)
				{
					WaitTime =200;
					SolCnt = TempFlashData.AutoLocktime*60;//Sol_Locktime;
					StatSol=RESET;
					InDeal = RESET;
					SOL_CON_OFF;
					rCardIN_ON;
					bLockFail;
					CmdResult = CardLockFail;
					LockDetectOFF;
				}
			//}		
		}
		else
		{
			CmdResult = NoCard;
		}
	return CmdResult;
}

/*
void CheckT1(uchar *Data,ushort Datasize)
{
	uchar TParameter = 0,CmdResult;
	uchar SetParaBuffer[7];
	SetParameter CmdData;
	ushort DataBufferSize = 0;
	TParameter = *(Data + 1);
//	if((TParameter & 0xe0) == 0xe0)
//	{
		if(g_pcb_version == PCB_GEM_POS)
		{
			CmdData.bmFindexDindex = 0x11;
			CmdData.bmTCCKST1 = 0x00;
			CmdData.bGuardTimeT1 = 0x00;
			CmdData.bWaitingIntegerT1 = 0x00;
			CmdData.bClockStop = 0;
			//CmdData.bIFSC = 0x32;
			//CmdData.bNadValue = 0;
			CmdResult = IC_CmdProc(Setpara,Data,&DataBufferSize,&CmdData.bmFindexDindex,6,0);
			//CmdResult = IC_CmdProc(0x6D,Data,&DataBufferSize,&CmdData.bmFindexDindex,6,0);
		}
		else if(g_pcb_version == PCB_CR30_R3)
		{
			CmdData.bmFindexDindex = 0xD2;
			CmdData.bmTCCKST1 = 0x00;
			CmdData.bGuardTimeT1 = 0x6D;
			CmdData.bWaitingIntegerT1 = 0x08;
			CmdData.bClockStop = 0;
			//CmdData.bIFSC = 0x32;
			//CmdData.bNadValue = 0;
			CmdResult = IC_CmdProc(Setpara,Data,&DataBufferSize,&CmdData.bmFindexDindex,6,0);
		}
		else
		{
			CmdData.bmFindexDindex = 0x11;
			CmdData.bmTCCKST1 = 0x10;
			CmdData.bGuardTimeT1 = 0x00;
			CmdData.bWaitingIntegerT1 = 0x45;
			CmdData.bClockStop = 0;
			CmdData.bIFSC = 0x32;
			CmdData.bNadValue = 0;
			CmdResult = IC_CmdProc(Setpara,Data,&DataBufferSize,&CmdData.bmFindexDindex,8,1);
		}
//	}
}
*/
uchar Escape_IC(uchar *Data,ushort Datasize,uchar SubCmd,uchar TypeLevel)
{
	uchar CmdResult;
	//uchar TParameter = 0;
	//uchar SetParaBuffer[7];
	Escape Escape_Data;
	ushort DataBufferSize = 0;
	uchar Dataleng;
	if(g_pcb_version == PCB_GEM_POS)
	{
		// EMV mode Set �̸�... GEMPOS Ĩ�� �´� 0x72������� ����.
		if(TypeLevel == 0x02) TypeLevel = 0x72;
	}
	//TParameter = *(Data + 1);
	switch(SubCmd)
	{
	case 0x02:
		Escape_Data.bCommandEscape = SubCmd;
		Dataleng = 2; // jsshin bug??? break ����!!!
		break; // jsshin break �߰�.
	case ChangeMode:
		Escape_Data.bCommandEscape = SubCmd;
		Escape_Data.bTypeLevel =TypeLevel;
		Dataleng = 3;
		break; // jsshin break �߰�.
	}
	/// always [2012/2/17] Data ���̴� +1���ٰ�
	CmdResult = IC_CmdProc(Escape_ICC,Data,&DataBufferSize,&Escape_Data.bCommandEscape,Dataleng,0);
	//WaitingICRespons(Escape_ICC);
	return CmdResult;
}
#ifdef USE_RF
void InitRFDATA()
{
	RFData_CNT = 0;
	memset(RFData,0,sizeof(RFData));
}

#ifdef RF_NFC
uchar RFReceive1Proc(uchar *Data,ushort *Datasize,uchar wait_time)
{

	uint i;
	#if 0		//pbbch 171213 warning across
	unsigned char temp=0;
	#endif
	uchar ch,status=0;
	uint nwait_time=0;
	ushort Data_Leng=0;
	ushort DataCNT =0;
	uchar Data_BCC=0;

	//ReceiveStart = RECEIVEREADY;
	status = _STX;
	#if 1		//pbbch 180321 ��� �ð��� �����ϱ� ���ؼ� ���� �߰�.
	if(wait_time)
	{
		nwait_time = wait_time * 2000;
	}
	else
	{
		nwait_time = 500; // 50ms
	}
	#else
	nwait_time = wait_time * 2000;
	#endif
	for(i=0;i<nwait_time;i++)			// 2�� Ÿ�Ӿƿ� Loop
	{
		if(RFData_CNT > DataCNT)
		{
			ch = RFData[DataCNT];
			//Uart_DBG_Printf("%x,",ch);
			switch(status)
			{
			case _STX:
				if(ch == STX)
				{
					*(Data + DataCNT++) = ch;
					status = _LENGTH_H;
				}
				else status = _STX;
				break;	
			case _LENGTH_H:
				Data_Leng = *(Data + DataCNT++) = ch;
				Data_BCC = ch;
				status = _LENGTH_L;
				break;
			case _LENGTH_L:
				*(Data + DataCNT++) = ch;
				Data_BCC ^= ch;
				Data_Leng = (Data_Leng << 8) | ch;
				status = _DATA;
				break;
			case _DATA:
				*(Data + DataCNT++) = ch;
				Data_BCC ^= ch;
				if(Data_Leng == DataCNT-3)
				{
					status = _BCC;
				}
				/*if((ch == ETX)&&(Data_Leng >= DataCNT-4))
				{
					status = _PACKET_END;
				}*/
				break;
			case _BCC:
				*(Data + DataCNT++) = ch;
				if(Data_BCC == ch)
				{
					status = _PACKET_END;
				}
				break;
			default:
				status = _STX;
				break;										
			}	
		}
		else
		{	
			delay_ms(1);	
		}	
		if(status == _PACKET_END)	
		{
			*Datasize = DataCNT;
			status = PRespons;
			break;
		}
	}
	if(i == nwait_time)
	{
		status = RFCommError;
	}
#if defined(DEBUG)//pbbch 180321 debug add
	printf("RF_Rev_State[%d]...wating_cnt[%d] \r\n",status,i);
	printf("RF Buffer Res[%d] : ",RFData_CNT);
	for(i=0; i< RFData_CNT; i++)
	{
		printf("%02X ", RFData[i]);
	}
	printf("\r\n\r\n");
#endif	
	InitRFDATA();
	return status;
}

#else
uchar RFReceive1Proc(uchar *Data,ushort *Datasize,uchar wait_time)
{

	uint i;
	#if 0		//pbbch 171213 warning across
	unsigned char temp=0;
	uchar Data_BCC=0;
	#endif
	uchar ch,status=0;
	uint nwait_time=0;
	ushort Data_Leng=0;
	ushort DataCNT =0;
	

	//ReceiveStart = RECEIVEREADY;
	status = _STX;
	#if 1		//pbbch 180321 ��� �ð��� �����ϱ� ���ؼ� ���� �߰�.//normal 115
	if(wait_time)
	{
		nwait_time = wait_time * 2000;
	}
	else
	{
		nwait_time = 200; // 50ms
	}
	#else
	nwait_time = wait_time * 2000;
	#endif
	for(i=0;i<nwait_time;i++)			// 2�� Ÿ�Ӿƿ� Loop
	{
		if(RFData_CNT > DataCNT)
		{
			ch = RFData[DataCNT];
			//Uart_DBG_Printf("%x,",ch);
			switch(status)
			{
			case _STX:
				if(ch == STX)
				{
					*(Data + DataCNT++) = ch;
					status = _LENGTH_H;
				}
				else status = _STX;
				break;	
			case _LENGTH_H:
				Data_Leng = *(Data + DataCNT++) = ch;
				#if 0		//pbbch 171213 warning across
				Data_BCC = ch;
				#endif
				status = _LENGTH_L;
				break;
			case _LENGTH_L:
				*(Data + DataCNT++) = ch;
				//Data_BCC ^= ch;
				Data_Leng = (Data_Leng << 8) | ch;
				status = _DATA;
				break;
			case _DATA:
				*(Data + DataCNT++) = ch;
				//Data_BCC ^= ch;
				if((ch == ETX)&&(Data_Leng >= DataCNT-4))
				{
					status = _PACKET_END;
				}
				break;
			default:
				status = _STX;
				break;										
			}	
		}
		else
		{	
			delay_ms(1);	
		}	
		if(status == _PACKET_END)	
		{
			*Datasize = DataCNT;
			status = PRespons;
			break;
		}
	}
	if(i == nwait_time)
	{
		status = RFCommError;
	}
#if defined(DEBUG)//pbbch 180321 debug add
	printf("RF_Rev_State[%d]...wating_cnt[%d] \r\n",status,i);
	printf("RF Buffer Res[%d] : ",RFData_CNT);
	for(i=0; i< RFData_CNT; i++)
	{
		printf("%02X ", RFData[i]);
	}
	printf("\r\n\r\n");
#endif		
	InitRFDATA();
	return status;
}
#endif

void Send_RF(uchar *CmdData,ushort CmdDatasize)
{
	ushort Send_Cnt;
	
	for(Send_Cnt = 0;Send_Cnt<CmdDatasize;Send_Cnt++)
	{
		if(g_pcb_version == PCB_CR30_R3)
		{
			Send_u3_data(*(CmdData+Send_Cnt));
		}
		else
		{
			Send_u1_data(*(CmdData+Send_Cnt));
		}
		//Uart_DBG_Printf("%x,",*(CmdData+Send_Cnt));
	}
	//Uart_DBG_Printf("\n");
}

uchar F_CmdProc(uchar *CmdData,ushort CmdDatasize,uchar *Data, ushort *Datasize,uchar wait_time)//pbbch 180321 waing time ������ ���� ����.
{
	
	uchar State=0;
	#if 0		//pbbch 171213 warning across
	uchar i;
	uchar RF_Cmd = 0;
	uchar RF_On_TIM = 0;
	
	RF_Cmd = *(CmdData+3);
	//RF_On_TIM = *(CmdData+5);
	#endif

	delay_ms(1);
	#if 1		//pbbch 171213 warning across
	InitRFDATA();
	Send_RF(CmdData,CmdDatasize);			
	State = RFReceive1Proc(Data,Datasize,wait_time);		//pbbch 180321 waing time ������ ���� ����.
	#else
	//for(i=0;i<3;i++)
	{
		InitRFDATA();
		Send_RF(CmdData,CmdDatasize);			
		State = RFReceive1Proc(Data,Datasize,1);
	//	if(State == PRespons)break;
	}
	#endif
	if(State != PRespons)
	{
		#if 0		//pbbch 171213 warning across.......but need debuging
		RFCommError;	
		#endif
	}
	if(State == PRespons)
	{
		Connect_RF_Module;	
	}
	else
	{
		Disconnect_RF_Module;
	}
	return State;
}
#endif
uint testcnt=1;

/*******************************************************************************
* Function Name  : Cmd_proc()
* Description    : ������ bcc����?���� ���� ��
*******************************************************************************/
void Cmd_proc_new(uchar Command,uchar Parameter,uchar *CmdData,ushort CmdDatasize, uchar UseUSB)
{
	//uchar DataBuffer[1024];
	uchar *DataBuffer;
	ushort DataBufferSize = 0;
	uchar CmdResult = ComandNotDefine;
	uint TempCnt;
	uchar Escape_Data[10];
	uchar Escape_Data_leng=0;
#if 0		//pbbch 171212 warning across	
	uchar ATR_Result =0;
#endif
#if ((!defined(DONT_CARE_LOCK)) ||defined(Hanmega_USA))		//pbbch 171212 warning across and move form switch case 'I'
	uchar i=0;
#endif	

	DataBuffer = Response_P_format.DATA;
	//memset(DataBuffer, 0, sizeof(uchar)*1024);

	switch(Command)
	{
		 case 0x30 :
			if(Parameter == 0x30)
			{
				DataBufferSize = 0;
				CmdResult = PRespons;
			 }
			else CmdResult = ComandNotDefine;
			break;
		case 0x33 :
			if(Parameter == 0x30)
			{
				CmdResult = E_CmdProc(200);
				DataBufferSize = 0;
			}
			else CmdResult = ComandNotDefine;
			break;
		case 0x36 :
			if(Parameter == 0x30)
			{
		        	CmdResult = M_CmdProc(DataBuffer,&DataBufferSize);
				if(PRespons == CmdResult)
				{
					Non_MS_DATA;
					#ifdef T1
					memset(gMS_DATA_T1,0,sizeof(gMS_DATA_T1));
					#endif
					#ifdef T2
					memset(gMS_DATA_T2,0,sizeof(gMS_DATA_T2));
					#endif
					#ifdef T3
					memset(gMS_DATA_T3,0,sizeof(gMS_DATA_T3));
					#endif
					Byte_DATA_cnt_All = 0;
					Status_T1 = READ_ERROR_BLANK;
					Status_T2 = READ_ERROR_BLANK;
					Status_T3 = READ_ERROR_BLANK;
					Status_ALL = 0;
				}
			}
			else if(Parameter == 0x31 || Parameter == 0x32 || Parameter == 0x33)
			{
				CmdData[0]=Parameter;
				CmdResult = m_CmdProc(DataBuffer,&DataBufferSize,CmdData);
				if(Parameter == 0x31) 
				{
					#ifdef T1
					Byte_DATA_cnt_T1 = 0;
					Status_T1 = READ_ERROR_BLANK;
					memset(gMS_DATA_T1,0,sizeof(gMS_DATA_T1));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
					#endif
				}
				if(Parameter == 0x32) 
				{
					#ifdef T2
					Byte_DATA_cnt_T2 = 0;
					Status_T2 = READ_ERROR_BLANK;
					memset(gMS_DATA_T2,0,sizeof(gMS_DATA_T2));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
					#endif
				}
				if(Parameter == 0x33) 
				{
					#ifdef T3
					Byte_DATA_cnt_T3 = 0;
					Status_T3 = READ_ERROR_BLANK;
					memset(gMS_DATA_T3,0,sizeof(gMS_DATA_T3));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
					#endif
				}
		    }
			else if(Parameter == 0x35)
			{
				/// always [2010/11/3] ī�� ����Ȯ��
	// jsshin 2015.10.12			memset(MS_DATA_ALL,0,sizeof(MS_DATA_ALL));
#ifdef T1
				memset(gMS_DATA_T1,0,sizeof(gMS_DATA_T1));
#endif
#ifdef T2
				memset(gMS_DATA_T2,0,sizeof(gMS_DATA_T2));
#endif
#ifdef T3
				memset(gMS_DATA_T3,0,sizeof(gMS_DATA_T3));
#endif
				Byte_DATA_cnt_All = 0;
				Status_T1 = READ_ERROR_BLANK;
				Status_T2 = READ_ERROR_BLANK;
				Status_T3 = READ_ERROR_BLANK;
				Status_ALL = 0;

				Non_MS_DATA;

				DataBufferSize = 0;
				CmdResult = PRespons;
			}
			else CmdResult = ComandNotDefine;
			break;
		case 0x37 :		
			if(Check_Sensor_State(LOCK_SEN) == SET) IFM_ICC_PRESENT;

			ICC_buffer_init();
			
			if(CmdDatasize)CmdDatasize--;//201021 �߰�. �������� �������� PM ������ �߰��Ǿ� ������ -1 ����.
			
			if(Parameter == 0x30)
			{
				if(UseUSB == SET)
				{
					CmdDatasize++;
				}
				//if((Check_Sensor_State(REAR_SEN) == SET && Check_Sensor_State(FRONT_SEN) == RESET) || card_select >0)
#if 1	//pbbch 180517 real front sensor change
				if((Check_Sensor_State(FRONT_REAL_SEN) == SET)||(card_select >0))
#else					
				if((Check_Sensor_State(FRONT_SEN) == SET)||(card_select >0))
#endif					
				{
#if !defined(DONT_CARE_LOCK)
					if(LockUse == RESET)//Lock Use
					{
						if((Check_Sensor_State(LOCK_SEN) == SET) || (card_select >0))
						{
							Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToEMV);
							CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
							#if 1		//pbbch 190320 retry function add	//pbbch 190329
							if(CmdResult != PRespons)
							{
								Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToEMV);
								CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
							}
							#endif
							#ifdef Hanmega_USA
							/// 20150216 hyesun : IC Error �� ī�� Eject �߰�
							if(CmdResult != PRespons) E_CmdProc(200);
							#endif
						}
							else CmdResult = CardLockFail;
						}
						else
#endif
						{
							Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToEMV);
							CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
							#if 1		//pbbch 190320 retry function add		//pbbch 190329 update
							if(CmdResult != PRespons)
							{
								Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToEMV);
								CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
							}
							#endif
						}
					}
					else
					{
						CmdResult = NoCard;
					}
			}
			else if(Parameter == 0x31)
			{
				
				if(UseUSB == SET)
				{
					CmdDatasize++;
				}
				//if((Check_Sensor_State(REAR_SEN) == SET && Check_Sensor_State(FRONT_SEN) == RESET) || card_select >0)
#if 1	//pbbch 180517 real front sensor change
  				if((Check_Sensor_State(FRONT_REAL_SEN) == SET)||(card_select >0))
#else				
				if((Check_Sensor_State(FRONT_SEN) == SET)||(card_select >0))
#endif					
				{
	// jsshin 2016.02.26 : Lock ��� �ϵ��� ���� �Ǿ��־� AutoLock ó�� �Ѵ�. 
	// ���� ī�尡 ������ ���� �Ǿ����� Lock ���� ������� ó�� ���� ����.
#if !defined(DONT_CARE_LOCK)
					if(LockUse == RESET)//Lock Use
					{
							if(Check_Sensor_State(LOCK_SEN) == SET || card_select >0)
							//if(LockStat == SET || card_select >0)
							{
								/// always [2012/7/20] gemcore default ISO
								Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToISO);
								CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
								#if 1		//pbbch 190320 retry function add	//pbbch 190329 update
								if(CmdResult != PRespons)
								{
									Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToISO);
									CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
								}
								#endif
								//if(CmdResult == PRespons)
								//ATR_Result = IC_Card_AnalyzeATRandSetParameter(card_select,DataBuffer,DataBufferSize);
								if(CmdResult == PRespons) {
									if(PRespons == IC_Card_AnalyzeATRandSetParameter(card_select,DataBuffer,DataBufferSize))
									{
										Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToEMV);
									}
								}
							}
							else CmdResult = CardLockFail;
						}
						else
#endif
						{
							/// always [2012/7/20] gemcore default ISO
							Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToISO);
							CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
							#if 1		//pbbch 190320 retry function add	//pbbch 190329 update
							if(CmdResult != PRespons)
							{
								Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToISO);
								CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
							}
							#endif
							//if(CmdResult == PRespons)
							//ATR_Result = IC_Card_AnalyzeATRandSetParameter(card_select,DataBuffer,DataBufferSize);
							if(CmdResult == PRespons) {
								if(PRespons == IC_Card_AnalyzeATRandSetParameter(card_select,DataBuffer,DataBufferSize))
								{
									Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToEMV);
								}
							}
						}
					}
					else
					{
						CmdResult = NoCard;
					}
			}
			else if(Parameter == 0x32)
			{
				if(UseUSB == SET)
				{
					CmdDatasize++;
				}
	        
				//if((Check_Sensor_State(REAR_SEN) == SET && Check_Sensor_State(FRONT_SEN) == RESET) || card_select >0)
#if 1	//pbbch 180517 real front sensor change
				if((Check_Sensor_State(FRONT_REAL_SEN) == SET)||(card_select >0))
#else				
				if((Check_Sensor_State(FRONT_SEN) == SET)||(card_select >0))
#endif					
				{
#if !defined(DONT_CARE_LOCK)
					if(LockUse == RESET)//Lock Use
					{
						if(Check_Sensor_State(LOCK_SEN) == SET || card_select >0)
						{
		                			if(IFM_STATE == 0)
					                {
					                  NO_ICC_Reset;
					                }
						#ifdef Hanmega_USA
//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
							if((DataBuffer[4]==0x00)&&(DataBuffer[1]==0xB2)||(DataBuffer[1]==0x84)||(DataBuffer[1]==0xCA))			
							{
								Cmd_data_len--;
								CmdDatasize--;
							}
			//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
							else if(DataBuffer[4]==0xFF && CmdDatasize==262) // Cmd_data_len = CmdDatasize = CMD+IC����DATA 							
							{
								//Cmd_data_len--;
								CmdDatasize--;// [Le] 1����Ʈ ����
							}
						#endif
		                			CmdResult = IC_CmdProc(Direct_Control,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
							if(CmdResult == 0)CmdResult=IFMCommunicationError;

						#ifdef Hanmega_USA
			//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
							if(DataBuffer[4]==0xFF && CmdDatasize==262) 							
							{
								if(DataBuffer[0]==0x61)
								{
									i=0;
									//memset(CmdData,0,sizeof(CmdData));
									//memcpy(&CmdData[i],"\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\xc0\x00\x00",13);	i+=13;
									memcpy(&CmdData[i],"\x00\xc0\x00\x00",4);	i+=4;
									CmdData[i]=DataBuffer[DataBufferSize-1];	i+=1;
									i+=1;// CMD 1����Ʈ ���̸� �߰�
									CmdDatasize=i;
									DataBufferSize=0;
									CmdResult = IC_CmdProc(Direct_Control,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
									if(CmdResult == 0)CmdResult=IFMCommunicationError;
								}
								else if(DataBuffer[0]==0x6c)
								{
									i=0;
									//memset(CmdData,0,sizeof(CmdData));
									//memcpy(&CmdData[i],"\x05\x00\x00\x00\x00\x00\x00\x00\x00",9);	i+=9;
									//memcpy(&CmdData[i],&CmdData[0],4);								i+=4;
									i+=4;
									CmdData[i]=DataBuffer[DataBufferSize-1];						i+=1;
									i+=1;// CMD 1����Ʈ ���̸� �߰�
									CmdDatasize=i;
									DataBufferSize=0;
									CmdResult = IC_CmdProc(Direct_Control,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
									if(CmdResult == 0)CmdResult=IFMCommunicationError;
								}
							}
						#endif
		              		}
		              		else CmdResult = CardLockFail;//CmdResult = NoCard;
		            		}
		            		else
#endif
					{
						if(IFM_STATE == 0)
						{
							NO_ICC_Reset;
						}
						#ifdef Hanmega_USA
			//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
						if((DataBuffer[4]==0x00)&&(DataBuffer[1]==0xB2)||(DataBuffer[1]==0x84)||(DataBuffer[1]==0xCA))						
						{
							Cmd_data_len--;
							CmdDatasize--;
						}
			//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
						else if(DataBuffer[4]==0xFF && CmdDatasize==262) // Cmd_data_len = CmdDatasize = CMD+IC����DATA 							
						{
							//Cmd_data_len--;
							CmdDatasize--;// [Le] 1����Ʈ ����
						}
						#endif
						CmdResult = IC_CmdProc(Direct_Control,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
						if(CmdResult == 0)CmdResult=IFMCommunicationError;

						#ifdef Hanmega_USA
			//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
						if(DataBuffer[4]==0xFF && CmdDatasize==262) 							
						{
							if(DataBuffer[0]==0x61)
							{
								i=0;
								//memset(CmdData,0,sizeof(CmdData));
								//memcpy(&CmdData[i],"\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\xc0\x00\x00",13);	i+=13;
								memcpy(&CmdData[i],"\x00\xc0\x00\x00",4);	i+=4;
								CmdData[i]=DataBuffer[DataBufferSize-1];	i+=1;
								i+=1;// CMD 1����Ʈ ���̸� �߰�
								CmdDatasize=i;
								DataBufferSize=0;
								CmdResult = IC_CmdProc(Direct_Control,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
								if(CmdResult == 0)CmdResult=IFMCommunicationError;
							}
							else if(DataBuffer[0]==0x6c)
							{
								i=0;
								//memset(CmdData,0,sizeof(CmdData));
								//memcpy(&CmdData[i],"\x05\x00\x00\x00\x00\x00\x00\x00\x00",9);	i+=9;
								//memcpy(&CmdData[i],&CmdData[0],4);								i+=4;
								i+=4;
								CmdData[i]=DataBuffer[DataBufferSize-1];						i+=1;
								i+=1;// CMD 1����Ʈ ���̸� �߰�
								CmdDatasize=i;
								DataBufferSize=0;
								CmdResult = IC_CmdProc(Direct_Control,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
								if(CmdResult == 0)CmdResult=IFMCommunicationError;
							}
						}
						#endif
					}
				}
				else
				{
					CmdResult = NoCard;//CmdResult = CardLockFail;//NoCard;
				}
			}
			else if(Parameter == 0x33)
			{
				if(UseUSB == SET)
				{
					CmdDatasize++;
				}
				

				if(IFM_STATE == 0)
				{
					//IFM_ON;
					#if 0		//pbbch 180320 eject ���������� CR30�� Card detect signal�� high�� �־� card�� ������ �˷� detective�� ������ �ؾ� �Ѵ�.
					IFM_ICC_PRESENT;
					#endif
					NO_ICC_Reset;
					//USART_Cmd(UART4, ENABLE);
					//delay_ms(50);
				}
				CmdResult = IC_CmdProc(Power_OFF,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);

				/// 20160106 hyesun : ���þ� SAGA ���� �����
				// jsshin : need sam mux select clear!
				if(CmdResult == PRespons)
				{
					if(card_select == 0)		NO_ICC_Reset;
					else if(card_select == 1)	Non_SAM1;
					else if(card_select == 2)	Non_SAM2;
					else if(card_select == 3)	Non_SAM3;
					#ifndef Hanmega_USA
					else if(card_select == 4)	Non_SAM4;
					#endif
				}
			}
			else CmdResult = ComandNotDefine;
			break;
		#ifdef USE_RF
		case 0x38 :
			if(Parameter == 0x30)CmdResult = F_CmdProc(CmdData,CmdDatasize,DataBuffer,&DataBufferSize,1);	//2sec timeout
 			else CmdResult = ComandNotDefine;
			break;
		#endif
		case 0x41 :
			if(Parameter == 0x30)
			{
				if(L_CmdProc(CmdData[0]) == ERROR)CmdResult = WrongCommandData;
				else CmdResult = PRespons;
			}
			else if(Parameter == 0x31)
			{
				Resp_normal_proc();
				CmdResult = Baudrate_Setting(CmdData[0]);
				return;
			}
			else if(Parameter == 0x32)
			{
				if(CmdData[0] == 0x30){LockUse = RESET;CmdResult = PRespons;}//Lock Use
				else if(CmdData[0] == 0x31){LockUse = SET;CmdResult = PRespons;}//Lock Unuse
				else CmdResult = WrongCommandData;
				DataBufferSize = 0;
				LockUse_Setting(LockUse);
			}
			else if(Parameter == 0x33)
			{
				#if 0
				if((CmdData[0]>='0')&&(CmdData[0]<='4'))//LED_Default, LED_Off, LED_On, LED_Toggle
				#else
				if((CmdData[0]>='0')&&(CmdData[0]<='3'))//LED_Default, LED_Off, LED_On, LED_Toggle
				#endif
				{
					LED_Control = CmdData[0];
					if(CmdData[0]=='3')
					{
						if(CmdData[1] == '1')LEDTime_Control=Toggle_slow; 
						else if(CmdData[1] == '2')LEDTime_Control=Toggle_1sec; 
						else if(CmdData[1] == '3')LEDTime_Control=Toggle_fast; 
						else LEDTime_Control=Toggle_1sec; 
					}
					#if 0
					else if(CmdData[0] == '4')
					{
						LED_Control = '3';
						if(CmdData[1])
						{
							LEDTime_Control=CmdData[1]; 
						}
						else LEDTime_Control=Toggle_1sec;
					}
					#endif
					CmdResult = PRespons;
				}
				else CmdResult = WrongCommandData;
			}
			else if(Parameter == 0x34)CmdResult = l_CmdProc();//Locking 
			#if 0
			else if(Parameter == 0x35)//Auto Locking �ð� ����.
			{
				if(CmdData[0] > 0x30 && CmdData[0] < 0x36)
				{
					TempFlashData.AutoLocktime = CmdData[0]-0x30;//(CmdData[0]-0x30)*60;
					SolCnt = TempFlashData.AutoLocktime*60;
					FlashDataWrite();
					CmdResult = PRespons;
				}
				else CmdResult = WrongCommandData;
			}
			#endif
			else CmdResult = ComandNotDefine;
			break;
		case 0x42 :
			if(Parameter == 0x30)
			{
				if(UseUSB == SET)
				{
					CmdDatasize -= 1;
				}
				else
				{
					//CmdDatasize -= 2;
					CmdDatasize -= 3;
				}
				CmdResult = K_CmdProc(CmdData,CmdDatasize);
			}
			else CmdResult = ComandNotDefine;
			break;
		case 0x43 :
			if(Parameter == 0x30)
			{
				DataBufferSize = 5;
				memcpy(DataBuffer,version,DataBufferSize);
				DataBuffer[DataBufferSize++] = TempFlashData.ModuleType;
				for(TempCnt = 0;TempCnt < 7;TempCnt++)
				{
					DataBuffer[DataBufferSize++] = TempFlashData.SerialNumber[TempCnt];
				}
				CmdResult = PRespons;
			}
			else CmdResult = ComandNotDefine;
			break;
		case 0x44 :			
			if(Parameter == 0x30)CmdResult = Q_CmdProc('D');
			else if(Parameter == 0x31)CmdResult = Q_CmdProc('I');
			else CmdResult = ComandNotDefine;
			break;

				
		/// always [2010/10/1] mode change to download
		case 'Z' ://0x5A
			if(Parameter == 0x30)
			{
				USART_Cmd(UART4, DISABLE);
				delay_ms(50);
				FW_Download_flag_ON();
				DataBufferSize = 0;
				CmdResult = PRespons;
			}
			else CmdResult = ComandNotDefine;
			break;
		#if 0
		case 'T' ://��� �׽�Ʈ�뵵.//0x54
			if(Parameter == 0x30)
			{
				if(CmdData[0]=='a')
				{
					ushort ADC_V = 0;
					uchar ADC_V_1, ADC_V_10, ADC_V_100 = 0;
					if(CmdData[1]=='1')//Lock Sensor
					{
						Check_Sensor_State(LOCK_SEN);
						#if 1	//pbbch 171212 across warning......but result value is float variable. so need to review again
						ADC_V = (ushort)((ADC_DATA[LOCK_SEN]*0.0008)*100);
						#else
						ADC_V = (ADC_DATA[LOCK_SEN]*0.0008)*100;
						#endif
						//ADC_V = (u16)(((u32)ADC_DATA[LOCK_SEN]*100)*0.0008);
					}
			//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
					else if(CmdData[1]=='2')//Rear Sensor
					{
						Check_Sensor_State(REAR_SEN);
						#if 1	//pbbch 171212 across warning......but result value is float variable. so need to review again
						ADC_V = (ushort)((ADC_DATA[REAR_SEN]*0.0008)*100);
						#else
						ADC_V = (ADC_DATA[REAR_SEN]*0.0008)*100;
						#endif
					}
					else if(CmdData[1]=='3')//Inner Sensor
					{
						Check_Sensor_State(FRONT_SEN);
						#if 1	//pbbch 171212 across warning......but result value is float variable. so need to review again
						ADC_V = (ushort)((ADC_DATA[FRONT_SEN]*0.0008)*100);
						#else
						ADC_V = (ADC_DATA[FRONT_SEN]*0.0008)*100;
						#endif
					}
					else if(CmdData[1]=='4')//real front Sensor
					{
						Check_Sensor_State(FRONT_REAL_SEN);
						#if 1	//pbbch 171212 across warning......but result value is float variable. so need to review again
						ADC_V = (ushort)((ADC_DATA[FRONT_REAL_SEN]*0.0008)*100);
						#else
						ADC_V = (ADC_DATA[FRONT_SEN]*0.0008)*100;
						#endif
					}				
					ADC_V_100=ADC_V/100;
					ADC_V_10=(ADC_V%100)/10;
					ADC_V_1=(ADC_V%100)%10;
		      
					DataBufferSize=0;
					DataBuffer[DataBufferSize++] = ADC_V_100 + 0x30;
					DataBuffer[DataBufferSize++] = '.';
					DataBuffer[DataBufferSize++] = ADC_V_10 + 0x30;
					DataBuffer[DataBufferSize++] = ADC_V_1 + 0x30;
					
					CmdResult = PRespons;
				}
				else CmdResult = T_CmdProc(CmdData,CmdData[0],DataBuffer,&DataBufferSize);	
			}
			else CmdResult = ComandNotDefine;
			break;	
		#endif
		default :
			CmdResult = ComandNotDefine;//Resp_N_proc(1);//Command Not Define
	    break;
			//return;
	} // end switch
	
	/// always [2011/5/4] uart ��� �� ����
	if(UseUSB == RESET)
	{
		if(CmdResult == PRespons)
		{
			Resp_DATA_proc(DataBuffer,DataBufferSize);			

			if(Command == 'Z')
			{
				NVIC_SystemReset();
			}		
		}
		else
		{
			Resp_N_proc(CmdResult);
		}
	}
	#ifdef UseUsb
	/// always [2011/5/4] usb ��Ž� ����
	else
	{
		if(CmdResult == PRespons)
		{
			TxUSBData('P',0,DataBuffer,DataBufferSize);
			if(Command == 'Z')
			{
				delay_ms(100);				
				NVIC_SystemReset();
			}
		}
		else
		{
			TxUSBData('N',CmdResult,DataBuffer,DataBufferSize);
		}
	}
	#endif
}

void Cmd_proc(uchar Command,uchar *CmdData,ushort CmdDatasize, uchar UseUSB)
{
	//uchar DataBuffer[1024];
	uchar *DataBuffer;
	ushort DataBufferSize = 0;
	uchar CmdResult = ComandNotDefine;
	uint TempCnt;
	uchar Escape_Data[10];
	uchar Escape_Data_leng=0;
#if 0		//pbbch 171212 warning across	
	uchar ATR_Result =0;
#endif
#if ((!defined(DONT_CARE_LOCK)) ||defined(Hanmega_USA))		//pbbch 171212 warning across and move form switch case 'I'
	uchar i=0;
#endif	

	DataBuffer = Response_P_format.DATA;
	//memset(DataBuffer, 0, sizeof(uchar)*1024);

	switch(Command)
	{
		/// always [2010/10/1] Status Request
		case 'S' :
				DataBufferSize = 0;
				CmdResult = PRespons;
				//Resp_normal_proc();
			break;
		/// always [2010/10/1] Read F/W Version of unit
		case 'V' :
				DataBufferSize = 5;
				memcpy(DataBuffer,version,DataBufferSize);
				DataBuffer[DataBufferSize++] = TempFlashData.ModuleType;
				for(TempCnt = 0;TempCnt < 7;TempCnt++)
				{
					DataBuffer[DataBufferSize++] = TempFlashData.SerialNumber[TempCnt];
				}
				CmdResult = PRespons;
			break;
				
		/// always [2010/10/1] PTSS(PPSS) Application
		//case 'P' :
		//	break;
		/// always [2010/12/24] All MS data �ʱ�ȭ
		case 'A' :
			if((TempFlashData.ModuleType == USEMS)||(TempFlashData.ModuleType == USEALL))
			{
				/// always [2010/11/3] ī�� ����Ȯ��
	// jsshin 2015.10.12			memset(MS_DATA_ALL,0,sizeof(MS_DATA_ALL));
#ifdef T1
				memset(gMS_DATA_T1,0,sizeof(gMS_DATA_T1));
#endif
#ifdef T2
				memset(gMS_DATA_T2,0,sizeof(gMS_DATA_T2));
#endif
#ifdef T3
				memset(gMS_DATA_T3,0,sizeof(gMS_DATA_T3));
#endif
				Byte_DATA_cnt_All = 0;
				Status_T1 = READ_ERROR_BLANK;
				Status_T2 = READ_ERROR_BLANK;
				Status_T3 = READ_ERROR_BLANK;
				Status_ALL = 0;

				Non_MS_DATA;

				DataBufferSize = 0;
				CmdResult = PRespons;
			}
			else
			{
				CmdResult = ComandNotDefine;
			}
			break;
		/// always [2010/10/1] Magnetic data read command
		case 'M' :
			#ifdef Hanmega_USA
			if(CmdDatasize>1)
			{
				CmdResult = NoCard;
				break;
			}
			#endif
			
		//if(CmdDatasize==1)// Dik ������ �Ǳ� ���̸� ����.
	    //{
			if((TempFlashData.ModuleType == USEMS)||(TempFlashData.ModuleType == USEALL))
			{
		        CmdResult = M_CmdProc(DataBuffer,&DataBufferSize);
				#if 1 //defined(KTC_MODEL) // �ѹ� �о ó���� �����ʹ� �����.
				if(PRespons == CmdResult)
				{
					Non_MS_DATA;
					#ifdef T1
					memset(gMS_DATA_T1,0,sizeof(gMS_DATA_T1));
					#endif
					#ifdef T2
					memset(gMS_DATA_T2,0,sizeof(gMS_DATA_T2));
					#endif
					#ifdef T3
					memset(gMS_DATA_T3,0,sizeof(gMS_DATA_T3));
					#endif
					Byte_DATA_cnt_All = 0;
					Status_T1 = READ_ERROR_BLANK;
					Status_T2 = READ_ERROR_BLANK;
					Status_T3 = READ_ERROR_BLANK;
					Status_ALL = 0;
				}
				#endif
				#if 0		//pbbch 180207 error case ms decoder chip reset 
				else
				{
					delay_ms(10);
					GPIO_ResetBits(GPIOD,GPIO_Pin_10);
					delay_ms(10);		//delay need minimum 100us after power on 
					GPIO_SetBits(GPIOD,GPIO_Pin_10);
				}
				#endif
			}
			else
			{
				CmdResult = ComandNotDefine;
			}
	    //}
			break;
		#if 0	//pbbch 180427 samsungpay command �߰�...ms 2track read�� ����� �����Ͽ� ���� �߰� command ����.
			case 'r' :
			break;
		#endif	
	/***************************************************************************************************************************///130425hs: Command �߰�~
		/// kimhyesun [2013/04/25] Magnetic data Single read command
	  // 2016.01.28 ���ڱ��� ����� ��� ��û���� �ٽ� ��� ��.
		case 'm' :
			if((TempFlashData.ModuleType == USEMS)||(TempFlashData.ModuleType == USEALL))
			{
				CmdResult = m_CmdProc(DataBuffer,&DataBufferSize,CmdData);
				#if 1 //defined(KTC_MODEL) // �ѹ� �о ó���� �����ʹ� �����.
				if(PRespons == CmdResult) 
				{
					if(CmdData[0] == 0x31) 
					{
						#ifdef T1
						Byte_DATA_cnt_T1 = 0;
						Status_T1 = READ_ERROR_BLANK;
						memset(gMS_DATA_T1,0,sizeof(gMS_DATA_T1));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
						#endif
					}
					if(CmdData[0] == 0x32) 
					{
						#ifdef T2
						Byte_DATA_cnt_T2 = 0;
						Status_T2 = READ_ERROR_BLANK;
						memset(gMS_DATA_T2,0,sizeof(gMS_DATA_T2));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
						#endif
					}
					if(CmdData[0] == 0x33) 
					{
						#ifdef T3
						Byte_DATA_cnt_T3 = 0;
						Status_T3 = READ_ERROR_BLANK;
						memset(gMS_DATA_T3,0,sizeof(gMS_DATA_T3));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
						#endif
					}
				}
				#endif
		    }
			else
			{
				CmdResult = ComandNotDefine;
			}
			break;
	/***************************************************************************************************************************///130425hs: ~Command �߰�

		 /// always [2010/10/1] 3DES ����
		case 'K' :
			if((TempFlashData.ModuleType == USEMS)||(TempFlashData.ModuleType == USEALL))
			{
				if(UseUSB == SET)
				{
					CmdDatasize -= 1;
				}
				else
				{
					CmdDatasize -= 2;
				}
				CmdResult = K_CmdProc(CmdData,CmdDatasize);
			}
			else
			{
				CmdResult = ComandNotDefine;
			}
			break;
	    /// always [2010/10/1] ICC Rest
		case 'C' :		//ISO Power on 
			//IFM_ON;
			#if 1		//pbbch 180320 eject ���������� CR30�� Card detect signal�� high�� �־� card�� ������ �˷� detective�� ������ �ؾ� �Ѵ�.
			if(Check_Sensor_State(LOCK_SEN) == SET) IFM_ICC_PRESENT;
			#else
			IFM_ICC_PRESENT;
			#endif
			//NO_ICC_Reset;/// ���þ� SAGA ���� ����� IC�� SAM ���� ��� �����ϵ��� ICĨ ���� ���� ������
	//		//SAM_initial();
	//		if(g_pcb_version == PCB_GEM_POS)
	//		{
	//			IFM_OFF;
	//			delay_ms(50); // 50ms ����.
	//			IFM_ON;	
	//			delay_ms(300); // 300ms ����.
	//		}
	//		else
	//		{
	//			CR30_RESET_ON;//CR30 reset active nRESET
	//			delay_ms(1);
	//			CR30_RESET_OFF;//CR30 reset
	//			delay_ms(50);
	//		}
			ICC_buffer_init();
			
			if((TempFlashData.ModuleType == USEIC)||(TempFlashData.ModuleType == USEALL))
			{
				
				if(UseUSB == SET)
				{
					CmdDatasize++;
				}
				//if((Check_Sensor_State(REAR_SEN) == SET && Check_Sensor_State(FRONT_SEN) == RESET) || card_select >0)
#if 1	//pbbch 180517 real front sensor change
  				if((Check_Sensor_State(FRONT_REAL_SEN) == SET)||(card_select >0))
#else				
				if((Check_Sensor_State(FRONT_SEN) == SET)||(card_select >0))
#endif					
				{
	// jsshin 2016.02.26 : Lock ��� �ϵ��� ���� �Ǿ��־� AutoLock ó�� �Ѵ�. 
	// ���� ī�尡 ������ ���� �Ǿ����� Lock ���� ������� ó�� ���� ����.
#if !defined(DONT_CARE_LOCK)
					if(LockUse == RESET)//Lock Use
					{
							if(Check_Sensor_State(LOCK_SEN) == SET || card_select >0)
							//if(LockStat == SET || card_select >0)
							{
								/// always [2012/7/20] gemcore default ISO
								Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToISO);
								CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
								#if 1		//pbbch 190320 retry function add	//pbbch 190329 update
								if(CmdResult != PRespons)
								{
									Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToISO);
									CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
								}
								#endif
								//if(CmdResult == PRespons)
								//ATR_Result = IC_Card_AnalyzeATRandSetParameter(card_select,DataBuffer,DataBufferSize);
								if(CmdResult == PRespons) {
									if(PRespons == IC_Card_AnalyzeATRandSetParameter(card_select,DataBuffer,DataBufferSize))
									{
										Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToEMV);
									}
								}
							}
							else CmdResult = CardLockFail;
						}
						else
#endif
						{
							/// always [2012/7/20] gemcore default ISO
							Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToISO);
							CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
							#if 1		//pbbch 190320 retry function add	//pbbch 190329 update
							if(CmdResult != PRespons)
							{
								Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToISO);
								CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
							}
							#endif
							//if(CmdResult == PRespons)
							//ATR_Result = IC_Card_AnalyzeATRandSetParameter(card_select,DataBuffer,DataBufferSize);
							if(CmdResult == PRespons) {
								if(PRespons == IC_Card_AnalyzeATRandSetParameter(card_select,DataBuffer,DataBufferSize))
								{
									Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToEMV);
								}
							}
						}
					}
					else
					{
						CmdResult = NoCard;
					}
			}
			else
			{
				CmdResult = ComandNotDefine;
			}
			break;
 	
		/// always [2010/10/1] ICC Rest
		case 'R' :		//EMV Power on
			//IFM_ON;
			#if 1		//pbbch 180320 eject ���������� CR30�� Card detect signal�� high�� �־� card�� ������ �˷� detective�� ������ �ؾ� �Ѵ�.
			if(Check_Sensor_State(LOCK_SEN) == SET) IFM_ICC_PRESENT;
			#else
	    		IFM_ICC_PRESENT;
			#endif
			
			//NO_ICC_Reset; /// ���þ� SAGA ���� ����� IC�� SAM ���� ��� �����ϵ��� ICĨ ���� ���� ������
	//		//SAM_initial();
	//		if(g_pcb_version == PCB_GEM_POS)
	//		{
	//			IFM_OFF;
	//			delay_ms(50); // 50ms ����.
	//			IFM_ON;	
	//			delay_ms(300); // 300ms ����.
	//		}
	//		else
	//		{
	//			CR30_RESET_ON;//CR30 reset active nRESET
	//			delay_ms(1);
	//			CR30_RESET_OFF;//CR30 reset
	//			delay_ms(50);
	//		}
			ICC_buffer_init();	
			
			if((TempFlashData.ModuleType == USEIC)||(TempFlashData.ModuleType == USEALL))
			{
				if(UseUSB == SET)
				{
					CmdDatasize++;
				}
				//if((Check_Sensor_State(REAR_SEN) == SET && Check_Sensor_State(FRONT_SEN) == RESET) || card_select >0)
#if 1	//pbbch 180517 real front sensor change
				if((Check_Sensor_State(FRONT_REAL_SEN) == SET)||(card_select >0))
#else					
				if((Check_Sensor_State(FRONT_SEN) == SET)||(card_select >0))
#endif					
				{
#if !defined(DONT_CARE_LOCK)
					if(LockUse == RESET)//Lock Use
					{
						if((Check_Sensor_State(LOCK_SEN) == SET) || (card_select >0))
						{
							Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToEMV);
							CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
							#if 1		//pbbch 190320 retry function add	//pbbch 190329
							if(CmdResult != PRespons)
							{
								Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToEMV);
								CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
							}
							#endif
							#ifdef Hanmega_USA
							/// 20150216 hyesun : IC Error �� ī�� Eject �߰�
							if(CmdResult != PRespons) E_CmdProc(200);
							#endif
						}
							else CmdResult = CardLockFail;
						}
						else
#endif
						{
						#if 0
							//testcnt++;
							//if(testcnt >= 11&&testcnt < 21)
							{
								//CmdResult = IFMCommunicationError;
								CR30_RESET_ON;//CR30 reset active nRESET
								delay_ms(1);
								CR30_RESET_OFF;//CR30 reset active nRESET
								delay_ms(50);
								Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToEMV);
								CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
								//delay_ms(11000);
								//CR30_RESET_OFF;//CR30 reset active nRESET
							}
						#else
						//	else
						
							{
								Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToEMV);
								CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
								#if 1		//pbbch 190320 retry function add		//pbbch 190329 update
								if(CmdResult != PRespons)
								{
									Escape_IC(Escape_Data,Escape_Data_leng,ChangeMode,ChangeToEMV);
									CmdResult = IC_CmdProc(Power_ON,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,Power_5V);
								}
								#endif
							}
						#endif
						}
					}
					else
					{
						CmdResult = NoCard;
					}
			}
			else
			{
				CmdResult = ComandNotDefine;
			}
			break;

	    /// always [2010/10/1] ICC Dirext Control
		case 'I' :
			#if 0		//pbbch 171212 warning across and move to function front
			uchar i=0;
			#endif
		//if(!checkDikInjected())// Dik ������ �Ǳ� ���̸� ����.
		//{
			if((TempFlashData.ModuleType == USEIC)||(TempFlashData.ModuleType == USEALL))
			{
				if(UseUSB == SET)
				{
					CmdDatasize++;
				}
	        
				//if((Check_Sensor_State(REAR_SEN) == SET && Check_Sensor_State(FRONT_SEN) == RESET) || card_select >0)
#if 1	//pbbch 180517 real front sensor change
				if((Check_Sensor_State(FRONT_REAL_SEN) == SET)||(card_select >0))
#else				
				if((Check_Sensor_State(FRONT_SEN) == SET)||(card_select >0))
#endif					
				{
#if !defined(DONT_CARE_LOCK)
					if(LockUse == RESET)//Lock Use
					{
						if(Check_Sensor_State(LOCK_SEN) == SET || card_select >0)
						{
		                			if(IFM_STATE == 0)
					                {
					                  //IFM_ON;
#if 0		//pbbch 180320 eject ���������� CR30�� Card detect signal�� high�� �־� card�� ������ �˷� detective�� ������ �ؾ� �Ѵ�.
								IFM_ICC_PRESENT;
#endif
					                  NO_ICC_Reset;
					                  //USART_Cmd(UART4, ENABLE);
					                  //delay_ms(50);
					                }
						#ifdef Hanmega_USA
#if 1			//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
							if((DataBuffer[4]==0x00)&&(DataBuffer[1]==0xB2)||(DataBuffer[1]==0x84)||(DataBuffer[1]==0xCA))
#else
							if((cmd_format.DATA[4]==0x00)&&(cmd_format.DATA[1]==0xB2)||(cmd_format.DATA[1]==0x84)||(cmd_format.DATA[1]==0xCA))
#endif								
							{
								Cmd_data_len--;
								CmdDatasize--;
							}
#if 1			//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
							else if(DataBuffer[4]==0xFF && CmdDatasize==262) // Cmd_data_len = CmdDatasize = CMD+IC����DATA 
#else
							else if(cmd_format.DATA[4]==0xFF && CmdDatasize==262) // Cmd_data_len = CmdDatasize = CMD+IC����DATA 
#endif								
							{
								//Cmd_data_len--;
								CmdDatasize--;// [Le] 1����Ʈ ����
							}
						#endif
		                			CmdResult = IC_CmdProc(Direct_Control,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
							if(CmdResult == 0)CmdResult=IFMCommunicationError;

						#ifdef Hanmega_USA
#if 1			//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
							if(DataBuffer[4]==0xFF && CmdDatasize==262) 
#else
							if(cmd_format.DATA[4]==0xFF && Cmd_data_len==262) 
#endif								
							{
								if(DataBuffer[0]==0x61)
								{
									i=0;
									//memset(CmdData,0,sizeof(CmdData));
									//memcpy(&CmdData[i],"\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\xc0\x00\x00",13);	i+=13;
									memcpy(&CmdData[i],"\x00\xc0\x00\x00",4);	i+=4;
									CmdData[i]=DataBuffer[DataBufferSize-1];	i+=1;
									i+=1;// CMD 1����Ʈ ���̸� �߰�
									CmdDatasize=i;
									DataBufferSize=0;
									CmdResult = IC_CmdProc(Direct_Control,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
									if(CmdResult == 0)CmdResult=IFMCommunicationError;
								}
								else if(DataBuffer[0]==0x6c)
								{
									i=0;
									//memset(CmdData,0,sizeof(CmdData));
									//memcpy(&CmdData[i],"\x05\x00\x00\x00\x00\x00\x00\x00\x00",9);	i+=9;
									//memcpy(&CmdData[i],&CmdData[0],4);								i+=4;
									i+=4;
									CmdData[i]=DataBuffer[DataBufferSize-1];						i+=1;
									i+=1;// CMD 1����Ʈ ���̸� �߰�
									CmdDatasize=i;
									DataBufferSize=0;
									CmdResult = IC_CmdProc(Direct_Control,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
									if(CmdResult == 0)CmdResult=IFMCommunicationError;
								}
							}
						#endif
		              		}
		              		else CmdResult = CardLockFail;//CmdResult = NoCard;
		            		}
		            		else
#endif
					{
						if(IFM_STATE == 0)
						{
							//IFM_ON;
							#if 0		//pbbch 180320 eject ���������� CR30�� Card detect signal�� high�� �־� card�� ������ �˷� detective�� ������ �ؾ� �Ѵ�.
							IFM_ICC_PRESENT;
							#endif
							NO_ICC_Reset;
							//USART_Cmd(UART4, ENABLE);
							//delay_ms(50);
						}
						#ifdef Hanmega_USA
#if 1			//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
						if((DataBuffer[4]==0x00)&&(DataBuffer[1]==0xB2)||(DataBuffer[1]==0x84)||(DataBuffer[1]==0xCA))
#else
						if((cmd_format.DATA[4]==0x00)&&(cmd_format.DATA[1]==0xB2)||(cmd_format.DATA[1]==0x84)||(cmd_format.DATA[1]==0xCA))
#endif							
						{
							Cmd_data_len--;
							CmdDatasize--;
						}
#if 1			//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
						else if(DataBuffer[4]==0xFF && CmdDatasize==262) // Cmd_data_len = CmdDatasize = CMD+IC����DATA 
#else
						else if(cmd_format.DATA[4]==0xFF && CmdDatasize==262) // Cmd_data_len = CmdDatasize = CMD+IC����DATA 
#endif							
						{
							//Cmd_data_len--;
							CmdDatasize--;// [Le] 1����Ʈ ����
						}
						#endif
						CmdResult = IC_CmdProc(Direct_Control,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
						if(CmdResult == 0)CmdResult=IFMCommunicationError;

						#ifdef Hanmega_USA
#if 1			//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
						if(DataBuffer[4]==0xFF && CmdDatasize==262) 
#else
						if(cmd_format.DATA[4]==0xFF && Cmd_data_len==262) 
#endif							
						{
							if(DataBuffer[0]==0x61)
							{
								i=0;
								//memset(CmdData,0,sizeof(CmdData));
								//memcpy(&CmdData[i],"\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\xc0\x00\x00",13);	i+=13;
								memcpy(&CmdData[i],"\x00\xc0\x00\x00",4);	i+=4;
								CmdData[i]=DataBuffer[DataBufferSize-1];	i+=1;
								i+=1;// CMD 1����Ʈ ���̸� �߰�
								CmdDatasize=i;
								DataBufferSize=0;
								CmdResult = IC_CmdProc(Direct_Control,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
								if(CmdResult == 0)CmdResult=IFMCommunicationError;
							}
							else if(DataBuffer[0]==0x6c)
							{
								i=0;
								//memset(CmdData,0,sizeof(CmdData));
								//memcpy(&CmdData[i],"\x05\x00\x00\x00\x00\x00\x00\x00\x00",9);	i+=9;
								//memcpy(&CmdData[i],&CmdData[0],4);								i+=4;
								i+=4;
								CmdData[i]=DataBuffer[DataBufferSize-1];						i+=1;
								i+=1;// CMD 1����Ʈ ���̸� �߰�
								CmdDatasize=i;
								DataBufferSize=0;
								CmdResult = IC_CmdProc(Direct_Control,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
								if(CmdResult == 0)CmdResult=IFMCommunicationError;
							}
						}
						#endif
					}
				}
				else
				{
					CmdResult = NoCard;//CmdResult = CardLockFail;//NoCard;
				}
			}
			else
			{
			CmdResult = ComandNotDefine;
			}
		//}
			break;
	   
		/// always [2010/10/1] ICC Deactivation
		case 'D' :
			if((TempFlashData.ModuleType == USEIC)||(TempFlashData.ModuleType == USEALL))
			{
				if(UseUSB == SET)
				{
					CmdDatasize++;
				}
				
					//if((Check_Sensor_State(REAR_SEN) == SET && Check_Sensor_State(FRONT_SEN) == RESET) || card_select >0)
	////////// jsshin 2016.02.26 : power off�� �׻� �ǵ��� �Ѵ�.  
	////////////				if((Front_Sensor_Status == SET)||(card_select >0))
	////////////				{
#if 0 //!defined(DONT_CARE_LOCK)
						if(LockUse == RESET)//Lock Use
						{
							if((Check_Sensor_State(LOCK_SEN) == SET) || (card_select >0))
							{
								if(IFM_STATE == 0)
								{
									//IFM_ON;
									IFM_ICC_PRESENT;
									NO_ICC_Reset;
									//USART_Cmd(UART4, ENABLE);
									//delay_ms(50);
								}
								CmdResult = IC_CmdProc(Power_OFF,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
							}
							else CmdResult = CardLockFail;
						}
						else
#endif
						{
							if(IFM_STATE == 0)
							{
								//IFM_ON;
								#if 0		//pbbch 180320 eject ���������� CR30�� Card detect signal�� high�� �־� card�� ������ �˷� detective�� ������ �ؾ� �Ѵ�.
								IFM_ICC_PRESENT;
								#endif
								NO_ICC_Reset;
								//USART_Cmd(UART4, ENABLE);
								//delay_ms(50);
							}
							CmdResult = IC_CmdProc(Power_OFF,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
						}
						/// 20160106 hyesun : ���þ� SAGA ���� �����
						// jsshin : need sam mux select clear!
						if(CmdResult == PRespons)
						{
							if(card_select == 0)		NO_ICC_Reset;
							else if(card_select == 1)	Non_SAM1;
							else if(card_select == 2)	Non_SAM2;
							else if(card_select == 3)	Non_SAM3;
							#ifndef Hanmega_USA
							else if(card_select == 4)	Non_SAM4;
							#endif
						}
	////////////				}
	////////////				else
	////////////				{
	////////////					CmdResult = NoCard;
	////////////				}
			}
			else
			{
				CmdResult = ComandNotDefine;
			}
			
			break;
		/// always [2010/10/1] IC Card Select
		case 'L' :
			if((TempFlashData.ModuleType == USEIC)||(TempFlashData.ModuleType == USEALL))
			{
			
				if(L_CmdProc(CmdData[0]) == ERROR)
				{
					CmdResult = WrongCommandData;
				}
				else
				{
					CmdResult = PRespons;
				}
			}
			else
			{
				CmdResult = ComandNotDefine;
			}
			break;

	    /// always [2010/10/1] Card Eject 
	    // jsshin 2016.02.26 : sol off command.
		case 'E' :
#if 0		//pbbch 180425 deactivation pre-action
			//CmdResult =IC_CmdProc(Power_OFF,DataBuffer,&DataBufferSize,CmdData,CmdDatasize,0);
			CR30_Deactivation_direct_execution();
#endif
			CmdResult = E_CmdProc(200);
			DataBufferSize = 0;
			break;
		case 'l' :
			CmdResult = l_CmdProc();
			DataBufferSize = 0;
			break;
		#ifdef Hanmega_USA
		case 'P' :
				DataBufferSize = 0;
				CmdResult = PRespons;
				break;
		#endif
		case 'H' :
			//CmdResult = E_CmdProc(200);
			if(CmdData[0] == 0x30){LockUse = RESET;CmdResult = PRespons;}//Lock Use
			else if(CmdData[0] == 0x31){LockUse = SET;CmdResult = PRespons;}//Lock Unuse
			else CmdResult = WrongCommandData;
			DataBufferSize = 0;
			LockUse_Setting(LockUse);
			break;		
		case 'h' :
			if(CmdData[0] > 0x30 && CmdData[0] < 0x36)
			{
				TempFlashData.AutoLocktime = CmdData[0]-0x30;//(CmdData[0]-0x30)*60;
				SolCnt = TempFlashData.AutoLocktime*60;
				FlashDataWrite();
				CmdResult = PRespons;
			}
			else CmdResult = WrongCommandData;
			DataBufferSize = 0;
			break;
		#ifndef  Hanmega_USA
		case 'G' :
			if((CmdData[0]>='0')&&(CmdData[0]<='4'))//LED_Default, LED_Off, LED_On, LED_Toggle
			{
				LED_Control = CmdData[0];
				if(CmdData[0]=='3')
				{
					if(CmdData[1] == '1')LEDTime_Control=Toggle_slow; 
					else if(CmdData[1] == '2')LEDTime_Control=Toggle_1sec; 
					else if(CmdData[1] == '3')LEDTime_Control=Toggle_fast; 
					else LEDTime_Control=Toggle_1sec; 
				}
				else if(CmdData[0] == '4')
				{
					LED_Control = '3';
					if(CmdData[1])
					{
						LEDTime_Control=CmdData[1]; 
					}
					else LEDTime_Control=Toggle_1sec;
				}
				CmdResult = PRespons;
			}
			else CmdResult = WrongCommandData;
			DataBufferSize = 0;
			break;	
		#endif
		/// always [2010/10/1] Baud rate Change
		//#ifndef	UseUsb
		case 'B' :
			Resp_normal_proc();
			CmdResult = Baudrate_Setting(CmdData[0]);
			return;
		//#endif
		/// always [2011/6/7] Reset command
		case 'Q' :
			//TxUSBData('P',0,Dummy,0);
			//TxUSBData('P',0,DataBuffer,DataBufferSize);
			//delay_ms(100);
			#ifdef Hanmega_Russia
			TempFlashData.SN_hanmega = cmd_format.SN;
			FlashDataWrite();
			#endif
						
			CmdResult = Q_CmdProc(CmdData[0]);
			break;
				
		/// always [2010/10/1] mode change to download
		case 'Z' :
			USART_Cmd(UART4, DISABLE);
			delay_ms(50);
			FW_Download_flag_ON();
			DataBufferSize = 0;
			CmdResult = PRespons;
			
			break;
		case 'T' ://��� �׽�Ʈ�뵵.
			if(CmdData[0]=='a')
			{
				ushort ADC_V = 0;
				uchar ADC_V_1, ADC_V_10, ADC_V_100 = 0;
				if(CmdData[1]=='1')//Lock Sensor
				{
					Check_Sensor_State(LOCK_SEN);
					#if 1	//pbbch 171212 across warning......but result value is float variable. so need to review again
					ADC_V = (ushort)((ADC_DATA[LOCK_SEN]*0.0008)*100);
					#else
					ADC_V = (ADC_DATA[LOCK_SEN]*0.0008)*100;
					#endif
					//ADC_V = (u16)(((u32)ADC_DATA[LOCK_SEN]*100)*0.0008);
				}
#if 1			//pbbch 180528 usb ȣȯ�� ���Ͽ� data ������ cmd_format.DATA[1]����  CmdData[1]���� ����.
				else if(CmdData[1]=='2')//Rear Sensor
				{
					Check_Sensor_State(REAR_SEN);
					#if 1	//pbbch 171212 across warning......but result value is float variable. so need to review again
					ADC_V = (ushort)((ADC_DATA[REAR_SEN]*0.0008)*100);
					#else
					ADC_V = (ADC_DATA[REAR_SEN]*0.0008)*100;
					#endif
				}
				else if(CmdData[1]=='3')//Inner Sensor
				{
					Check_Sensor_State(FRONT_SEN);
					#if 1	//pbbch 171212 across warning......but result value is float variable. so need to review again
					ADC_V = (ushort)((ADC_DATA[FRONT_SEN]*0.0008)*100);
					#else
					ADC_V = (ADC_DATA[FRONT_SEN]*0.0008)*100;
					#endif
				}
				else if(CmdData[1]=='4')//real front Sensor
				{
					Check_Sensor_State(FRONT_REAL_SEN);
					#if 1	//pbbch 171212 across warning......but result value is float variable. so need to review again
					ADC_V = (ushort)((ADC_DATA[FRONT_REAL_SEN]*0.0008)*100);
					#else
					ADC_V = (ADC_DATA[FRONT_SEN]*0.0008)*100;
					#endif
				}
#else
				else if(cmd_format.DATA[1]=='2')//Rear Sensor
				{
					Check_Sensor_State(REAR_SEN);
					#if 1	//pbbch 171212 across warning......but result value is float variable. so need to review again
					ADC_V = (ushort)((ADC_DATA[REAR_SEN]*0.0008)*100);
					#else
					ADC_V = (ADC_DATA[REAR_SEN]*0.0008)*100;
					#endif
				}
				else if(cmd_format.DATA[1]=='3')//Inner Sensor
				{
					Check_Sensor_State(FRONT_SEN);
					#if 1	//pbbch 171212 across warning......but result value is float variable. so need to review again
					ADC_V = (ushort)((ADC_DATA[FRONT_SEN]*0.0008)*100);
					#else
					ADC_V = (ADC_DATA[FRONT_SEN]*0.0008)*100;
					#endif
				}
				else if(cmd_format.DATA[1]=='4')//real front Sensor
				{
					Check_Sensor_State(FRONT_REAL_SEN);
					#if 1	//pbbch 171212 across warning......but result value is float variable. so need to review again
					ADC_V = (ushort)((ADC_DATA[FRONT_REAL_SEN]*0.0008)*100);
					#else
					ADC_V = (ADC_DATA[FRONT_SEN]*0.0008)*100;
					#endif
				}
#endif				
				ADC_V_100=ADC_V/100;
				ADC_V_10=(ADC_V%100)/10;
				ADC_V_1=(ADC_V%100)%10;
	      
				DataBufferSize=0;
				DataBuffer[DataBufferSize++] = ADC_V_100 + 0x30;
				DataBuffer[DataBufferSize++] = '.';
				DataBuffer[DataBufferSize++] = ADC_V_10 + 0x30;
				DataBuffer[DataBufferSize++] = ADC_V_1 + 0x30;
				
				CmdResult = PRespons;
			}
			else CmdResult = T_CmdProc(CmdData,CmdData[0],DataBuffer,&DataBufferSize);	
					
			break;	
		#ifdef SamsungPay
		/// 20151130 hyesun : �Ｚ���� ���� Ŀ��� �߰�
		case 'p' :

#ifdef T1
				memset(gMS_DATA_T1,0,sizeof(gMS_DATA_T1));
#endif
#ifdef T2
				memset(gMS_DATA_T2,0,sizeof(gMS_DATA_T2));
#endif
#ifdef T3
				memset(gMS_DATA_T3,0,sizeof(gMS_DATA_T3));
#endif
				Byte_DATA_cnt_All = 0;
				Status_T1 = READ_ERROR_BLANK;
				Status_T2 = READ_ERROR_BLANK;
				Status_T3 = READ_ERROR_BLANK;
				Status_ALL = 0;
				Non_MS_DATA;
					
				if(CmdData[0]=='0')
				{
					SamsungPayMode = SET;
					CmdResult = PRespons;
				}
				else if(CmdData[0]=='1')
				{
					SamsungPayMode = RESET;
					CmdResult = PRespons;
				}
				else
				{
					CmdResult = ComandNotDefine;
				}
				break;
		#endif
				
		#ifdef USE_RF
		case 'F' :
			#if 1		//pbbch 180321 waiting time ������ ���Ͽ� �ɼ� �߰�.
			CmdResult = F_CmdProc(CmdData,CmdDatasize,DataBuffer,&DataBufferSize,1);	//2sec timeout
			#else
			CmdResult = F_CmdProc(CmdData,CmdDatasize,DataBuffer,&DataBufferSize);
			#endif
			break;
		#endif
				
		default :
			CmdResult = ComandNotDefine;//Resp_N_proc(1);//Command Not Define
	    break;
			//return;
	} // end switch
	
	/// always [2011/5/4] uart ��� �� ����
	if(UseUSB == RESET)
	{
		if(CmdResult == PRespons)
		{
			Resp_DATA_proc(DataBuffer,DataBufferSize);			

			if(Command == 'Z')
			{
				NVIC_SystemReset();
			}		
		}
		else
		{
			Resp_N_proc(CmdResult);
		}
	}
	#ifdef UseUsb
	/// always [2011/5/4] usb ��Ž� ����
	else
	{
		if(CmdResult == PRespons)
		{
			TxUSBData('P',0,DataBuffer,DataBufferSize);
			if(Command == 'Z')
			{
				delay_ms(100);				
				NVIC_SystemReset();
			}
		}
		else
		{
			TxUSBData('N',CmdResult,DataBuffer,DataBufferSize);
		}
	}
	#endif
}
#ifdef UseUsb
/*******************************************************************************
* Function Name  : USBCmdProc()
* Description    : USB�� ���� �� Ŀ�ǵ� ����
*******************************************************************************/
void USBCmdProc()
{
	//uchar *pData;
	uchar chainCnt;
	uchar ChainNum;
	uchar DataCnt,USBTxStat;
	//pData = &RX_Buffer[4];
	uint8_t Send_Buffer[63];	
	//Cmd_data_len = Length;
#if 0		//pbbch 171211 debug add
	unsigned char i=0;

	memset(Send_Buffer,0x00,sizeof(Send_Buffer));
#endif
	
	if(ReportID == 'O')
	{
		Cmd_proc(USBCmd,RX_Buffer,Length,SET);
	}
	/// always [2011/5/4] �׽�Ʈ��
	else
	{
		ChainNum = Length/63;
		for(chainCnt = 0;chainCnt <= ChainNum;chainCnt++)
		{
			for(DataCnt = 0;DataCnt < 63;)
			{
				Send_Buffer[DataCnt] = RX_Buffer[DataCnt+chainCnt*63];
				if((DataCnt+chainCnt*63) == Length)
				{
					break;
				}
				DataCnt++;
			}
			UserToPMABufferCopy(Send_Buffer, ENDP1_TXADDR, 63);
			SetEPTxCount(USBEndpoint, 63);
			SetEPTxValid(USBEndpoint);
							
			/// always [2011/4/20] ���� ������ ���� ���ε�� �� ����Ÿ�� ������� ���� �߻� ������ �ʿ�
			
			/// always [2011/7/12] ��Ŷ ���� �߰� tx �Ϸ� Ȯ��
			while(1)
			{
				/// always [2011/7/12] endp 1
				USBTxStat=GetEPTxStatus(1);//��� �����Ͱ� ���ۿϷ����� üũ�մϴ�.
				USBTxStat = USBTxStat & 0x30;
				if(USBTxStat==0x20) // ���ۿϷ� ���¶�� breakó��!     �ƴϸ� ��� ��ٸ��ϴ�.
				break;
				if((bDeviceState != 4)||(USBConStat == RESET)||(USBPowerStat == SET))
				{
					break;
				}
			}
		}
	}
	/// always [2011/3/24] usb test
#if 0		//pbbch 171211 debug add
	for (i=0; i<63; i++)
	{
		Send_Response_data(Send_Buffer[i]);
	}
	Send_Response_data(0x5c);
	Send_Response_data(0x72);
	Send_Response_data(0x5c);
	Send_Response_data(0x6e);
#endif
	//Send_Buffer[0] = 0x50;
}
#endif
/*******************************************************************************
* Function Name  : DMAInit()
* Description    : USB�� ���� �� ���Ǵ� DMA ���� �ʱ�ȭ
*******************************************************************************/
void DMAInit()
{
	CmdCnt = 0;
	CmdDMACnt = 0;
	CmdLRC = 0;
	Cmd_data_len = 0;
	DataCnt = 0;
	ReceiveStat = _STX;
	memset(DMATEst,0,sizeof(DMATEst));
	
	/// always [2011/5/17] DMA ī���͸� �ʱ�ȭ �ϱ� ���ؼ��� DMA ��ü�� �� ���� �Ͽ��� �Ѵ�.
	if(g_pcb_version == PCB_CR30_R3)
	{
		DMA_Cmd(DMA1_Channel5, DISABLE);
		DMA_CNDTR5_INT;
		DMA_Cmd(DMA1_Channel5, ENABLE);
	}
	else
	{
		DMA_Cmd(DMA1_Channel3, DISABLE);
		DMA_CNDTR3_INT;
		DMA_Cmd(DMA1_Channel3, ENABLE);
	}
}
/*******************************************************************************
* Function Name  : CMDTimeInit()
* Description    : ���� ��� �� Ÿ�̸� �ʱ�ȭ
*******************************************************************************/
void CMDTimeInit()
{
	TIM_Cmd(TIM6, DISABLE);//TIM_Cmd(TIM2, DISABLE);
	TIM6_CNT = TempFlashData.Main_WatingTime_INComm;//TIM2_CNT = TempFlashData.Main_WatingTime_INComm;
}

/*******************************************************************************
* Function Name  : CheckCMD()
* Description    : protocol parsing
*******************************************************************************/
void CheckCMD_org()
{
	uchar rxdata;
	rxdata = DMATEst[CmdCnt];
	#if defined(DEBUG)		//pbbch 181011 test add
printf("%02x ",rxdata);
#endif	
	switch(ReceiveStat)
	{
	case _STX:
		if(rxdata == STX)
		{	
			CMDTimeInit();
			CmdCnt++;
			ReceiveStat = _LENGTH_H;
			if_bcc_count=0;
		}
		else
		{
			ReceiveStat = ERROR;
		}
		break;
	case _LENGTH_H:
		CMDTimeInit();
		CmdLRC ^= rxdata;
		cmd_format.Len_H = rxdata;
		ReceiveStat = _LENGTH_L;
		CmdCnt++;
		break;
	case _LENGTH_L:
		CMDTimeInit();
		CmdLRC ^= rxdata;
		cmd_format.Len_L = rxdata;
		Make_Cmd_data_Len(cmd_format.Len_H,cmd_format.Len_L);//CM~DATA ���� 
		{
			ReceiveStat = _COMMAND;
		}
		 CmdCnt++;
		break;
	case _COMMAND:
		{
			if(Cmd_data_len==1 && (rxdata=='Z' || rxdata == 'V'))
			{
				gProtocol = OLD;
				CMDTimeInit();
				CmdLRC ^= rxdata;
				cmd_format.CMD= rxdata;
				CmdCnt++;
				ReceiveStat = _DATA;
			}
			else if(((rxdata >= 0x30)&&(rxdata <= 0x44))||rxdata=='Z')
			{
				gProtocol = NEW;
				CMDTimeInit();
				cmd_format.CMD= rxdata;
				CmdCnt++;
				ReceiveStat = _PARAMETER;
			}
			else
			{
				ReceiveStat = ERROR;
			}
		}
		break;
	case _PARAMETER:
		{
			CMDTimeInit();
			cmd_format.PM= rxdata;
			CmdCnt++;
			if(Cmd_data_len == 2)ReceiveStat = _CRC;// Data�� ���� 
			else ReceiveStat = _DATA;
		}
		break;
	case _DATA:
		CMDTimeInit();
		CmdLRC ^= rxdata;
		cmd_format.DATA[DataCnt++] = rxdata;
		CmdCnt++;
		if((rxdata == ETX) && (Cmd_data_len == 1))// 'V' or 'Z' (old protocol)
		{
			ReceiveStat = _BCC;
		}
		else if(DataCnt == (Cmd_data_len-2))//CM PM ������ DATA ���̸�..
		{
			ReceiveStat = _CRC;
		}
		#if 1		//pbbch 180220 when protocol length error or etc erro occure, nack response send and 
		else if(Cmd_data_len < DataCnt)
		{
			Response('K');		
			//DMAInit();
			ReceiveStat = ERROR;
		}
		#endif
		break;
	case _CRC:
		CMDTimeInit();
		CmdCnt++;
		if(if_bcc_count)
		{
			if_bcc_data = (ushort)(if_bcc_data << 8) & 0xFF00 | ((ushort)rxdata & 0x00FF);
			#if 0
			if(!Crc16Calc(if_bcc_data,cmd_format,Cmd_data_len,0))
			{
				ReceiveStat = _ETX;
			}
			else
			{
				Response('K');		
				ReceiveStat = ERROR;
			}
			#else
			ReceiveStat = _ETX;
			#endif
		}
		else 
		{
			if_bcc_data = (ushort)rxdata;
		}
		if_bcc_count++;
		break;
	case _ETX:
		CMDTimeInit();
		cmd_format.DATA[DataCnt++] = rxdata;
		CmdCnt++;
		if(ETX == rxdata)
		{
			#if 1
			if(!Crc16Calc(if_bcc_data,&cmd_format.Len_H,Cmd_data_len+3,0))//Len_H ���� ETX Field���� CRC16�� ��
			{
				ReceiveStat = _PACKET_END;
			}
			else
			{
				#if 1
				Response('K');		
				ReceiveStat = ERROR;
				#else//test
				ReceiveStat = _PACKET_END;
				#endif
			}
			#else
			if(if_bcc_data == getCRC(&cmd_format.Len_H,0,Cmd_data_len+1))
			{
				ReceiveStat = _PACKET_END;
			}
			else
			{
				Response('K');		
				ReceiveStat = ERROR;
			}
			#endif
			
		}
		else
		{
			Response('K');
			DMAInit();
		}	
		break;
	case _BCC:
		CMDTimeInit();
		if(CmdLRC == rxdata)
		{
			ReceiveStat = _PACKET_END;
		}
		else
		{
			Response('K');
			DMAInit();
		}	
		break;
	default:
		ReceiveStat = ERROR;
		break;
	}
	
	if(ReceiveStat == _PACKET_END)
	{
		#if defined(USE_IWDG_RESET)
		/* Reload IWDG counter */
		IWDG_ReloadCounter();
		#endif
		{
			#if 0		//pbbch 180221 dll test�뵵�� ������ nack ó��.
			Response('K');	
			#else
			if(gProtocol)Cmd_proc_new(cmd_format.CMD,cmd_format.PM,cmd_format.DATA,Cmd_data_len,RESET);
			else Cmd_proc(cmd_format.CMD,cmd_format.DATA,Cmd_data_len,RESET);
			#endif
		}
		DMAInit();
		#if 1 		//pbbch 180129 uart normal packet detect. so fucture uart only use.
		 gusb_protect.packet_complete=2;
		 #endif
	}
	else if(ReceiveStat == ERROR)
	{
		DMAInit();
	}
	else
	{
		TIM_Cmd(TIM6, ENABLE); //TIM_Cmd(TIM2, ENABLE);
	}
}

// domyst
void CheckCMD()
{
	uchar rxdata;
	rxdata = DMATEst[CmdCnt];
	#if defined(DEBUG)		//pbbch 181011 test add
printf("%02x ",rxdata);
#endif	
	switch(ReceiveStat)
	{
	case _STX:
		if(rxdata == STX)
		{	
			CMDTimeInit();
			CmdCnt++;
			ReceiveStat = _LENGTH_H;
			if_bcc_count=0;
		}
		else
		{
			ReceiveStat = ERROR;
		}
		break;
	case _LENGTH_H:
		CMDTimeInit();
		CmdLRC ^= rxdata;
		cmd_format.Len_H = rxdata;
		ReceiveStat = _LENGTH_L;
		CmdCnt++;
		break;
	case _LENGTH_L:
		CMDTimeInit();
		CmdLRC ^= rxdata;
		cmd_format.Len_L = rxdata;
		Make_Cmd_data_Len(cmd_format.Len_H,cmd_format.Len_L);//CM~DATA ���� 
		{
			ReceiveStat = _COMMAND;
		}
		 CmdCnt++;
		break;
	case _COMMAND:
		{
			if(Cmd_data_len==1 && (rxdata=='Z' || rxdata == 'V'))
			{
				gProtocol = OLD;
				CMDTimeInit();
				CmdLRC ^= rxdata;
				cmd_format.CMD= rxdata;
				CmdCnt++;
				ReceiveStat = _DATA;
			}
			else if(((rxdata >= 0x30)&&(rxdata <= 0x44))||rxdata=='Z')
			{
				gProtocol = NEW;
				CMDTimeInit();
				cmd_format.CMD= rxdata;
				CmdCnt++;
				ReceiveStat = _PARAMETER;
			}
			else
			{
				ReceiveStat = ERROR;
			}
		}
		break;
	case _PARAMETER:
		{
			CMDTimeInit();
			cmd_format.PM= rxdata;
			CmdCnt++;
			if(Cmd_data_len == 2)ReceiveStat = _CRC;// Data�� ���� 
			else ReceiveStat = _DATA;
		}
		break;
	case _DATA:
		CMDTimeInit();
		CmdLRC ^= rxdata;
		cmd_format.DATA[DataCnt++] = rxdata;
		CmdCnt++;
		if((rxdata == ETX) && (Cmd_data_len == 1))// 'V' or 'Z' (old protocol)
		{
			ReceiveStat = _BCC;
		}
		else if(DataCnt == (Cmd_data_len-2))//CM PM ������ DATA ���̸�..
		{
			ReceiveStat = _CRC;
		}
		#if 1		//pbbch 180220 when protocol length error or etc erro occure, nack response send and 
		else if(Cmd_data_len < DataCnt)
		{
			Response('K');		
			//DMAInit();
			ReceiveStat = ERROR;
		}
		#endif
		break;
	case _CRC:
		CMDTimeInit();
		CmdCnt++;
		if(if_bcc_count)
		{
			if_bcc_data = (ushort)(if_bcc_data << 8) & 0xFF00 | ((ushort)rxdata & 0x00FF);
			#if 0
			if(!Crc16Calc(if_bcc_data,cmd_format,Cmd_data_len,0))
			{
				ReceiveStat = _ETX;
			}
			else
			{
				Response('K');		
				ReceiveStat = ERROR;
			}
			#else
			ReceiveStat = _ETX;
			#endif
		}
		else 
		{
			if_bcc_data = (ushort)rxdata;
		}
		if_bcc_count++;
		break;
	case _ETX:
		CMDTimeInit();
		cmd_format.DATA[DataCnt++] = rxdata;
		CmdCnt++;
		if(ETX == rxdata)
		{
			#if 1
			if(!Crc16Calc(if_bcc_data,&cmd_format.Len_H,Cmd_data_len+3,0))//Len_H ���� ETX Field���� CRC16�� ��
			{
				ReceiveStat = _PACKET_END;
			}
			else
			{
				#if 1
				Response('K');		
				ReceiveStat = ERROR;
				#else//test
				ReceiveStat = _PACKET_END;
				#endif
			}
			#else
			if(if_bcc_data == getCRC(&cmd_format.Len_H,0,Cmd_data_len+1))
			{
				ReceiveStat = _PACKET_END;
			}
			else
			{
				Response('K');		
				ReceiveStat = ERROR;
			}
			#endif
			
		}
		else
		{
			Response('K');
			DMAInit();
		}	
		break;
	case _BCC:
		CMDTimeInit();
		if(CmdLRC == rxdata)
		{
			ReceiveStat = _PACKET_END;
		}
		else
		{
			Response('K');
			DMAInit();
		}	
		break;
	default:
		ReceiveStat = ERROR;
		break;
	}
	
	if(ReceiveStat == _PACKET_END)
	{
		#if defined(USE_IWDG_RESET)
		/* Reload IWDG counter */
		IWDG_ReloadCounter();
		#endif
		{
			#if 0		//pbbch 180221 dll test�뵵�� ������ nack ó��.
			Response('K');	
			#else
			if(gProtocol)Cmd_proc_new(cmd_format.CMD,cmd_format.PM,cmd_format.DATA,Cmd_data_len,RESET);
			else Cmd_proc(cmd_format.CMD,cmd_format.DATA,Cmd_data_len,RESET);
			#endif
		}
		DMAInit();
		#if 1 		//pbbch 180129 uart normal packet detect. so fucture uart only use.
		 gusb_protect.packet_complete=2;
		 #endif
	}
	else if(ReceiveStat == ERROR)
	{
		DMAInit();
	}
	else
	{
		//domyst TIM_Cmd(TIM6, ENABLE); //TIM_Cmd(TIM2, ENABLE);
		TIM_Cmd(TIMM0, TIM_6, ENABLE); 	// for mh1903
	}
}

void CheckCMD1()	// uart
{
	uchar rxdata;
	//rxdata = DMATEst[CmdCnt];
	rxdata = pop(&uart);
	#if defined(DEBUG)		//pbbch 181011 test add
printf("%02x ",rxdata);
#endif	
	switch(ReceiveStat)
	{
	case _STX:
		if(rxdata == STX)
		{	
			CMDTimeInit();
			CmdCnt++;
			ReceiveStat = _LENGTH_H;
			if_bcc_count=0;
		}
		else
		{
			ReceiveStat = ERROR;
		}
		break;
	case _LENGTH_H:
		CMDTimeInit();
		CmdLRC ^= rxdata;
		cmd_format.Len_H = rxdata;
		ReceiveStat = _LENGTH_L;
		CmdCnt++;
		break;
	case _LENGTH_L:
		CMDTimeInit();
		CmdLRC ^= rxdata;
		cmd_format.Len_L = rxdata;
		Make_Cmd_data_Len(cmd_format.Len_H,cmd_format.Len_L);//CM~DATA ���� 
		{
			ReceiveStat = _COMMAND;
		}
		 CmdCnt++;
		break;
	case _COMMAND:
		{
			if(Cmd_data_len==1 && (rxdata=='Z' || rxdata == 'V'))
			{
				gProtocol = OLD;
				CMDTimeInit();
				CmdLRC ^= rxdata;
				cmd_format.CMD= rxdata;
				CmdCnt++;
				ReceiveStat = _DATA;
			}
			else if(((rxdata >= 0x30)&&(rxdata <= 0x44))||rxdata=='Z')
			{
				gProtocol = NEW;
				CMDTimeInit();
				cmd_format.CMD= rxdata;
				CmdCnt++;
				ReceiveStat = _PARAMETER;
			}
			else
			{
				ReceiveStat = ERROR;
			}
		}
		break;
	case _PARAMETER:
		{
			CMDTimeInit();
			cmd_format.PM= rxdata;
			CmdCnt++;
			if(Cmd_data_len == 2)ReceiveStat = _CRC;// Data�� ���� 
			else ReceiveStat = _DATA;
		}
		break;
	case _DATA:
		CMDTimeInit();
		CmdLRC ^= rxdata;
		cmd_format.DATA[DataCnt++] = rxdata;
		CmdCnt++;
		if((rxdata == ETX) && (Cmd_data_len == 1))// 'V' or 'Z' (old protocol)
		{
			ReceiveStat = _BCC;
		}
		else if(DataCnt == (Cmd_data_len-2))//CM PM ������ DATA ���̸�..
		{
			ReceiveStat = _CRC;
		}
		#if 1		//pbbch 180220 when protocol length error or etc erro occure, nack response send and 
		else if(Cmd_data_len < DataCnt)
		{
			Response('K');		
			//DMAInit();
			ReceiveStat = ERROR;
		}
		#endif
		break;
	case _CRC:
		CMDTimeInit();
		CmdCnt++;
		if(if_bcc_count)
		{
			if_bcc_data = (ushort)(if_bcc_data << 8) & 0xFF00 | ((ushort)rxdata & 0x00FF);
			#if 0
			if(!Crc16Calc(if_bcc_data,cmd_format,Cmd_data_len,0))
			{
				ReceiveStat = _ETX;
			}
			else
			{
				Response('K');		
				ReceiveStat = ERROR;
			}
			#else
			ReceiveStat = _ETX;
			#endif
		}
		else 
		{
			if_bcc_data = (ushort)rxdata;
		}
		if_bcc_count++;
		break;
	case _ETX:
		CMDTimeInit();
		cmd_format.DATA[DataCnt++] = rxdata;
		CmdCnt++;
		if(ETX == rxdata)
		{
			#if 1
			if(!Crc16Calc(if_bcc_data,&cmd_format.Len_H,Cmd_data_len+3,0))//Len_H ���� ETX Field���� CRC16�� ��
			{
				ReceiveStat = _PACKET_END;
			}
			else
			{
				#if 1
				Response('K');		
				ReceiveStat = ERROR;
				#else//test
				ReceiveStat = _PACKET_END;
				#endif
			}
			#else
			if(if_bcc_data == getCRC(&cmd_format.Len_H,0,Cmd_data_len+1))
			{
				ReceiveStat = _PACKET_END;
			}
			else
			{
				Response('K');		
				ReceiveStat = ERROR;
			}
			#endif
			
		}
		else
		{
			Response('K');
			DMAInit();
		}	
		break;
	case _BCC:
		CMDTimeInit();
		if(CmdLRC == rxdata)
		{
			ReceiveStat = _PACKET_END;
		}
		else
		{
			Response('K');
			DMAInit();
		}	
		break;
	default:
		ReceiveStat = ERROR;
		break;
	}
	
	if(ReceiveStat == _PACKET_END)
	{
		#if defined(USE_IWDG_RESET)
		/* Reload IWDG counter */
		IWDG_ReloadCounter();
		#endif
		{
			#if 0		//pbbch 180221 dll test�뵵�� ������ nack ó��.
			Response('K');	
			#else
			if(gProtocol)Cmd_proc_new(cmd_format.CMD,cmd_format.PM,cmd_format.DATA,Cmd_data_len,RESET);
			else Cmd_proc(cmd_format.CMD,cmd_format.DATA,Cmd_data_len,RESET);
			#endif
		}
		DMAInit();
		#if 1 		//pbbch 180129 uart normal packet detect. so fucture uart only use.
		 gusb_protect.packet_complete=2;
		 #endif
	}
	else if(ReceiveStat == ERROR)
	{
		DMAInit();
	}
	else
	{
		//domyst TIM_Cmd(TIM6, ENABLE); //TIM_Cmd(TIM2, ENABLE);
		TIM_Cmd(TIMM0, TIM_6, ENABLE); 	// for mh1903
	}
}
#if 1
void ICPowerON_Test()
{
	
	if(testcnt)
	{
		testcnt++;
		memset(cmd_format.DATA,0,sizeof(cmd_format.DATA));
		cmd_format.CMD='R';
		Cmd_data_len=1;
		Cmd_proc(cmd_format.CMD,cmd_format.DATA,Cmd_data_len,RESET);
		memset(Response_P_format.DATA,0,sizeof(Response_P_format.DATA));
		DMAInit();

		memset(cmd_format.DATA,0,sizeof(cmd_format.DATA));
		cmd_format.CMD='D';
		Cmd_data_len=1;
		Cmd_proc(cmd_format.CMD,cmd_format.DATA,Cmd_data_len,RESET);
		memset(Response_P_format.DATA,0,sizeof(Response_P_format.DATA));
		DMAInit();
		//delay_ms(1000);
	}
}

#endif
                   
