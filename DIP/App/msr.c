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
//* Description    : 역방향 리딩의 경우 버퍼의 순서를 뒤집어 준다.
//*******************************************************************************/
//void Back_Read(uchar *MS_bit_data,uchar *TempBuffer,ushort length)
//{
//	ushort Temp_cnt;
//	ushort compare_cnt;
//
//	compare_cnt = length;
//		
//	/// always [2010/11/12] 반대로 돼있는 것을 정방향으로 돌린다.
//	for(Temp_cnt=0;Temp_cnt<compare_cnt;Temp_cnt++)
//	{		
//		TempBuffer[Temp_cnt] = MS_bit_data[(compare_cnt-1)-Temp_cnt];
//	}
//	
//}
//
///*******************************************************************************
//* Function Name  : Back_Read(uchar *MS_bit_data,ushort length)
//* Description    : 역방향 리딩의 경우 버퍼의 순서를 뒤집어 준다.
//*******************************************************************************/
//void Back_Time(ushort *MS_bit_data,ushort length)
//{
//	ushort Temp_cnt;
//	ushort compare_cnt;
//	ushort MS_Time_back[max_Bit_buffer_T13];
//	
//	compare_cnt = length;
//		
//	/// always [2010/11/12] 반대로 돼있는 것을 정방향으로 돌린다.
//	for(Temp_cnt=0;Temp_cnt<compare_cnt;Temp_cnt++)
//	{		
//		
//		MS_Time_back[Temp_cnt] = MS_bit_data[(compare_cnt-1)-Temp_cnt];
//	}
//	memcpy(MS_bit_data,MS_Time_back,(length*sizeof(ushort)));
//}

/*******************************************************************************
* Function Name  : uchar Compare_Time_Dec(uchar MS_bit_data,uchar MS_bit_time_data,ushort length)
* Description    : 타이머의 데이터와 디코더의 데이터 비교
*******************************************************************************/
uchar Compare_Time_Dec(uchar *d_status,uchar *t_status,uchar *MS_bit_data,uchar *MS_bit_time_data,uchar *d_length,uchar *t_length)
{
	/// always [2010/11/12] 반대로 돼있는 것을 정방향으로 돌린다.

	if((*(d_status) != SUCCESS)&&(*(t_status) == SUCCESS))
	{
		*(d_status) = *(t_status);
		*(d_length) = *(t_length);
		memcpy(MS_bit_data,MS_bit_time_data,sizeof(uchar)*(*(t_length)));

		return ERROR;
	}
	/// always [2011/12/15] 만약 둘다 오류라면 타이머의 오류값으로 처리
	else if((*(d_status) != SUCCESS)&&(*(t_status) != SUCCESS))
	{
		*(d_status) = *(t_status);
	}

	return SUCCESS;
}

/*******************************************************************************
* Function Name  : Make_All_data(uchar stat_T1,uchar stat_T2,uchar stat_T3, u8 *MS_DATA_ALL)
* Description    : 각 트랙 정보를 하나로 합친다.
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

	memset(MS_DATA_T1,0,sizeof(MS_DATA_T1));//*******************************************130425hs:주석처리
	memset(MS_DATA_T2,0,sizeof(MS_DATA_T2));//*******************************************130425hs:주석처리
	memset(MS_DATA_T3,0,sizeof(MS_DATA_T3));//*******************************************130425hs:주석처리

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
		/// 20150107 hyesun : 삭제 , 추가 수정
		MS_DATA_ALL[Byte_DATA_cnt_All++] = 'N';
		MS_DATA_ALL[Byte_DATA_cnt_All++] = ((Status_T1 & 0xf0)>>4)+0x30;
		MS_DATA_ALL[Byte_DATA_cnt_All++] = (Status_T1 & 0x0f)+0x30;	
		#else
		/// always [2010/11/12] 트랙의 실패여부를 검사하여 실패했다면 해당 data의 자리에 NAK 삽입
		MS_DATA_ALL[Byte_DATA_cnt_All++] = Status_T1;			
		#endif
	}
	
	/// always [2010/11/12] 각 트랙의 구분은 중간에 0x00으로 확인한다.
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
		/// 20150107 hyesun : 삭제 , 추가 수정
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
		/// 20150107 hyesun : 삭제 , 추가 수정
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
	// 20150401 hyesun : 키트로닉스 와 상이한 부분 보완(Forward Read 시 Sensor Satate 값) -> 카드 투입시 카드가 완전히 들어온 후에 IS MS DATA를 SET 시키도록 수정
	if(MS_Read_IN == BACKWARD)IS_MS_DATA;
	else;// is_ms_data_forward = SET;// 20160720 : VT04적용, 한메가 요청
	#else
	/// always [2010/11/12] 각트랙의 정보가 하나로 완성 돼었다면, ms data의 존재를 알린다.
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
* Description    : Decoder를 통해 들어오는 bit 정보 수집
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
	uint TCnt		= 30000; // 20000값 check sensor 함수 사용시 총 걸리는 시간 500ms 소요 되는것으로 판단 됨.
	//uint TCnt		= 300000; // delay_us(1) 사용시 300ms wait
#if 1	//pbbch 180212  ms reading중 stop상태시 retry 추가.
	unsigned char ltimeout_retry_cnt=0;
#endif
#if 1		//pbbch 180305 timer overflow flg add
	#if 0		//pbbch 180306 reading rate를 높이기 위해서 기존 코드 사용....추가 코드 막음.
	unsigned short lt1_over_flg_cnt1=0;
	unsigned short lt1_over_flg_cnt4=0;
	unsigned short lt1_over_flg_cnt5=0;
	#endif
	unsigned int lmax_loop_cnt=1;
#endif
	
	#ifdef USE_Time_DBG
	int test_cnt =0;
	#endif

	/// always [2010/11/3] port의 변화를 비교하기 위한 값
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

//	memset(MS_DATA_T1,0,sizeof(MS_DATA_T1));//[20131107_hyesun]: 버퍼 초기화 추가
//	memset(MS_DATA_T2,0,sizeof(MS_DATA_T2));//[20131107_hyesun]: 버퍼 초기화 추가
//	memset(MS_DATA_T3,0,sizeof(MS_DATA_T3));//[20131107_hyesun]: 버퍼 초기화 추가
//	memset(MS_DATA_ALL,0,sizeof(MS_DATA_ALL));//[20131107_hyesun]: 버퍼 초기화 추가

// jsshin 2015.10.07 MS_Bit_DATA buffer clear
	memset(MS_Bit_DATA,0,sizeof(MS_Bit_DATA));//[20131107_hyesun]: 버퍼 초기화 추가
#if 0		//pbbch 171212 across warning	
	ErrorState = ErrorFlag;
#endif	
	// jssin 2016.06.06
	// 카드 신호가 감지 되면 flag set 한다.
	// 카드가 삽입완료(Front, Rear 모두 SET)되었으나 gMSDetect 값이 셋팅 되어 있지 않으면 읽은 데이터 모두 삭제 처리.
	gMSDetect = 1;

//1st ms read bit 출력 전까지 soft timeout 약 500ms라고 주석 된 만큼 기다림. 시간은 측정 안됨. 	
MS_BIT_RETRY_START:							//pbbch 180212 ms card reading중 stop시 retry 추가...max 2sec  

	while(MS_nCLS_1_IN)
	{
		//delay_us(1); // jsshin 2016.03.01 track data 적은 카드의 경우 타임아웃 발생한다.
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
//2st ms read bit data 수집

#if 1	//pbbch 180305 MS_nCLS_1_IN low로 유지 될때 timeout이 안걸리는 case가 있어 개별 flg 셋팅 함.  
	while(++lmax_loop_cnt<MAX_R_LOOP_CNT)
#else
	while(1)
#endif		
	{
	#ifdef USE_Time
		/// always [2010/11/9] JITTER 측정을 위한 타임 값 저장
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
		/// always [2010/11/9] DECODER에서 들어오는 BIT의 저장
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
				// always [2010/11/3] 데이터핀의 값이 0 일때 1을 1일 때 0을 저장
			}

			/// always [2010/11/3] 지금의 값을 비교 값으로 저장
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
				/// always [2010/11/3] 데이터핀의 값이 0 일때 1을 1일 때 0을 저장
				//MS_Bit_DATA[Bit_DATA_cnt_T2++] ^= ((uchar)MS_MRD_2_IN  << shift_1) ^ Track_2;
			}
			
			/// always [2010/11/3] 지금의 값을 비교 값으로 저장
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
				/// always [2010/11/3] 데이터핀의 값이 0 일때 1을 1일 때 0을 저장
				//MS_Bit_DATA[Bit_DATA_cnt_T3++] ^= ((uchar)MS_MRD_3_IN << shift_2) ^ Track_3;
			}
			
			/// always [2010/11/3] 지금의 값을 비교 값으로 저장
			Bofore_T3 = MS_RCP_3_IN;
		}
		#endif
				
		/// always [2010/11/12] ms read를 끝내는 3가지 경우
                        
//////		#ifndef SamsungPay
//////		if(Check_Sensor_State(REAR_SEN) == SET)
//////		{
//////			break;
//////		}
//////		#endif
			
		if(MS_nCLS_1_IN)
		{
	#if 1	//pbbch 180212  ms reading중 stop상태시 retry 추가.
			if((Tim_DATA_cnt_T2> 5)&&(Tim_DATA_cnt_T2< 100))
			{
		#if 1	//pbbch 180515 3초 타임아웃 발생으로 retry 1count 줄임.
				if( (++ltimeout_retry_cnt < 3) &&(CheckFront == SET))		
		#else
				if( (++ltimeout_retry_cnt <= 3) &&(CheckFront == SET))		//pbbch 180212 backword만 적용 ..about respone time 2sec delay
		#endif			
				{
		
					TCnt		= 110000;
					#if 1		//pbbch 180305 timer disable code add
					lmax_loop_cnt=0;
					TIMER_5_DIS;
					TIMER_4_DIS;
					TIMER_1_DIS;
				//	TIMER_4_RESET;		//reset시 retry 시도할 경우 data가 깨짐
				//	TIMER_1_RESET;
				//	TIMER_5_RESET;
					#endif
					goto MS_BIT_RETRY_START;
				}
				else
				{
					CheckFront = RESET;
					asm("NOP");
					break; /// CLS 신호 deActivate!!!
				}
			}
			else
			{
				CheckFront = RESET;
				asm("NOP");
				break; /// CLS 신호 deActivate!!!
			}
	#else
			CheckFront = RESET;
			asm("NOP");
			break; /// CLS 신호 deActivate!!!
	#endif		
		}
		/// always [2010/12/2] time buffer over flow 시 아무 처리도 않함
		if((Tim_DATA_cnt_T1 >max_Bit_buffer_T13) ||(Tim_DATA_cnt_T2 >max_Bit_buffer_T2)||(Tim_DATA_cnt_T3 >max_Bit_buffer_T13))
		{
			CheckFront = RESET;
			return;	
		}

		/// always [2010/12/2] bit 정보가 over flow 시 아무 처리도 않함
		if((Bit_DATA_cnt_T1 >max_Bit_buffer_T13) ||(Bit_DATA_cnt_T2 >max_Bit_buffer_T2)||(Bit_DATA_cnt_T3 >max_Bit_buffer_T13))
		{
			CheckFront = RESET;
			return;	
		}

#if 0	 //pbbch 180306 reading rate를 높이기 위해서 기존 코드 사용....추가 코드 막음. //pbbch 180305 MS_nCLS_1_IN low로 유지 될때 timeout이 안걸리는 case가 있어 개별 flg 셋팅 함. 
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
	// Backward Read일때 F2F interval time & RCP Bit 신호 값을 뒤집어 준다.
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
	// RCP data를 먼저 파싱하여 정상적으로 읽혔으면 F2F데이터 분석을 생략 하도록 하자!
	// RCP BIT 데이터로 ISO 데이터 생성.
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


	/// always [2011/3/9] 속도 확인, 1카운트 당 0.486us, Track 2 비트당 간격 0.3386mm/bit
	/// always [2011/3/9] Track 2 기준 약 200값이 3386mm/s
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
		
		/// always [2010/11/16] 만약 정방향 에서 읽은 데이터가 성공했다면 data를 처리하지 않는다.
		#ifdef T1
		if((Status_T1 != SUCCESS)| (MS_DATA_IN == RESET))
		{
			Byte_DATA_cnt_TIME_T1 = 0;
			memset(MS_DATA_TIME_T1,0,sizeof(MS_DATA_TIME_T1));

			Bit_Time_cnt_T1 = chk_j(MS_Bit_Temp_T1,MS_Time_T1,Tim_DATA_cnt_T1,Track_1,5,5);
			
			//jsshin
			/// always [2010/11/16] 뽑을 때 읽은 데이터를 반대 방향으로 다시 바꿔야 한다.
			//Back_Read(MS_Bit_DATA,Tempbuffer_D,Bit_DATA_cnt_T1);
			// RCP BIT 데이터로 ISO 데이터 생성.
			// 2016.03.01 코드 윗부분에서 RCP데이터 먼저 파싱 한다.
			//Status_T1 = MS_Bit_to_Byte(MS_Bit_DATA,MS_DATA_T1,&Byte_DATA_cnt_T1,Bit_DATA_cnt_T1,Track_1,RESET);

			//jsshin
			//Back_Read(MS_Bit_Temp_T1,Tempbuffer_T,Bit_Time_cnt_T1);
			// F2F데이터를 chk_j()이용 해서 파싱된 BIT 데이터로 ISO 데이터 생성.
			Status_TIME_T1 = MS_Bit_to_Byte(MS_Bit_Temp_T1,MS_DATA_TIME_T1,&Byte_DATA_cnt_TIME_T1,Bit_Time_cnt_T1,Track_1,SET);
			
			/// always [2011/12/13] 만약 Read 실패시 스레솔드 값을 변경 하면서 Bit 구성을 제 시도 한다.
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

			/// always [2010/11/16] 뽑을 때 읽은 데이터를 반대 방향으로 다시 바꿔야 한다.
			//jsshin
			//Back_Read(MS_Bit_DATA,Tempbuffer_D,Bit_DATA_cnt_T2);
			// 2016.03.01 RCP 수신 후 파싱처리 한다.
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

			/// always [2010/11/16] 뽑을 때 읽은 데이터를 반대 방향으로 다시 바꿔야 한다.
			//jsshin
			//Back_Read(MS_Bit_DATA,Tempbuffer_D,Bit_DATA_cnt_T3);
			// 2016.03.01 RCP 수신 후 파싱처리 한다.
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
		// All track read 명령 수신 시, 각 track 데이터를 조합하여 응답하도록 변경.
		//Status_ALL = Make_All_data(Status_T1,Status_T2,Status_T3, MS_DATA_ALL );
		if((Status_T1 != SUCCESS)&&(Status_T2 != SUCCESS)&&(Status_T3 != SUCCESS))
		{
			Non_MS_DATA;
		} else IS_MS_DATA;
#endif

		//if(HW_TYPE == NEW_TYPE)
		//{
#ifdef Hanmega_USA
		// 2016.02.26 : 한메가 미주향에서 아래 강제 셋팅 요청. 타 사이트 삭제 함!!!
		Front_Detect_Non;
		MS_F_Read_ON;
#endif
		//CheckFront = RESET;// 20160921 hyesun : 한메가 센서값 이상 이슈사항 대응으로 삭제, 하단 루틴에 추가
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
			// RCP BIT 데이터로 ISO 데이터 생성.
			// 2016.03.01 코드 윗부분에서 RCP데이터 먼저 파싱 한다.
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
			// RCP BIT 데이터로 ISO 데이터 생성.
			// 2016.03.01 코드 윗부분에서 RCP데이터 먼저 파싱 한다.
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
			// RCP BIT 데이터로 ISO 데이터 생성.
			// 2016.03.01 코드 윗부분에서 RCP데이터 먼저 파싱 한다.
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
			//is_ms_data_forward = SET;// 20160720 : VT04적용, 한메가 요청
#else
		// jsshin 2015.10.12
		// All track read 명령 수신 시, 각 track 데이터를 조합하여 응답하도록 변경.
		//Status_ALL = Make_All_data(Status_T1,Status_T2,Status_T3, MS_DATA_ALL );
		if((Status_T1 != SUCCESS)&&(Status_T2 != SUCCESS)&&(Status_T3 != SUCCESS))
		{
//			//++ jsshin 2016.06.01
//			if(ms_retry) {
//				ms_retry = 0;
//				MS_R_Read_ON;
//				goto bw_read_retry; // jsshin 2016.06.01 fw 읽기 실패 시에는 bw로 다시 한번 읽기 시도.
//			}
//			//-- jsshin 2016.06.01			
			Non_MS_DATA; // MS Data All Track Read Err
		} else IS_MS_DATA; // MS데이터 정상 Read, Track 1개라도 읽었으면 셋팅 된다.
#endif
	}
	CheckFront = RESET;// 20160921 hyesun : 한메가 센서값 이상 이슈사항 대응
	//Green_ON();
	//AllLedOff;// 20140529 hyesun : LED control 커멘드 추가하면서 삭제함
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
* Description    : ETX의 parity bit 위치를 리턴
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
	/// always [2011/2/17] LRC에서 1이 검출되고 트렉당 사용되는 Bit 수의 2배 안에서 ETX가 검출 되어야 한다.
	AvailableETX=(Track_bit+1)*4;
	StatETX=RESET;
	//StatNextByte = RESET;
	#ifdef USE_Time_DBG
	Uart_DBG_Printf("ErrorLeng = %d, length = %d,Track_bit = %d, NullNum = %d  \n ",ErrorLeng,length,Track_bit,NullNum);
	#endif
	/// always [2011/2/7] ETX를 찾아서 해당 비튼 넘버를 전송
	for(BitCnt = length-1;BitCnt > ErrorLeng+1;BitCnt--)
	{

		if(((MS_bit_data[BitCnt] >> shift_num)& 0x01)&&(StatETX==RESET))
		{
			StatETX = SET;
			/// always [2011/2/22] 첫 번째 발견된 1은 제외 시킨다. 이는 무조건 LRC의 1이기 때문이다.
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

				/// always [2011/2/7] ETX의 패릿 부호위치를 리턴한다.
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
					/// always [2010/11/11] etx가 들어올 때 까지 값을 한 칸씩 shift 한다.
					BitCnt--;
					//Uart_DBG_Printf("4BitCnt = %d\n ",BitCnt);
					ETXTemp <<= 1;
				}	

				if(BitCnt < ErrorLeng+1)
				{
					return ERROR;
				}
			}
			/// always [2011/2/17] Null 중간에 1이 섞이더라도 다시 한번 ETX를 찾을 수 있게 함
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
* Description    : 발생한 Parity Error 지점에서 ETX를 포함한 비트의 갯수를 가지고, 트랙당 바이트 구성 갯수를 통한 남은 Byte와
			비트의 오독으로 인한 비트 갯수의 변동을 보정하기 위한 나머지 비트를 연산한다.
* Date	: 20110209
*******************************************************************************/
uchar RearByte(ushort ErrorLeng,uchar Track_bit,ushort *ByteNum,ushort *BitNum,ushort BitCnt)
{
	
	ushort RemainBit = 0;

	uchar BitNumError = 0xff;

//	*BitCnt = FindETX(MS_bit_data,ErrorLeng,length,shift_num,full_bit,TrackETX);

	if(BitCnt)
	{
		/// always [2011/2/10] 구해진 Bit의 갯수에서 에러난 지점까지 사용한 Bit를 제한다.
		RemainBit = BitCnt - ErrorLeng;
		/// always [2011/2/10] 만약 비트 오독으로 인해 비트가 추가되거나 모자를 경우를 대비해 나머지비트값을 구한다.
		*BitNum = RemainBit % (Track_bit + 1);
		/// always [2011/2/10] 에러 위치에서 ETX를 포함한 위치까지의 바이트 갯수를 구한다.
		*ByteNum = RemainBit / (Track_bit +1);

		return SUCCESS;
	}
	
	return ERROR;
}
/*******************************************************************************
* Function Name  : uchar MakeLRC(uchar *MS_bit_data,ushort ErrorLeng,ushort length, uchar shift_num,
								uchar full_bit,uchar TrackETX,uchar Track_bit,ushort *BitCnt,uchar *DataLRC)
* Description    : LRC 부호를 찾고, LRC의 패릿을 확인한다.
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
	
	/// always [2011/2/17] ETX의 Parity Bit 위치
	*BitCnt = FindETX(MS_bit_data,ErrorLeng,length,shift_num,full_bit,TrackETX,NullNum,Track_bit);

	

	if(*BitCnt == ERROR)
	{
		//Uart_DBG_Printf("NO ETX ErrorLeng =  %d  \n ",ErrorLeng);
		return ERROR;
	}

	while(RetryLRC)
	{
		/// always [2011/2/22] LRC 만들기
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
		/// always [2011/2/22] ETX 의 앞 Data확인 용
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
			/// always [2011/2/10] 패릿 비트가 1인지, ETX 전 비트가 1인지, LRC의 첫 비트가 1인지 확인
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
			/// always [2011/2/10] 패릿 부호가 맞다 하더라도 LRC의 다음 부호가 1일 경우에는 한 번 더LRC의 패릿을 확인한다.
			if((((MS_bit_data[*BitCnt-(Track_bit+1)] >> shift_num)& 0x01) == 1)&&(BitNum >=1)&&(BitNum <Track_bit -1))
			{
				/// always [2011/2/23] 재시도가 실패한다면 그 전값 리턴
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
			/// always [2011/2/22] 만약 LRC가 ETX와 같은 값이 들어올 경우(2,3 트랙만 적용)
			else if((BitNum == 4)&&(LRCTemp == 0x01)&&(frontData == 0x0f)&&(Track_bit == 4))
			{
				//Uart_DBG_Printf("frontData = %x, *BitCnt  = %d,Track_bit = %d  \n ",frontData,*BitCnt ,Track_bit);

				/// always [2011/2/23] 재시도가 실패한다면 그 전값 리턴
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
* Description    : 발생한 Parity Error 지점에서 ETX와 LRC의 위치를 파악하여 역 추적, 에러난 바이트를 복구한다.

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

	/// always [2011/2/10] 에러난 위치의 바이트 위치 정보를 가진다.
	ErrorByteNum = *Byte_data_cnt ;
	/// always [2011/2/10] 비트를 통한 바이트 값을 구하기 위한 트랙당 비트의 갯수
	TrackBitTemp = Track_bit;

	/// always [2011/2/10] LRC값을 구한다.
	StatMakeLRC = MakeLRC(MS_bit_data,*ErrorLeng,length,shift_num, full_bit,TrackETX,Track_bit,&BitCnt,&DataLRC,NullNum);
	/// always [2011/2/10] 받은 BitCnt는 ETX의 패릿 부호에 위치 하므로 LRC의 패릿 부호 위치를 EndBit로 저장한다.
	EndBit = BitCnt +Track_bit+ 1;

	//Uart_DBG_Printf("bit cnt = %d, Byte_data_cnt= %d,FullByte = %d \n ",BitCnt,*Byte_data_cnt,FullByte);
	/// always [2011/2/9] parity에러 발생 시 무조건 한번은 실행하므로, 이 곳에서 STX에서 LRC까지의 DATA를 백업해 둔다.
	if(StatMakeLRC == ERROR)
	{
		return ERROR;
	}
	
	/// always [2011/2/10] 구해진 BitCnt값을 통해 남아 있는 Byte의 수와 비트가 빠졌는지 첨가 됬는지 확인한다.
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

	/// always [2011/2/10] ETX까지 Data 저장한다.
	*Byte_data_cnt = FullByte+1;

	/// always [2011/2/10] ETX를 다시한번 확인하기 위한 BitCnt 값
	BitCnt -= (Track_bit+1);
	
	//Uart_DBG_Printf("3bit cnt = %d, Byte_data_cnt= %d,FullByte = %d \n ",BitCnt,*Byte_data_cnt,FullByte);
	if(StatMakeNum == SUCCESS)
	{
		/// always [2011/2/10] 에러난 지점의 바이트 값 바로 전까지 ETX에서 시작하여 역으로 DATA를 작성해 나간다.
		for( ;FullByte > ErrorByteNum;)
		{
			while(TrackBitTemp)
			{
				ByteDataTemp |= ((MS_bit_data[(BitCnt+TrackBitTemp--)] >> shift_num)& 0x01);

				/// always [2011/2/10] 4번 반복하고 시프트 하기 전에 나간다.
				if(TrackBitTemp == 0)
				{
					break;
				}
		
				ByteDataTemp = (ByteDataTemp << 1) & full_bit;
			}
			/// always [2011/2/15] bitcnt = 다음 디코딩 패릿 부호 위치
			//Uart_DBG_Printf("shift = %d , bit cnt = %d, ByteDataTemp= %x ,FullByte = %d\n ",shift_num,BitCnt,ByteDataTemp,FullByte);
			/// always [2011/2/10] 구해진 값의 패릿 부호 검사
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
			
			/// always [2011/2/10] 구해진 값들을 역으로 XOR하면서 값을 추적한다.
			DataLRC ^= ByteDataTemp;
			#ifdef USE_Time_DBG
			Uart_DBG_Printf("ByteDataTemp = %x, DataLRC = %x, BitCnt= %d \n ",ByteDataTemp,DataLRC,BitCnt);
			#endif
			/// always [2011/2/10] 구해진 값은 ETX에서 부터 역으로 채워 나간다.
			MS_byte_data[FullByte--] = ByteDataTemp;
			
			/// always [2011/2/10] 다음 구해질 bit값의 초기 위치
			BitCnt -= (Track_bit+1);

			/// always [2011/2/10] 바이트 합성을 위한 임시값의 초기화
			ByteDataTemp = 0;
			TrackBitTemp = Track_bit;

		}
		

		/// always [2011/2/10] 에러 발생 전까지 구해진 LRC와 역 추적한 LRC를 통해 에러난 Data를 구한다.
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
* Description    : 모인 바이트 정보의 lrc 체크
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
* Description    : Bit Data의 post amble 에러를 확인한다.
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
* Description    : 구해진 MS의 DATA를 아스키 부호화 한다.
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
* Description    : 바이트의 parity 부호 체크
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
* Description    : ms bit data 를 byte data로 변환 하여 트랙별 데이터 저장
			MS_bit_data : bit data 저장 소
					MS_Bit_DATA
			MS_byte_data : 변환 후 byte data 저장
					MS_DATA_T1
					MS_DATA_T2
					MS_DATA_T3
			Byte_data_cnt : 트랙별 데이타 갯수
					Byte_DATA_cnt_T1
					Byte_DATA_cnt_T2
					Byte_DATA_cnt_T3
			length : bit data 갯수
					Bit_DATA_cnt_T1
					Bit_DATA_cnt_T2
					Bit_DATA_cnt_T3
			Track_bit : track에 따른 byte의 구성 bit 수
					T1_bit_num
					T23_bit_num		
			shift_num : 트랙에 따른 bit 정보의 위치 정보
					shift_1
					shift_2
					shift_3
			stx : 각 트랙 별 stx	
					ISO1_STX
					ISO23_STX
			etx : 각 트랙 별 etx	
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
	
	/// always [2010/11/11] 초기 오류시 블랭크 에러
	if(length < 10)
	{
		#ifdef USE_Time_DBG
		Uart_DBG_Printf("READ_ERROR_BLANK  \n ");
		#endif
		return READ_ERROR_BLANK;
	}

	//while(!((MS_bit_data[bit_temp_cnt++] >> shift_num)& 0x01));
	
	/// always [2010/11/11] stx가 검출 될때까지 비트를 shift 하면서 확인
	for(;bit_temp_cnt < length;)
	{

		/// always [2010/11/11] 하나의 bit 데이터에 0번은 트랙1, 1번은 트랙2, 2번은 트랙3 이므로 원하는 트랙에 맞게 값을 검출한다.
		STX_Temp |= ((MS_bit_data[bit_temp_cnt++] >> shift_num)& 0x01);

		/// always [2010/11/11] stx시 break
		if((STX_Temp & full_bit) == back_stx)
		{
			break;
		}
		else
		{
			/// always [2010/11/11] stx가 들어올 때 까지 값을 한 칸씩 shift 한다.
			STX_Temp <<= 1;
		}			
	}
	/// always [2010/11/11] stx의 parity 검사
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
	
	/// always [2010/11/11] stx가 검출 될때까지의 길이를 확인하여 오류 검출
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
	/// always [2010/11/11] 비트데이터를 바이트 데이터로 변환하여 저장
	for(;bit_temp_cnt < length;)
	{
		while(Track_bit_temp)
		{
			//Uart_DBG_Printf("Track_bit_temp = %d , bit cnt = %d,  \n ",Track_bit_temp,bit_temp_cnt);
			
			/// always [2010/11/12] 각트랙마다 bit의 위치가 다르기 때문에 트랙에 맞게 시프트 해줘야 한다.
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
		/// always [2010/11/11] 각 바이트 정보에 대해 parity 검사
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

			/// always [2011/2/9] 에러가 발생한 위치에서 ETX를 찾고 역으로 Data를 추적하여 에러 발생 Data를 복구한다.
			StatRecover = MakeErrorByte(MS_bit_data,&bit_temp_cnt,length,shift_num,full_bit,etx,LRCTemp,Track_bit,MS_byte_data,Byte_data_cnt,NullNum);

			Uart_DBG_Printf("Byte_data_cnt = %d \n ",*Byte_data_cnt);
			/// always [2011/2/10] 복구 실패 시 패릿에러 리턴, 성공시 PostAmble 에러 검사
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
				/// always [2011/2/22] Test 위해 제외 시킨 것임
				TransASC(MS_byte_data,Byte_data_cnt,trans_asc);
				
				return SUCCESS;
			}
			#else

			return READ_ERROR_PARITY;
			
			#endif

		}

		LRCTemp ^=  MS_byte_data[(*Byte_data_cnt)];
		
		Track_bit_temp = Track_bit;

		/// always [2010/11/11] lrc 검사와 postamble 검사를 위해 etx 이후 2 바이트를 더 검출한다.
		if(MS_byte_data[((*Byte_data_cnt)-1)] == etx )
		{
			/// always [2010/11/11] lrc 검사
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

			/// always [2010/11/11] postamble 검사
			/// always [2010/11/12] 데이터가 다 읽혔는지 뒤에 있는 비트가 0 또는 1로 채워져 있는지를 통해서 검사한다.
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

