/************************** (C) COPYRIGHT 2010 TITENG Co., Ltd Inc. *********************************/
/* project 	  : ky-7911 ms/r         		    				    */
/* processor 	  : CORETEX-M3(STM32F103vc)         		    				    */
/* compiler       : IAR EWARM Compiler								    */
/* file name      : icc.c								    */
/* purpose       : icc���� ��� 					    */
/* program by	  : jongil yun								    */
/* History:											    */
/* 		10/22/2010 - Version 0.1 Started						    */
/* copy right	  : TITENG Co., Ltd								    */
/****************************************************************************************************/
#include "icc.h"
	
/// always [2010/10/29] ���� header ����
unsigned int Lenth_ICC_DATA = 0;

unsigned char icc_buffer[525];
/// always [2010/10/29] icc�� cmd ���� ����
//unsigned char icc_Send_buffer[512];
/// always [2010/10/29] �۽� �� cmd ������ ����
volatile unsigned short Send_cmd_cnt;
/// always [2010/10/29] icc ���� ������ ���� �� �����ϴ� �������� ī��Ʈ ��
volatile unsigned int ICC_com_cnt = 0;
//volatile unsigned char Error_set;
/// always [2010/10/29] ������ �������� NAK ���� �� ����
volatile unsigned char recieve_status;
/// always [2010/10/29] icc�� cmd �� ���� �ߴ��� ����, ��ɾ��� ����� ���� �Է��� ��� ��
unsigned char send_icc_flag = 0;
/// always [2010/11/15] icc ��� ������ led �� ������ ���� flag
unsigned char icc_err_flag = 0;
/// always [2010/10/29] ������ data �� icc ��
unsigned char send_icc_bcc = 0;
/// always [2010/10/29] ������ cmd �� ����ü
//ICC_Cmd_Temp Icc_CMD_head;

/// always [2010/12/30] SAM �� ������ Ȯ���ϱ� ���� �÷���
//extern uchar check_SAM;
/// always [2010/12/30] �ӽ� sam4���� ����
uchar status_sam4;

/// always [2011/6/14] IC ī�� ����
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
* Description    : ICC ���ſ� ���� ������ ���� �ʱ�ȭ
*******************************************************************************/
void ICC_buffer_init()
{
	ICC_com_cnt = 0;
	memset(icc_buffer,0,sizeof(icc_buffer));
}


/*******************************************************************************
* Function Name  : Send_Response_data(uchar data)
* Description    : ��� ���� ��� �� ���� �� bcc �ۼ�
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
* Description    : ��� ���� ��� �� ���� �� bcc �ۼ�
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
* Description    : ��� ���� ��� �� ���� �� bcc �ۼ�
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
* Description    : ��� ���� ��� �� ���� �� bcc �ۼ�
*******************************************************************************/
void Send_u3_data(uchar data)
{
	send_icc_bcc = 0;
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART3->DR = (data & (u16)0x01FF);
	
}

/*******************************************************************************
* Function Name  : Send_cmd_icc(uchar *cmd, uchar size)
* Description    : ICC�� CMD ����
				Message : �޼��� Ÿ��
				size : ������ ������
				Slot : ���� �ѹ� 0x00 = ī��, 0x01 = SAM1
				resend : �۽� CMD ���� ���� SET ���� , RESET ���� ��
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
	/// always [2010/10/29] set �� ������Ʈ ���� ���� ���´� ��ɿ� ���� ������
	
	if(g_pcb_version != PCB_GEM_POS)
	{
		if(Slot) Slot = 1; // CR30�� SAM1�� �� �־� ��� ���� �� �׻� SAM1���̴�.
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
* Description    : icc �� ���� ������ data���� head �и� �� data ũ�� ȹ��
				���� ���� �� ACK ����
				       ���� �� NAK
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
		#if 0		//pbbch 180321 timeout ����.
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
			#if 0	//pbbch 180321 timeout ����.
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
				/// always [2013/2/26] 0x80~0xff �� ���� �� ��� �ð��� �÷��޶�� gempro�� ��û���� ��� �ð� ����
				#if 1		//pbbch 171212 0x80���� ũ�ų� ������ 1byte������ 0x80 ~ 0xff���� ������ ������.  �׷��Ƿ� "TempRxData <= 0xff"�� �׻� ���̵Ǿ� warning �߻�. ���� ���� ����.
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
					#if 1		//pbbch 180321 length ������ �������� ���� ���ؼ� erroró����.
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
		if(IC_Init == RESET) // CR30 �ʱ�ȭ ���� ���� �ƴϸ�... IC_Init ���� ��� ���� �ʰ� �����Ұ�.. �ⱸ�� ���� ��Ȯ�� ���� �ϴ��� Ȯ�� �ʿ�.
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
* Description    : SAM Card Ȯ��
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
* Description    : �����ڵ忡 ���� N response ����
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
	case 0xF4 :/// always [2012/2/15] T1ī�� ���� �� �ö�� T0�� �ٲٴ� ���� ���� �̻� �߻�
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
* Description    : NAK ��ȣ ���ο� ���� ���� ��� �� ���� �� host�� �߰�, 5ȸ ������
*******************************************************************************/
uchar check_NAK(ICC_Rx_Temp *ICC_Rx)
{
	uchar Cmd_State = SUCCESS;
	icc_err_flag = RESET;
	
	/// 20160307 hyesun : ��� ���ϴ� ��ƾ
	/// always [2010/12/30] SAM ���� �ʱ�ȭ �� ���
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
			/// always [2011/6/14] ���¿� �����ߴٸ� ��ī�� ������ Ȯ���ϰ� �ٽ� OFF ��Ų��.
			SAM_OFF = SET;
		}
		else
		{
			SAM_OFF = RESET;
			check_SAM = RESET;
			send_icc_flag = RESET;
			
		}
		ICC_buffer_init();
        return Cmd_State; // jsshin 2016.03.03: �ϴ� SUCCESS ����!
	}
	*/
		
	/// always [2010/10/29] ���� �ڵ� ���� �� �ش� �����ڵ忡 ���� ����
	if((ICC_Rx->Resp_status >= 0x40) && (ICC_Rx->Resp_status <= 0x42))
	{
		#if 0		//pbbch 1712112 volatile ����ȯ �Ұ�.....
		#else
		Cmd_State = check_Error(ICC_Rx->Slot_Num,ICC_Rx->Error_code);
		#endif
		ICC_buffer_init();
	}
	else
	{
		/// always [2010/11/1] ���� �� power on, off�� ���� stat�� ����
		// jsshin 2016.03.29 : ICC_0�� SAM_1 �̿��� ������ ������ �����Ƿ� ��Ȯ�� �Ұ�!!!
		switch(card_select)//card_select_dum//switch(ICC_Rx->Slot_Num)
		{
		/// always [2010/11/3] ����Ʈī�� ���信 ���� ó��
		case ICC_0 :
			if(ICC_Rx->Resp_status == 0x00)
			{
				IC_Reset_ON;	
				break;
			}
			else if(ICC_Rx->Resp_status == 0x01)
			{
				NO_ICC_Reset;
				/// always [2010/11/3] �Ŀ� off  ��ɿ� ���� ���� �� host�� p ������ �� �־�� �Ѵ�.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					break;
				}
				
			}
			else if(ICC_Rx->Resp_status == 0x02)
			{
				/// always [2010/11/3] �Ŀ� off  ��ɿ� ���� ���� �� host�� N ������ �� �־�� �Ѵ�.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')////if(Icc_CMD_head.Message_T == Power_OFF)
				{
					Cmd_State = NoCard;//13;
				}
			}
			
			send_icc_flag = RESET;
			ICC_buffer_init();
			break;
				
		/// always [2010/11/3] SAM 1 ī�� ���信 ���� ó��
		case SAM_1 :
			if(ICC_Rx->Resp_status == 0x00)
			{
				IS_SAM1;
				break;
			}
			else if(ICC_Rx->Resp_status == 0x01)
			{
				IS_SAM1;
			
				/// always [2010/11/3] �Ŀ� off  ��ɿ� ���� ���� �� host�� p ������ �� �־�� �Ѵ�.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')////if(Icc_CMD_head.Message_T == Power_OFF)
				{
					break;
				}
				
			}
			else if(ICC_Rx->Resp_status == 0x02)
			{
				Non_SAM1;
				/// always [2010/11/3] �Ŀ� off  ��ɿ� ���� ���� �� host�� N ������ �� �־�� �Ѵ�.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					Cmd_State = NoCard;//13;
				}
				
			}
			send_icc_flag = RESET;
			ICC_buffer_init();
			break;
		/// always [2010/11/3] SAM 2 ī�� ���信 ���� ó��
		case SAM_2 :
			if(ICC_Rx->Resp_status == 0x00)
			{
				IS_SAM2;
				break;
			}
			else if(ICC_Rx->Resp_status == 0x01)
			{
				IS_SAM2;
				
				/// always [2010/11/3] �Ŀ� off  ��ɿ� ���� ���� �� host�� p ������ �� �־�� �Ѵ�.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					break;
				}
			}
			else if(ICC_Rx->Resp_status == 0x02)
			{
				Non_SAM2;
				/// always [2010/11/3] �Ŀ� off  ��ɿ� ���� ���� �� host�� N ������ �� �־�� �Ѵ�.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					Cmd_State = NoCard;//13;
				}
			}
			send_icc_flag = RESET;
			ICC_buffer_init();
			break;
		/// always [2010/11/3] SAM 3 ī�� ���信 ���� ó��
		case SAM_3:
			if(ICC_Rx->Resp_status == 0x00)
			{
				IS_SAM3;
				break;
			}
			else if(ICC_Rx->Resp_status == 0x01)
			{
				IS_SAM3;
				
				/// always [2010/11/3] �Ŀ� off  ��ɿ� ���� ���� �� host�� p ������ �� �־�� �Ѵ�.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					break;
				}
			}
			else if(ICC_Rx->Resp_status == 0x02)
			{
				Non_SAM3;
				/// always [2010/11/3] �Ŀ� off  ��ɿ� ���� ���� �� host�� N ������ �� �־�� �Ѵ�.
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
		/// always [2010/11/3] SAM 4 ī�� ���信 ���� ó��
		case SAM_4 :
			if(ICC_Rx->Resp_status == 0x00)
			{
				IS_SAM4;
				break;
			}
			else if(ICC_Rx->Resp_status == 0x01)
			{
				IS_SAM4;
				
				/// always [2010/11/3] �Ŀ� off  ��ɿ� ���� ���� �� host�� p ������ �� �־�� �Ѵ�.
				//if(DMATEst[3] == 'D')//
				if(cmd_format.CMD == 'D')//if(Icc_CMD_head.Message_T == Power_OFF)
				{
					break;
				}
				
			}
			else if(ICC_Rx->Resp_status == 0x02)
			{
				Non_SAM4;
				/// always [2010/11/3] �Ŀ� off  ��ɿ� ���� ���� �� host�� N ������ �� �־�� �Ѵ�.
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
	/// always [2010/11/1] icc�� ������ flag�� ��
	send_icc_flag = RESET;
	return Cmd_State;
}





