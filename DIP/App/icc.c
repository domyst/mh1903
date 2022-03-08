/************************** (C) COPYRIGHT 2010 TITENG Co., Ltd Inc. *********************************/
/* project 	  : ky-7911 ms/r         		    				    */
/* processor 	  : CORETEX-M3(STM32F103vc)         		    				    */
/* compiler       : IAR EWARM Compiler								    */
/* file name      : icc.c								    */
/* purpose       : icc와의 통신 					    */
/* program by	  : jongil yun								    */
/* History:											    */
/* 		10/22/2010 - Version 0.1 Started						    */
/* copy right	  : TITENG Co., Ltd								    */
/****************************************************************************************************/
#include "icc.h"
	
/// always [2010/10/29] 수신 header 버퍼
unsigned int Lenth_ICC_DATA = 0;

unsigned char icc_buffer[525];
/// always [2010/10/29] icc로 cmd 전송 버퍼
//unsigned char icc_Send_buffer[512];
/// always [2010/10/29] 송신 할 cmd 사이즈 저장
volatile unsigned short Send_cmd_cnt;
/// always [2010/10/29] icc 에서 데이터 수신 시 수신하는 데이터의 카운트 값
volatile unsigned int ICC_com_cnt = 0;
//volatile unsigned char Error_set;
/// always [2010/10/29] 수신한 데이터의 NAK 리턴 값 저장
volatile unsigned char recieve_status;
/// always [2010/10/29] icc에 cmd 를 전송 했는지 여부, 명령없이 노이즈에 의한 입력을 대비 함
unsigned char send_icc_flag = 0;
/// always [2010/11/15] icc 통신 에러시 led 색 변경을 위한 flag
unsigned char icc_err_flag = 0;
/// always [2010/10/29] 전송할 data 의 icc 값
unsigned char send_icc_bcc = 0;
/// always [2010/10/29] 전송할 cmd 의 구조체
//ICC_Cmd_Temp Icc_CMD_head;

/// always [2010/12/30] SAM 의 유무를 확인하기 위한 플래그
//extern uchar check_SAM;
/// always [2010/12/30] 임시 sam4상태 저장
uchar status_sam4;

/// always [2011/6/14] IC 카드 상태
//uchar ICCmdStat = 0;
uchar SAM_OFF=0;
//extern unsigned char tim_resend_cnt;
extern uchar IC_Init;

extern FlashData TempFlashData;
extern Cmd_Temp cmd_format;
extern unsigned short Cmd_data_len;
extern unsigned char card_select;
//extern unsigned char card_select_dum;

#if 1		//pbbch 171212 warning across
extern void delay_us(volatile u32 usCount);
extern u8 Check_Sensor_State();
#endif
/*******************************************************************************
* Function Name  : ICC_buffer_init()
* Description    : ICC 수신에 사용된 변수에 대한 초기화
*******************************************************************************/
void ICC_buffer_init()
{
	ICC_com_cnt = 0;
	memset(icc_buffer,0,sizeof(icc_buffer));
}


/*******************************************************************************
* Function Name  : Send_Response_data(uchar data)
* Description    : 명령 수행 결과 값 전송 및 bcc 작성
*******************************************************************************/
void Send_ICC_byte_bcc(uchar data)
{
	send_icc_bcc ^= data;
//#if 1//defined(DEBUG)
//	USART1->DR = (data & (u16)0x01FF);
//	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
//#endif
	
	UART4->DR = (data & (u16)0x01FF);
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
}

/*******************************************************************************
* Function Name  : Send_Response_data(uchar data)
* Description    : 명령 수행 결과 값 전송 및 bcc 작성
*******************************************************************************/
void Send_ICC_byte(uchar data)
{
	//while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
	UART4->DR = (data & (u16)0x01FF);
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
}
void Send_ICC_bcc(uchar data)
{
	send_icc_bcc ^= data;
	//Make_resp_data_bcc(data);
	
	UART4->DR = (data & (u16)0x01FF);
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
}

/*******************************************************************************
* Function Name  : Send_Response_data(uchar data)
* Description    : 명령 수행 결과 값 전송 및 bcc 작성
*******************************************************************************/
/*
void Send_u3_data_bcc(uchar data)
{
	send_icc_bcc ^= data;
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART3->DR = (data & (u16)0x01FF);
	
}
*/
/*******************************************************************************
* Function Name  : Send_Response_data(uchar data)
* Description    : 명령 수행 결과 값 전송 및 bcc 작성
*******************************************************************************/
void Send_u3_data(uchar data)
{
	send_icc_bcc = 0;
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART3->DR = (data & (u16)0x01FF);
	
}

/*******************************************************************************
* Function Name  : Send_cmd_icc(uchar *cmd, uchar size)
* Description    : ICC로 CMD 전송
				Message : 메세지 타입
				size : 데이터 사이즈
				Slot : 슬롯 넘버 0x00 = 카드, 0x01 = SAM1
				resend : 송신 CMD 갱신 여부 SET 갱신 , RESET 기존 값
*******************************************************************************/
void Send_cmd_icc(uint Message,uchar *data,ushort size, uchar Slot, uchar resend,uchar RFU_0)
{	
	//ushort Send_cmd_cnt ;
	ICC_Cmd_Temp Icc_CMD_head;
	
	ushort temp_cnt;
	//ushort DATASIZE;
	send_icc_bcc=0;
	send_icc_flag = SET;

	TIM_Cmd(TIM6, DISABLE);//TIM_Cmd(TIM2, DISABLE);
	//TIM2_CNT = TempFlashData.IFM_WatingTime_Respons<<2;
	/// always [2010/10/29] set 시 업데이트 없이 전에 보냈던 명령에 대한 재전송
	
	if(g_pcb_version != PCB_GEM_POS)
	{
		if(Slot) Slot = 1; // CR30은 SAM1번 만 있어 명령 전송 시 항상 SAM1번이다.
	}
	
	if(resend == RESET)
	{
		Send_cmd_cnt = size;

		memcpy(Icc_CMD_head.DATA,data,size);

		Icc_CMD_head.Message_T = Message;

		Icc_CMD_head.dwLength[0] = (uchar)(size & (ushort)0x000000ff);
		Icc_CMD_head.dwLength[1] = (uchar)((size & (ushort)0x0000ff00)>>8);
		Icc_CMD_head.dwLength[2] = 0x00;
		Icc_CMD_head.dwLength[3] = 0x00;

		Icc_CMD_head.bSlot = Slot;
	}

	/*
	Send_ICC_byte_bcc(SYNC);
	Send_ICC_byte_bcc(ACK);
	Send_ICC_byte_bcc(Icc_CMD_head.Message_T);

	for(temp_cnt = 0; temp_cnt <  4; temp_cnt++)
	{
		Send_ICC_byte_bcc(Icc_CMD_head.dwLength[temp_cnt]);
	}

	Send_ICC_byte_bcc(Icc_CMD_head.bSlot);

	Send_ICC_byte_bcc(Seq);
	

	Send_ICC_byte_bcc(RFU_0);
	Send_ICC_byte_bcc(RFU);
	Send_ICC_byte_bcc(RFU);
	
	for(temp_cnt = 0; temp_cnt < Send_cmd_cnt ; temp_cnt++)
	{
		Send_ICC_byte_bcc(Icc_CMD_head.DATA[temp_cnt]);
	}
	
	Send_ICC_byte(send_icc_bcc);
  */
  u8 txcrc = 0;
  u16 txlen = 0;
  u8 txbuf[512];
  
	txbuf[txlen++] = SYNC;
	txbuf[txlen++] = ACK;
	txbuf[txlen++] = Icc_CMD_head.Message_T;
	txbuf[txlen++] = Icc_CMD_head.dwLength[0];
	txbuf[txlen++] = Icc_CMD_head.dwLength[1];
	txbuf[txlen++] = Icc_CMD_head.dwLength[2];
	txbuf[txlen++] = Icc_CMD_head.dwLength[3];
	txbuf[txlen++] = Icc_CMD_head.bSlot;
	txbuf[txlen++] = Seq;
	txbuf[txlen++] = RFU_0;
	txbuf[txlen++] = RFU;
	txbuf[txlen++] = RFU;
	for(temp_cnt = 0; temp_cnt < Send_cmd_cnt ; temp_cnt++)
	{
		txbuf[txlen++] = Icc_CMD_head.DATA[temp_cnt];
	}
  // crc calc
  for(temp_cnt = 0; temp_cnt < txlen ; temp_cnt++)
  {
    txcrc ^= txbuf[temp_cnt];
  }
	txbuf[txlen++] = txcrc;
	
  //printf("\n\n txbuf[%d]=", txlen);
  for(temp_cnt = 0; temp_cnt < txlen ; temp_cnt++)
  {
    Send_ICC_byte(txbuf[temp_cnt]);
	//Send_u5_data(txbuf[temp_cnt]);
	//for(i=0; i<EMV_DataBufferSize; i++)
	//printf("%02X ", txbuf[temp_cnt]);
  }
  //printf("\n\n");

 #if defined(DEBUG)//pbbch 180425 debug add
		printf("\r\nIC Buffer Req[%d] : ",txlen);
		for(temp_cnt = 0; temp_cnt < txlen ; temp_cnt++)
		{
			printf("%02X ", txbuf[temp_cnt]);
		}
		printf("\r\n\r\n");
#endif
  
  //	TIM_Cmd(TIM2, ENABLE);
	
}

/*******************************************************************************
* Function Name  : icc_process()
* Description    : icc 로 부터 수신한 data에서 head 분리 및 data 크기 획득
				수신 성공 시 ACK 리턴
				       실패 시 NAK
*******************************************************************************/
uchar icc_process(uchar MaxTimeCnt,ICC_Rx_Temp *ICC_Rx)
{
	unsigned short icc_buffer_cnt=0;
	uint Wating_Time_Cnt=0;
	uchar TempRxData=0;
	uchar ICC_State = 0;
	uchar Leng_Cnt = 0;
	uchar Rx_Process_Step = ICCRx_Sync;
	uint Rx_DataCnt = 0;
	uchar Rx_BCC = 0;
	//uchar t1_cnt=0;
	uchar ic_wait_retry =0;
	uint waitTime;
	//ICC_Rx_Temp ICC_Rx;
#if defined(DEBUG)//pbbch 180321 debug add
	unsigned int i=0;
#endif
	if(MaxTimeCnt)
	{
		#if 0		//pbbch 180321 timeout 줄임.
		waitTime = (MaxTimeCnt*5000);
		#else
		waitTime = (MaxTimeCnt*10000);
		#endif
	}
	else
	{
		waitTime = 500; // 50ms
	}
	
	for(Wating_Time_Cnt = 0;Wating_Time_Cnt < (uint)waitTime;Wating_Time_Cnt++)
	{
		if(icc_buffer_cnt < ICC_com_cnt)
		{
			#if 0	//pbbch 180321 timeout 줄임.
			Wating_Time_Cnt=0;
			#endif
			TempRxData = icc_buffer[icc_buffer_cnt++];
			switch(Rx_Process_Step)
			{
			case ICCRx_Sync:
				if(TempRxData == SYNC)
				{
					Rx_BCC = SYNC;
					Rx_Process_Step = ICCRx_ACK;
				}
				else if(TempRxData == 0x50)
				{
					Wating_Time_Cnt = 0;
				}
				/// always [2013/2/26] 0x80~0xff 값 수신 시 대기 시간을 늘려달라는 gempro의 요청으로 대기 시간 리셋
				#if 1		//pbbch 171212 0x80보다 크거나 같으면 1byte에서는 0x80 ~ 0xff까지 범위가 정해짐.  그러므로 "TempRxData <= 0xff"는 항상 참이되어 warning 발생. 차후 수정 요함.
				else if((TempRxData >= 0x80)||(TempRxData == 0x60))//else if((TempRxData >= 0x80)&&(TempRxData <= 0xff))
				#else
				else if(((TempRxData >= 0x80)&&(TempRxData <= 0xff))||(TempRxData == 0x60))//else if((TempRxData >= 0x80)&&(TempRxData <= 0xff))
				#endif
				{
					Wating_Time_Cnt = 0;
				}
				break;
			case ICCRx_ACK:
				if(TempRxData == ACK)
				{
					Rx_BCC ^= TempRxData;
					Rx_Process_Step = ICCRx_MessageType;
				}
				else if(TempRxData == NAK)
				{
					ICC_State = NAK;
				}
				break;
			case ICCRx_MessageType:
				Rx_BCC ^= TempRxData;
				ICC_Rx->Messsage_type = TempRxData;
				ICC_Rx->Lenth_ICC_DATA = 0;
				
				Rx_Process_Step = ICCRx_Leng;
				break;
			case ICCRx_Leng:
				Rx_BCC ^= TempRxData;
				ICC_Rx->Lenth_ICC_DATA |= ((uint)TempRxData << (Leng_Cnt*8)) & ((uint)0xff<< (Leng_Cnt*8));
				Leng_Cnt++;
				if(Leng_Cnt > 3)
				{
					#if 1		//pbbch 180321 length 오류시 오동작을 막기 위해서 error처리함.
					if(ICC_Rx->Lenth_ICC_DATA>sizeof(ICC_Rx->icc_data_buffer))
					{
						ICC_State = IFMCommunicationError;
					}
					else
					{
						Rx_Process_Step = ICCRx_Slot;
					}
					#else
					Rx_Process_Step = ICCRx_Slot;
					#endif
				}
				break;
			case ICCRx_Slot:
				Rx_BCC ^= TempRxData;
				ICC_Rx->Slot_Num = TempRxData;
				Rx_Process_Step = ICCRx_Seq;
				break;
			case ICCRx_Seq:
				Rx_BCC ^= TempRxData;
				ICC_Rx->Seqence_Num= TempRxData;
				Rx_Process_Step = ICCRx_RespState;
				break;
			case ICCRx_RespState:
				if(TempRxData >= 0x80)
				{	
					ic_wait_retry = SET;											
				}
				Rx_BCC ^= TempRxData;
				ICC_Rx->Resp_status= TempRxData;
				Rx_Process_Step = ICCRx_ErrorCode;
				break;
			case ICCRx_ErrorCode:
				Rx_BCC ^= TempRxData;
				ICC_Rx->Error_code= TempRxData;
				Rx_Process_Step = ICCRx_RFU_ProtocolNum;
				break;
			case ICCRx_RFU_ProtocolNum:
				Rx_BCC ^= TempRxData;
				ICC_Rx->RFU_ProtocolNum = TempRxData;
				if(ICC_Rx->Lenth_ICC_DATA == 0)
				{
					Rx_Process_Step = ICCRx_BCC;
				}
				else
				{
					Rx_Process_Step = ICCRx_Data;
				}
				break;
			case ICCRx_Data:
				Rx_BCC ^= TempRxData;
				ICC_Rx->icc_data_buffer[Rx_DataCnt++] = TempRxData;
				if(Rx_DataCnt == ICC_Rx->Lenth_ICC_DATA)
				{
					Rx_Process_Step = ICCRx_BCC;
				}
				break;
			case ICCRx_BCC:
				if(Rx_BCC == TempRxData)
				{
					ICC_State = SUCCESS;
				}
				break;
			default:
				Rx_Process_Step = ICCRx_Sync;
				break;
				
			}
		}
		if(ICC_State != 0)
		{
			if(ic_wait_retry)
			{
				icc_buffer_cnt=0;
				
				Rx_Process_Step = ICCRx_Sync;
				Rx_BCC = 0;
				Rx_DataCnt = 0;
				Leng_Cnt = 0;
				//data_count = 0;
				ic_wait_retry = 0;
				ICC_Rx->Lenth_ICC_DATA = 0;
				ICC_Rx->Resp_status = 0;
				ICC_Rx->Error_code = 0;
				ICC_Rx->Slot_Num = 0;
				memset((uchar*)ICC_Rx->icc_data_buffer,0,sizeof(ICC_Rx->icc_data_buffer));
			}
			else break;
		}
		delay_us(100);
		// jsshin 2016.01.27 add card present check
		if(IC_Init == RESET) // CR30 초기화 설정 중이 아니면... IC_Init 변수 사용 하지 않게 유도할것.. 기구물 센서 정확히 동작 하는지 확인 필요.
		{
			if((card_select == ICC_0) && (Check_Sensor_State(REAR_SEN) == RESET))//&& (Check_Sensor_State(FRONT_SEN) == RESET)) // 2016.01.26 jsshin : card present check
			{
				ICC_State = ICCardContactError;
				break;
			}
		}
	}
	// end for loop
#if defined(DEBUG)//pbbch 180321 debug add
		printf("\r\nIC Buffer Res[%d] : ",ICC_com_cnt);
		for(i=0; i< ICC_com_cnt; i++)
		{
			printf("%02X ", icc_buffer[i]);
		}
		printf("\r\n\r\n");
#endif
	if(Wating_Time_Cnt == waitTime)
	{
		ICC_State = IFMCommunicationError;
	}
	if(ICC_State == SUCCESS)
	{
		ICC_State = check_NAK(ICC_Rx);
	}
#if defined(USE_IWDG_RESET)
	/* Reload IWDG counter */
	IWDG_ReloadCounter();
#endif
#if defined(DEBUG)//pbbch 180321 debug add
	printf("ICC_State[%d]...wating_cnt[%d] \r\n",ICC_State,Wating_Time_Cnt);
#endif		
	return	ICC_State;
	
}


/*******************************************************************************
* Function Name  : SAM_init(uchar slot_number,uchar status)
* Description    : SAM Card 확인
*******************************************************************************/
void SAM_init(uchar slot_number,uchar status)
{
	if(status == SUCCESS)
	{
		switch(slot_number)
		{	
		case SAM_1:
			IS_SAM1;
			break;
		case SAM_2:
			IS_SAM2;
			break;
		case SAM_3:
			IS_SAM3;
			break;
		#ifndef Hanmega_USA
		case SAM_4:
			IS_SAM4;
			break;
		#endif
		}
	}
	else
	{
		switch(slot_number)
		{	
		case SAM_1:
			Non_SAM1;
			break;
		case SAM_2:
			Non_SAM2;
			break;
		case SAM_3:
			Non_SAM3;
			break;
		#ifndef Hanmega_USA
		case SAM_4:
			Non_SAM4;
			break;
		#endif
		}
	}
}	


/*******************************************************************************
* Function Name  : check_Error(uchar status)
* Description    : 에러코드에 대한 N response 전송
*******************************************************************************/
uchar check_Error(uchar Slot_Num,uchar status)
{
	switch(status)
	{
	case 0x00 :
		return CommandCancel;//
	case 0x01 :
		return ICCardWrongCommandlength;//
	case 0x02 :
		return ICCardDetectsExcessiveCurrent;//
	case 0x03 :
		return ICCardDetectsDefectiveVoltage;//
	case 0x05 :
		return ICCardCommandNotAllowed;
	case 0x07 :
	case 0x08 :
	case 0x09 :
	case 0x0A :
	case 0x15 :
		return DataFail;
	case 0xA2 :
		return ICCardShortCircuiting;			
	case 0xA3 :
		return ICCardATRLong;
	case 0xBB :
	case 0xBD :
		return CardFail;
	case 0xBE :
		return ICCardAPDULengthWrong;
	case 0xB0 :
	case 0x83 :
		return ICCardBufferOverflow;
	case 0xF4 :/// always [2012/2/15] T1카드 리드 시 올라옴 T0로 바꾸는 도중 절차 이상 발생
	case 0xF7 :
	case 0xF8 :
	case 0xFC :
		return DataFail;
	case 0xFD :
		return ICCardAPDULengthWrong;
	case 0xFE :
		if(g_pcb_version == PCB_GEM_POS)
		{
			if(Slot_Num > ICC_0)
			{
				SAM_init(Slot_Num,ERROR);
			}
		}
		else
		{
			if(card_select > ICC_0)
			{
				SAM_init(card_select,ERROR);
			}
		}
		return ICCardContactError;
		//return NoCard;
	}
	return CardFail;// jsshin 2016.03.03: default CardFail
}



/*******************************************************************************
* Function Name  : check_NAK(uchar status)
* Description    : NAK 신호 여부에 따라서 동일 명령 재 전송 및 host로 중계, 5회 재전송
*******************************************************************************/
uchar check_NAK(ICC_Rx_Temp *ICC_Rx)
{
	uchar Cmd_State = SUCCESS;
	icc_err_flag = RESET;
	
	/// 20160307 hyesun : 사용 안하는 루틴
	/// always [2010/12/30] SAM 상태 초기화 시 사용
	/*
	if(check_SAM == SET)
	{
		if((ICC_Rx->Resp_status >= 0x40) && (ICC_Rx->Resp_status <= 0x42))
		{
			SAM_init(card_select,ERROR);
			check_SAM = RESET;
			send_icc_flag = RESET;
			
		}
		else if((ICC_Rx->Resp_status == 0x00)&&(SAM_OFF == RESET))
		{
			SAM_init(card_select,SUCCESS);
			/// always [2011/6/14] 리셋에 성공했다면 샘카드 유무를 확인하고 다시 OFF 시킨다.
			SAM_OFF = SET;
		}
		else
		{
			SAM_OFF = RESET;
			check_SAM = RESET;
			send_icc_flag = RESET;
			
		}
		ICC_buffer_init();
        return Cmd_State; // jsshin 2016.03.03: 일단 SUCCESS 응답!
	}
	*/
		
	/// always [2010/10/29] 에러 코드 수신 시 해당 에러코드에 대한 응답
	if((ICC_Rx->Resp_status >= 0x40) && (ICC_Rx->Resp_status <= 0x42))
	{
		#if 0		//pbbch 1712112 volatile 형변환 불가.....
		#else
		Cmd_State = check_Error(ICC_Rx->Slot_Num,ICC_Rx->Error_code);
		#endif
		ICC_buffer_init();
	}
	else
	{
		/// always [2010/11/1] 수신 시 power on, off에 따라 stat값 변경
		// jsshin 2016.03.29 : ICC_0와 SAM_1 이외의 조건은 문제가 있으므로 재확인 할것!!!
		switch(card_select)//card_select_dum//switch(ICC_Rx->Slot_Num)
		{
		/// always [2010/11/3] 스마트카드 응답에 대한 처리
		case ICC_0 :
			if(ICC_Rx->Resp_status == 0x00)
			{
				IC_Reset_ON;	
				break;
			}
			else if(ICC_Rx->Resp_status == 0x01)
			{
				NO_ICC_Reset;
				/// always [2010/11/3] 파워 off  명령에 대한 수행 시 host로 p 응답을 해 주어야 한다.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					break;
				}
				
			}
			else if(ICC_Rx->Resp_status == 0x02)
			{
				/// always [2010/11/3] 파워 off  명령에 대한 수행 시 host로 N 응답을 해 주어야 한다.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')////if(Icc_CMD_head.Message_T == Power_OFF)
				{
					Cmd_State = NoCard;//13;
				}
			}
			
			send_icc_flag = RESET;
			ICC_buffer_init();
			break;
				
		/// always [2010/11/3] SAM 1 카드 응답에 대한 처리
		case SAM_1 :
			if(ICC_Rx->Resp_status == 0x00)
			{
				IS_SAM1;
				break;
			}
			else if(ICC_Rx->Resp_status == 0x01)
			{
				IS_SAM1;
			
				/// always [2010/11/3] 파워 off  명령에 대한 수행 시 host로 p 응답을 해 주어야 한다.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')////if(Icc_CMD_head.Message_T == Power_OFF)
				{
					break;
				}
				
			}
			else if(ICC_Rx->Resp_status == 0x02)
			{
				Non_SAM1;
				/// always [2010/11/3] 파워 off  명령에 대한 수행 시 host로 N 응답을 해 주어야 한다.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					Cmd_State = NoCard;//13;
				}
				
			}
			send_icc_flag = RESET;
			ICC_buffer_init();
			break;
		/// always [2010/11/3] SAM 2 카드 응답에 대한 처리
		case SAM_2 :
			if(ICC_Rx->Resp_status == 0x00)
			{
				IS_SAM2;
				break;
			}
			else if(ICC_Rx->Resp_status == 0x01)
			{
				IS_SAM2;
				
				/// always [2010/11/3] 파워 off  명령에 대한 수행 시 host로 p 응답을 해 주어야 한다.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					break;
				}
			}
			else if(ICC_Rx->Resp_status == 0x02)
			{
				Non_SAM2;
				/// always [2010/11/3] 파워 off  명령에 대한 수행 시 host로 N 응답을 해 주어야 한다.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					Cmd_State = NoCard;//13;
				}
			}
			send_icc_flag = RESET;
			ICC_buffer_init();
			break;
		/// always [2010/11/3] SAM 3 카드 응답에 대한 처리
		case SAM_3:
			if(ICC_Rx->Resp_status == 0x00)
			{
				IS_SAM3;
				break;
			}
			else if(ICC_Rx->Resp_status == 0x01)
			{
				IS_SAM3;
				
				/// always [2010/11/3] 파워 off  명령에 대한 수행 시 host로 p 응답을 해 주어야 한다.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					break;
				}
			}
			else if(ICC_Rx->Resp_status == 0x02)
			{
				Non_SAM3;
				/// always [2010/11/3] 파워 off  명령에 대한 수행 시 host로 N 응답을 해 주어야 한다.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					Cmd_State = NoCard;//13;
				}
			}
			send_icc_flag = RESET;
			ICC_buffer_init();
			break;
		#ifndef Hanmega_USA
		/// always [2010/11/3] SAM 4 카드 응답에 대한 처리
		case SAM_4 :
			if(ICC_Rx->Resp_status == 0x00)
			{
				IS_SAM4;
				break;
			}
			else if(ICC_Rx->Resp_status == 0x01)
			{
				IS_SAM4;
				
				/// always [2010/11/3] 파워 off  명령에 대한 수행 시 host로 p 응답을 해 주어야 한다.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					break;
				}
				
			}
			else if(ICC_Rx->Resp_status == 0x02)
			{
				Non_SAM4;
				/// always [2010/11/3] 파워 off  명령에 대한 수행 시 host로 N 응답을 해 주어야 한다.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					Cmd_State = NoCard;//13;
				}
				
			}
			send_icc_flag = RESET;
			ICC_buffer_init();
			break;
		#endif
		}
	}
	/// always [2010/11/1] icc로 전송중 flag를 끔
	send_icc_flag = RESET;
	return Cmd_State;
}





