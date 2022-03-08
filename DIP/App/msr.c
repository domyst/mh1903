/************************** (C) COPYRIGHT 2010 TITENG Co., Ltd Inc. *********************************/
/* project 	  : ky-7911 ms/r         		    				    */
/* processor 	  : CORETEX-M3(STM32F103vc)         		    				    */
/* compiler       : IAR EWARM Compiler								    */
/* file name      : msr.c								    */
/* purpose       : ms data read					    */
/* program by	  : jongil yun								    */
/* History:											    */
/* 		11/03/2010 - Version 0.1 Started						    */
/* copy right	  : TITENG Co., Ltd								    */
/****************************************************************************************************/
#include "msr.h"

#define MAX_R_LOOP_CNT		610000		//about 1.9sec time out//pbbch 180306 ms read max loop counter define add

extern u8 Check_Sensor_State();
#if defined(KTC_MODEL)
extern void DLK_MemoryRead32(void *pDst, void *pSrc, u32 len);
extern void DLK_MemoryWrite32(void *pDst, void *pSrc, u32 len);
#else
#define  DLK_MemoryRead32 memcpy
#define  DLK_MemoryWrite32 memcpy
#endif
	
u8 gMSDetect = 0;
#ifdef T1
uchar gMS_DATA_T1[DATA_MAX_T1];
#endif
#ifdef T2
uchar gMS_DATA_T2[DATA_MAX_T2];
#endif
#ifdef T3
uchar gMS_DATA_T3[DATA_MAX_T3];
#endif
// jsshin 2015.10.12 uchar MS_DATA_ALL[DATA_MAX_ALL];
	
uchar Status_T1 = READ_ERROR_BLANK;
uchar Status_T2 = READ_ERROR_BLANK;
uchar Status_T3 = READ_ERROR_BLANK;
uchar Status_ALL = READ_ERROR_BLANK;

uchar Status_TIME_T1;
uchar Status_TIME_T2;
uchar Status_TIME_T3;

uchar Byte_DATA_cnt_T1 = 0;
uchar Byte_DATA_cnt_T2 = 0;
uchar Byte_DATA_cnt_T3 = 0;
uchar Byte_DATA_cnt_All = 0;

uchar Byte_DATA_cnt_TIME_T1 = 0;
uchar Byte_DATA_cnt_TIME_T2 = 0;
uchar Byte_DATA_cnt_TIME_T3 = 0;

uchar is_ms_data_forward = RESET;
extern uchar CheckFront;
extern uchar SamsungPayMode;

//extern uchar Lock_Sensor_Status;
//extern uchar Front_Sensor_Status;
//extern uchar Rear_Sensor_Status;
//extern uchar Board_Check_Sensor_Status;
//extern uchar HW_TYPE;

//extern ushort systickCnt[max_Time_buffer] ;

///*******************************************************************************
//* Function Name  : Back_Read(uchar *MS_bit_data,ushort length)
//* Description    : ������ ������ ��� ������ ������ ������ �ش�.
//*******************************************************************************/
//void Back_Read(uchar *MS_bit_data,uchar *TempBuffer,ushort length)
//{
//	ushort Temp_cnt;
//	ushort compare_cnt;
//
//	compare_cnt = length;
//		
//	/// always [2010/11/12] �ݴ�� ���ִ� ���� ���������� ������.
//	for(Temp_cnt=0;Temp_cnt<compare_cnt;Temp_cnt++)
//	{		
//		TempBuffer[Temp_cnt] = MS_bit_data[(compare_cnt-1)-Temp_cnt];
//	}
//	
//}
//
///*******************************************************************************
//* Function Name  : Back_Read(uchar *MS_bit_data,ushort length)
//* Description    : ������ ������ ��� ������ ������ ������ �ش�.
//*******************************************************************************/
//void Back_Time(ushort *MS_bit_data,ushort length)
//{
//	ushort Temp_cnt;
//	ushort compare_cnt;
//	ushort MS_Time_back[max_Bit_buffer_T13];
//	
//	compare_cnt = length;
//		
//	/// always [2010/11/12] �ݴ�� ���ִ� ���� ���������� ������.
//	for(Temp_cnt=0;Temp_cnt<compare_cnt;Temp_cnt++)
//	{		
//		
//		MS_Time_back[Temp_cnt] = MS_bit_data[(compare_cnt-1)-Temp_cnt];
//	}
//	memcpy(MS_bit_data,MS_Time_back,(length*sizeof(ushort)));
//}

/*******************************************************************************
* Function Name  : uchar Compare_Time_Dec(uchar MS_bit_data,uchar MS_bit_time_data,ushort length)
* Description    : Ÿ�̸��� �����Ϳ� ���ڴ��� ������ ��
*******************************************************************************/
uchar Compare_Time_Dec(uchar *d_status,uchar *t_status,uchar *MS_bit_data,uchar *MS_bit_time_data,uchar *d_length,uchar *t_length)
{
	/// always [2010/11/12] �ݴ�� ���ִ� ���� ���������� ������.

	if((*(d_status) != SUCCESS)&&(*(t_status) == SUCCESS))
	{
		*(d_status) = *(t_status);
		*(d_length) = *(t_length);
		memcpy(MS_bit_data,MS_bit_time_data,sizeof(uchar)*(*(t_length)));

		return ERROR;
	}
	/// always [2011/12/15] ���� �Ѵ� ������� Ÿ�̸��� ���������� ó��
	else if((*(d_status) != SUCCESS)&&(*(t_status) != SUCCESS))
	{
		*(d_status) = *(t_status);
	}

	return SUCCESS;
}

/*******************************************************************************
* Function Name  : Make_All_data(uchar stat_T1,uchar stat_T2,uchar stat_T3, u8 *MS_DATA_ALL)
* Description    : �� Ʈ�� ������ �ϳ��� ��ģ��.
*******************************************************************************/
uchar Make_All_data(uchar stat_T1,uchar stat_T2,uchar stat_T3, u8 *MS_DATA_ALL)
{	
	ushort data_cnt_temp ;
	#ifdef USE_Time_DBG
	ushort test_cnt;
	#endif
	
uchar MS_DATA_T1[DATA_MAX_T1];
uchar MS_DATA_T2[DATA_MAX_T2];
uchar MS_DATA_T3[DATA_MAX_T3];

	Byte_DATA_cnt_All = 0;

	memset(MS_DATA_T1,0,sizeof(MS_DATA_T1));//*******************************************130425hs:�ּ�ó��
	memset(MS_DATA_T2,0,sizeof(MS_DATA_T2));//*******************************************130425hs:�ּ�ó��
	memset(MS_DATA_T3,0,sizeof(MS_DATA_T3));//*******************************************130425hs:�ּ�ó��

	#ifndef Hanmega_USA
	if((Status_T1 != SUCCESS)&&(Status_T2 != SUCCESS)&&(Status_T3 != SUCCESS))
	{	
		Non_MS_DATA;
		return ERROR;
	}
	#endif
	
#ifdef T1
	if((stat_T1 == SUCCESS)&&(Byte_DATA_cnt_T1 > 1))
	{
		DLK_MemoryRead32(MS_DATA_T1, gMS_DATA_T1, sizeof(gMS_DATA_T1)); // jsshin 2015.10.12
		memcpy(MS_DATA_ALL,MS_DATA_T1,sizeof(uchar)*(Byte_DATA_cnt_T1-1));
		//memcpy(MS_DATA_T1_Temp,MS_DATA_T1,sizeof(uchar)*(Byte_DATA_cnt_T1-1));
		Byte_DATA_cnt_All = Byte_DATA_cnt_T1-1;
	}
	else
#endif
	{
		if(Status_T1 < 0x08)Status_T1 = READ_ERROR_BLANK;
		#ifdef Hanmega_USA
		/// 20150107 hyesun : ���� , �߰� ����
		MS_DATA_ALL[Byte_DATA_cnt_All++] = 'N';
		MS_DATA_ALL[Byte_DATA_cnt_All++] = ((Status_T1 & 0xf0)>>4)+0x30;
		MS_DATA_ALL[Byte_DATA_cnt_All++] = (Status_T1 & 0x0f)+0x30;	
		#else
		/// always [2010/11/12] Ʈ���� ���п��θ� �˻��Ͽ� �����ߴٸ� �ش� data�� �ڸ��� NAK ����
		MS_DATA_ALL[Byte_DATA_cnt_All++] = Status_T1;			
		#endif
	}
	
	/// always [2010/11/12] �� Ʈ���� ������ �߰��� 0x00���� Ȯ���Ѵ�.
	MS_DATA_ALL[Byte_DATA_cnt_All++] = 0x00;

#ifdef T2
	if((stat_T2 == SUCCESS)&&(Byte_DATA_cnt_T2 > 1))
	{
		DLK_MemoryRead32(MS_DATA_T2, gMS_DATA_T2, sizeof(gMS_DATA_T2)); // jsshin 2015.10.12
		for(data_cnt_temp = 0;data_cnt_temp < Byte_DATA_cnt_T2-1;data_cnt_temp++)
		{
			MS_DATA_ALL[Byte_DATA_cnt_All++] = MS_DATA_T2[data_cnt_temp];
			//MS_DATA_T2_Temp[Byte_DATA_cnt_T2++] = MS_DATA_T2[data_cnt_temp];
		}
	}
	else
#endif
	{
		if(Status_T2 < 0x08)Status_T2 = READ_ERROR_BLANK;
		#ifdef Hanmega_USA
		/// 20150107 hyesun : ���� , �߰� ����
		MS_DATA_ALL[Byte_DATA_cnt_All++] = 'N';
		MS_DATA_ALL[Byte_DATA_cnt_All++] = ((Status_T2 & 0xf0)>>4)+0x30;
		MS_DATA_ALL[Byte_DATA_cnt_All++] = (Status_T2 & 0x0f)+0x30;	
		#else
		MS_DATA_ALL[Byte_DATA_cnt_All++] = Status_T2;
		#endif
	}

	MS_DATA_ALL[Byte_DATA_cnt_All++] = 0x00;
	
#ifdef T3
	if((stat_T3 == SUCCESS)&&(Byte_DATA_cnt_T3 > 1))
	{
		DLK_MemoryRead32(MS_DATA_T3, gMS_DATA_T3, sizeof(gMS_DATA_T3)); // jsshin 2015.10.12
		for(data_cnt_temp = 0;data_cnt_temp < Byte_DATA_cnt_T3-1;data_cnt_temp++)
		{
			//Uart_DBG_Printf("%c ",MS_DATA_T3[data_cnt_temp] );
			MS_DATA_ALL[Byte_DATA_cnt_All++] = MS_DATA_T3[data_cnt_temp];
			//MS_DATA_T3_Temp[Byte_DATA_cnt_T3++] = MS_DATA_T3[data_cnt_temp];
		}
	}
	else
#endif
	{
		if(Status_T3 < 0x08)Status_T3 = READ_ERROR_BLANK;
		#ifdef Hanmega_USA
		/// 20150107 hyesun : ���� , �߰� ����
		MS_DATA_ALL[Byte_DATA_cnt_All++] = 'N';
		MS_DATA_ALL[Byte_DATA_cnt_All++] = ((Status_T3 & 0xf0)>>4)+0x30;
		MS_DATA_ALL[Byte_DATA_cnt_All++] = (Status_T3 & 0x0f)+0x30;	
		#else
		MS_DATA_ALL[Byte_DATA_cnt_All++] = Status_T3;
		#endif
	}
	
	#ifndef Hanmega_USA
	MS_DATA_ALL[Byte_DATA_cnt_All++] = 0x00;
	#endif

	#ifdef Hanmega_USA
	// 20150401 hyesun : ŰƮ�δн� �� ������ �κ� ����(Forward Read �� Sensor Satate ��) -> ī�� ���Խ� ī�尡 ������ ���� �Ŀ� IS MS DATA�� SET ��Ű���� ����
	if(MS_Read_IN == BACKWARD)IS_MS_DATA;
	else;// is_ms_data_forward = SET;// 20160720 : VT04����, �Ѹް� ��û
	#else
	/// always [2010/11/12] ��Ʈ���� ������ �ϳ��� �ϼ� �ž��ٸ�, ms data�� ���縦 �˸���.
	IS_MS_DATA;
	#endif

	#ifdef SamsungPay
	if(Status_T2 == SUCCESS)IS_MS_DATA;
	else 
	{
		Non_MS_DATA;
		return ERROR;
	}
	#endif
	
	
	#ifdef USE_Time_DBG
	Uart_DBG_Printf("MS_DATA_ALL =  \n ");
	for(test_cnt = 0; test_cnt < Byte_DATA_cnt_All ; test_cnt++)
	{
		Uart_DBG_Printf("%x ",MS_DATA_ALL[test_cnt] );
	}
	Uart_DBG_Printf("\n");
	#endif
	// stack clear!
	memset(MS_DATA_T1,0,sizeof(MS_DATA_T1));
	memset(MS_DATA_T2,0,sizeof(MS_DATA_T2));
	memset(MS_DATA_T3,0,sizeof(MS_DATA_T3));

	return SUCCESS;
	
}

/*******************************************************************************
* Function Name  : MS_Bit_DATA_R()
* Description    : Decoder�� ���� ������ bit ���� ����
*******************************************************************************/
void MS_Bit_DATA_R()
{
#ifdef T1
	uchar Bofore_T1;
	uchar Bofore_F2F_T1;
#endif
#ifdef T2
	uchar Bofore_T2;
	uchar Bofore_F2F_T2;
#endif
#ifdef T3
	uchar Bofore_T3;
	uchar Bofore_F2F_T3;
#endif

	ushort Tim_DATA_cnt_T1 = 0;
	ushort Tim_DATA_cnt_T2 = 0;
	ushort Tim_DATA_cnt_T3 = 0;

	ushort Bit_DATA_cnt_T1 = 0;
	ushort Bit_DATA_cnt_T2 = 0;
	ushort Bit_DATA_cnt_T3 = 0;

	ushort Bit_Time_cnt_T1 = 0;
	ushort Bit_Time_cnt_T2 = 0;
	ushort Bit_Time_cnt_T3 = 0;


	#ifdef	T1
	uchar MS_Bit_Temp_T1[max_Bit_buffer_T13 ];
	ushort MS_Time_T1[max_Bit_buffer_T13];
	uchar MS_DATA_TIME_T1[DATA_MAX_T1];
	uchar MS_DATA_T1[DATA_MAX_T1];
	#endif
	#ifdef	T2
	uchar MS_Bit_Temp_T2[max_Bit_buffer_T2];
	ushort MS_Time_T2[max_Bit_buffer_T2];
	uchar MS_DATA_TIME_T2[DATA_MAX_T2];
	uchar MS_DATA_T2[DATA_MAX_T2];
	#endif
	#ifdef	T3
	uchar MS_Bit_Temp_T3[max_Bit_buffer_T13];
	ushort MS_Time_T3[max_Bit_buffer_T13];
	uchar MS_DATA_TIME_T3[DATA_MAX_T3];
	uchar MS_DATA_T3[DATA_MAX_T3];
	#endif

	uchar MS_Bit_DATA[max_Bit_buffer_T13];
#if 0		//pbbch 171212 across warning	
	uchar ErrorState;
#endif

	uchar RetryCnt	= 0;
	uint Tout10ms	= 50000;//30000;
	uint TCnt		= 30000; // 20000�� check sensor �Լ� ���� �� �ɸ��� �ð� 500ms �ҿ� �Ǵ°����� �Ǵ� ��.
	//uint TCnt		= 300000; // delay_us(1) ���� 300ms wait
#if 1	//pbbch 180212  ms reading�� stop���½� retry �߰�.
	unsigned char ltimeout_retry_cnt=0;
#endif
#if 1		//pbbch 180305 timer overflow flg add
	#if 0		//pbbch 180306 reading rate�� ���̱� ���ؼ� ���� �ڵ� ���....�߰� �ڵ� ����.
	unsigned short lt1_over_flg_cnt1=0;
	unsigned short lt1_over_flg_cnt4=0;
	unsigned short lt1_over_flg_cnt5=0;
	#endif
	unsigned int lmax_loop_cnt=1;
#endif
	
	#ifdef USE_Time_DBG
	int test_cnt =0;
	#endif

	/// always [2010/11/3] port�� ��ȭ�� ���ϱ� ���� ��
#ifdef T1
	Bofore_T1 = MS_RCP_1_IN;
	Bofore_F2F_T1 = F2F_1_IN;
#endif
#ifdef T2
	Bofore_T2 = MS_RCP_2_IN;
	Bofore_F2F_T2 = F2F_2_IN;
#endif
#ifdef T3
	Bofore_T3 = MS_RCP_3_IN;
	Bofore_F2F_T3 = F2F_3_IN;
#endif

	
	Bit_DATA_cnt_T1 = 0;
	Bit_DATA_cnt_T2 = 0;
	Bit_DATA_cnt_T3 = 0;

	Bit_Time_cnt_T1=0;
	Bit_Time_cnt_T2=0;
	Bit_Time_cnt_T3=0;
	
	Tim_DATA_cnt_T1 = 0;
	Tim_DATA_cnt_T2 = 0;
	Tim_DATA_cnt_T3 = 0;

//	memset(MS_DATA_T1,0,sizeof(MS_DATA_T1));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
//	memset(MS_DATA_T2,0,sizeof(MS_DATA_T2));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
//	memset(MS_DATA_T3,0,sizeof(MS_DATA_T3));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
//	memset(MS_DATA_ALL,0,sizeof(MS_DATA_ALL));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�

// jsshin 2015.10.07 MS_Bit_DATA buffer clear
	memset(MS_Bit_DATA,0,sizeof(MS_Bit_DATA));//[20131107_hyesun]: ���� �ʱ�ȭ �߰�
#if 0		//pbbch 171212 across warning	
	ErrorState = ErrorFlag;
#endif	
	// jssin 2016.06.06
	// ī�� ��ȣ�� ���� �Ǹ� flag set �Ѵ�.
	// ī�尡 ���ԿϷ�(Front, Rear ��� SET)�Ǿ����� gMSDetect ���� ���� �Ǿ� ���� ������ ���� ������ ��� ���� ó��.
	gMSDetect = 1;

//1st ms read bit ��� ������ soft timeout �� 500ms��� �ּ� �� ��ŭ ��ٸ�. �ð��� ���� �ȵ�. 	
MS_BIT_RETRY_START:							//pbbch 180212 ms card reading�� stop�� retry �߰�...max 2sec  

	while(MS_nCLS_1_IN)
	{
		//delay_us(1); // jsshin 2016.03.01 track data ���� ī���� ��� Ÿ�Ӿƿ� �߻��Ѵ�.
		//#ifndef SamsungPay
		if(SamsungPayMode == RESET)
		{
			if(Check_Sensor_State(REAR_SEN))
			{
				if(MS_Read_IN == BACKWARD)
				{
				  	rCardIN_ON;
				}
				break;
			}
		}
		//#endif
		TCnt--; 
		if(TCnt == 0)
		{
			break;
		}
	}
	TIMER_5_EN; // Track 3
	TIMER_4_EN; // Track 1
	TIMER_1_EN; // Track 2
//2st ms read bit data ����

#if 1	//pbbch 180305 MS_nCLS_1_IN low�� ���� �ɶ� timeout�� �Ȱɸ��� case�� �־� ���� flg ���� ��.  
	while(++lmax_loop_cnt<MAX_R_LOOP_CNT)
#else
	while(1)
#endif		
	{
	#ifdef USE_Time
		/// always [2010/11/9] JITTER ������ ���� Ÿ�� �� ����
		#ifdef	T1
		if((Bofore_F2F_T1 !=F2F_1_IN) )
		{

			MS_Time_T1[Tim_DATA_cnt_T1++] = (ushort)TIMER_4_CNT;
			TIMER_4_RESET;
			Bofore_F2F_T1 = F2F_1_IN;
		}
		#endif
		#ifdef T2
		if((Bofore_F2F_T2 !=F2F_2_IN) )
		{

			MS_Time_T2[Tim_DATA_cnt_T2++] = (ushort)TIMER_1_CNT;
			TIMER_1_RESET;
			Bofore_F2F_T2 = F2F_2_IN;
		}
		#endif
		#ifdef T3
		if((Bofore_F2F_T3 !=F2F_3_IN) )
		{

			MS_Time_T3[Tim_DATA_cnt_T3++] = (ushort)TIMER_5_CNT;
			TIMER_5_RESET;
			Bofore_F2F_T3 = F2F_3_IN;
		}
		#endif
	#endif
		/// always [2010/11/9] DECODER���� ������ BIT�� ����
		#ifdef T1
		if(Bofore_T1!=MS_RCP_1_IN)
		{
			if(MS_RCP_1_IN == 0)
			{
				if(MS_MRD_1_IN == 0)
				{
					MS_Bit_DATA[Bit_DATA_cnt_T1++] ^=  Track_1;
				}
				else
				{
					MS_Bit_DATA[Bit_DATA_cnt_T1++] ^=  0x00;
				}
				// always [2010/11/3] ���������� ���� 0 �϶� 1�� 1�� �� 0�� ����
			}

			/// always [2010/11/3] ������ ���� �� ������ ����
			Bofore_T1 = MS_RCP_1_IN;
		}
		#endif
		#ifdef T2
		if(Bofore_T2!=MS_RCP_2_IN)
		{
			if(MS_RCP_2_IN == 0)
			{
				if(MS_MRD_2_IN == 0)
				{
					MS_Bit_DATA[Bit_DATA_cnt_T2++] ^=  Track_2;
				}
				else
				{
					MS_Bit_DATA[Bit_DATA_cnt_T2++] ^=  0x00;
				}
				/// always [2010/11/3] ���������� ���� 0 �϶� 1�� 1�� �� 0�� ����
				//MS_Bit_DATA[Bit_DATA_cnt_T2++] ^= ((uchar)MS_MRD_2_IN  << shift_1) ^ Track_2;
			}
			
			/// always [2010/11/3] ������ ���� �� ������ ����
			Bofore_T2 = MS_RCP_2_IN;
		}
		#endif
		#ifdef T3
		if(Bofore_T3!=MS_RCP_3_IN)
		{	
			if(MS_RCP_3_IN == 0)
			{
				if(MS_MRD_3_IN == 0)
				{
					MS_Bit_DATA[Bit_DATA_cnt_T3++] ^=  Track_3;
				}
				else
				{
					MS_Bit_DATA[Bit_DATA_cnt_T3++] ^=  0x00;
				}
				/// always [2010/11/3] ���������� ���� 0 �϶� 1�� 1�� �� 0�� ����
				//MS_Bit_DATA[Bit_DATA_cnt_T3++] ^= ((uchar)MS_MRD_3_IN << shift_2) ^ Track_3;
			}
			
			/// always [2010/11/3] ������ ���� �� ������ ����
			Bofore_T3 = MS_RCP_3_IN;
		}
		#endif
				
		/// always [2010/11/12] ms read�� ������ 3���� ���
                        
//////		#ifndef SamsungPay
//////		if(Check_Sensor_State(REAR_SEN) == SET)
//////		{
//////			break;
//////		}
//////		#endif
			
		if(MS_nCLS_1_IN)
		{
	#if 1	//pbbch 180212  ms reading�� stop���½� retry �߰�.
			if((Tim_DATA_cnt_T2> 5)&&(Tim_DATA_cnt_T2< 100))
			{
		#if 1	//pbbch 180515 3�� Ÿ�Ӿƿ� �߻����� retry 1count ����.
				if( (++ltimeout_retry_cnt < 3) &&(CheckFront == SET))		
		#else
				if( (++ltimeout_retry_cnt <= 3) &&(CheckFront == SET))		//pbbch 180212 backword�� ���� ..about respone time 2sec delay
		#endif			
				{
		
					TCnt		= 110000;
					#if 1		//pbbch 180305 timer disable code add
					lmax_loop_cnt=0;
					TIMER_5_DIS;
					TIMER_4_DIS;
					TIMER_1_DIS;
				//	TIMER_4_RESET;		//reset�� retry �õ��� ��� data�� ����
				//	TIMER_1_RESET;
				//	TIMER_5_RESET;
					#endif
					goto MS_BIT_RETRY_START;
				}
				else
				{
					CheckFront = RESET;
					asm("NOP");
					break; /// CLS ��ȣ deActivate!!!
				}
			}
			else
			{
				CheckFront = RESET;
				asm("NOP");
				break; /// CLS ��ȣ deActivate!!!
			}
	#else
			CheckFront = RESET;
			asm("NOP");
			break; /// CLS ��ȣ deActivate!!!
	#endif		
		}
		/// always [2010/12/2] time buffer over flow �� �ƹ� ó���� ����
		if((Tim_DATA_cnt_T1 >max_Bit_buffer_T13) ||(Tim_DATA_cnt_T2 >max_Bit_buffer_T2)||(Tim_DATA_cnt_T3 >max_Bit_buffer_T13))
		{
			CheckFront = RESET;
			return;	
		}

		/// always [2010/12/2] bit ������ over flow �� �ƹ� ó���� ����
		if((Bit_DATA_cnt_T1 >max_Bit_buffer_T13) ||(Bit_DATA_cnt_T2 >max_Bit_buffer_T2)||(Bit_DATA_cnt_T3 >max_Bit_buffer_T13))
		{
			CheckFront = RESET;
			return;	
		}

#if 0	 //pbbch 180306 reading rate�� ���̱� ���ؼ� ���� �ڵ� ���....�߰� �ڵ� ����. //pbbch 180305 MS_nCLS_1_IN low�� ���� �ɶ� timeout�� �Ȱɸ��� case�� �־� ���� flg ���� ��. 
		if(((u16)TIMER_1_CNT > Tout10ms))lt1_over_flg_cnt1=1;
		if(((u16)TIMER_4_CNT > Tout10ms))lt1_over_flg_cnt4=1;
		if(((u16)TIMER_5_CNT > Tout10ms))lt1_over_flg_cnt5=1;
		if(lt1_over_flg_cnt1 && lt1_over_flg_cnt4 && lt1_over_flg_cnt5)
		{
			TIMER_5_DIS;
			TIMER_4_DIS;
			TIMER_1_DIS;
			break;
		}
			
#else
		//pbbch 180212 time out calculation...it is time out when ms reading status..so not neet retry
		if(((u16)TIMER_5_CNT > Tout10ms)&&((u16)TIMER_1_CNT > Tout10ms)&&((u16)TIMER_4_CNT > Tout10ms))
		{
			TIMER_5_DIS;
			TIMER_4_DIS;
			TIMER_1_DIS;
			break;
		}
#endif		
	}
	// end while
  
	TIMER_5_DIS;
	TIMER_4_DIS;
	TIMER_1_DIS;
	TIMER_4_RESET;
	TIMER_1_RESET;
	TIMER_5_RESET;
	#if 1		//pbbch 180306 if max loop counter overflow, msr reset add
	if(lmax_loop_cnt>=MAX_R_LOOP_CNT) //660000
	{
		//Response('K');
		//The CIN low period must be greater than 100 micro-seconds. The minimal rise time requirement of CIN is 250 micro-seconds
		TR6201_RESET_ON;//TR6201 Reset
		delay_ms(20);
		TR6201_RESET_OFF;//TR6201 Reset
	}
	#endif
	if((Tim_DATA_cnt_T1 <30)&&(Tim_DATA_cnt_T2 <30)&&(Tim_DATA_cnt_T3 <30))
	{
		CheckFront = RESET;
		return;
	}
	
	//++++++++++++++++++++
	// jsshin 2015.10.07
	// Backward Read�϶� F2F interval time & RCP Bit ��ȣ ���� ������ �ش�.
	//++++++++++++++++++++
	if(MS_Read_IN == BACKWARD)		
	{
		u16 i,j;
	#ifdef USE_Time
		#ifdef	T1
		j = Tim_DATA_cnt_T1>>1; // divide 2
		for(i=0;i<j;i++)				
		{
			u16 t = MS_Time_T1[i];
			MS_Time_T1[i] = MS_Time_T1[Tim_DATA_cnt_T1-1-i];
			MS_Time_T1[Tim_DATA_cnt_T1-1-i] = t;
		}
		#endif
		#ifdef	T2
		j = Tim_DATA_cnt_T2>>1; // divide 2
		for(i=0;i<j;i++)				
		{
			u16 t = MS_Time_T2[i];
			MS_Time_T2[i] = MS_Time_T2[Tim_DATA_cnt_T2-1-i];
			MS_Time_T2[Tim_DATA_cnt_T2-1-i] = t;
		}
		#endif
		#ifdef	T3
		j = Tim_DATA_cnt_T3>>1; // divide 2
		for(i=0;i<j;i++)				
		{
			u16 t = MS_Time_T3[i];
			MS_Time_T3[i] = MS_Time_T3[Tim_DATA_cnt_T3-1-i];
			MS_Time_T3[Tim_DATA_cnt_T3-1-i] = t;
		}
		#endif
	#endif
		#ifdef	T1
		j = Bit_DATA_cnt_T1>>1; // divide 2
		for(i=0;i<j;i++)
		{
			u8 t = MS_Bit_DATA[i];
			MS_Bit_DATA[i] &= (~Track_1); // Track1 bit clear.
			MS_Bit_DATA[i] |= (MS_Bit_DATA[Bit_DATA_cnt_T1-1-i]&Track_1);
			
			MS_Bit_DATA[Bit_DATA_cnt_T1-1-i] &= (~Track_1); // Track1 bit clear.
			MS_Bit_DATA[Bit_DATA_cnt_T1-1-i] |= (t&Track_1);
		}
		#endif
		#ifdef	T2
		j = Bit_DATA_cnt_T2>>1; // divide 2
		for(i=0;i<j;i++)				
		{
			u8 t = MS_Bit_DATA[i];
			MS_Bit_DATA[i] &= (~Track_2); // Track2 bit clear.
			MS_Bit_DATA[i] |= (MS_Bit_DATA[Bit_DATA_cnt_T2-1-i]&Track_2);
			
			MS_Bit_DATA[Bit_DATA_cnt_T2-1-i] &= (~Track_2); // Track2 bit clear.
			MS_Bit_DATA[Bit_DATA_cnt_T2-1-i] |= (t&Track_2);
		}
		#endif
		#ifdef	T3
		j = Bit_DATA_cnt_T3>>1; // divide 2
		for(i=0;i<j;i++)				
		{
			u8 t = MS_Bit_DATA[i];
			MS_Bit_DATA[i] &= (~Track_3); // Track3 bit clear.
			MS_Bit_DATA[i] |= (MS_Bit_DATA[Bit_DATA_cnt_T3-1-i]&Track_3);
			
			MS_Bit_DATA[Bit_DATA_cnt_T3-1-i] &= (~Track_3); // Track3 bit clear.
			MS_Bit_DATA[Bit_DATA_cnt_T3-1-i] |= (t&Track_3);
		}
		#endif
	}
//---------------
	// jsshin : 2016.03.01
	// RCP data�� ���� �Ľ��Ͽ� ���������� �������� F2F������ �м��� ���� �ϵ��� ����!
	// RCP BIT �����ͷ� ISO ������ ����.
	#ifdef	T1
	Byte_DATA_cnt_T1 = 0;
	memset(MS_DATA_T1,0,sizeof(MS_DATA_T1));
	Status_T1 = MS_Bit_to_Byte(MS_Bit_DATA,MS_DATA_T1,&Byte_DATA_cnt_T1,Bit_DATA_cnt_T1,Track_1,RESET);
	#endif
	#ifdef	T2
	Byte_DATA_cnt_T2 = 0;
	memset(MS_DATA_T2,0,sizeof(MS_DATA_T2));
	Status_T2 = MS_Bit_to_Byte(MS_Bit_DATA,MS_DATA_T2,&Byte_DATA_cnt_T2,Bit_DATA_cnt_T2,Track_2,RESET);
	#endif
	#ifdef	T3
	Byte_DATA_cnt_T3 = 0;
	memset(MS_DATA_T3,0,sizeof(MS_DATA_T3));
	Status_T3 = MS_Bit_to_Byte(MS_Bit_DATA,MS_DATA_T3,&Byte_DATA_cnt_T3,Bit_DATA_cnt_T3,Track_3,RESET);
	#endif
//---------------


	/// always [2011/3/9] �ӵ� Ȯ��, 1ī��Ʈ �� 0.486us, Track 2 ��Ʈ�� ���� 0.3386mm/bit
	/// always [2011/3/9] Track 2 ���� �� 200���� 3386mm/s
#ifdef USE_Time_DBG
	Uart_DBG_Printf("\n MS_Time_T1 :  \n");
	for(test_cnt = 0; test_cnt < Tim_DATA_cnt_T1; test_cnt++)
	{
		Uart_DBG_Printf("%d,",MS_Time_T1[test_cnt] );
		if((test_cnt % 10) == 9)
		{
			Uart_DBG_Printf("\n");		
		}

	}
	Uart_DBG_Printf("\n MS_Time_T2 :  \n");
	for(test_cnt = 0; test_cnt < Tim_DATA_cnt_T2 ; test_cnt++)
	{
		Uart_DBG_Printf("%d,",MS_Time_T2[test_cnt] );
		if((test_cnt % 10) == 9)
		{
			Uart_DBG_Printf("\n");		
		}

	}
	Uart_DBG_Printf("\n MS_Time_T3 :  \n");
	for(test_cnt = 0; test_cnt < Tim_DATA_cnt_T3 ; test_cnt++)
	{
		Uart_DBG_Printf("%d,",MS_Time_T3[test_cnt] );
		if((test_cnt % 10) == 9)
		{
			Uart_DBG_Printf("\n");		
		}

	}
#endif
		#ifdef Hanmega_USA
		Status_T1 = READ_ERROR_BLANK;
		Status_T2 = READ_ERROR_BLANK;
		Status_T3 = READ_ERROR_BLANK;
		Status_ALL = 0;
		#endif
		
#ifdef MakeMS	
	if(MS_Read_IN == BACKWARD)				
	{
		#ifdef USE_Time_DBG
		Uart_DBG_Printf("backward  \n");
		#endif
		
		/// always [2010/11/16] ���� ������ ���� ���� �����Ͱ� �����ߴٸ� data�� ó������ �ʴ´�.
		#ifdef T1
		if((Status_T1 != SUCCESS)| (MS_DATA_IN == RESET))
		{
			Byte_DATA_cnt_TIME_T1 = 0;
			memset(MS_DATA_TIME_T1,0,sizeof(MS_DATA_TIME_T1));

			Bit_Time_cnt_T1 = chk_j(MS_Bit_Temp_T1,MS_Time_T1,Tim_DATA_cnt_T1,Track_1,5,5);
			
			//jsshin
			/// always [2010/11/16] ���� �� ���� �����͸� �ݴ� �������� �ٽ� �ٲ�� �Ѵ�.
			//Back_Read(MS_Bit_DATA,Tempbuffer_D,Bit_DATA_cnt_T1);
			// RCP BIT �����ͷ� ISO ������ ����.
			// 2016.03.01 �ڵ� ���κп��� RCP������ ���� �Ľ� �Ѵ�.
			//Status_T1 = MS_Bit_to_Byte(MS_Bit_DATA,MS_DATA_T1,&Byte_DATA_cnt_T1,Bit_DATA_cnt_T1,Track_1,RESET);

			//jsshin
			//Back_Read(MS_Bit_Temp_T1,Tempbuffer_T,Bit_Time_cnt_T1);
			// F2F�����͸� chk_j()�̿� �ؼ� �Ľ̵� BIT �����ͷ� ISO ������ ����.
			Status_TIME_T1 = MS_Bit_to_Byte(MS_Bit_Temp_T1,MS_DATA_TIME_T1,&Byte_DATA_cnt_TIME_T1,Bit_Time_cnt_T1,Track_1,SET);
			
			/// always [2011/12/13] ���� Read ���н� �����ֵ� ���� ���� �ϸ鼭 Bit ������ �� �õ� �Ѵ�.
			for(RetryCnt = 2; RetryCnt <= 12 ;RetryCnt+=2)
			{
				if(Status_TIME_T1 != SUCCESS)
				{
					Bit_Time_cnt_T1 = 0;
					memset(MS_Bit_Temp_T1,0,sizeof(MS_Bit_Temp_T1));
				
					Byte_DATA_cnt_TIME_T1 = 0;
					memset(MS_DATA_TIME_T1,0,sizeof(MS_DATA_TIME_T1));
					MS_Read_IN ^= 1;
					Bit_Time_cnt_T1 = chk_j(MS_Bit_Temp_T1,MS_Time_T1,Tim_DATA_cnt_T1,Track_1,RetryCnt,RetryCnt/2);
					//jsshin
					//Back_Read(MS_Bit_Temp_T1,Tempbuffer_T,Bit_Time_cnt_T1);
					Status_TIME_T1 = MS_Bit_to_Byte(MS_Bit_Temp_T1,MS_DATA_TIME_T1,&Byte_DATA_cnt_TIME_T1,Bit_Time_cnt_T1,Track_1,SET);
					MS_Read_IN ^= 1;
				}
				else
				{
					break;
				}

				if(Status_TIME_T1 != SUCCESS)
				{
					Bit_Time_cnt_T1 = 0;
					memset(MS_Bit_Temp_T1,0,sizeof(MS_Bit_Temp_T1));
							
					Byte_DATA_cnt_TIME_T1 = 0;
					memset(MS_DATA_TIME_T1,0,sizeof(MS_DATA_TIME_T1));
	
					//MS_Read_IN ^= 1;
					Bit_Time_cnt_T1 = chk_j(MS_Bit_Temp_T1,MS_Time_T1,Tim_DATA_cnt_T1,Track_1,RetryCnt,RetryCnt/2);
					//jsshin
					//Back_Read(MS_Bit_Temp_T1,Tempbuffer_T,Bit_Time_cnt_T1);
					Status_TIME_T1 = MS_Bit_to_Byte(MS_Bit_Temp_T1,MS_DATA_TIME_T1,&Byte_DATA_cnt_TIME_T1,Bit_Time_cnt_T1,Track_1,SET);
					//MS_Read_IN ^= 1;
				}
				else
				{
					break;
				}
			}
			Compare_Time_Dec(&Status_T1,&Status_TIME_T1, MS_DATA_T1,MS_DATA_TIME_T1,&Byte_DATA_cnt_T1,&Byte_DATA_cnt_TIME_T1);
		}
		#endif
		#ifdef T2
		if((Status_T2 != SUCCESS)| (MS_DATA_IN == RESET))
		{
			Byte_DATA_cnt_TIME_T2 = 0;
			memset(MS_DATA_TIME_T2,0,sizeof(MS_DATA_TIME_T2));

			Bit_Time_cnt_T2 = chk_j(MS_Bit_Temp_T2,MS_Time_T2,Tim_DATA_cnt_T2,Track_2,5,5);

			/// always [2010/11/16] ���� �� ���� �����͸� �ݴ� �������� �ٽ� �ٲ�� �Ѵ�.
			//jsshin
			//Back_Read(MS_Bit_DATA,Tempbuffer_D,Bit_DATA_cnt_T2);
			// 2016.03.01 RCP ���� �� �Ľ�ó�� �Ѵ�.
			//Status_T2 = MS_Bit_to_Byte(MS_Bit_DATA,MS_DATA_T2,&Byte_DATA_cnt_T2,Bit_DATA_cnt_T2,Track_2,RESET);

			//jshsin
			//Back_Read(MS_Bit_Temp_T2,Tempbuffer_T,Bit_Time_cnt_T2);
			Status_TIME_T2 = MS_Bit_to_Byte(MS_Bit_Temp_T2,MS_DATA_TIME_T2,&Byte_DATA_cnt_TIME_T2,Bit_Time_cnt_T2,Track_2,SET);
			
			for(RetryCnt = 2; RetryCnt <= 12 ;RetryCnt+=2)
			{
				if(Status_TIME_T2 != SUCCESS)
				{
					Bit_Time_cnt_T2 = 0;
					memset(MS_Bit_Temp_T2,0,sizeof(MS_Bit_Temp_T2));
			
					Byte_DATA_cnt_TIME_T2 = 0;
					memset(MS_DATA_TIME_T2,0,sizeof(MS_DATA_TIME_T2));
					MS_Read_IN ^= 1;
					Bit_Time_cnt_T2 = chk_j(MS_Bit_Temp_T2,MS_Time_T2,Tim_DATA_cnt_T2,Track_2,RetryCnt,RetryCnt/2);
					//jsshin
					//Back_Read(MS_Bit_Temp_T2,Tempbuffer_T,Bit_Time_cnt_T2);
					Status_TIME_T2 = MS_Bit_to_Byte(MS_Bit_Temp_T2,MS_DATA_TIME_T2,&Byte_DATA_cnt_TIME_T2,Bit_Time_cnt_T2,Track_2,SET);
					MS_Read_IN ^= 1;
				}
				else
				{
					break;
				}
			
				if(Status_TIME_T2 != SUCCESS)
				{
					Bit_Time_cnt_T2 = 0;
					memset(MS_Bit_Temp_T2,0,sizeof(MS_Bit_Temp_T2));
			
					Byte_DATA_cnt_TIME_T2 = 0;
					memset(MS_DATA_TIME_T2,0,sizeof(MS_DATA_TIME_T2));
	
					//MS_Read_IN ^= 1;
					Bit_Time_cnt_T2 = chk_j(MS_Bit_Temp_T2,MS_Time_T2,Tim_DATA_cnt_T2,Track_2,RetryCnt,RetryCnt/2);
					//jsshin
					//Back_Read(MS_Bit_Temp_T2,Tempbuffer_T,Bit_Time_cnt_T2);
					Status_TIME_T2 = MS_Bit_to_Byte(MS_Bit_Temp_T2,MS_DATA_TIME_T2,&Byte_DATA_cnt_TIME_T2,Bit_Time_cnt_T2,Track_2,SET);
					//MS_Read_IN ^= 1;
				}
				else
				{
					break;
				}
			}
			Compare_Time_Dec(&Status_T2,&Status_TIME_T2, MS_DATA_T2,MS_DATA_TIME_T2,&Byte_DATA_cnt_T2,&Byte_DATA_cnt_TIME_T2);
		}
		#endif
		#ifdef T3
		if((Status_T3 != SUCCESS) | (MS_DATA_IN == RESET))
		{
			Byte_DATA_cnt_TIME_T3 = 0;
			memset(MS_DATA_TIME_T3,0,sizeof(MS_DATA_TIME_T3));

			Bit_Time_cnt_T3 = chk_j(MS_Bit_Temp_T3,MS_Time_T3,Tim_DATA_cnt_T3,Track_3,5,5);

			/// always [2010/11/16] ���� �� ���� �����͸� �ݴ� �������� �ٽ� �ٲ�� �Ѵ�.
			//jsshin
			//Back_Read(MS_Bit_DATA,Tempbuffer_D,Bit_DATA_cnt_T3);
			// 2016.03.01 RCP ���� �� �Ľ�ó�� �Ѵ�.
			//Status_T3 = MS_Bit_to_Byte(MS_Bit_DATA,MS_DATA_T3,&Byte_DATA_cnt_T3,Bit_DATA_cnt_T3,Track_3,RESET);
			//jsshin
			//Back_Read(MS_Bit_Temp_T3,Tempbuffer_T,Bit_Time_cnt_T3);
			Status_TIME_T3 = MS_Bit_to_Byte(MS_Bit_Temp_T3,MS_DATA_TIME_T3,&Byte_DATA_cnt_TIME_T3,Bit_Time_cnt_T3,Track_3,SET);
			
			for(RetryCnt = 2; RetryCnt <= 12 ;RetryCnt+=2)
			{
				if(Status_TIME_T3 != SUCCESS)
				{
					Bit_Time_cnt_T3 = 0;
					memset(MS_Bit_Temp_T3,0,sizeof(MS_Bit_Temp_T3));
					
					Byte_DATA_cnt_TIME_T3 = 0;
					memset(MS_DATA_TIME_T3,0,sizeof(MS_DATA_TIME_T3));
					
					MS_Read_IN ^= 1;
					Bit_Time_cnt_T3 = chk_j(MS_Bit_Temp_T3,MS_Time_T3,Tim_DATA_cnt_T3,Track_3,RetryCnt,RetryCnt/2);
					//jsshin
					//Back_Read(MS_Bit_Temp_T3,Tempbuffer_T,Bit_Time_cnt_T3);
					Status_TIME_T3 = MS_Bit_to_Byte(MS_Bit_Temp_T3,MS_DATA_TIME_T3,&Byte_DATA_cnt_TIME_T3,Bit_Time_cnt_T3,Track_3,SET);
					MS_Read_IN ^= 1;
				}
				else
				{
					break;
				}

				if(Status_TIME_T3 != SUCCESS)
				{
					Bit_Time_cnt_T3 = 0;
					memset(MS_Bit_Temp_T3,0,sizeof(MS_Bit_Temp_T3));
					
					Byte_DATA_cnt_TIME_T3 = 0;
					memset(MS_DATA_TIME_T3,0,sizeof(MS_DATA_TIME_T3));
					
					//MS_Read_IN ^= 1;
					Bit_Time_cnt_T3 = chk_j(MS_Bit_Temp_T3,MS_Time_T3,Tim_DATA_cnt_T3,Track_3,RetryCnt,RetryCnt/2);
					//jsshin
					//Back_Read(MS_Bit_Temp_T3,Tempbuffer_T,Bit_Time_cnt_T3);
					Status_TIME_T3 = MS_Bit_to_Byte(MS_Bit_Temp_T3,MS_DATA_TIME_T3,&Byte_DATA_cnt_TIME_T3,Bit_Time_cnt_T3,Track_3,SET);
					//MS_Read_IN ^= 1;
				}
				else
				{
					break;
				}
			}
			Compare_Time_Dec(&Status_T3,&Status_TIME_T3, MS_DATA_T3,MS_DATA_TIME_T3,&Byte_DATA_cnt_T3,&Byte_DATA_cnt_TIME_T3);
		}
		#endif

		#ifdef USE_Time_DBG		
		Uart_DBG_Printf("Status    = %d \t%d \t%d \n",Status_T1,Status_T2,Status_T3);
		Uart_DBG_Printf("Status_T = %d \t%d \t%d \n",Status_TIME_T1,Status_TIME_T2,Status_TIME_T3 );
		#endif
#ifdef Hanmega_USA
		IS_MS_DATA;
#else
		// jsshin 2015.10.12
		// All track read ��� ���� ��, �� track �����͸� �����Ͽ� �����ϵ��� ����.
		//Status_ALL = Make_All_data(Status_T1,Status_T2,Status_T3, MS_DATA_ALL );
		if((Status_T1 != SUCCESS)&&(Status_T2 != SUCCESS)&&(Status_T3 != SUCCESS))
		{
			Non_MS_DATA;
		} else IS_MS_DATA;
#endif

		//if(HW_TYPE == NEW_TYPE)
		//{
#ifdef Hanmega_USA
		// 2016.02.26 : �Ѹް� �����⿡�� �Ʒ� ���� ���� ��û. Ÿ ����Ʈ ���� ��!!!
		Front_Detect_Non;
		MS_F_Read_ON;
#endif
		//CheckFront = RESET;// 20160921 hyesun : �Ѹް� ������ �̻� �̽����� �������� ����, �ϴ� ��ƾ�� �߰�
		//}
	}
	else
	{ // forward Read
		#ifdef USE_Time_DBG
		Uart_DBG_Printf("forward \n" );
		#endif

#ifdef T1
		if((Status_T1 != SUCCESS)| (MS_DATA_IN == RESET))
		{
			Byte_DATA_cnt_TIME_T1 = 0;
			memset(MS_DATA_TIME_T1,0,sizeof(MS_DATA_TIME_T1));
			
			Bit_Time_cnt_T1 = chk_j(MS_Bit_Temp_T1,MS_Time_T1,Tim_DATA_cnt_T1,Track_1,5,5);
			
			//jsshin
			//Back_Read(MS_Bit_DATA,Tempbuffer_D,Bit_DATA_cnt_T1);
			// RCP BIT �����ͷ� ISO ������ ����.
			// 2016.03.01 �ڵ� ���κп��� RCP������ ���� �Ľ� �Ѵ�.
			//Status_T1 = MS_Bit_to_Byte(MS_Bit_DATA,MS_DATA_T1,&Byte_DATA_cnt_T1,Bit_DATA_cnt_T1,Track_1,RESET);
			
			//Back_Read(MS_Bit_Temp_T1,Tempbuffer_T,Bit_Time_cnt_T1);
			Status_TIME_T1 = MS_Bit_to_Byte(MS_Bit_Temp_T1,MS_DATA_TIME_T1,&Byte_DATA_cnt_TIME_T1,Bit_Time_cnt_T1,Track_1,SET);
			
			for(RetryCnt = 2; RetryCnt <= 12 ;RetryCnt+=2)
			{
				if(Status_TIME_T1 != SUCCESS)
				{
					Bit_Time_cnt_T1 = 0;
					memset(MS_Bit_Temp_T1,0,sizeof(MS_Bit_Temp_T1));
					Byte_DATA_cnt_TIME_T1 = 0;
					memset(MS_DATA_TIME_T1,0,sizeof(MS_DATA_TIME_T1));
					
					MS_Read_IN ^= 1;
					Bit_Time_cnt_T1 = chk_j(MS_Bit_Temp_T1,MS_Time_T1,Tim_DATA_cnt_T1,Track_1,RetryCnt,RetryCnt/2);
					Status_TIME_T1 = MS_Bit_to_Byte(MS_Bit_Temp_T1,MS_DATA_TIME_T1,&Byte_DATA_cnt_TIME_T1,Bit_Time_cnt_T1,Track_1,SET);
					MS_Read_IN ^= 1;
				}
				else
				{
					break;
				}
			
				if(Status_TIME_T1 != SUCCESS)
				{
					Bit_Time_cnt_T1 = 0;
					memset(MS_Bit_Temp_T1,0,sizeof(MS_Bit_Temp_T1));
					Byte_DATA_cnt_TIME_T1 = 0;
					memset(MS_DATA_TIME_T1,0,sizeof(MS_DATA_TIME_T1));
		
					//MS_Read_IN ^= 1;
					Bit_Time_cnt_T1 = chk_j(MS_Bit_Temp_T1,MS_Time_T1,Tim_DATA_cnt_T1,Track_1,RetryCnt,RetryCnt/2);
					Status_TIME_T1 = MS_Bit_to_Byte(MS_Bit_Temp_T1,MS_DATA_TIME_T1,&Byte_DATA_cnt_TIME_T1,Bit_Time_cnt_T1,Track_1,SET);
					//MS_Read_IN ^= 1;
				}
				else
				{
					break;
				}
			}
			Compare_Time_Dec(&Status_T1,&Status_TIME_T1, MS_DATA_T1,MS_DATA_TIME_T1,&Byte_DATA_cnt_T1,&Byte_DATA_cnt_TIME_T1);
					
			#ifdef USE_Time_DBG
			Uart_DBG_Printf("ms data by decoder 1=  \n ");
			for(test_cnt = 0; test_cnt < Bit_DATA_cnt_T1 ; test_cnt++)
			{
				Uart_DBG_Printf("%d,",((MS_Bit_DATA[test_cnt] >> 0) & 0x01));
			}
			Uart_DBG_Printf(" \n ");

			Uart_DBG_Printf("ms data by time data 1=  \n ");
			for(test_cnt = 0; test_cnt < Bit_Time_cnt_T1 ; test_cnt++)
			{
				Uart_DBG_Printf("[%d]%d,",test_cnt,MS_Bit_Temp_T1[test_cnt] );
			}
			Uart_DBG_Printf(" \n ");
			Uart_DBG_Printf("MS_DATA_T1 =  \n ");
			for(test_cnt = 0; test_cnt < Byte_DATA_cnt_T1 ; test_cnt++)
			{
				Uart_DBG_Printf("[%d]%x,",test_cnt,MS_DATA_T1[test_cnt] );
			}
			Uart_DBG_Printf(" \n ");

			Uart_DBG_Printf("MS_DATA_TIME_T1 =  \n ");
			for(test_cnt = 0; test_cnt < Byte_DATA_cnt_TIME_T1 ; test_cnt++)
			{
				Uart_DBG_Printf("[%d]%x \t",test_cnt,MS_DATA_TIME_T1[test_cnt] );
			}
			Uart_DBG_Printf(" \n ");
			#endif
		}
#endif
#ifdef T2
		if((Status_T2 != SUCCESS)| (MS_DATA_IN == RESET))
		{
			Byte_DATA_cnt_TIME_T2 = 0;
			memset(MS_DATA_TIME_T2,0,sizeof(MS_DATA_TIME_T2));
			
			Bit_Time_cnt_T2 = chk_j(MS_Bit_Temp_T2,MS_Time_T2,Tim_DATA_cnt_T2,Track_2,5,5);

			//Back_Read(MS_Bit_DATA,Tempbuffer_D,Bit_DATA_cnt_T2);
			// RCP BIT �����ͷ� ISO ������ ����.
			// 2016.03.01 �ڵ� ���κп��� RCP������ ���� �Ľ� �Ѵ�.
			//Status_T2 = MS_Bit_to_Byte(MS_Bit_DATA,MS_DATA_T2,&Byte_DATA_cnt_T2,Bit_DATA_cnt_T2,Track_2,RESET);
			
			//Back_Read(MS_Bit_Temp_T2,Tempbuffer_T,Bit_Time_cnt_T2);
			Status_TIME_T2 = MS_Bit_to_Byte(MS_Bit_Temp_T2,MS_DATA_TIME_T2,&Byte_DATA_cnt_TIME_T2,Bit_Time_cnt_T2,Track_2,SET);
			
			for(RetryCnt = 2; RetryCnt <= 12 ;RetryCnt+=2)
			{
				if(Status_TIME_T2 != SUCCESS)
				{
					Bit_Time_cnt_T2 = 0;
					memset(MS_Bit_Temp_T2,0,sizeof(MS_Bit_Temp_T2));
					Byte_DATA_cnt_TIME_T2 = 0;
					memset(MS_DATA_TIME_T2,0,sizeof(MS_DATA_TIME_T2));	
			
					MS_Read_IN ^= 1;
					Bit_Time_cnt_T2 = chk_j(MS_Bit_Temp_T2,MS_Time_T2,Tim_DATA_cnt_T2,Track_2,RetryCnt,RetryCnt/2);
					Status_TIME_T2 = MS_Bit_to_Byte(MS_Bit_Temp_T2,MS_DATA_TIME_T2,&Byte_DATA_cnt_TIME_T2,Bit_Time_cnt_T2,Track_2,SET);
					MS_Read_IN ^= 1;
				}
				else
				{
					break;
				}
				
				if(Status_TIME_T2 != SUCCESS)
				{
					Bit_Time_cnt_T2 = 0;
					memset(MS_Bit_Temp_T2,0,sizeof(MS_Bit_Temp_T2));
					Byte_DATA_cnt_TIME_T2 = 0;
					memset(MS_DATA_TIME_T2,0,sizeof(MS_DATA_TIME_T2));	
			
					//MS_Read_IN ^= 1;
					Bit_Time_cnt_T2 = chk_j(MS_Bit_Temp_T2,MS_Time_T2,Tim_DATA_cnt_T2,Track_2,RetryCnt,RetryCnt/2);
					Status_TIME_T2 = MS_Bit_to_Byte(MS_Bit_Temp_T2,MS_DATA_TIME_T2,&Byte_DATA_cnt_TIME_T2,Bit_Time_cnt_T2,Track_2,SET);
					//MS_Read_IN ^= 1;
				}
				else
				{
					break;
				}
			}
			Compare_Time_Dec(&Status_T2,&Status_TIME_T2, MS_DATA_T2,MS_DATA_TIME_T2,&Byte_DATA_cnt_T2,&Byte_DATA_cnt_TIME_T2);
				
			#ifdef USE_Time_DBG
			Uart_DBG_Printf("ms data by decoder 2=  \n ");
			for(test_cnt = 0; test_cnt < Bit_DATA_cnt_T2 ; test_cnt++)
			{
				Uart_DBG_Printf("%d,",((MS_Bit_DATA[test_cnt] >> 1) & 0x01));
			}
			Uart_DBG_Printf(" \n ");

			Uart_DBG_Printf("ms data by time data 2=  \n ");
			for(test_cnt = 0; test_cnt < Bit_Time_cnt_T2 ; test_cnt++)
			{
				Uart_DBG_Printf("[%d]%d,",test_cnt,MS_Bit_Temp_T2[test_cnt] );
			}
			Uart_DBG_Printf(" \n ");
			
			Uart_DBG_Printf("MS_DATA_T2 =  \n ");
			for(test_cnt = 0; test_cnt < Byte_DATA_cnt_T2 ; test_cnt++)
			{
				Uart_DBG_Printf("[%d]%x,",test_cnt,MS_DATA_T2[test_cnt] );
			}
			Uart_DBG_Printf(" \n ");

			Uart_DBG_Printf("MS_DATA_TIME_T2 =  \n ");
			for(test_cnt = 0; test_cnt < Byte_DATA_cnt_TIME_T2 ; test_cnt++)
			{
				Uart_DBG_Printf("[%d]%x \t",test_cnt,MS_DATA_TIME_T2[test_cnt] );
			}
			Uart_DBG_Printf(" \n ");
			#endif
		}
#endif
#ifdef T3
		if((Status_T3 != SUCCESS)| (MS_DATA_IN == RESET))
		{
			Byte_DATA_cnt_TIME_T3 = 0;
			memset(MS_DATA_TIME_T3,0,sizeof(MS_DATA_TIME_T3));
			
			Bit_Time_cnt_T3 = chk_j(MS_Bit_Temp_T3,MS_Time_T3,Tim_DATA_cnt_T3,Track_3,5,5);
			
			//Back_Read(MS_Bit_DATA,Tempbuffer_D,Bit_DATA_cnt_T3);
			// RCP BIT �����ͷ� ISO ������ ����.
			// 2016.03.01 �ڵ� ���κп��� RCP������ ���� �Ľ� �Ѵ�.
			//Status_T3 = MS_Bit_to_Byte(MS_Bit_DATA,MS_DATA_T3,&Byte_DATA_cnt_T3,Bit_DATA_cnt_T3,Track_3,RESET);
			
			//Back_Read(MS_Bit_Temp_T3,Tempbuffer_T,Bit_Time_cnt_T3);
			Status_TIME_T3 = MS_Bit_to_Byte(MS_Bit_Temp_T3,MS_DATA_TIME_T3,&Byte_DATA_cnt_TIME_T3,Bit_Time_cnt_T3,Track_3,SET);
			
			for(RetryCnt = 2; RetryCnt <= 12 ;RetryCnt+=2)
			{
				if(Status_TIME_T3 != SUCCESS)
				{
					Bit_Time_cnt_T3 = 0;
					memset(MS_Bit_Temp_T3,0,sizeof(MS_Bit_Temp_T3));
				
					Byte_DATA_cnt_TIME_T3 = 0;
					memset(MS_DATA_TIME_T3,0,sizeof(MS_DATA_TIME_T3));
				
					MS_Read_IN ^= 1;
					Bit_Time_cnt_T3 = chk_j(MS_Bit_Temp_T3,MS_Time_T3,Tim_DATA_cnt_T3,Track_3,RetryCnt,RetryCnt/2);
					Status_TIME_T3 = MS_Bit_to_Byte(MS_Bit_Temp_T3,MS_DATA_TIME_T3,&Byte_DATA_cnt_TIME_T3,Bit_Time_cnt_T3,Track_3,SET);
					MS_Read_IN ^= 1;
				}
				else
				{
					break;
				}
				

				if(Status_TIME_T3 != SUCCESS)
				{
					Bit_Time_cnt_T3 = 0;
					memset(MS_Bit_Temp_T3,0,sizeof(MS_Bit_Temp_T3));
					Byte_DATA_cnt_TIME_T3 = 0;
					memset(MS_DATA_TIME_T3,0,sizeof(MS_DATA_TIME_T3));
			
					//MS_Read_IN ^= 1;
					Bit_Time_cnt_T3 = chk_j(MS_Bit_Temp_T3,MS_Time_T3,Tim_DATA_cnt_T3,Track_3,RetryCnt,RetryCnt/2);
					Status_TIME_T3 = MS_Bit_to_Byte(MS_Bit_Temp_T3,MS_DATA_TIME_T3,&Byte_DATA_cnt_TIME_T3,Bit_Time_cnt_T3,Track_3,SET);
					//MS_Read_IN ^= 1;
				}
				else
				{
					break;
				}
			}
			Compare_Time_Dec(&Status_T3,&Status_TIME_T3, MS_DATA_T3,MS_DATA_TIME_T3,&Byte_DATA_cnt_T3,&Byte_DATA_cnt_TIME_T3);
				
			#ifdef USE_Time_DBG
			Uart_DBG_Printf("ms data by decoder =  \n ");
			for(test_cnt = 0; test_cnt < Bit_DATA_cnt_T3 ; test_cnt++)
			{
				Uart_DBG_Printf("%d,",((MS_Bit_DATA[test_cnt] >> 2) & 0x01));
			}
			Uart_DBG_Printf(" \n ");

			Uart_DBG_Printf("ms data by time data =  \n ");
			for(test_cnt = 0; test_cnt < Bit_Time_cnt_T3 ; test_cnt++)
			{
				Uart_DBG_Printf("[%d]%d,",test_cnt,MS_Bit_Temp_T3[test_cnt] );
			}
			Uart_DBG_Printf(" \n ");

			Uart_DBG_Printf("MS_DATA_T3 =  \n ");
			for(test_cnt = 0; test_cnt < Byte_DATA_cnt_T3 ; test_cnt++)
			{
				Uart_DBG_Printf("[%d]%x,",test_cnt,MS_DATA_T3[test_cnt] );
			}
			Uart_DBG_Printf(" \n ");

			Uart_DBG_Printf("MS_DATA_TIME_T3 =  \n ");
			for(test_cnt = 0; test_cnt < Byte_DATA_cnt_TIME_T3 ; test_cnt++)
			{
				Uart_DBG_Printf("[%d]%x \t",test_cnt,MS_DATA_TIME_T3[test_cnt] );
			}
			Uart_DBG_Printf(" \n ");				
			#endif
		}
#endif
				
				
		#ifdef USE_Time_DBG
		Uart_DBG_Printf("Status    = %d \t%d \t%d \n ",Status_T1,Status_T2,Status_T3);
		Uart_DBG_Printf("Status_T = %d \t%d \t%d \n",Status_TIME_T1,Status_TIME_T2,Status_TIME_T3 );
		#endif
		
#ifdef Hanmega_USA
			//is_ms_data_forward = SET;// 20160720 : VT04����, �Ѹް� ��û
#else
		// jsshin 2015.10.12
		// All track read ��� ���� ��, �� track �����͸� �����Ͽ� �����ϵ��� ����.
		//Status_ALL = Make_All_data(Status_T1,Status_T2,Status_T3, MS_DATA_ALL );
		if((Status_T1 != SUCCESS)&&(Status_T2 != SUCCESS)&&(Status_T3 != SUCCESS))
		{
//			//++ jsshin 2016.06.01
//			if(ms_retry) {
//				ms_retry = 0;
//				MS_R_Read_ON;
//				goto bw_read_retry; // jsshin 2016.06.01 fw �б� ���� �ÿ��� bw�� �ٽ� �ѹ� �б� �õ�.
//			}
//			//-- jsshin 2016.06.01			
			Non_MS_DATA; // MS Data All Track Read Err
		} else IS_MS_DATA; // MS������ ���� Read, Track 1���� �о����� ���� �ȴ�.
#endif
	}
	CheckFront = RESET;// 20160921 hyesun : �Ѹް� ������ �̻� �̽����� ����
	//Green_ON();
	//AllLedOff;// 20140529 hyesun : LED control Ŀ��� �߰��ϸ鼭 ������
	//SysTick_CounterCmd(SysTick_Counter_Disable);

//++ jsshin
// 2015.10.12
//stack clear!
#ifdef T1
// save to gMS_DATA_T1
	memset(gMS_DATA_T1, 0x00, sizeof(gMS_DATA_T1));
	if(Status_T1 == SUCCESS) DLK_MemoryWrite32(gMS_DATA_T1, MS_DATA_T1, sizeof(gMS_DATA_T1));
	memset(MS_Bit_Temp_T1, 0, sizeof(MS_Bit_Temp_T1));
	memset(MS_Time_T1, 0, sizeof(MS_Time_T1));
	memset(MS_DATA_TIME_T1, 0, sizeof(MS_DATA_TIME_T1));
	memset(MS_DATA_T1, 0, sizeof(MS_DATA_T1));
#endif
#ifdef T2
// save to gMS_DATA_T2
	memset(gMS_DATA_T2, 0x00, sizeof(gMS_DATA_T2));
	if(Status_T2 == SUCCESS) DLK_MemoryWrite32(gMS_DATA_T2, MS_DATA_T2, sizeof(gMS_DATA_T2));
	memset(MS_Bit_Temp_T2, 0, sizeof(MS_Bit_Temp_T2));
	memset(MS_Time_T2, 0, sizeof(MS_Time_T2));
	memset(MS_DATA_TIME_T2, 0, sizeof(MS_DATA_TIME_T2));
	memset(MS_DATA_T2, 0, sizeof(MS_DATA_T2));
#endif
#ifdef T3
// save to gMS_DATA_T3
	memset(gMS_DATA_T3, 0x00, sizeof(gMS_DATA_T3));
	if(Status_T3 == SUCCESS) DLK_MemoryWrite32(gMS_DATA_T3, MS_DATA_T3, sizeof(gMS_DATA_T3));
	memset(MS_Bit_Temp_T3, 0, sizeof(MS_Bit_Temp_T3));
	memset(MS_Time_T3, 0, sizeof(MS_Time_T3));
	memset(MS_DATA_TIME_T3, 0, sizeof(MS_DATA_TIME_T3));
	memset(MS_DATA_T3, 0, sizeof(MS_DATA_T3));
#endif
memset(MS_Bit_DATA, 0, sizeof(MS_Bit_DATA));
//-- jsshin

#endif //MakeMS
	
}


/*******************************************************************************
* Function Name  : ushort FindETX(uchar *MS_bit_data,ushort ErrorLeng,ushort length, uchar shift_num,uchar full_bit,uchar TrackETX)
* Description    : ETX�� parity bit ��ġ�� ����
* Date	: 20110209
*******************************************************************************/
#ifdef USE_MAKE_BYTE
ushort FindETX(uchar *MS_bit_data,ushort ErrorLeng,ushort length, uchar shift_num,uchar full_bit,uchar TrackETX,uchar NullNum,uchar Track_bit)
{
	ushort BitCnt;
	ushort TestCnt;
	uchar TempData;
	uchar ETXTemp = 0;
	uchar AvailableETX;
	uchar StatETX;
	ushort FirstHigh =0;
	uchar TempETX;
	//uchar TempParity;
	//uchar StatNextByte;

	//TempParity = 0;

	TempETX = 0x1f;

	full_bit |= 0x10;
	/// always [2011/2/17] LRC���� 1�� ����ǰ� Ʈ���� ���Ǵ� Bit ���� 2�� �ȿ��� ETX�� ���� �Ǿ�� �Ѵ�.
	AvailableETX=(Track_bit+1)*4;
	StatETX=RESET;
	//StatNextByte = RESET;
	#ifdef USE_Time_DBG
	Uart_DBG_Printf("ErrorLeng = %d, length = %d,Track_bit = %d, NullNum = %d  \n ",ErrorLeng,length,Track_bit,NullNum);
	#endif
	/// always [2011/2/7] ETX�� ã�Ƽ� �ش� ��ư �ѹ��� ����
	for(BitCnt = length-1;BitCnt > ErrorLeng+1;BitCnt--)
	{

		if(((MS_bit_data[BitCnt] >> shift_num)& 0x01)&&(StatETX==RESET))
		{
			StatETX = SET;
			/// always [2011/2/22] ù ��° �߰ߵ� 1�� ���� ��Ų��. �̴� ������ LRC�� 1�̱� �����̴�.
			BitCnt--;
		}
		#ifdef USE_Time_DBG
		Uart_DBG_Printf("MS_bit_data[%d] = %x  \n ",BitCnt,MS_bit_data[BitCnt]);
		#endif
		
		if(StatETX == SET)
		{
			FirstHigh = BitCnt;
//					Uart_DBG_Printf("FirstHigh = %x  \n ",FirstHigh);
			while(AvailableETX--)
			{
				ETXTemp |= ((MS_bit_data[BitCnt] >> shift_num)& 0x01);
				//Uart_DBG_Printf("1BitCnt = %d\n ",BitCnt);
				#ifdef USE_Time_DBG
				Uart_DBG_Printf("MS_bit_data[%d] = %x  \n ",BitCnt,MS_bit_data[BitCnt]);
				Uart_DBG_Printf("ETXTemp = %x  \n ",ETXTemp);
					#endif

				/// always [2011/2/7] ETX�� �и� ��ȣ��ġ�� �����Ѵ�.
				if((ETXTemp & TempETX ) == TempETX)
				{	
					TempData = 0;
					for(TestCnt=1; TestCnt < (Track_bit+2); TestCnt++)
					{
						TempData |= MS_bit_data[BitCnt-TestCnt];
						#ifdef USE_Time_DBG
						Uart_DBG_Printf("MS_bit_data[%d] = %x  \n ",BitCnt-TestCnt,MS_bit_data[BitCnt-TestCnt]);
						Uart_DBG_Printf("TempData1 = %x  \n ",TempData);
						#endif
						if(TestCnt == (Track_bit+1))
						{
							break;
						}
						TempData <<= 1;
							
					}

					if((TempData & full_bit) == TempETX)
					{
						//BitCnt--;
						//Uart_DBG_Printf("2BitCnt = %d\n ",BitCnt);
						//StatNextByte = RESET;
						BitCnt += (Track_bit-1);	
						//BitCnt--;
						Uart_DBG_Printf("3BitCnt = %d\n ",BitCnt);
						TempData = 0;
						ETXTemp = 0;
						//AvailableETX = (Track_bit+1)*2;
					}
					else
					{
						#ifdef USE_Time_DBG
						Uart_DBG_Printf("TempData3 = %x  \n ",TempData);
						Uart_DBG_Printf("TrackETX = %x  \n ",TrackETX);
						Uart_DBG_Printf("MS_bit_data[%d] = %x  \n ",BitCnt+Track_bit,MS_bit_data[BitCnt+Track_bit]);
						#endif
						return BitCnt+Track_bit;
					}
				}
				else
				{
					/// always [2010/11/11] etx�� ���� �� ���� ���� �� ĭ�� shift �Ѵ�.
					BitCnt--;
					//Uart_DBG_Printf("4BitCnt = %d\n ",BitCnt);
					ETXTemp <<= 1;
				}	

				if(BitCnt < ErrorLeng+1)
				{
					return ERROR;
				}
			}
			/// always [2011/2/17] Null �߰��� 1�� ���̴��� �ٽ� �ѹ� ETX�� ã�� �� �ְ� ��
			if(FirstHigh > (length - NullNum))
			{
				BitCnt = FirstHigh;
				StatETX = RESET;
			}
			else
			{
				break;
			}
		}
	}
	return ERROR;
}


/*******************************************************************************
* Function Name  : uchar RearByte(ushort ErrorLeng,uchar Track_bit,ushort *ByteNum,ushort *BitNum,ushort BitCnt)
* Description    : �߻��� Parity Error �������� ETX�� ������ ��Ʈ�� ������ ������, Ʈ���� ����Ʈ ���� ������ ���� ���� Byte��
			��Ʈ�� �������� ���� ��Ʈ ������ ������ �����ϱ� ���� ������ ��Ʈ�� �����Ѵ�.
* Date	: 20110209
*******************************************************************************/
uchar RearByte(ushort ErrorLeng,uchar Track_bit,ushort *ByteNum,ushort *BitNum,ushort BitCnt)
{
	
	ushort RemainBit = 0;

	uchar BitNumError = 0xff;

//	*BitCnt = FindETX(MS_bit_data,ErrorLeng,length,shift_num,full_bit,TrackETX);

	if(BitCnt)
	{
		/// always [2011/2/10] ������ Bit�� �������� ������ �������� ����� Bit�� ���Ѵ�.
		RemainBit = BitCnt - ErrorLeng;
		/// always [2011/2/10] ���� ��Ʈ �������� ���� ��Ʈ�� �߰��ǰų� ���ڸ� ��츦 ����� ��������Ʈ���� ���Ѵ�.
		*BitNum = RemainBit % (Track_bit + 1);
		/// always [2011/2/10] ���� ��ġ���� ETX�� ������ ��ġ������ ����Ʈ ������ ���Ѵ�.
		*ByteNum = RemainBit / (Track_bit +1);

		return SUCCESS;
	}
	
	return ERROR;
}
/*******************************************************************************
* Function Name  : uchar MakeLRC(uchar *MS_bit_data,ushort ErrorLeng,ushort length, uchar shift_num,
								uchar full_bit,uchar TrackETX,uchar Track_bit,ushort *BitCnt,uchar *DataLRC)
* Description    : LRC ��ȣ�� ã��, LRC�� �и��� Ȯ���Ѵ�.
* Date	: 20110209
*******************************************************************************/
uchar MakeLRC(uchar *MS_bit_data,ushort ErrorLeng,ushort length, uchar shift_num,uchar full_bit,uchar TrackETX,uchar Track_bit,ushort *BitCnt,uchar *DataLRC,uchar NullNum)
{
	
	uchar TrackBitTemp=0;
	uchar LRCTemp = 0;
	uchar RetryLRC = 4;
	uchar frontData = 0;;
	ushort ByteNum=0;
	ushort BitNum=0;
	uchar StatSucce;
	ushort SucceBitCnt;
	uchar SucceLRC;

	SucceLRC =0 ;
	SucceBitCnt = 0;
	StatSucce = RESET;
	TrackBitTemp = Track_bit;
	
	/// always [2011/2/17] ETX�� Parity Bit ��ġ
	*BitCnt = FindETX(MS_bit_data,ErrorLeng,length,shift_num,full_bit,TrackETX,NullNum,Track_bit);

	

	if(*BitCnt == ERROR)
	{
		//Uart_DBG_Printf("NO ETX ErrorLeng =  %d  \n ",ErrorLeng);
		return ERROR;
	}

	while(RetryLRC)
	{
		/// always [2011/2/22] LRC �����
		while(TrackBitTemp)
		{
			LRCTemp |= ((MS_bit_data[(*BitCnt+TrackBitTemp)] >> shift_num)& 0x01);
//					Uart_DBG_Printf("LRCTemp = %x, LRC Bit Cnt = %d  \n ",LRCTemp,(*BitCnt+TrackBitTemp));
			TrackBitTemp--;

			if(TrackBitTemp == 0)
			{
				TrackBitTemp = Track_bit;
				break;
			}

			LRCTemp <<= 1;// (LRCTemp << 1) & full_bit;
		}
		/// always [2011/2/22] ETX �� �� DataȮ�� ��
		while(TrackBitTemp)
		{
			frontData |= ((MS_bit_data[(*BitCnt-(TrackBitTemp+1))] >> shift_num)& 0x01);
//					Uart_DBG_Printf("LRCTemp = %x, LRC Bit Cnt = %d  \n ",LRCTemp,(*BitCnt+TrackBitTemp));
			TrackBitTemp--;

			if(TrackBitTemp == 0)
			{
				break;
			}

			frontData <<= 1;// (LRCTemp << 1) & full_bit;
		}
		#ifdef USE_Time_DBG
		Uart_DBG_Printf("LRCTemp = %x, LRC Bit Cnt = %d  \n ",LRCTemp,*BitCnt);
		#endif
		if(!((MS_bit_data[*BitCnt+Track_bit+1] >> shift_num)& 0x01) ^ Check_parity(LRCTemp,Track_bit))
		{
			/// always [2011/2/10] �и� ��Ʈ�� 1����, ETX �� ��Ʈ�� 1����, LRC�� ù ��Ʈ�� 1���� Ȯ��
			if(!(((MS_bit_data[*BitCnt+Track_bit+1] >> shift_num)& 0x01) == 1)&&(((MS_bit_data[*BitCnt-(Track_bit+1)] >> shift_num)& 0x01) == 1))
			{
				#ifdef USE_Time_DBG
				Uart_DBG_Printf("parity = %d, *BitCnt-(Track_bit+1) = %d  \n ",*BitCnt+Track_bit+1,*BitCnt-(Track_bit+1));
				#endif
				(*BitCnt)--;
				LRCTemp =0;
				TrackBitTemp = Track_bit;
				RetryLRC--;
			}
			else
			{
				#ifdef USE_Time_DBG
				Uart_DBG_Printf("Next parity = %d, *BitCnt-(Track_bit+1) = %d  \n ",*BitCnt+Track_bit+1,*BitCnt-(Track_bit+1));
				#endif
				break;
			}
		}
		else
		{
//					Uart_DBG_Printf("LRC Bit Cnt = %d  \n ",*BitCnt);
			
			RearByte(ErrorLeng,Track_bit,&ByteNum,&BitNum,*BitCnt);
			#ifdef USE_Time_DBG
			Uart_DBG_Printf("ETX Bit Cnt = %d,ByteNum = %d, BitNum = %d,  \n ",*BitCnt,ByteNum,BitNum);		
			#endif
			/// always [2011/2/10] �и� ��ȣ�� �´� �ϴ��� LRC�� ���� ��ȣ�� 1�� ��쿡�� �� �� ��LRC�� �и��� Ȯ���Ѵ�.
			if((((MS_bit_data[*BitCnt-(Track_bit+1)] >> shift_num)& 0x01) == 1)&&(BitNum >=1)&&(BitNum <Track_bit -1))
			{
				/// always [2011/2/23] ��õ��� �����Ѵٸ� �� ���� ����
				StatSucce = SET;
				SucceBitCnt = (*BitCnt);
				SucceLRC = LRCTemp;
				
				//Uart_DBG_Printf("*BitCnt1  = %d  \n ",*BitCnt );
				(*BitCnt)--;
				//Uart_DBG_Printf("*BitCnt2  = %d  \n ",*BitCnt );
				LRCTemp =0;
				TrackBitTemp = Track_bit;
				RetryLRC--;
			}
			/// always [2011/2/22] ���� LRC�� ETX�� ���� ���� ���� ���(2,3 Ʈ���� ����)
			else if((BitNum == 4)&&(LRCTemp == 0x01)&&(frontData == 0x0f)&&(Track_bit == 4))
			{
				//Uart_DBG_Printf("frontData = %x, *BitCnt  = %d,Track_bit = %d  \n ",frontData,*BitCnt ,Track_bit);

				/// always [2011/2/23] ��õ��� �����Ѵٸ� �� ���� ����
				StatSucce = SET;
				SucceBitCnt = (*BitCnt);
				SucceLRC = LRCTemp;
				
				*BitCnt -= 4;
				//Uart_DBG_Printf("*BitCnt3  = %d  \n ",*BitCnt );
				LRCTemp =0;
				TrackBitTemp = Track_bit;
				RetryLRC--;
			}
			else
			{
				//Uart_DBG_Printf("LRC Bit Cnt = %d,LRCTemp = %x  \n ",*BitCnt,LRCTemp);

				*DataLRC = LRCTemp;
				return SUCCESS;
			}
		}
	}
	if(StatSucce == SET)
	{
		(*BitCnt) = SucceBitCnt;
		*DataLRC = SucceLRC;
		return SUCCESS;
	}
	return ERROR;
}

/*******************************************************************************
* Function Name  : uchar MakeErrorByte(uchar *MS_bit_data,ushort *ErrorLeng,ushort length,uchar shift_num,
					uchar full_bit,uchar TrackETX,uchar PreLRC,uchar Track_bit,uchar *MS_byte_data,uchar *Byte_data_cnt)
* Description    : �߻��� Parity Error �������� ETX�� LRC�� ��ġ�� �ľ��Ͽ� �� ����, ������ ����Ʈ�� �����Ѵ�.

* Date	: 20110209
*******************************************************************************/
uchar MakeErrorByte(uchar *MS_bit_data,ushort *ErrorLeng,ushort length,uchar shift_num,uchar full_bit,uchar TrackETX,uchar PreLRC,uchar Track_bit,uchar *MS_byte_data,uchar *Byte_data_cnt,uchar NullNum)
{
	
	ushort RearByteNum = 0;
	ushort BitCnt;

	ushort ByteCnt;

	static ushort FullByte;

	uchar StatMakeNum;
	uchar StatMakeLRC;

	uchar TrackBitTemp;

	uchar ByteDataTemp = 0;
	uchar DataLRC;

	ushort OverBitNum;
	ushort ErrorByteNum;
	uchar LRCParityERROR = RESET;

	ushort EndBit;

	ushort test_cnt;

	/// always [2011/2/10] ������ ��ġ�� ����Ʈ ��ġ ������ ������.
	ErrorByteNum = *Byte_data_cnt ;
	/// always [2011/2/10] ��Ʈ�� ���� ����Ʈ ���� ���ϱ� ���� Ʈ���� ��Ʈ�� ����
	TrackBitTemp = Track_bit;

	/// always [2011/2/10] LRC���� ���Ѵ�.
	StatMakeLRC = MakeLRC(MS_bit_data,*ErrorLeng,length,shift_num, full_bit,TrackETX,Track_bit,&BitCnt,&DataLRC,NullNum);
	/// always [2011/2/10] ���� BitCnt�� ETX�� �и� ��ȣ�� ��ġ �ϹǷ� LRC�� �и� ��ȣ ��ġ�� EndBit�� �����Ѵ�.
	EndBit = BitCnt +Track_bit+ 1;

	//Uart_DBG_Printf("bit cnt = %d, Byte_data_cnt= %d,FullByte = %d \n ",BitCnt,*Byte_data_cnt,FullByte);
	/// always [2011/2/9] parity���� �߻� �� ������ �ѹ��� �����ϹǷ�, �� ������ STX���� LRC������ DATA�� ����� �д�.
	if(StatMakeLRC == ERROR)
	{
		return ERROR;
	}
	
	/// always [2011/2/10] ������ BitCnt���� ���� ���� �ִ� Byte�� ���� ��Ʈ�� �������� ÷�� ����� Ȯ���Ѵ�.
	StatMakeNum = RearByte(*ErrorLeng,Track_bit,&RearByteNum,&OverBitNum,BitCnt);
	
	if(StatMakeNum == ERROR)
	{
		return ERROR;
	}

	//if((OverBitNum == 1)||(OverBitNum == 0))
	if((OverBitNum == 0))
	{
		FullByte = *Byte_data_cnt + RearByteNum;
		//Uart_DBG_Printf("1bit cnt = %d, Byte_data_cnt= %d,FullByte = %d \n ",BitCnt,*Byte_data_cnt,FullByte);
	}
//	else if(OverBitNum >= 4)
//	{
//		FullByte = *Byte_data_cnt + RearByteNum +1;
//		Uart_DBG_Printf("2bit cnt = %d, Byte_data_cnt= %d,FullByte = %d \n ",BitCnt,*Byte_data_cnt,FullByte);
//	}
	else
	{
		return ERROR;
	}

	/// always [2011/2/10] ETX���� Data �����Ѵ�.
	*Byte_data_cnt = FullByte+1;

	/// always [2011/2/10] ETX�� �ٽ��ѹ� Ȯ���ϱ� ���� BitCnt ��
	BitCnt -= (Track_bit+1);
	
	//Uart_DBG_Printf("3bit cnt = %d, Byte_data_cnt= %d,FullByte = %d \n ",BitCnt,*Byte_data_cnt,FullByte);
	if(StatMakeNum == SUCCESS)
	{
		/// always [2011/2/10] ������ ������ ����Ʈ �� �ٷ� ������ ETX���� �����Ͽ� ������ DATA�� �ۼ��� ������.
		for( ;FullByte > ErrorByteNum;)
		{
			while(TrackBitTemp)
			{
				ByteDataTemp |= ((MS_bit_data[(BitCnt+TrackBitTemp--)] >> shift_num)& 0x01);

				/// always [2011/2/10] 4�� �ݺ��ϰ� ����Ʈ �ϱ� ���� ������.
				if(TrackBitTemp == 0)
				{
					break;
				}
		
				ByteDataTemp = (ByteDataTemp << 1) & full_bit;
			}
			/// always [2011/2/15] bitcnt = ���� ���ڵ� �и� ��ȣ ��ġ
			//Uart_DBG_Printf("shift = %d , bit cnt = %d, ByteDataTemp= %x ,FullByte = %d\n ",shift_num,BitCnt,ByteDataTemp,FullByte);
			/// always [2011/2/10] ������ ���� �и� ��ȣ �˻�
			if(!((MS_bit_data[BitCnt+Track_bit+1] >> shift_num)& 0x01) ^ Check_parity(ByteDataTemp,Track_bit))
			{
				#ifdef USE_Time_DBG
				Uart_DBG_Printf("Make Error READ_ERROR_PARITY  \n ");
				Uart_DBG_Printf("shift = %d , bit cnt = %d, ByteDataTemp= %x \n ",shift_num,BitCnt,ByteDataTemp);
				Uart_DBG_Printf("READ_ERROR_DATA =   \n ");
				for(test_cnt = EndBit; test_cnt >= BitCnt; test_cnt--)
				{
					Uart_DBG_Printf("%d,%d,  ",test_cnt,((MS_bit_data[test_cnt] >> shift_num)& 0x01));
					if(!(test_cnt%5))
					{
						Uart_DBG_Printf(" \n ");
					}
				}
				Uart_DBG_Printf(" \n ");
				

				Uart_DBG_Printf(" FullByte= %d \n ",FullByte);
				#endif
				return ERROR;
			}
			
			/// always [2011/2/10] ������ ������ ������ XOR�ϸ鼭 ���� �����Ѵ�.
			DataLRC ^= ByteDataTemp;
			#ifdef USE_Time_DBG
			Uart_DBG_Printf("ByteDataTemp = %x, DataLRC = %x, BitCnt= %d \n ",ByteDataTemp,DataLRC,BitCnt);
			#endif
			/// always [2011/2/10] ������ ���� ETX���� ���� ������ ä�� ������.
			MS_byte_data[FullByte--] = ByteDataTemp;
			
			/// always [2011/2/10] ���� ������ bit���� �ʱ� ��ġ
			BitCnt -= (Track_bit+1);

			/// always [2011/2/10] ����Ʈ �ռ��� ���� �ӽð��� �ʱ�ȭ
			ByteDataTemp = 0;
			TrackBitTemp = Track_bit;

		}
		

		/// always [2011/2/10] ���� �߻� ������ ������ LRC�� �� ������ LRC�� ���� ������ Data�� ���Ѵ�.
		*ErrorLeng = EndBit;
		MS_byte_data[FullByte] = PreLRC ^ DataLRC;
		#ifdef USE_Time_DBG
		Uart_DBG_Printf("EndBit = %d, MS_byte_data[%d] = %x \n ",EndBit,FullByte,MS_byte_data[FullByte]);
		#endif
		//*EndNum = FullByte;
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}
#endif //end of USE_MAKE_BYTE
/*******************************************************************************
* Function Name  : Check_LRC(uchar *MS_byte_data,ushort length)
* Description    : ���� ����Ʈ ������ lrc üũ
*******************************************************************************/
uchar Check_LRC(uchar *MS_byte_data,uchar length,uchar stx)
{

	uchar LRC;

	uchar length_cnt;
	LRC = stx;

	for(length_cnt = 0;length_cnt < length ; length_cnt++ )
	{
		LRC ^= MS_byte_data[length_cnt];
	}
	//Uart_DBG_Printf("MS_byte_data[%d] = %c, LRC  = %x \n",length,MS_byte_data[length_cnt],LRC);
	
	if(LRC != MS_byte_data[length])
	{
		return READ_ERROR_LRC;
	}
	return RESET;
}

/*******************************************************************************
* Function Name  : PostAmbleCheck(uchar *MS_bit_data, uchar bit_temp_cnt,uchar ShiftNum,uchar FullBit,uchar Track_bit)
* Description    : Bit Data�� post amble ������ Ȯ���Ѵ�.
* Date	: 20110209
*******************************************************************************/
uchar PostAmbleCheck(uchar *MS_bit_data, ushort bit_temp_cnt,uchar ShiftNum,uchar FullBit,uchar Track_bit)
{
	uchar Track_bit_temp= 0;
	uchar preamble_temp = 0;
	#ifdef USE_Time_DBG
	ushort test_cnt;
	#endif
	Track_bit_temp = Track_bit;
	
	//Uart_DBG_Printf("Post AMble shift = %d , bit cnt = %d, FullBit = %x, Track_bit = %x \n ",ShiftNum,bit_temp_cnt,FullBit,Track_bit);
	
	while(Track_bit_temp)
	{
		preamble_temp |= ((MS_bit_data[(bit_temp_cnt+Track_bit_temp+1)] >> ShiftNum)& 0x01);

		Track_bit_temp--;

		if(Track_bit_temp == 0)
		{
			break;
		}
		
		preamble_temp =  (preamble_temp << 1) & FullBit;

	}
	if(preamble_temp  != 0x00)
	{
		if(preamble_temp  != FullBit)
		{
			#ifdef USE_Time_DBG
			Uart_DBG_Printf("READ_ERROR_POST  \n ");
			Uart_DBG_Printf("shift = %d , bit cnt = %d \n ",ShiftNum,bit_temp_cnt);
			Uart_DBG_Printf("READ_ERROR_DATA =   \n ");
			for(test_cnt = 0; test_cnt < (bit_temp_cnt+Track_bit+1); test_cnt++)
			{
				Uart_DBG_Printf("%d,%d,  ",test_cnt,((MS_bit_data[test_cnt] >> ShiftNum)& 0x01));
				if(!(test_cnt%10))
				{
					Uart_DBG_Printf(" \n ");
				}
			}
			Uart_DBG_Printf(" \n ");
			#endif
			
			return READ_ERROR_POST;
		}
	}

	return SUCCESS;
}
/*******************************************************************************
* Function Name  : TransASC(uchar *MS_byte_data,uchar *Byte_data_cnt, uchar TransAsc)
* Description    : ������ MS�� DATA�� �ƽ�Ű ��ȣȭ �Ѵ�.
* Date	: 20110209
*******************************************************************************/
void TransASC(uchar *MS_byte_data,uchar *Byte_data_cnt, uchar TransAsc)
{
	uchar trans_cnt;
	
	for(trans_cnt = 0 ; trans_cnt <(*Byte_data_cnt) ; trans_cnt++)
	{
		MS_byte_data[trans_cnt] += TransAsc;
		//Uart_DBG_Printf("MS_byte_data[%d] = %c, %x \n",trans_cnt,MS_byte_data[trans_cnt],MS_byte_data[trans_cnt]);
	}
}
/*******************************************************************************
* Function Name  : Check_parity(uchar MS_byte_data,ushort length)
* Description    : ����Ʈ�� parity ��ȣ üũ
*******************************************************************************/
uchar Check_parity(uchar MS_byte_data,ushort length)
{
	uchar  parity_cnt=0,Parity_temp = 0;
	
	for(parity_cnt=0;parity_cnt<length;parity_cnt++)
	{
		Parity_temp ^=(MS_byte_data>> parity_cnt)&0x01;
	}	
	
	return (Parity_temp & 0x01);
}




/*************************************************************************************************************
* Function Name  : MS_Bit_to_Byte(uchar *MS_bit_data,uchar *MS_byte_data,ushort *Byte_data_cnt,ushort length,uchar Track_bit)
* Description    : ms bit data �� byte data�� ��ȯ �Ͽ� Ʈ���� ������ ����
			MS_bit_data : bit data ���� ��
					MS_Bit_DATA
			MS_byte_data : ��ȯ �� byte data ����
					MS_DATA_T1
					MS_DATA_T2
					MS_DATA_T3
			Byte_data_cnt : Ʈ���� ����Ÿ ����
					Byte_DATA_cnt_T1
					Byte_DATA_cnt_T2
					Byte_DATA_cnt_T3
			length : bit data ����
					Bit_DATA_cnt_T1
					Bit_DATA_cnt_T2
					Bit_DATA_cnt_T3
			Track_bit : track�� ���� byte�� ���� bit ��
					T1_bit_num
					T23_bit_num		
			shift_num : Ʈ���� ���� bit ������ ��ġ ����
					shift_1
					shift_2
					shift_3
			stx : �� Ʈ�� �� stx	
					ISO1_STX
					ISO23_STX
			etx : �� Ʈ�� �� etx	
					ISO1_ETX
					ISO23_ETX
**************************************************************************************************************/
uchar MS_Bit_to_Byte(uchar *MS_bit_data,uchar *MS_byte_data,uchar *Byte_data_cnt,ushort length,uchar track,uchar time_use)
{
	uchar full_bit;
	uchar Track_bit;
	uchar back_stx;
	uchar stx;
	uchar etx;
	uchar trans_asc;
	
	ushort bit_temp_cnt;
	
	uchar STX_Temp;
	uchar Track_bit_temp;
	//uchar StatPostAmle;
	uchar shift_num;
	uchar LRCTemp;
	//uchar NullNum;
	
	#ifdef USE_Time_DBG
	int test_cnt = 0;
	#endif

	if( track == Track_1)
	{
		shift_num = shift_0;			
		Track_bit = T1_bit_num;
		Track_bit_temp = Track_bit;
		back_stx = ISO1_back_STX;
		stx = ISO1_STX;
		etx = ISO1_ETX;
		full_bit = 0x3f;
		trans_asc = 0x20;	
		//NullNum = 20;
	}
	else if( track == Track_2)
	{
		if(time_use == SET)
		{
			shift_num = shift_0;
		}
		else
		{
			shift_num = shift_1;
		}
		Track_bit = T23_bit_num;
		Track_bit_temp = Track_bit;
		back_stx = ISO23_back_STX;
		stx = ISO23_STX;
		etx = ISO23_ETX;
		full_bit = 0x0f;
		trans_asc = 0x30;	
		//NullNum = 10;

	}
	else if( track == Track_3)
	{
		if(time_use == SET)
		{
			shift_num = shift_0;
		}
		else
		{
			shift_num = shift_2;
		}
		Track_bit = T23_bit_num;
		Track_bit_temp = Track_bit;
		back_stx = ISO23_back_STX;
		stx = ISO23_STX;
		etx = ISO23_ETX;
		full_bit = 0x0f;
		trans_asc = 0x30;	
		//NullNum =20;
	}
	
	STX_Temp = 0;
	bit_temp_cnt = 0;
	LRCTemp = stx;
	
	/// always [2010/11/11] �ʱ� ������ ��ũ ����
	if(length < 10)
	{
		#ifdef USE_Time_DBG
		Uart_DBG_Printf("READ_ERROR_BLANK  \n ");
		#endif
		return READ_ERROR_BLANK;
	}

	//while(!((MS_bit_data[bit_temp_cnt++] >> shift_num)& 0x01));
	
	/// always [2010/11/11] stx�� ���� �ɶ����� ��Ʈ�� shift �ϸ鼭 Ȯ��
	for(;bit_temp_cnt < length;)
	{

		/// always [2010/11/11] �ϳ��� bit �����Ϳ� 0���� Ʈ��1, 1���� Ʈ��2, 2���� Ʈ��3 �̹Ƿ� ���ϴ� Ʈ���� �°� ���� �����Ѵ�.
		STX_Temp |= ((MS_bit_data[bit_temp_cnt++] >> shift_num)& 0x01);

		/// always [2010/11/11] stx�� break
		if((STX_Temp & full_bit) == back_stx)
		{
			break;
		}
		else
		{
			/// always [2010/11/11] stx�� ���� �� ���� ���� �� ĭ�� shift �Ѵ�.
			STX_Temp <<= 1;
		}			
	}
	/// always [2010/11/11] stx�� parity �˻�
	if(!((MS_bit_data[bit_temp_cnt] >> shift_num)& 0x01) ^ Check_parity(STX_Temp,Track_bit))
	{
		#ifdef USE_Time_DBG
		Uart_DBG_Printf("READ_ERROR_PARITY  \n ");
		Uart_DBG_Printf("shift = %d , bit cnt = %d \n ",shift_num,bit_temp_cnt);
		Uart_DBG_Printf("READ_ERROR_DATA =   \n ");
		for(test_cnt = 0; test_cnt < bit_temp_cnt; test_cnt++)
		{
			Uart_DBG_Printf("%d,%d,  ",test_cnt,((MS_bit_data[test_cnt] >> shift_num)& 0x01));
			if(!(test_cnt%10))
			{
				Uart_DBG_Printf(" \n ");
			}
		}
		Uart_DBG_Printf(" \n ");
		
		#endif
		//return READ_ERROR_PARITY;
		return READ_ERROR_PRE;
	}
	
	/// always [2010/11/11] stx�� ���� �ɶ������� ���̸� Ȯ���Ͽ� ���� ����
	if(bit_temp_cnt > (length/2))
	{
		#ifdef USE_Time_DBG
		Uart_DBG_Printf("READ_ERROR_PRE  \n ");
		Uart_DBG_Printf("shift = %d , bit cnt = %d \n ",shift_num,bit_temp_cnt);
		Uart_DBG_Printf("READ_ERROR_DATA =   \n ");
		for(test_cnt = 0; test_cnt < bit_temp_cnt; test_cnt++)
		{
			Uart_DBG_Printf("%d,%d,  ",test_cnt,((MS_bit_data[test_cnt] >> shift_num)& 0x01));
			if(!(test_cnt%10))
			{
				Uart_DBG_Printf(" \n ");
			}
		}
		
		Uart_DBG_Printf(" \n ");
		#endif
		return READ_ERROR_PRE;
	}
	
	#ifdef USE_Time_DBG
	Uart_DBG_Printf("End STX shift = %d , bit cnt = %d \n ",shift_num,bit_temp_cnt);
	#endif				
	/// always [2010/11/11] ��Ʈ�����͸� ����Ʈ �����ͷ� ��ȯ�Ͽ� ����
	for(;bit_temp_cnt < length;)
	{
		while(Track_bit_temp)
		{
			//Uart_DBG_Printf("Track_bit_temp = %d , bit cnt = %d,  \n ",Track_bit_temp,bit_temp_cnt);
			
			/// always [2010/11/12] ��Ʈ������ bit�� ��ġ�� �ٸ��� ������ Ʈ���� �°� ����Ʈ ����� �Ѵ�.
			MS_byte_data[*Byte_data_cnt] |= ((MS_bit_data[(bit_temp_cnt+Track_bit_temp)] >> shift_num)& 0x01);
			#ifdef USE_Time_DBG
			Uart_DBG_Printf("%d, ",MS_bit_data[(bit_temp_cnt+Track_bit_temp)]);
			#endif
			Track_bit_temp--;
			if(Track_bit_temp == 0)
			{
				//Uart_DBG_Printf("Track_bit_temp = %d , bit cnt = %d, MS_byte_data[(*Byte_data_cnt)] = %x \n ",Track_bit_temp,bit_temp_cnt,MS_byte_data[(*Byte_data_cnt)]);
				break;
			}
			
			MS_byte_data[*Byte_data_cnt] = (MS_byte_data[*Byte_data_cnt] << 1) & full_bit;
		}

		bit_temp_cnt +=Track_bit+1;
		#ifdef USE_Time_DBG
		Uart_DBG_Printf("%d ,\n ",MS_bit_data[bit_temp_cnt]);
		#endif
		/// always [2010/11/11] �� ����Ʈ ������ ���� parity �˻�
		if(!((MS_bit_data[bit_temp_cnt] >> shift_num)& 0x01)  ^ Check_parity(MS_byte_data[(*Byte_data_cnt)],Track_bit))
		{
			#ifdef USE_Time_DBG
			Uart_DBG_Printf("READ_ERROR_PARITY  \n ");
			Uart_DBG_Printf("shift = %d , bit cnt = %d, MS_byte_data[(*Byte_data_cnt)] = %x \n ",shift_num,bit_temp_cnt,MS_byte_data[(*Byte_data_cnt)]);
			Uart_DBG_Printf("READ_ERROR_DATA =   \n ");
			for(test_cnt = 0; test_cnt <= bit_temp_cnt; test_cnt++)
			{
				Uart_DBG_Printf("%d,%d,  ",test_cnt,((MS_bit_data[test_cnt] >> shift_num)& 0x01));
				if(!(test_cnt%10))
				{
					Uart_DBG_Printf(" \n ");
				}
			}
			Uart_DBG_Printf(" \n ");
			#endif

			#ifdef USE_MAKE_BYTE
			//Uart_DBG_Printf("length = %d,shift = %d , bit cnt = %d, MS_byte_data[(*Byte_data_cnt)] = %x \n ",length,shift_num,bit_temp_cnt,MS_byte_data[(*Byte_data_cnt)]);

			/// always [2011/2/9] ������ �߻��� ��ġ���� ETX�� ã�� ������ Data�� �����Ͽ� ���� �߻� Data�� �����Ѵ�.
			StatRecover = MakeErrorByte(MS_bit_data,&bit_temp_cnt,length,shift_num,full_bit,etx,LRCTemp,Track_bit,MS_byte_data,Byte_data_cnt,NullNum);

			Uart_DBG_Printf("Byte_data_cnt = %d \n ",*Byte_data_cnt);
			/// always [2011/2/10] ���� ���� �� �и����� ����, ������ PostAmble ���� �˻�
			if(StatRecover == ERROR)
			{
				return READ_ERROR_PARITY;
			}
			else
			{
			
				/*
				StatPostAmle = PostAmbleCheck(MS_bit_data, bit_temp_cnt,shift_num, full_bit, Track_bit);
				if(StatPostAmle ==READ_ERROR_POST)
					{
						return READ_ERROR_POST;
					}
				*/
				/// always [2011/2/22] Test ���� ���� ��Ų ����
				TransASC(MS_byte_data,Byte_data_cnt,trans_asc);
				
				return SUCCESS;
			}
			#else

			return READ_ERROR_PARITY;
			
			#endif

		}

		LRCTemp ^=  MS_byte_data[(*Byte_data_cnt)];
		
		Track_bit_temp = Track_bit;

		/// always [2010/11/11] lrc �˻�� postamble �˻縦 ���� etx ���� 2 ����Ʈ�� �� �����Ѵ�.
		if(MS_byte_data[((*Byte_data_cnt)-1)] == etx )
		{
			/// always [2010/11/11] lrc �˻�
			if(Check_LRC(MS_byte_data,(*Byte_data_cnt),stx) == READ_ERROR_LRC)
			{
				#ifdef USE_Time_DBG
				Uart_DBG_Printf("READ_ERROR_LRC  \n ");
				Uart_DBG_Printf("shift = %d , bit cnt = %d \n ",shift_num,bit_temp_cnt);
				Uart_DBG_Printf("READ_ERROR_DATA =   \n ");
				for(test_cnt = 0; test_cnt < bit_temp_cnt; test_cnt++)
				{
					Uart_DBG_Printf("%d,%d,  ",test_cnt,((MS_bit_data[test_cnt] >> shift_num)& 0x01));
					if(!(test_cnt%10))
					{
						Uart_DBG_Printf(" \n ");
					}
				}
				Uart_DBG_Printf(" \n ");
				#endif
				return READ_ERROR_LRC;
			}
			//Uart_DBG_Printf("shift = %d , bit cnt = %d \n ",shift_num,bit_temp_cnt);

			/// always [2010/11/11] postamble �˻�
			/// always [2010/11/12] �����Ͱ� �� �������� �ڿ� �ִ� ��Ʈ�� 0 �Ǵ� 1�� ä���� �ִ����� ���ؼ� �˻��Ѵ�.
			/*
			StatPostAmle = PostAmbleCheck(MS_bit_data,bit_temp_cnt,shift_num, full_bit, Track_bit);

			if(StatPostAmle ==READ_ERROR_POST)
			{
				return READ_ERROR_POST;
			}
			*/
			TransASC(MS_byte_data,Byte_data_cnt,trans_asc);
			
			return SUCCESS;
			
		}

		(*Byte_data_cnt)++;
	}	
	return READ_ERROR_POST;
}

