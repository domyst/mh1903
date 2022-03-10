//domyst
#include	"main.h"
#incldue 	"systick.h"

#define uchar 	unsigned char
#define ushort	unsigned short

#define bOptionByte *(u32 *)(0x1ffff804)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

//domyst	������... bit banding ó����
bool rCardIN;
bool bLockState;


uint8_t gSTAT_B = 0;
uint8_t gSTAT2_B = 0;

#define delay_us(usec)		udelay(usec)
#define delay_ms(msec)		mdelay(msec)

///////
//#include "main.h"

static u8 gSTATE[8] @0x20000000 = { 0,0,0,0 };

#if 1 //pbbch 180129 usb protect structure add		//pbbch 180201 reset ���� �ʱ�ȭ ������ ���Ͽ� ���� �� define �߰� 
#define PROTCOL_SELECT_STATE   		0
#define USB_PROCESSING_STATE   		1
#define UART_PROCESSING_STATE  	2
#define END_STATE   					3

usb_protect gusb_protect;
unsigned char gmain_process_status=PROTCOL_SELECT_STATE;
#endif

extern u8 gMSDetect;
extern unsigned char icc_err_flag;
extern ushort ETX_flag;
//unsigned char Short_card_flag = 0;
unsigned char NO_card_flag = 0;
//unsigned char card_contact_flag = 0;
u32 SAM_check = 0;
int test_num_2;
char pow_ON = 0;

/// always [2010/12/27] �ڵ� ���巹��Ʈ ���� �׽�Ʈ
//ushort usart_T_test[20];
//extern uchar sync_check;
unsigned int test_cnt = 0;
//uchar t_test;
//uchar time_cnt = 0;
uchar StatSol = 0;
uint SolCnt;// = Sol_Locktime;

uchar Status_F2F_1;
uchar Status_F2F_2;
uchar Status_F2F_3;
uchar Retry_Cnt = 0;
uchar InDeal = RESET;
uchar LockUse = RESET;

// for renewal pcb sensor check.
ushort ADC_DMA_Buffer[Number_of_ADC*Number_of_Buf];
ushort ADC_DATA[Number_of_ADC];

extern ushort CmdDMACnt;
extern ushort CmdCnt ;

extern uchar InitSensor ;
// jsshin no use //extern unsigned char Cmd_buffer_cnt;
	
extern uchar icc_receive_done;
extern uchar cmd_receive_done;
//extern volatile unsigned char head_receive;
//extern volatile unsigned char recieve_status;
extern uchar USBReceiveStat;

extern FlashData TempFlashData;

extern uchar gMS_DATA_T1[DATA_MAX_T1];
extern uchar gMS_DATA_T2[DATA_MAX_T2];
extern uchar gMS_DATA_T3[DATA_MAX_T3];

// jsshin 2015.10.12 extern uchar MS_DATA_ALL[DATA_MAX_ALL];

extern uchar Byte_DATA_cnt_T1;
extern uchar Byte_DATA_cnt_T2;
extern uchar Byte_DATA_cnt_T3;
extern uchar Byte_DATA_cnt_All;

extern uchar Status_T1;
extern uchar Status_T2;
extern uchar Status_T3;
extern uchar Status_ALL;


extern uchar CheckFront;
extern uchar SolDuty;
//extern uchar Device_Response_cnt;
extern uchar is_ms_data_forward;
//extern uchar TimeCnt;
//#ifdef SamsungPay
uchar SamsungPayMode = RESET;
//#endif


//uchar Test_flag = 0;

extern void CheckCMD();

u16 stat;

#if 1	//pbbch 181015 add system tick verialble for soft ware uart init.
u32 gpre_systick_ctr = 0;
#endif

//////
void Green_ON()
{
	GREEN_ON;
	RED_OFF;
}

void Red_ON()
{
	RED_ON;
	GREEN_OFF;
}

void Green_T()
{
	GREEN_TOG;
	RED_OFF;
}

void Red_T()
{
	GREEN_OFF;
	RED_TOG;
}


#if 0 //domyst
void delay_us(volatile u32 usCount)
{
	volatile u32 us;
	for(;usCount != 0;usCount--)
	{
		for(us=0;us<4;)
		{
			us++;
		}
	}
}

void delay_ms(volatile u32 msCount)
{	
	for(;msCount != 0;msCount--)	delay_us(1024);
}
#endif //

// 1: card detect..... 0: card not detect
u8 Check_Sensor_State(u8 sensorNum)
{
	/// always [2013/3/29] 10���� ADC���� ����Ѵ�.
	/// always [2013/3/29] �����ʿ�
	/// hyesun [2013/6/11] ADC���ø� �������� ����
	u8 val = -1;
	if(g_pcb_version == PCB_GEM_POS)
	{
		switch(sensorNum)
		{
		case LOCK_SEN: val = (GPIO_LOCK_SENSOR_STAT); break;	// high ACTIVE
		case REAR_SEN: val = (GPIO_REAR_SENSOR_STAT); break;	// high ACTIVE
		case FRONT_SEN: val = (GPIO_FRONT_SENSOR_STAT); break; 	// high ACTIVE
		default: val= -1; break;
		}
	}
	else
	{
#if 1		//pbbch 180517 inner sensor �߰� display�� ���ؼ� �ڵ� ����.
		if(gsen_array_type == SEN12345_ARRAY)
		{
			switch(sensorNum)
			{
				case LOCK_SEN: val = ((Check_ADC_Sensor_State(LOCK_SEN))^1); break;		// low ACTIVE ������ ���߱� ���� invert �Ѵ�.
				case REAR_SEN: val =  Check_ADC_Sensor_State(REAR_SEN); break;			// high ACTIVE
				case FRONT_SEN: 
					val = (Check_ADC_Sensor_State(FRONT_SEN))^1; 						//inner sensor
					break;
				case FRONT_REAL_SEN:
					val = (Check_ADC_Sensor_State(FRONT_REAL_SEN))^1; 					// real front sensor low ACTIVE ������ ���߱� ���� invert �Ѵ�.
					break;
				default:
					val = -1;
			}
		}
		else
		{
			switch(sensorNum)
			{
				case LOCK_SEN: val = ((Check_ADC_Sensor_State(LOCK_SEN))^1); break;		// low ACTIVE ������ ���߱� ���� invert �Ѵ�.
				case REAR_SEN: val =  Check_ADC_Sensor_State(REAR_SEN); break;			// high ACTIVE
				case FRONT_SEN: 
				case FRONT_REAL_SEN:
					val = (Check_ADC_Sensor_State(FRONT_SEN))^1; 	// low ACTIVE ������ ���߱� ���� invert �Ѵ�.//inner sensor
					//if(val == SET) {
					//  if(Check_ADC_Sensor_State(REAR_SEN) == RESET) val = RESET;
					//}
					break;
				default:
					val = -1;
			}
		}
#else
		switch(sensorNum)
		{
		case LOCK_SEN: val = ((Check_ADC_Sensor_State(LOCK_SEN))^1); break;		// low ACTIVE ������ ���߱� ���� invert �Ѵ�.
		case REAR_SEN: val =  Check_ADC_Sensor_State(REAR_SEN); break;			// high ACTIVE
		case FRONT_SEN: 
			#if 1	//pbbch 180503 sen5 �߰��� board�� ���� sensor ó���� ���� �ؾ� ��. ���� ������ �߰�.  
			if(gsen_array_type == SEN12345_ARRAY)
			{
				val = (Check_ADC_Sensor_State(FRONT_REAL_SEN))^1; 	// low ACTIVE ������ ���߱� ���� invert �Ѵ�.
			}
			else
			{
				val = (Check_ADC_Sensor_State(FRONT_SEN))^1; 	// low ACTIVE ������ ���߱� ���� invert �Ѵ�.
			}
			#else
			val = (Check_ADC_Sensor_State(FRONT_SEN))^1; 	// low ACTIVE ������ ���߱� ���� invert �Ѵ�.
			#endif
			//if(val == SET) {
			//  if(Check_ADC_Sensor_State(REAR_SEN) == RESET) val = RESET;
			//}
			break;
		default:
			val = -1;
		}
#endif		
	}
	return val;
	
////////	if(g_pcb_version == PCB_GEM_POS)
////////	{
////////		Lock_Sensor_Status = GPIO_LOCK_SENSOR_STAT;		// high ACTIVE
////////		Rear_Sensor_Status = GPIO_REAR_SENSOR_STAT;		// high ACTIVE
////////		Front_Sensor_Status = GPIO_FRONT_SENSOR_STAT;	// high ACTIVE
////////	}
////////	else
////////	{
////////		Lock_Sensor_Status = (Check_ADC_Sensor_State(LOCK_SEN))^1;		// low ACTIVE ������ ���߱� ���� invert �Ѵ�.
////////		Rear_Sensor_Status = Check_ADC_Sensor_State(REAR_SEN);			// high ACTIVE
////////		Front_Sensor_Status = (Check_ADC_Sensor_State(FRONT_SEN))^1; 	// low ACTIVE ������ ���߱� ���� invert �Ѵ�.
////////		if(Front_Sensor_Status == SET)
////////		{
////////		  if(Rear_Sensor_Status == RESET) Front_Sensor_Status = RESET;
////////		}
////////	}
}


#if 1 		//pbbch 180129 we need to know usb connect or not connect. so function add
void protect_usb_detect(void)
{
	unsigned char ldetect_cnt=0;
	unsigned char i=0;

	for(i=0;i<3;i++)
	{
		if(USBConStat == SET)
		{
			ldetect_cnt++;
			delay_ms(10);
		}
	}

	if(ldetect_cnt>=3)	gusb_protect.detect=1;
}

void protect_usb_processing(void)
{
	if(USBReceiveStat == SET)
	{
		USBReceiveStat = RESET;
		USBCmdProc();
	}
}

void protect_uart_processing(void)
{
	/// always [2011/5/17] while������ ��� ���� �� �ӵ� ���� �߻�
	if(g_pcb_version == PCB_CR30_R3)
	{
		CmdDMACnt = DMABufferSize - DMA_CNDTR5;
	}
	else
	{
		CmdDMACnt = DMABufferSize - DMA_CNDTR3;
	}	
	if(CmdDMACnt > CmdCnt)
	{
		CheckCMD();
	}
	if(CmdDMACnt < CmdCnt)
	{
		DMAInit();
	}	

	#if 1		//pbbch 180220 when protocol length error or etc erro occure,  timout nack send
	if(lprotocol_timeout_flg)
	{
		Response('K');
		lprotocol_timeout_flg=0;
	}
	#endif
}

void protect_uart_processing1(void)
{
	/// always [2011/5/17] while������ ��� ���� �� �ӵ� ���� �߻�
	// if(g_pcb_version == PCB_CR30_R3)
	// {
	// 	CmdDMACnt = DMABufferSize - DMA_CNDTR5;
	// }
	// else
	// {
	// 	CmdDMACnt = DMABufferSize - DMA_CNDTR3;
	// }	
	// if(CmdDMACnt > CmdCnt)
	// {
	// 	CheckCMD();
	// }
	// if(CmdDMACnt < CmdCnt)
	// {
	// 	DMAInit();
	// }	

    if (RESET != DMA_GetRawStatus(DMA_Channel_0, DMA_IT_DMATransferComplete))
    {
        CheckCMD();
    }
    
	#if 1		//pbbch 180220 when protocol length error or etc erro occure,  timout nack send
	if(lprotocol_timeout_flg)
	{
		Response('K');
		lprotocol_timeout_flg=0;
	}
	#endif
}

void protect_uart_processing2(void)
{
	/// always [2011/5/17] while������ ��� ���� �� �ӵ� ���� �߻�
	if(g_pcb_version == PCB_CR30_R3)
	{
		CmdDMACnt = DMABufferSize - DMA_CNDTR5;
	}
	else
	{
		CmdDMACnt = DMABufferSize - DMA_CNDTR3;
	}	
	if(CmdDMACnt > CmdCnt)
	{
		CheckCMD();
	}
	if(CmdDMACnt < CmdCnt)
	{
		DMAInit();
	}	

	if (!isEmpty(&uart))
	{
		CheckCMD();
	}

	#if 1		//pbbch 180220 when protocol length error or etc erro occure,  timout nack send
	if(lprotocol_timeout_flg)
	{
		Response('K');
		lprotocol_timeout_flg=0;
	}
	#endif
}
#endif

#if 1//pbbch 180126 usb protect code apply.
void 
CheckDMA(void)			//pbbch 180129 void add
{
	#if 0		//pbbch 180201 reset���� ���� �ʱ�ȭ ������ �־�  global ������ ����. 
	enum{PROTCOL_SELECT_STATE = 0, USB_PROCESSING_STATE,  UART_PROCESSING_STATE, END_STATE};
	static unsigned char lmain_status=PROTCOL_SELECT_STATE;
	#endif

	switch(gmain_process_status)				//pbbch 180201 reset ���� �ʱ�ȭ ������ ���Ͽ� ����.
	{
		case PROTCOL_SELECT_STATE:		//protocol slect part
			protect_usb_detect();
			if(gusb_protect.detect)		//usb detect
			{
				#ifdef UseUsb
				if(!gusb_protect.configured_flag)
				{
					init_usb_configure_exe();
				}
				protect_usb_processing();
				#endif
				protect_uart_processing();	//uart detect
			}
			else							//uart detect
			{
				protect_uart_processing();
				#if 1	//pbbch 181015 add system tick verialble for soft ware uart init.//pbbch 181016 ī�� ���Խ� green on ���°� �߻��Ͽ� main���� ��ġ ����
				if((gsys_tick_cnt-gpre_systick_ctr)>TIMEOUT_10SEC)
				{
					ErrorFlag = SET;
					Red_T();
					OptionByte_initial();	//HOST comm initialize
					gpre_systick_ctr =gsys_tick_cnt;
					#if defined(DEBUG)		//pbbch 181011 test add
					printf("uart reinit:%d\r\n",gpre_systick_ctr);
					#endif
					//delay_ms(100);
					//Green_T();
				}
					
				#endif
			}
			switch(gusb_protect.packet_complete)
			{
				case 1:
					#if 1		//pbbch 180201 reset���� ���� �ʱ�ȭ ������ �־�  global ������ ����.
					gmain_process_status=USB_PROCESSING_STATE;
					#else
					lmain_status=USB_PROCESSING_STATE;		//pbbch 180129 ���� ���� ���� ���� .....fail�� ���� ����.
					#endif
				break;
				case 2:
					#if 1		//pbbch 180201 reset���� ���� �ʱ�ȭ ������ �־�  global ������ ����.
					gmain_process_status=UART_PROCESSING_STATE;
					#if 1	//pbbch 181015 add system tick verialble for soft ware uart init.//pbbch 181016 ī�� ���Խ� green on ���°� �߻��Ͽ� main���� ��ġ ����
					ErrorFlag = RESET;
					Green_T();
					#endif
					#else
					lmain_status=UART_PROCESSING_STATE;
					#endif
				break;
				default: 
				break;
			}
		break;
		case USB_PROCESSING_STATE:				//usb processing part
			protect_usb_processing();
		break;
		case UART_PROCESSING_STATE:			// uart processing part
			protect_uart_processing();
		break;
		case END_STATE:
		default:
		break;
		
	}
}
#else
void CheckDMA(void)			//pbbch 180129 void add
{
/// always [2011/5/17] while������ ��� ���� �� �ӵ� ���� �߻�
	if(g_pcb_version == PCB_CR30_R3)
	{
		CmdDMACnt = DMABufferSize - DMA_CNDTR5;
	}
	else
	{
		CmdDMACnt = DMABufferSize - DMA_CNDTR3;
	}	
	if(CmdDMACnt > CmdCnt)
	{
		CheckCMD();
	}
	if(CmdDMACnt < CmdCnt)
	{
		DMAInit();
	}
	/// always [2011/5/12] USB �� Ŀ�ǵ� ���� ��
	#ifdef UseUsb
	if(USBReceiveStat == SET)
	{
		USBReceiveStat = RESET;
		USBCmdProc();
	}
	#endif
}
#endif
/******************************************************************************************************/
/* Check_Sensor:	������ üũ�Ͽ� ������ ī�� ���¸� Ȯ���Ѵ�.   		        				      */												
/******************************************************************************************************/
void Check_Sensor()
{
	//uchar LockState = 0;
	uchar WaitTime = 250; // 500 ���� 250���� ����. uchar ����Ʈ ���� ��.
	uchar WaitTime_ = 200;
#if 0	//pbbch 180425 ic deactivation add
	unsigned char ltemp_buf[10]={0,};	
#endif
	
	/// ī�尡 ���� ����
#if 1	//pbbch 180517 real front sensor change
	if((Check_Sensor_State(REAR_SEN) == RESET) && (Check_Sensor_State(FRONT_REAL_SEN) == RESET))
#else	
	if((Check_Sensor_State(REAR_SEN) == RESET) && (Check_Sensor_State(FRONT_SEN) == RESET))
#endif		
	{
		
		NO_ICC_Reset;
		// if(g_pcb_version == PCB_GEM_POS)
		// {
		// 	Front_Detect_Non;
		// 	MS_F_Read_ON;
		// }
		// else
		// {
			if(CheckFront == RESET)
			{
				Front_Detect_Non;
				MS_F_Read_ON;
			}
		// }			
		
		Rear_Detect_Non;
		
		gMSDetect = 0; // ī�� ���� �� �ش� �÷��� ����.
		NO_card_flag = SET;
		//bCardIN_OFF;
//		bShortCard_OFF; // 2012.12.28
//		bJamCard_OFF; // 2015.12.28
		rCardIN_OFF;
		bLockSuc;
#if 1	//pbbch 180517 inner sensor check add
		if(gsen_array_type == SEN12345_ARRAY)
		{
			if(Check_Sensor_State(FRONT_SEN)== RESET)	Inner_Detect_Non;
		}
#endif
		/// always [2011/3/8] ó�� ���� Ȯ��
		if((TempFlashData.ModuleType == USEIC)||(TempFlashData.ModuleType == USEALL))
		{
			if(InitSensor == SET)
			{
				InitSensor = RESET;
			}
			
			if(LockUse == RESET)//Lock Use//20140512_hyesun: ī�� Lock ��ġ ��� ����Ȯ��
			{
				if(StatSol == SET)
				{
					SolCnt = TempFlashData.AutoLocktime*60;//Sol_Locktime;
					StatSol=RESET;
					InDeal = RESET;
					SOL_CON_OFF;		//xxx domyst .. PWM ó�� Ȯ�� nxp���� pwm���� ó����. Unlock
				}
			}
			else InDeal = RESET;
		}			
	}
	/// always [2010/12/20] ī�尡 front �� ������ ��
#if 1	//pbbch 180517 real front sensor change
	if((Check_Sensor_State(REAR_SEN) == RESET) && (Check_Sensor_State(FRONT_REAL_SEN) == SET)&&(rCardIN==RESET))
#else	
	if((Check_Sensor_State(REAR_SEN) == RESET) && (Check_Sensor_State(FRONT_SEN) == SET)&&(rCardIN==RESET))
#endif		
	{
		NO_ICC_Reset; // �ش� ���� ���¸� STAT ������ bit bending ���� �ݿ� ���� �ش�.
		Front_Detect_ON;
		Rear_Detect_Non;
#if 1	//pbbch 180517 inner sensor check add
		if(gsen_array_type == SEN12345_ARRAY)
		{
			if(Check_Sensor_State(FRONT_SEN)== SET)	Inner_Detect_ON;		// nxp������ ó������ ����. �ٽ� Ȯ��
		}
#endif		
	}
	
	/// ī�尡 ���� ���� ��
#if 1	//pbbch 180517 real front sensor change
	if((Check_Sensor_State(REAR_SEN) == SET) && (Check_Sensor_State(FRONT_REAL_SEN) == SET))//&&(rCardIN == RESET))
#else	
	if((Check_Sensor_State(REAR_SEN) == SET) && (Check_Sensor_State(FRONT_SEN) == SET))//&&(rCardIN == RESET))
#endif		
	{
		if((TempFlashData.ModuleType == USEIC)||(TempFlashData.ModuleType == USEALL))
		{
			/// always [2011/3/9] �ڵ� ���� ��
			if(InitSensor == RESET)
			{
				if(LockUse == RESET)//Lock Use//20140512_hyesun: ī�� Lock ��ġ ��� ����Ȯ��
				{
					if((StatSol == RESET)&&(rCardIN==RESET))
					{
						__disable_irq(); //__SETPRIMASK(); // jsshin 2015.08.18 change with CMSIS core func
						SolOn(100,SolDuty);
						rCardIN_ON;
						Retry_Cnt = 0;
						StatSol=SET;
						InDeal = SET;
						__enable_irq(); //__RESETPRIMASK(); // jsshin 2015.08.18 change with CMSIS core func
						
						while(WaitTime)
						{
							if(Check_Sensor_State(LOCK_SEN) == SET)
							{
								
								bLockSuc;//20140116_hyesun:Ǫ����� ���� ���� �߰� 
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
							SOL_CON_OFF;		//xxx
							rCardIN_ON;			//xxx
							bLockFail;
						}
					}
				}
				else
				{
					rCardIN_ON;
					InDeal = SET;
				}
			}
			
			/// ���� �� ����
			if(((SolCnt == 0)||!(SOL_CON))&&(StatSol == SET))
			{
				/////////////////////////
				// jsshin 2016.05.30 : add unlock retry
				SOL_CON_OFF;
				delay_ms(50);
				if(Check_Sensor_State(LOCK_SEN) == SET)
				{
					__disable_irq();
					SolOn(50,SolDuty);				
					__enable_irq(); //__RESETPRIMASK();
					delay_ms(50);
				}
				/////////////////////////
				
				SolCnt = TempFlashData.AutoLocktime*60;//Sol_Locktime;
				StatSol=RESET;
				InDeal = RESET;
				SOL_CON_OFF;
				rCardIN_ON;
			}
		}
	    // Front insert �Ϸ�� MS ������ ���� ��Ų��! // ������ backward Read ��!
	    if(NO_card_flag == SET)
	    {
	    #if 1//defined(KTC_MODEL)
			// ī�尡 ���� �Ǿ����� MSData Detect ���� �ʾҴ�. ���� ������ ����.
			#ifdef Hanmega_USA
			gMSDetect = 0; // Forward Read �� MS Data Clear!! : �Ѹް� ��û 20160720
			#endif
			if(0 == gMSDetect)
			{
				#ifdef T1
				memset(gMS_DATA_T1,0,sizeof(gMS_DATA_T1));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
				#endif
				#ifdef T2
				memset(gMS_DATA_T2,0,sizeof(gMS_DATA_T2));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
				#endif
				#ifdef T3
				memset(gMS_DATA_T3,0,sizeof(gMS_DATA_T3));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
				#endif
				Byte_DATA_cnt_All = 0;
				Byte_DATA_cnt_T1 = Byte_DATA_cnt_T2 = Byte_DATA_cnt_T3 = 0;
				Non_MS_DATA;
				Status_T1 = READ_ERROR_BLANK;
				Status_T2 = READ_ERROR_BLANK;
				Status_T3 = READ_ERROR_BLANK;
				Status_ALL = 0;
			}
			gMSDetect = 0; // ī�� ���� �� �ش� �÷��� ����.
		#endif

#if 0		//pbbch 180319 ic reset�� ic card error �߻��ø�  ���� �Ѵ�. Test�� ���ؼ� �ڵ� ����.
			// 2016.02.01 ī�尡 ��� ���� CR30 ����!
#if defined(USE_ICC_115200)		//pbbch 180209 ms eject read �� delay �߻� ��Ű�� �κ�// ���� �� �κ� ���� ����.
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
#endif
	    }
		Front_Detect_ON;
		Rear_Detect_ON;
		MS_R_Read_ON;
		NO_card_flag = RESET;
#if 1	//pbbch 180517 inner sensor check add
		if(gsen_array_type == SEN12345_ARRAY)
		{
			if(Check_Sensor_State(FRONT_SEN)== SET)	Inner_Detect_ON;
		}
#endif			
		// 20150401 hyesun : ŰƮ�δн� �� ������ �κ� ����(Forward Read �� Sensor Satate ��)
		if(is_ms_data_forward == SET)
		{
			is_ms_data_forward = RESET;
			IS_MS_DATA;
		}
		
	}
#if 1	//pbbch 180517 real front sensor change
	if((Check_Sensor_State(REAR_SEN) == SET) && (Check_Sensor_State(FRONT_REAL_SEN) == RESET))//&&(!(SOL_CON))) // SOL_CON�� ������(���� ���̸�)
#else	
	if((Check_Sensor_State(REAR_SEN) == SET) && (Check_Sensor_State(FRONT_SEN) == RESET))//&&(!(SOL_CON))) // SOL_CON�� ������(���� ���̸�)
#endif		
	{
		Rear_Detect_ON;
		Front_Detect_Non;
		E_CmdProc(200);
	}
#if 1	//pbbch 180517 inner sensor check add
	if(gsen_array_type == SEN12345_ARRAY)
	{
		if((Check_Sensor_State(FRONT_REAL_SEN) == SET)&&(Check_Sensor_State(REAR_SEN) == RESET) 
			&& (Check_Sensor_State(FRONT_SEN) == RESET))//real fron sen on and rear sen off  and inner sen off
		{
				Rear_Detect_Non;
				Inner_Detect_Non;
		}
	}
#endif
	
	if(((SolCnt == 0)||!(SOL_CON))&&(StatSol == SET)&&(rCardIN)) // jsshin rCardIN_ON --> rCardIN
	{
		/////////////////////////
		// jsshin 2016.05.30 : add unlock retry
		SOL_CON_OFF;
		delay_ms(50);
		if(Check_Sensor_State(LOCK_SEN) == SET)
		{
			__disable_irq();
			SolOn(50,SolDuty);				
			__enable_irq();
			delay_ms(50);
		}
		/////////////////////////
		
		SolCnt = TempFlashData.AutoLocktime*60;//Sol_Locktime;
		StatSol=RESET;
		InDeal = RESET;
		SOL_CON_OFF;
		rCardIN_ON;

		while(WaitTime_)
		{
			if(Check_Sensor_State(LOCK_SEN) != SET)
			{
				//SolCnt = Sol_Locktime;
				SolCnt = TempFlashData.AutoLocktime*60;
				StatSol = RESET;
				InDeal = RESET;
				SOL_CON_OFF;
				rCardIN_ON;
				bLockSuc;//20140116_hyesun:Ǫ����� ���� ���� �߰�
				break;
			}
			delay_ms(1);
			WaitTime_--;
		}
		if(WaitTime_ == 0)
		{
			WaitTime_ =200;
			InDeal = RESET;
			bLockFail;
		}
	}
	
	/// always [2011/11/25]  �׽� Lock ������ ���¸� ���� �Ѵ�.
	if(Check_Sensor_State(LOCK_SEN) == SET)
	{
		#if 1		//pbbch 180320 eject ���������� CR30�� Card detect signal�� high�� �־� card�� ������ �˷� detective�� ������ �ؾ� �Ѵ�.
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9))	// domyst ��Ʈ Ȯ������..
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_9);
		}
		#endif
		LockDetectON;
	}
	else
	{
		//pbbch 180425 lock�� ��� 10�� timeout ���� �ڵ����� lock�� Ǯ�� ������ �� ��ȣ�� card detect�� ��ġ�ϱ��  ������ �־�
		// eject command�� lock ���� sw�� ���� �Ǹ� remove�� �ν� �ϵ��� ���� ��. �Ʒ� �ڵ� ����.
		#if 0		//pbbch 180320 eject ���������� CR30�� Card detect signal�� high�� �־� card�� ������ �˷� detective�� ������ �ؾ� �Ѵ�.
		if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9))
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_9);
		}
		#endif
		LockDetectOFF;
	}
	
}

/******************************************************************************************************/
/* Check_STAT	:		        						      */												
/******************************************************************************************************///
/// always [2010/12/21] �� 3~4us �ҿ�
void Check_STAT(void)
{
//	uchar ErrorState;
//	uchar test=0;
	
	Check_Sensor();
	
	CheckDMA();
#if 0		//pbbch 180305 __disable_irq() test add
	if(MS_nCLS_1_IN != SET)
	{
		__disable_irq(); //__SETPRIMASK(); // jsshin 2015.08.18 change with CMSIS core func
		
		CheckFront = SET;
		MS_Bit_DATA_R();

		/// always [2011/12/5] ��� ���� �� MS ���带 �����ϸ� Ÿ�Ӿƿ� �߻��� �Բ� �����鼭 ����Ÿ�� �ʱ�ȭ �Ѵ�.
		/// always [2011/12/5] ���� Ÿ�Ӿƿ��� ������ ���� �Ŀ� ��� ��Ʈ�� Ŭ�����ϰ� �����͸� üũ�ϵ��� �Ѵ�.
		//TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		__enable_irq(); //__RESETPRIMASK(); // jsshin 2015.08.18 change with CMSIS core func
		
		Status_F2F_1 = F2F_1_IN;
		Status_F2F_2 = F2F_2_IN;
		Status_F2F_3 = F2F_3_IN;
	}
#else
	/// Decoder ���� 
#if !defined(KTC_MODEL)
// jsshin 2016.01.26 // ī�带 ������ MS ���ڵ� �ϵ��� �Ѵ�.
	#ifndef SamsungPay
		#ifdef Hanmega_USA
		if(((TempFlashData.ModuleType == USEMS) || (TempFlashData.ModuleType == USEALL)) && (Check_Sensor_State(LOCK_SEN) == RESET) && MS_Read_IN)
		#else
		if(((TempFlashData.ModuleType == USEMS) || (TempFlashData.ModuleType == USEALL)) && (Check_Sensor_State(LOCK_SEN) == RESET))
		#endif
	#else
	if(Check_Sensor_State(LOCK_SEN) == RESET||SamsungPayMode == SET)
	#endif
  	{
		if((MS_nCLS_1_IN != SET)&&((Status_F2F_1 != F2F_1_IN)||(Status_F2F_2 != F2F_2_IN)||(Status_F2F_3 != F2F_3_IN)))
		{
			__disable_irq(); //__SETPRIMASK(); // jsshin 2015.08.18 change with CMSIS core func
			
			CheckFront = SET;
			MS_Bit_DATA_R();

			/// always [2011/12/5] ��� ���� �� MS ���带 �����ϸ� Ÿ�Ӿƿ� �߻��� �Բ� �����鼭 ����Ÿ�� �ʱ�ȭ �Ѵ�.
			/// always [2011/12/5] ���� Ÿ�Ӿƿ��� ������ ���� �Ŀ� ��� ��Ʈ�� Ŭ�����ϰ� �����͸� üũ�ϵ��� �Ѵ�.
			//TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
			
			__enable_irq(); //__RESETPRIMASK(); // jsshin 2015.08.18 change with CMSIS core func
			
			Status_F2F_1 = F2F_1_IN;
			Status_F2F_2 = F2F_2_IN;
			Status_F2F_3 = F2F_3_IN;
		}
	}
#endif
#endif
}

/*
void CheckUSBCon()
{
	if((bDeviceState != CONFIGURED)&&(USBConStat == SET))
}
*/
//pbbch 181012 host uart init fucntion add....kiss �뷡�� ���� ��.
void reinit_host_port_init(void)
{
#if 1	//pbbch 181015 add system tick verialble for soft ware uart init.
	unsigned char lloop=1;

	gpre_systick_ctr =gsys_tick_cnt;

	#if 1	//pbbch 181016 ī�� ���Խ� green on ���°� �߻��Ͽ� main���� ��ġ ����
	do
	{
		CheckDMA();
		switch(gmain_process_status)
		{
			case USB_PROCESSING_STATE:
			case UART_PROCESSING_STATE:
				lloop=0;
				break;
			case PROTCOL_SELECT_STATE:
			default:
			break;
		}
	}while(lloop);
	#endif
#else
	unsigned long i=0;

	for(i=0; i<0xffffe0;i++);
	OptionByte_initial();	//HOST comm initialize
#endif
}

void OptionByte_initial_Response()
{
	uchar OptionByte;
#if 0		//pbbch 171212 warning across	
	uchar BaudrateStat;
//	uint Baudrate;
	uchar HardwareFlowStat;
#endif
//	ushort HardwareFlow;
	uchar ResetFlag;
	uchar DownloadFlag;
	uchar LockuseFlag;

	uchar Dummy[1];

	OptionByte = (uchar)bOptionByte & 0xff  ;

	if(OptionByte == 0xff)
	{
		OptionByte = 0x10;
		FLASH_Unlock();
		FLASH_EraseOptionBytes();					
		FLASH_ProgramOptionByteData(0x1ffff804,OptionByte);
	}

#if 0		//pbbch 171212 warning across	
	BaudrateStat = (OptionByte >> 4)&(0x03);
	HardwareFlowStat = (OptionByte >> 4)&(0x0c);
#endif	
	ResetFlag = (OptionByte >> 1) & (0x1);
	LockuseFlag = (OptionByte >> 2) & (0x1);
	DownloadFlag = OptionByte & 0x1;

	LockUse = LockuseFlag;
	if((ResetFlag == SET)|(DownloadFlag == SET))
	{
		if(ResetFlag == SET)
		{
			#ifdef UseUsb
			/// always [2011/5/4] usb ��Ž� ����
			//pbbch 180201 option byte init response�� init ��� ���� ���� �ϳ�, usb ������ �ȵǸ� ���� ����.
			//���� protocol select�� �ܸ��� �������� �ƴ� host ù��° ��� �� ������ �Ǿ�� �ϹǷ� ���� �ʱ�ȭ ��Ŵ.
			#if 1		
			if(gusb_protect.detect)
			{
				TxUSBData('P',0,Dummy,0);
				gusb_protect.detect=0;
				gusb_protect.packet_complete=0;
				gmain_process_status=0;
			}
			#else
			TxUSBData('P',0,Dummy,0);
			#endif
			#endif
			/// always [2011/5/4] uart ��� �� ����.
			Resp_DATA_proc(Dummy,0);
		
			OptionByte &= ~(0x01 <<1);
		}
		if(DownloadFlag == SET)
		{
			OptionByte &= ~(0x01);
		}
		
		FLASH_Unlock();////20140721 hyesun ó�� ��ġ ����
		FLASH_EraseOptionBytes();////20140721 hyesun ó�� ��ġ ����					
		FLASH_ProgramOptionByteData(0x1ffff804,OptionByte);////20140721 hyesun ó�� ��ġ ����
	}
}

//pbbch 181012 host uart init fucntion add....kiss �뷡�� ���� ��.
void reinit_host_port_init(void)
{
#if 1	//pbbch 181015 add system tick verialble for soft ware uart init.
	unsigned char lloop=1;

	gpre_systick_ctr =gsys_tick_cnt;

	#if 1	//pbbch 181016 ī�� ���Խ� green on ���°� �߻��Ͽ� main���� ��ġ ����
	do
	{
		CheckDMA();
		switch(gmain_process_status)
		{
			case USB_PROCESSING_STATE:
			case UART_PROCESSING_STATE:
				lloop=0;
				break;
			case PROTCOL_SELECT_STATE:
			default:
			break;
		}
	}while(lloop);
	#endif
#else
	unsigned long i=0;

	for(i=0; i<0xffffe0;i++);
	OptionByte_initial();	//HOST comm initialize
#endif
}

void appmain(void)
{
    memset(gSTATE, 0, sizeof(gSTATE));
	CORTEX_initial(); 

	// Status_F2F_1 = F2F_1_IN;
	// Status_F2F_2 = F2F_2_IN;
	// Status_F2F_3 = F2F_3_IN;

	#if defined(DEBUG)		//pbbch 181011 test add
	printf("main init end\r\n");
	#endif

	#if 1		//pbbch 180319 avoid protocol error, move init response posion	
	OptionByte_initial_Response();	//����
	#endif

	#if 1//pbbch 181012 host uart init fucntion add....kiss �뷡�� ���� ��.
	reinit_host_port_init();
	#endif

#if 0		//pbbch 180305 __disable_irq() test add
	__disable_irq(); 
	//__SETPRIMASK();
#endif
	
	while(1)
	{	
		#if defined(USE_IWDG_RESET)
		/* Reload IWDG counter */
		IWDG_ReloadCounter();
		#endif
		Check_STAT();

		#if 0
		ICPowerON_Test();
		#endif
	}
	
}