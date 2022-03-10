/*******************************(C) COPYRIGHT 2007 INSEM Inc.****************************************/
/* processor 	  :          		    				    */
/* compiler       : 								    */
/* program by	  : 								    */
/* History:											    */
/* 04/13/2007     : Version 1.0									    */
/* copy right	  : 									    */
/****************************************************************************************************/
#define __MH1903_H__    // domyst check

#include "stm32f10x.h"

#include "main.h"
//#include "Cmd.h"

#include "mhscpu_dma.h"     //domyst

#include "usb_lib.h"
#include "usb_core.h"
#include "usb_desc.h"
#include "usb_prop.h"
#include "usb_pwr.h"


/* Private macro -------------------------------------------------------------*/
#define IFM_VCC_5V_3V_PORT	GPIOH					
#define IFM_VCC_1_8V_PORT	GPIOH					
#define IFM_VCC_5V_3V_PIN	GPIO_Pin_1				
#define IFM_VCC_1_8V_PIN	GPIO_Pin_0				

#define SAM_VCC_5V_3V_PORT	GPIOH					
#define SAM_VCC_1_8V_PORT	GPIOH					
#define SAM_VCC_5V_3V_PIN	GPIO_Pin_3
#define SAM_VCC_1_8V_PIN	GPIO_Pin_2	
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
RCC_ClocksTypeDef RCC_ClockFreq;
NVIC_InitTypeDef NVIC_InitStructure;
static volatile ErrorStatus HSEStartUpStatus = SUCCESS;
USART_InitTypeDef USART_InitStruct;

EXTI_InitTypeDef EXTI_InitStruct;
DMA_InitTypeDef DMA_InitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
ADC_InitTypeDef ADC_InitStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

FlashData TempFlashData;

uchar InitSensor = RESET;
u8 g_pcb_version = 0xff;

#if 1		//pbbch 180503 sen5 추가로 board에 따른 sensor 처리를 변경 해야 함. 따라서 구분자 추가.  
unsigned char gsen_array_type=0;
#endif

extern unsigned char icc_buffer[525];
extern uchar DMATEst[DMABufferSize];

//extern ushort *MS_Time_T1[740];
//extern ushort *MS_Time_T2[740];
//extern ushort *MS_Time_T3[740];

//extern uchar t_test;

extern uchar MasterkeyUse;
extern uchar MasterKey[DES3_USER_KEY_LEN];
extern const unsigned char Defaultkey[DES3_USER_KEY_LEN];

//extern uchar check_SAM;

extern uchar LockUse;
//extern uchar IC_Init;
extern uint SolCnt;
extern ushort ADC_DMA_Buffer[Number_of_ADC*Number_of_Buf];
extern Cmd_Temp cmd_format;

extern u8 SAM_initial(void);		//pbbch 180321 void add.
extern void Send_ICC_byte(uchar data);

// Usb function
extern void USB_Init(void);
extern uint8_t DeviceState;
extern uint8_t DeviceStatus[6];
extern uint32_t bDeviceState; /* USB device status */
extern uint8_t CustomHID_StringSerial[CUSTOMHID_SIZ_STRING_SERIAL];

extern uchar Escape_IC(uchar *Data,ushort Datasize,uchar SubCmd,uchar TypeLevel);

#if 1		//pbbch 171212 warning across

extern void FlashDataWrite();
#if 1//pbbch 180321 waing time 조정을 위해 수정.
extern uchar F_CmdProc(uchar *CmdData,ushort CmdDatasize,uchar *Data, ushort *Datasize,uchar wait_time);
#else
extern uchar F_CmdProc(uchar *CmdData,ushort CmdDatasize,uchar *Data, ushort *Datasize);		//pbbch 171212 선언 할 경우 ushort *Datasize에서 error발생. 디버깅 필요.
#endif
extern void TxUSBData(uchar ResponseType,uchar NCord,uchar *Data,ushort DataSize);

#endif

#if 0
/******************************************************************************************************/
/* Internal_timer_Proc : 평상시 tmxfg는 항상 "1" 이다	(timer_setup 시 tmxg를 "0"으로 만든다)	      */ 											
/******************************************************************************************************/
void Internal_timer_Proc() {				

  if(tm1ms_f) { tm1ms_f =0;	//every 1msec
      if(!tm0fg) {	if(!--timer_buf[0]) tm0fg =1;	}		
      if(!tm1fg) {	if(!--timer_buf[1]) tm1fg =1;	}
      if(!tm2fg) {	if(!--timer_buf[2]) tm2fg =1;	}
      if(!tm3fg) {	if(!--timer_buf[3]) tm3fg =1;	}
      if(!tm4fg) {	if(!--timer_buf[4]) tm4fg =1;	}
      if(!tm5fg) {	if(!--timer_buf[5]) tm5fg =1;	}
      if(!tm6fg) {	if(!--timer_buf[6]) tm6fg =1;	}
      if(!tm7fg) {	if(!--timer_buf[7]) tm7fg =1;	}
  }
}
/******************************************************************************************************/
/* timer_setup : 해당 tmxfg를 "0"으로 만든 후 원하는 timer 값을 처리한다.		              */ 											
/******************************************************************************************************/
void timer_setup(uchar tmno, ushort timer) {
    clrbit(tm.slag, tmno);	timer_buf[tmno] =timer;
}
/******************************************************************************************************/
/* timer base bit initial :		                                                              */																						
/******************************************************************************************************/
void Gp_initial(void){
    tm.slag =0xff;
}
#endif

/******************************************************************************************************/
/* PLL_initial	:	 Configures the system clocks.						      */												
/******************************************************************************************************/
void PLL_initial(void) {	//PLL configuration
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
  }
 #if defined STM32F100_ENABLE
// flash access wait time
// 0 = Zero wait state, if 0 < SYSCLK≤ 24 MHz
// 1 = One wait state,  if 24 MHz < SYSCLK ≤ 48 MHz
// 2 = Two wait states, if 48 MHz < SYSCLK ≤ 72 MHz

 /* Flash 1 wait state */
  //FLASH_SetLatency(FLASH_Latency_1);

  /* HCLK = MAX SYSCLK = 24MHz/1 = 24MHz */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);

  /* configure MAX PCLK2 = 24MHz/1 = 24MHz */
  RCC_PCLK2Config(RCC_HCLK_Div1);

  /* configure MAX PCLK1 = 24MHz/1 = 24MHz */
  RCC_PCLK1Config(RCC_HCLK_Div1);

  /* configure MAX PLLCLK = 8MHz/1 * 3 = 24MHz */
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_3);
#endif
#if defined STM32F101_ENABLE
// flash access wait time
// 0 = Zero wait state, if 0 < SYSCLK≤ 24 MHz
// 1 = One wait state,  if 24 MHz < SYSCLK ≤ 48 MHz
// 2 = Two wait states, if 48 MHz < SYSCLK ≤ 72 MHz

 /* Flash 1 wait state */
  FLASH_SetLatency(FLASH_Latency_1);

  /* HCLK = MAX SYSCLK = 36MHz/1 = 36MHz */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);

  /* configure MAX PCLK2 = 36MHz/1 = 36MHz */
  RCC_PCLK2Config(RCC_HCLK_Div1);

  /* configure MAX PCLK1 = 36MHz/1 = 36MHz */
  RCC_PCLK1Config(RCC_HCLK_Div1);

  /* configure MAX PLLCLK = 8MHz/2 * 9 = 36MHz */
  RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);

#endif
#if defined STM32F102_ENABLE

 /* Flash 1 wait state */
  FLASH_SetLatency(FLASH_Latency_1);

  /* HCLK = MAX SYSCLK = 48MHz/1=48MHz */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);

  /* configure MAX PCLK2 = 48MHz/1=48MHz */
  RCC_PCLK2Config(RCC_HCLK_Div1);

  /* configure MAX PCLK1 = 48MHz/2=24MHz */
  RCC_PCLK1Config(RCC_HCLK_Div2);

  /* configure MAX PLLCLK = 8MHz/1 * 6 =48MHz */
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);

#endif
#if defined STM32F103_ENABLE

 /* Flash 2 wait state */
  FLASH_SetLatency(FLASH_Latency_2);

  /* HCLK = MAX SYSCLK = 72MHz/1=72MHz*/
  RCC_HCLKConfig(RCC_SYSCLK_Div1);

  /* configure MAX PCLK2 = 72MHz/1=72MHz */
  RCC_PCLK2Config(RCC_HCLK_Div1);

  /* configure MAX PCLK1 = 72MHz/2=36MHz */
  RCC_PCLK1Config(RCC_HCLK_Div2);

  /* configure MAX PLLCLK = 12MHz/1 * 6= 72MHz */
  /// always [2010/11/18] 만약 외부 클럭이 변할 시 stm32f103_conf.h에 있는 HSE 값도 바꿔 주어야 한다.
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);

#endif

  /* Enable PLL */
  RCC_PLLCmd(ENABLE);

  /* Wait till PLL is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  }

  /* Select PLL as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  /* Wait till PLL is used as system clock source */
  while(RCC_GetSYSCLKSource() != 0x08)
  {
  }

  /* Enable Prefetch Buffer */
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

  /* This function fills a RCC_ClocksTypeDef structure with the current
     frequencies of different on chip clocks (for debug purpose) */
  RCC_GetClocksFreq(&RCC_ClockFreq);

  /* Enable Clock Security System(CSS) */
  RCC_ClockSecuritySystemCmd(ENABLE);

}
/******************************************************************************************************/
/* NVIC_initial	:	vector configuration   					                      */												
/******************************************************************************************************/
void NVIC_initial(void){

#ifdef  VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, VECT_TAB_OFFSET); // jsshin 2015.08.18!!! vector table offset remap!!!
#endif

  /* Enable and configure RCC global IRQ channel */
  NVIC_InitStructure.NVIC_IRQChannel = RCC_IRQn; //RCC_IRQChannel; // jsshin 2015.08.18 changed IRQ channel name at the StdLib 3.5
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}
/******************************************************************************************************/
/* SYSTICK_initial	:	SYSTICK_ configuration   					      */												
/******************************************************************************************************/
void SYSTICK_initial(void)
{
//++ jsshin 2015.08.18
// systick initialze changed with CMSIS func
#if !defined(KTC_MODEL)
	//SysTick_Config(1000000 * 9);//(RCC_Clocks.HCLK_Frequency/8 / 1000000)); // per 1sec interval.
	SysTick_Config(100000 * 9);//(RCC_Clocks.HCLK_Frequency/8 / 1000000)); // per 100ms interval interrupt
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
#else
	SysTick_Config(100000 * 9);//(RCC_Clocks.HCLK_Frequency/8 / 1000000)); // per 100ms interval interrupt
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
#endif
//-- jsshin 2015.08.18

}
/******************************************************************************************************/
/* GPIO_port_initial	:								              */												
/******************************************************************************************************/
void GPIO_port_initial_org(void)
{
	/********************
	/// GPIOA
	*********************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/********************
	GPIO_Pin_0	DMS_AMP1
	GPIO_Pin_1	DMS_AMP2
	GPIO_Pin_2	DMS_AMP3
	GPIO_Pin_3	F2F_2
	GPIO_Pin_6	F2F_3
	GPIO_Pin_8	F2F_1
	*********************/
	/// always [2010/11/2] MS card Amplitude 측정 용 ADC 연결
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
//// for anti skimm control, currently not used!
//	/// 20150921 hyesun Anti Skimming-defense sol & func sensor
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_2;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/// always [2011/3/2] F2F 신호 수신
	//	GPIO_Pin_3	F2F_2
	//	GPIO_Pin_6	F2F_3
	//	GPIO_Pin_8	F2F_1
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_6| GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/// USB Con ON
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	#if 1		//pbbch 180129 usb con on set....DP line must do pullup
	GPIO_ResetBits(GPIOA, GPIO_Pin_7);
	#endif

	/// sensor 3(Front), 4(Inner) AIN
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	/********************
	GPIOB
	**********************/
	/// always [2010/11/2] Enable GPIOE clock for SOL_CON, IFM_ON, SENSOR_ON, USART 3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	/********************
	GPIO_Pin_0	SOL_CON
	GPIO_Pin_1	/RESET GEMPRO
	GPIO_Pin_7	CARD_DETECTION
	GPIO_Pin_8	SENSOR_PWR_ON
	**********************/
	#ifdef USE_PWM
	/// always [2010/11/30] PWM for sol
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	#else
	/// always [2010/11/30] PWM for sol
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	#endif

#if 1		//pbbch 190412 reset pin 분리 reset rising time 500ns>100ns
	// IIC_WP, IIC_SCL, IIC_SDA, SENSOR_ON, CARD_STATE_OUT
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5| GPIO_Pin_6|GPIO_Pin_7| GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_9);			// card detect pin high init//pbbch 190329 add

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);			//Reset control port

#else
	// IFM_nRESET, IIC_WP, IIC_SCL, IIC_SDA, SENSOR_ON, CARD_STATE_OUT
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1|GPIO_Pin_5| GPIO_Pin_6|GPIO_Pin_7| GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	#if 1		//pbbch 180320 Card detect default high....no card detect status..detect time is lock sensing or activation sequence
	#if 1		//pbbch 190319 Reset port low init		//pbbch 190329 update\
	GPIO_SetBits(GPIOB, GPIO_Pin_9);			// card detect pin high init
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);		//Reset control port
	#else
	GPIO_SetBits(GPIOB, GPIO_Pin_9);	//
	GPIO_SetBits(GPIOB, GPIO_Pin_1);	//pbbch 180321 reset high setting

	//IFM_reset_excution(0);			//pbbch 180321 reset function add..but CR30 do fail in first transaction...we don't know reason. so ifm reset don't need.
	#endif
	#endif
#endif
	// BOOT1
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// SW_INPUT
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/// RFU
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/********************
	GPIOC
	**********************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	/// always [2010/11/2] Enable GPIOC clock for RDD signal 1,2,3, USART4 TX, RX, USART5 TX
	//	GPIO_Pin_10	U4_TX
	//	GPIO_Pin_11	U4_RX
	//	GPIO_Pin_12	U5_TX
	/// Sensor1,2
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0| GPIO_Pin_1;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

#if 0
	/// 20150921 hyesun Anti Skimming input
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
#else
	#if 1		//pbbch 180528 sensor5(front sensor) gpio config change
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	#else
	/// 20160307 hyesun PCB1 SAM MUX Ctrl
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4| GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	#endif
#endif

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6| GPIO_Pin_7| GPIO_Pin_8| GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    	GPIO_Init(GPIOC, &GPIO_InitStructure);
	#if 1		//pbbch 190319 power on port low init		//pbbch 190329 update
	GPIO_ResetBits(GPIOC,GPIO_Pin_9);		//power control port
	#endif

	/// NC
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/********************
	GPIOD
	**********************/
	/// always [2010/11/2] Enable GPIOC clock for SENSOR 1,2,3,4, LED 1,2, F2F signal 1, 2, 3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	/********************
	GPIO_Pin_8	RCP3
	GPIO_Pin_9	MRD3
	GPIO_Pin_10	CEN
	GPIO_Pin_11	RCP2
	GPIO_Pin_12	MRD2
	GPIO_Pin_13	RCP1
	GPIO_Pin_14	MRD1
	GPIO_Pin_15	nCLS1
	*****************/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/// always [2010/11/2] MS DECODER CHIP ENABLE
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	#if 1		//pbbch 180207 ms decoder chip reset signal low setting
	GPIO_ResetBits(GPIOD,GPIO_Pin_10);
	#endif

	/// PCB2 SAM MUX Ctrl
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	#if 1		//pbbch 180503 sen5 추가로 board에 따른 sensor 처리를 변경 해야 함. 따라서 구분자 추가.  
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	#else
	/// NC
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	#endif
		
	/********************
	GPIOE
	**********************/
	/// always [2010/11/2] Enable GPIOE clock for SENSOR 1,2,3,4, LED 1,2, F2F signal 1, 2, 3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	/********************
	GPIO_Pin_4	CARD_EJECT
	GPIO_Pin_5	RED LED
	
	GPIO_Pin_7	YELLOW LED
	GPIO_Pin_9	USB D
	*****************/
	// PCB_GEM_POS Sensor input
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/// eject button (interrupt)
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/// DMS_AMP 1, 2, 3
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_10| GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // decoder read에 영향이 있는지 확인 할것.
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/// USB_DET
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_12  | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	/// NC
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

}

// domyst
// IFM port init
void IFM_SCI0_IOConfig(void)
{
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, GPIO_Remap_0); //ALT0
    //card detect
    SYSCTRL->PHER_CTRL &= ~BIT(16); // SCI0 card detection signal -> 0 : active high (1: ative low)
//    SYSCTRL->PHER_CTRL |= BIT(16);
    //Choose active level(Low level active).
    SYSCTRL->PHER_CTRL |= BIT(20);  // SCI0 VCC effective signal level selection -> 1 : active low (0: active high)
}

// SAM port init
void SAM_SCI2_IOConfig(void)
{
	GPIO_PinRemapConfig(GPIOE, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12, GPIO_Remap_0); //ALT0
    //card detect
    SYSCTRL->PHER_CTRL &= ~BIT(18); // SCI0 card detection signal -> 0 : active high (1: ative low)
//    SYSCTRL->PHER_CTRL |= BIT(16);
    //Choose active level(Low level active).
    SYSCTRL->PHER_CTRL |= BIT(22);  // SCI0 VCC effective signal level selection -> 1 : active low (0: active high)
}

void Select_IFM_VCC(void)
{
/*	
#define IFM_VCC_5V_3V_PORT	GPIOH					
#define IFM_VCC_1_8V_PORT	GPIOH					
#define IFM_VCC_5V_3V_PIN	GPIO_Pin_1				
#define IFM_VCC_1_8V_PIN	GPIO_Pin_0				
*/	
	GPIO_InitTypeDef  GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;		// IFM_5V_3.3V_EN
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);			

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;		// IFM_1.8V_EN
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);
	
 	GPIO_SetBits(IFM_VCC_1_8V_PORT,IFM_VCC_1_8V_PIN);					//In order to use the VCC_5V_3V, you have to set the 1.8V Pin to High.
//	GPIO_ResetBits(VCC_1_8V_PORT,VCC_1_8V_PIN);					//In order to use the 1.8V, you have to set the VCC_5V_3V Pin to High.
	
	GPIO_SetBits(IFM_VCC_5V_3V_PORT,IFM_VCC_5V_3V_PIN);		//5V select	
//	GPIO_ResetBits(IFM_VCC_5V_3V_PORT,IFM_VCC_5V_3V_PIN);		//3V select
}

void Select_SAM_VCC(void)
{
/*	
#define SAM_VCC_5V_3V_PORT	GPIOH					
#define SAM_VCC_1_8V_PORT	GPIOH					
#define SAM_VCC_5V_3V_PIN	GPIO_Pin_3
#define SAM_VCC_1_8V_PIN	GPIO_Pin_2					
*/	
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;		// SAM_5V_3.3V_EN
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;		// IFM_1.8V_EN
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
 	GPIO_SetBits(SAM_VCC_1_8V_PORT, SAM_VCC_1_8V_PIN);					//In order to use the VCC_5V_3V, you have to set the 1.8V Pin to High.
//	GPIO_ResetBits(SAM_VCC_1_8V_PORT,VCC_1_8V_PIN);					//In order to use the 1.8V, you have to set the VCC_5V_3V Pin to High.
	
	GPIO_SetBits(SAM_VCC_5V_3V_PORT, SAM_VCC_5V_3V_PIN);		//5V select	
//	GPIO_ResetBits(SAM_VCC_5V_3V_PORT,VCC_5V_3V_PIN);		//3V select
}

void IFM_SCI0_NVICConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);
    
    NVIC_InitStructure.NVIC_IRQChannel = SCI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

void SAM_SCI2_NVICConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);
    
    NVIC_InitStructure.NVIC_IRQChannel = SCI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

// IFM configuration
void IFM_Configuration(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_SCI0, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_SCI0, ENABLE);

	IFM_SCI0_IOConfig();

	Select_IFM_VCC();
	SCI_ConfigEMV(0x01, 3000000);

	//SCI_NVICConfig();	
	IFM_SCI0_NVICConfig();
}

// SAM configuration
void SAM_Configuration(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_SCI2, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_SCI2, ENABLE);

	SAM_SCI2_IOConfig();

	Select_SAM_VCC();
	SCI_ConfigEMV(0x01, 3000000);

	//SCI_NVICConfig();	
	SAM_SCI2_NVICConfig();
}

// CPU_TXD2, CPU_RXD2 for debug, barcode
// 0 for debug, 1 for barcode
void UART2_select(uint8_t data)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);

	if (data == 1)
        GPIO_SetBits(GPIOH, GPIO_Pin_7);        // barcode      
	else        
        GPIO_ResetBits(GPIOH, GPIO_Pin_7);      // debug
}

// UART0, UART1, UART2
void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;

	//UART0 for main(host)
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);	
	
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	
	UART_Init(UART0, &UART_InitStructure);

	//UART1 for RF
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART1, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART1, ENABLE);
	
	GPIO_PinRemapConfig(GPIOB, GPIO_Pin_12 | GPIO_Pin_13, GPIO_Remap_3);	
	
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	
	UART_Init(UART1, &UART_InitStructure);

	//UART2 for debug, barcode
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART2, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART2, ENABLE);
	
	GPIO_PinRemapConfig(GPIOB, GPIO_Pin_8 | GPIO_Pin_9, GPIO_Remap_2);	
	
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	
	UART_Init(UART2, &UART_InitStructure);

	// uart select for uart2
}

// MMD1100 port init
void MMD1100_MSR_Configuration(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStruct;

    // SPI2
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_SPI2, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_SPI2, ENABLE);

    GPIO_PinRemapConfig(GPIOB, GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5, GPIO_Remap_0); // GPIOE ??

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;		// 확인 필 ???
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_0;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // 확인 필??
	SPI_InitStructure.SPI_RXFIFOFullThreshold = SPI_RXFIFOFullThreshold_1;
	SPI_InitStructure.SPI_TXFIFOEmptyThreshold = SPI_TXFIFOEmptyThreshold_10;
	
	SPI_Init(SPIM2, &SPI_InitStructure);
    SPI_Cmd(SPIM2, ENABLE);

	// PB2	SPI_CLK		ALT0
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;		// GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5; ???
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_0;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// PB1	Ready		ALT1  
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// PB3	RST
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);



	// //PE14	SPI_MOSI	ALT2
	// GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	// GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	// GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;		//modified by domyst GPIO_Remap_1
	// GPIO_Init(GPIOE, &GPIO_InitStruct);

	// //PE15	SPI_MISO	ALT2
	// GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	// GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	// GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;		//modified by domyst GPIO_Remap_1
	// GPIO_Init(GPIOE, &GPIO_InitStruct);
}

void GPIO_port_initial(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	// UART_SELECT
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);

	// IFM_POWER_ON
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	// SOLENOID POWER ON
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	// SENSOR POWER ON
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// MMD1100

	// EEPROM PG13, 14, 15

	// IFM, SAM 1.8, 3, 5V

	// LED, MST
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);

	// MST
	// GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	// GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	// GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	// GPIO_Init(GPIOH, &GPIO_InitStruct);

	// CPU_MODE, USB DETECT, IC_CARD_DETECTION
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 \
							   GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);

	// MST SIG, SAM CARD DETECTION
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// FRONT, INNER SENSOR, ANTI_SKIM, CARD_END SENSOR, SOL LOCK SENSOR
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 \
							   GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	// SAM MUX
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	// BARCODE RST, TRIGGER
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	// BARCODE GOOD LED
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	// USB RST
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	// CARD EJECT
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOG, &GPIO_InitStruct);






#if 0 //
	/********************
	/// GPIOA
	*********************/




	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/********************
	GPIO_Pin_0	DMS_AMP1
	GPIO_Pin_1	DMS_AMP2
	GPIO_Pin_2	DMS_AMP3
	GPIO_Pin_3	F2F_2
	GPIO_Pin_6	F2F_3
	GPIO_Pin_8	F2F_1
	*********************/
	/// always [2010/11/2] MS card Amplitude 측정 용 ADC 연결
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
//// for anti skimm control, currently not used!
//	/// 20150921 hyesun Anti Skimming-defense sol & func sensor
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_2;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/// always [2011/3/2] F2F 신호 수신
	//	GPIO_Pin_3	F2F_2
	//	GPIO_Pin_6	F2F_3
	//	GPIO_Pin_8	F2F_1
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_6| GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/// USB Con ON
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	#if 1		//pbbch 180129 usb con on set....DP line must do pullup
	GPIO_ResetBits(GPIOA, GPIO_Pin_7);
	#endif

	/// sensor 3(Front), 4(Inner) AIN
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	/********************
	GPIOB
	**********************/
	/// always [2010/11/2] Enable GPIOE clock for SOL_CON, IFM_ON, SENSOR_ON, USART 3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	/********************
	GPIO_Pin_0	SOL_CON
	GPIO_Pin_1	/RESET GEMPRO
	GPIO_Pin_7	CARD_DETECTION
	GPIO_Pin_8	SENSOR_PWR_ON
	**********************/
	#ifdef USE_PWM
	/// always [2010/11/30] PWM for sol
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	#else
	/// always [2010/11/30] PWM for sol
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	#endif

#if 1		//pbbch 190412 reset pin 분리 reset rising time 500ns>100ns
	// IIC_WP, IIC_SCL, IIC_SDA, SENSOR_ON, CARD_STATE_OUT
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5| GPIO_Pin_6|GPIO_Pin_7| GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_9);			// card detect pin high init//pbbch 190329 add

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);			//Reset control port

#else
	// IFM_nRESET, IIC_WP, IIC_SCL, IIC_SDA, SENSOR_ON, CARD_STATE_OUT
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1|GPIO_Pin_5| GPIO_Pin_6|GPIO_Pin_7| GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	#if 1		//pbbch 180320 Card detect default high....no card detect status..detect time is lock sensing or activation sequence
	#if 1		//pbbch 190319 Reset port low init		//pbbch 190329 update\
	GPIO_SetBits(GPIOB, GPIO_Pin_9);			// card detect pin high init
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);		//Reset control port
	#else
	GPIO_SetBits(GPIOB, GPIO_Pin_9);	//
	GPIO_SetBits(GPIOB, GPIO_Pin_1);	//pbbch 180321 reset high setting

	//IFM_reset_excution(0);			//pbbch 180321 reset function add..but CR30 do fail in first transaction...we don't know reason. so ifm reset don't need.
	#endif
	#endif
#endif
	// BOOT1
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// SW_INPUT
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/// RFU
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/********************
	GPIOC
	**********************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	/// always [2010/11/2] Enable GPIOC clock for RDD signal 1,2,3, USART4 TX, RX, USART5 TX
	//	GPIO_Pin_10	U4_TX
	//	GPIO_Pin_11	U4_RX
	//	GPIO_Pin_12	U5_TX
	/// Sensor1,2
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0| GPIO_Pin_1;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

#if 0
	/// 20150921 hyesun Anti Skimming input
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
#else
	#if 1		//pbbch 180528 sensor5(front sensor) gpio config change
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	#else
	/// 20160307 hyesun PCB1 SAM MUX Ctrl
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4| GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	#endif
#endif

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6| GPIO_Pin_7| GPIO_Pin_8| GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    	GPIO_Init(GPIOC, &GPIO_InitStructure);
	#if 1		//pbbch 190319 power on port low init		//pbbch 190329 update
	GPIO_ResetBits(GPIOC,GPIO_Pin_9);		//power control port
	#endif

	/// NC
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/********************
	GPIOD
	**********************/
	/// always [2010/11/2] Enable GPIOC clock for SENSOR 1,2,3,4, LED 1,2, F2F signal 1, 2, 3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	/********************
	GPIO_Pin_8	RCP3
	GPIO_Pin_9	MRD3
	GPIO_Pin_10	CEN
	GPIO_Pin_11	RCP2
	GPIO_Pin_12	MRD2
	GPIO_Pin_13	RCP1
	GPIO_Pin_14	MRD1
	GPIO_Pin_15	nCLS1
	*****************/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/// always [2010/11/2] MS DECODER CHIP ENABLE
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	#if 1		//pbbch 180207 ms decoder chip reset signal low setting
	GPIO_ResetBits(GPIOD,GPIO_Pin_10);
	#endif

	/// PCB2 SAM MUX Ctrl
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	#if 1		//pbbch 180503 sen5 추가로 board에 따른 sensor 처리를 변경 해야 함. 따라서 구분자 추가.  
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	#else
	/// NC
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	#endif
		
	/********************
	GPIOE
	**********************/
	/// always [2010/11/2] Enable GPIOE clock for SENSOR 1,2,3,4, LED 1,2, F2F signal 1, 2, 3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	/********************
	GPIO_Pin_4	CARD_EJECT
	GPIO_Pin_5	RED LED
	
	GPIO_Pin_7	YELLOW LED
	GPIO_Pin_9	USB D
	*****************/
	// PCB_GEM_POS Sensor input
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/// eject button (interrupt)
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/// DMS_AMP 1, 2, 3
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_10| GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // decoder read에 영향이 있는지 확인 할것.
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/// USB_DET
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_12  | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	/// NC
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
#endif
}
/******************************************************************************************************/
/* TIME1_initial	:	Track 2							              */
/* 0.03185sec time set. */
/******************************************************************************************************/
void TIME1_initial(void){

  /* TIM1 clock enable */   //max - 72mHz
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

   TIM_DeInit(TIM1);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 34;  //0.486us /bit
  TIM_TimeBaseStructure.TIM_Period = 0xffff;    //1Mhz/1 =->0.1us timer

  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

   TIM_ARRPreloadConfig(TIM1,ENABLE);
  /* TIM enable counter */
  //TIM_Cmd(TIM1, ENABLE);
  /// always [2011/5/17] 타이머 pwm 사용 시 타이머 1,8의 경우 따로 pwm output enable 함수를 실행해야 함
 // TIM_CtrlPWMOutputs(TIM1, ENABLE);
}


/******************************************************************************************************/
/* TIME3_initial	:		for Solanoid 						              */												
/******************************************************************************************************/

void TIME3_initial(void){

	//ushort TIM3_ARR3 = 350;
	ushort TIM3_CCR3 = 0;

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	/* Time base configuration */
	/// always [2010/11/30] TIM4 Channel1 duty cycle = (TIM4_CCR1 / TIM4_ARR + 1) * 100
	TIM_TimeBaseStructure.TIM_Period = TIM3_ARR3;
	TIM_TimeBaseStructure.TIM_Prescaler = 6;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* PWM4 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = TIM3_CCR3;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC3Init(TIM3, &TIM_OCInitStructure);

	//PWM_4_EN;

	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);

	//TIM_GenerateEvent(TIM4, TIM_EventSource_Update);

	/* TIM4 enable counter */
	TIM_Cmd(TIM3, ENABLE);

}
/******************************************************************************************************/
/* TIME4_initial	:				Track 1		              */			
/* 0.0109sec time set.*/
/******************************************************************************************************/

void TIME4_initial(void){

  /* TIM4 clock enable */   //max - 72mHz
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_DeInit(TIM4);

  /* Time base configuration */
  /// always [2011/3/14] 3배 빠르게 실험
  TIM_TimeBaseStructure.TIM_Prescaler = 11;
  TIM_TimeBaseStructure.TIM_Period = 0xffff;

  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

   TIM_ARRPreloadConfig(TIM4,ENABLE);


}

/******************************************************************************************************/
/* TIME3_initial	:					for Track 3 of MS			              */		
/* 0.0109sec time set.*/
/******************************************************************************************************/

void TIME5_initial(void){

  /* TIM5 clock enable */   //max - 72mHz
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_DeInit(TIM5);

  /* Time base configuration */
  /// always [2010/11/12] 71+1된 값이 TIM_Prescaler 값이 된다.
  TIM_TimeBaseStructure.TIM_Prescaler = 11;
  TIM_TimeBaseStructure.TIM_Period = 0xffff;

  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

   TIM_ARRPreloadConfig(TIM5,ENABLE);

  /* TIM enable counter */
  //TIM_Cmd(TIM5, ENABLE);

  /* TIM IT enable */
  //TIM_ITConfig(TIM5,TIM_IT_Update, ENABLE);

}
/******************************************************************************************************/
/* TIME2_initial	:		host cmd 수신 중 time out 200ms						              */												
/******************************************************************************************************/
void TIME2_initial(void){

  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_DeInit(TIM2);
  	
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 7199;  //72Mhz/7200=10khz , 0.00001s
  TIM_TimeBaseStructure.TIM_Period = 500;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* TIM enable counter */
  //
  TIM_Cmd(TIM2, DISABLE);
  //TIM_Cmd(TIM2, ENABLE);
    /// always [2010/11/1]  이때 클리어 플래그를 않해준다면 인터럽트 벡터를 실행 시킨다.
   TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  /* TIM IT enable */
  TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; //TIM2_IRQChannel; // jsshin 2015.08.18 changed IRQ channel name at the StdLib 3.5
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************************************/
/* TIME6_initial	:	Front Sensor Time Cal						              */												
/******************************************************************************************************/
void TIME6_initial(void)
{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM_OCInitStructure;

  	/* TIM6 clock enable */   //max - 72mHz
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	//TIM_DeInit(TIM6);
	#if 1		//pbbch 180220 timer setting revision...until timeout, 2sec apply
	/* Time base configuration *///pbbch 180220 12.525ms setting
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;//35999;   // prescale 6MHz : ((72Mhz/6Mhz)-1) = 11///
	TIM_TimeBaseStructure.TIM_Period = 20000;//500;//4000;//2초   10000;//5sec ///
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//TIM_CKD_DIV4;//0;///
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	#else
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;//35999;   // prescale 6MHz : ((72Mhz/6Mhz)-1) = 11///
	TIM_TimeBaseStructure.TIM_Period = 500;//500;//4000;//2초   10000;//5sec ///
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;//TIM_CKD_DIV4;//0;///
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	#endif
	
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM6, ENABLE);

	 /* TIM enable counter */
  //
  TIM_Cmd(TIM6, DISABLE);
  //TIM_Cmd(TIM2, ENABLE);
    /// always [2010/11/1]  이때 클리어 플래그를 않해준다면 인터럽트 벡터를 실행 시킨다.
   TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
  /* TIM IT enable */
  TIM_ITConfig(TIM6,TIM_IT_Update, ENABLE);

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;//TIM6_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 8;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}	

/******************************************************************************************************/
/* TIME8_initial	:	??						              */												
/******************************************************************************************************/
void TIME8_initial(void)
{
	ushort TIM8_CCR3 = 0;

	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = TIM3_ARR3;
	TIM_TimeBaseStructure.TIM_Prescaler = 6;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

	/* PWM4 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = TIM8_CCR3;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC3Init(TIM8, &TIM_OCInitStructure);

	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM8, ENABLE);

	//TIM_GenerateEvent(TIM8, TIM_EventSource_Update);

	/* TIM8 enable counter */
	TIM_Cmd(TIM8, ENABLE);
}	

/******************************************************************************************************/
/* UART1_initial	:		R3 : HOST, other : RF					       						      */												
/******************************************************************************************************/
// PCB_CR30_R3 : HOST, other : RF
void UART1_initial(uint Baudrate,ushort HardFlow)
{ 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* Enable USART1 clocks */
  	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE);

	/// always [2010/11/2] USART1 Tx (PA9) as alternate function push-pull
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/// always [2010/11/2] USART1 Rx (PA10) as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_DeInit(USART1);
	
	/*usart1 baudrate = 19200, parity = no, stopbit = 1, */
	USART_InitStruct.USART_BaudRate = Baudrate;
	USART_InitStruct.USART_HardwareFlowControl = HardFlow;
	USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStruct);

	/* Enable the USART Receive interrupt: this interrupt is generated when the
   	   USART receive data register is not empty */
   	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	if(g_pcb_version == PCB_CR30_R3)
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	
		USART_DMACmd(USART1,USART_DMAReq_Rx, ENABLE);
	}
	else
	{
		/* Enable the USART1 Interrupt */
	  	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //UART5_IRQChannel; // jsshin 2015.08.18 changed IRQ channel name at the StdLib 3.5
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  	NVIC_Init(&NVIC_InitStructure);
	}
	/* Enable USART1 */
	USART_Cmd(USART1, ENABLE);
}
// UART0 for HOST, no hardflow 
void UART0_initial(uint Baudrate,ushort HardFlow)
{ 	
    UART_InitTypeDef UART_InitStructure;

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);	
	
	UART_InitStructure.UART_BaudRate = Baudrate;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	
	UART_Init(UART1, &UART_InitStructure);

	//UART_AutoFlowCtrlCmd(UART0, ENABLE);

}

/******************************************************************************************************/
/* UART2_initial	:	MMD1000							              */												
/******************************************************************************************************/
// 20150129 hyesun : New Diptype MMD1000 통신 추가
void UART2_initial()//(uint Baudrate)
{
	/* Enable USART2 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	/* Configure USART2 Tx (PB10) as alternate function push-pull */

		GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
		
		/* Configure USART2 Tx (PD5) as push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		/* Configure USART2 Rx (PD6) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

	USART_InitStruct.USART_BaudRate = 9600;//Baudrate;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure the USART2 */
	USART_Init(USART2, &USART_InitStruct);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable USART2 */
	USART_Cmd(USART2, ENABLE);
	//USART_Cmd(USART2, DISABLE);
}

/******************************************************************************************************/
/* UART3_initial	: R3 : RF, other : HOST							       						      */												
/******************************************************************************************************/
void UART3_initial(uint Baudrate,ushort HardFlow)
{ 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/// always [2011/9/27] 초기화 시 gpio를 먼저 시킨다면 노이즈 데이터가 발생
	/// always [2010/11/2] USART3 Tx (PB10), USART1 RTS (PB14) as alternate function push-pull
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 |GPIO_Pin_14;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/// always [2010/11/2] USART3 Rx (PB11), USART1 CTS (PB13) as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_13;
	#if 1		//pbbch 181114 PIO_Mode_IN_FLOATING 설정 시, CR30 Reset 후 RX가 Low로 유지 할 수 있음.따라서  Input PULLup으로 설정 변겅.//pbbch 190329 update  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	#else
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
	#endif
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_DeInit(USART3);
	
	/*usart3 baudrate, parity = no, stopbit = 1, */
	USART_InitStruct.USART_BaudRate = Baudrate;
	USART_InitStruct.USART_HardwareFlowControl = HardFlow;
	USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3,&USART_InitStruct);

	if(g_pcb_version == PCB_CR30_R3)
	{
		/* Enable the USART Receive interrupt: this interrupt is generated when the
	   	   USART receive data register is not empty */
	   	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

		/* Enable the USART1 Interrupt */
	  	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //USART1_IRQChannel; // jsshin 2015.08.18 changed IRQ channel name at the StdLib 3.5
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  	NVIC_Init(&NVIC_InitStructure);
	}
	else
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	
		USART_DMACmd(USART3,USART_DMAReq_Rx, ENABLE);
	}
	/* Enable USART1 */
	USART_Cmd(USART3, ENABLE);

	
}

/******************************************************************************************************/
/* UART4_initial	:	ICC (GEMPOS, CR30)							      						      */												
/******************************************************************************************************/
void UART4_initial(u32 Type)
{ 	
#if 0		//pbbch 181116 CR30 init후 첫 TX byte 유실 부분 보완 코드...1회에 clear 되지만 보완상 max 255번으로 코드 추가.//pbbch 190401 update
	unsigned short i=0;
#endif
	/* Enable UART4 clocks */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

		/// always [2010/11/2] USART4 Tx (PC10), USART5 Tx (PC12) as alternate function push-pull
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
	/// always [2010/11/2] USART4 Rx (PC11) as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	#if 1		//pbbch 181114 PIO_Mode_IN_FLOATING 설정 시, CR30 Reset 후 RX가 Low로 유지 할 수 있음.따라서  Input PULLup으로 설정 변겅.//pbbch 190329 update  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	#else
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	#endif
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	USART_DeInit(UART4);
	
	/*usart1 baudrate = 19200, parity = no, stopbit = 1, */
	USART_InitStruct.USART_BaudRate = Type;	//CR30 or GEMPOS baudrate
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_2;//gempospro stop bit 2
	//USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(UART4,&USART_InitStruct);

	/* Enable the USART Receive interrupt: this interrupt is generated when the
   	   USART receive data register is not empty */
   	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);


	/* Enable the USART4 Interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; //UART4_IRQChannel; // jsshin 2015.08.18 changed IRQ channel name at the StdLib 3.5
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	/* Enable UART4 */
	USART_Cmd(UART4, ENABLE);
#if 0		//pbbch 181116 CR30 init후 첫 TX byte 유실 부분 보완 코드...1회에 clear 되지만 보완상 max 255번으로 코드 추가.//pbbch 190401 update
	USART_GetFlagStatus(UART4, USART_FLAG_TXE);
	USART_GetFlagStatus(UART4, USART_FLAG_TC);
	for(i=0; i<0xff;i++)
	{
		if(USART_GetFlagStatus(UART4, USART_FLAG_TC) == SET) break;
	}

	delay_ms(5);//5
#endif		
}
/******************************************************************************************************/
/* UART5_initial	:	DEBUG       						      */												
/******************************************************************************************************/
void UART5_initial()
{ 	
	/* Enable UART5 clocks */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

		/// always [2010/11/2] USART4 Tx (PC10), USART5 Tx (PC12) as alternate function push-pull
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/// always [2010/11/2] USART5 Rx (PD2) as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	#if 1		//pbbch 190329 pull up update
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	#else
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	#endif
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	USART_DeInit(UART5);
	
	/*usart5 baudrate = 115200, parity = no, stopbit = 1, */
	USART_InitStruct.USART_BaudRate = 115200;
	//USART_InitStruct.USART_BaudRate = 38400;
	//USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(UART5,&USART_InitStruct);

	//uart5_cr1 |= (0x0d<<5);

	/* Enable the USART Receive interrupt: this interrupt is generated when the
   	   USART receive data register is not empty */
   	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);


	/* Enable the USART5 Interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; //UART5_IRQChannel; // jsshin 2015.08.18 changed IRQ channel name at the StdLib 3.5
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART5 */
	USART_Cmd(UART5, ENABLE);
}

/******************************************************************************************************/
/* EXTI_initial	:		FRONT SENSOR, REAR SENSOR 작동에 의한 MS 디코드, CARD EJECT				      */												
/******************************************************************************************************/
void EXTI_initial()
{
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	if(g_pcb_version == PCB_GEM_POS)
	{
		/*
		/// always [2011/3/2] Card insert / Front Sensor interrupt
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource1);
		EXTI_InitStruct.EXTI_Line = EXTI_Line1;
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_Init(&EXTI_InitStruct);
		EXTI_ClearITPendingBit(EXTI_Line1);
		*/
	
		/// always [2011/3/2] Card remove / Rear Sensor interrupt
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);
		EXTI_InitStruct.EXTI_Line = EXTI_Line2;
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_Init(&EXTI_InitStruct);
		EXTI_ClearITPendingBit(EXTI_Line2);
	}
	else
	{
		/// always [2011/3/2] Card remove / Rear Sensor interrupt
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource1);
		EXTI_InitStruct.EXTI_Line = EXTI_Line1;
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_Init(&EXTI_InitStruct);
		EXTI_ClearITPendingBit(EXTI_Line1);
	}

	/// always [2011/3/2] Solenoid UnLock interrupt
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	/// always [2011/3/24] usb 테스트용
	//EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);
	EXTI_ClearITPendingBit(EXTI_Line4);
	
#ifdef	UseUsb
	/// always [2011/3/21] usb
	// jsshin 2015.11.25 : USB_DET 라인은 PE9 이다!!! 오류!!!
	EXTI_ClearITPendingBit(EXTI_Line18);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line18;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	//EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
#endif

	/* Enable the EXT Interrupt */
	if(g_pcb_version == PCB_GEM_POS)
	{
		/*
		// card insert
	  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;
	  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  	NVIC_Init(&NVIC_InitStructure);
		*/	
		// card remove
	  	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  	NVIC_Init(&NVIC_InitStructure);	
	}
	else
	{
		// card remove
	  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  	NVIC_Init(&NVIC_InitStructure);
	}

	// solenoid unLock interrupt
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************************************/
/* DMA_initial	:		used for ADC 						      */
/******************************************************************************************************/
void DMA_initial_org(void)
{

#ifdef USE_UART_DMA
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	DMA_StructInit(&DMA_InitStructure);

	/* DMA1 channel3 configuration ----------------------------------------------*/
	if(g_pcb_version == PCB_CR30_R3)
	{
		DMA_DeInit(DMA1_Channel5);
		DMA_InitStructure.DMA_PeripheralBaseAddr = uart1_DR1;//uart3_DR1;		//pbbch 1712112 volatile 형변환 불가.....
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&DMATEst;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = DMABufferSize;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //DMA_Mode_Normal; // 2016.01.27 changed normal -> circular!
		DMA_InitStructure.DMA_Priority = DMA_Priority_High; 	/* DMA_Priority_Medium */
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		//DMA_ITConfig(DMA1_Channel3, DMA_IT_TE, ENABLE);
		DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	    /* Enable DMA channel1 */
		DMA_Cmd(DMA1_Channel5, ENABLE);
	}
	else
	{
		DMA_DeInit(DMA1_Channel3);
		DMA_InitStructure.DMA_PeripheralBaseAddr = uart3_DR1;					//pbbch 1712112 volatile 형변환 불가.....
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&DMATEst;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = DMABufferSize;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //DMA_Mode_Normal; // 2016.01.27 changed normal -> circular!
		DMA_InitStructure.DMA_Priority = DMA_Priority_High; 	/* DMA_Priority_Medium */
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		//DMA_ITConfig(DMA1_Channel3, DMA_IT_TE, ENABLE);
		DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	    /* Enable DMA channel1 */
		DMA_Cmd(DMA1_Channel3, ENABLE);
	}
#endif

	if(!(g_pcb_version & PCB_GEM_POS))
	{
		// gempos는 ADC 사용안함.
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

		DMA_StructInit(&DMA_InitStructure);

		// DMA1 channel3 configuration ----------------------------------------------//
		DMA_DeInit(DMA1_Channel1);
		DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_1_DR;				//pbbch 1712112 volatile 형변환 불가.....
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_DMA_Buffer;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = (Number_of_ADC*Number_of_Buf);	//pbbch 180503 sen5 추가로 ADC Buffer size 늘림.
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High; 	//// DMA_Priority_Medium ///
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		//DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

		DMA_Init(DMA1_Channel1, &DMA_InitStructure);
		   /// Enable DMA channel1 /
		DMA_Cmd(DMA1_Channel1, ENABLE);
	}
}

//domyst
void DMA_initial(void)
{

	DMA_InitTypeDef DMA_InitStruct;
	
	//memory to memory
	DMA_InitStruct.DMA_DIR = DMA_DIR_Peripheral_To_Memory;
	DMA_InitStruct.DMA_Peripheral = NULL;
	//DMA_InitStruct.DMA_PeripheralBaseAddr = uart1_DR1;					//(uint32_t)&src_Buf[0];
	DMA_InitStruct.DMA_PeripheralBaseAddr = UART0->OFFSET_0.RBR;
    DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Nochange;				//DMA_Inc_Increment;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_Byte;			//DMA_DataSize_Word;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_1;
	

	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)&DMATEst;					//(uint32_t)&dst_Buf[0];
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_Byte;				//DMA_DataSize_Word;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_BlockSize = sizeof(src_Buf) / 4;		// ???
	
	DMA_Init(DMA_Channel_0,&DMA_InitStruct);

	// domyst Enable 위치 확인 필
	DMA_ChannelCmd(DMA_Channel_0, ENABLE);
	//
#ifdef USE_UART_DMA
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	DMA_StructInit(&DMA_InitStructure);

	/* DMA1 channel3 configuration ----------------------------------------------*/
	if(g_pcb_version == PCB_CR30_R3)
	{
		DMA_DeInit(DMA1_Channel5);
		DMA_InitStructure.DMA_PeripheralBaseAddr = uart1_DR1;//uart3_DR1;		//pbbch 1712112 volatile 형변환 불가.....
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&DMATEst;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = DMABufferSize;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //DMA_Mode_Normal; // 2016.01.27 changed normal -> circular!
		DMA_InitStructure.DMA_Priority = DMA_Priority_High; 	/* DMA_Priority_Medium */
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		//DMA_ITConfig(DMA1_Channel3, DMA_IT_TE, ENABLE);
		DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	    /* Enable DMA channel1 */
		DMA_Cmd(DMA1_Channel5, ENABLE);
	}
	else
	{
		DMA_DeInit(DMA1_Channel3);
		DMA_InitStructure.DMA_PeripheralBaseAddr = uart3_DR1;					//pbbch 1712112 volatile 형변환 불가.....
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&DMATEst;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = DMABufferSize;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //DMA_Mode_Normal; // 2016.01.27 changed normal -> circular!
		DMA_InitStructure.DMA_Priority = DMA_Priority_High; 	/* DMA_Priority_Medium */
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		//DMA_ITConfig(DMA1_Channel3, DMA_IT_TE, ENABLE);
		DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	    /* Enable DMA channel1 */
		DMA_Cmd(DMA1_Channel3, ENABLE);
	}
#endif

	if(!(g_pcb_version & PCB_GEM_POS))
	{
		// gempos는 ADC 사용안함.
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

		DMA_StructInit(&DMA_InitStructure);

		// DMA1 channel3 configuration ----------------------------------------------//
		DMA_DeInit(DMA1_Channel1);
		DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_1_DR;				//pbbch 1712112 volatile 형변환 불가.....
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_DMA_Buffer;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = (Number_of_ADC*Number_of_Buf);	//pbbch 180503 sen5 추가로 ADC Buffer size 늘림.
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High; 	//// DMA_Priority_Medium ///
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		//DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

		DMA_Init(DMA1_Channel1, &DMA_InitStructure);
		   /// Enable DMA channel1 /
		DMA_Cmd(DMA1_Channel1, ENABLE);
	}
}
/******************************************************************************************************/
/* ADC_initial	:		AMP 						      */												
/******************************************************************************************************/

void ADC_initial(void)
{
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = Number_of_ADC;				//pbbch 180503 sen5 추가로 ADC Buffer size 늘림.
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channel14 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);//SENSOR1 // LOCK 0
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);//SENSOR2 // REAR 1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5,  3, ADC_SampleTime_55Cycles5);//SENSOR4 // INNERR 2

	#if 1		//pbbch 180503 sen5 추가로 board에 따른 sensor 처리를 변경 해야 함. 따라서 구분자 추가.  
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 4, ADC_SampleTime_55Cycles5);//SENSOR5 // FRONT
	#endif

//	//ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_55Cycles5);//SENSOR3 // FRONT 현재 사용 안함. 센서 미 장착 상태. why???
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 3, ADC_SampleTime_55Cycles5);//SENSOR4	// INNERR
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);//SENSOR1 // LOCK
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);//SENSOR2 //CARD END

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC1 reset calibaration register */
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));

	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/******************************************************************************************************/
/* FLASH_ReadChar	: 저장된 플래시의 char 값을 읽어온다.					      */												
/******************************************************************************************************/
FLASH_Status FLASH_ReadChar(u32 Address, uchar *Data,uchar Leng )
{
	uchar AddrCnt;
  	FLASH_Status status = FLASH_COMPLETE;

  	/* Wait for last operation to be completed */
//  	status = FLASH_WaitForLastOperation(ProgramTimeout); // jsshin 2015.08.18

  	for(AddrCnt = 0; AddrCnt < Leng;AddrCnt++)
  		{
//  			if(status == FLASH_COMPLETE) // jsshin 2015.08.18
  				{
      					*(Data+AddrCnt) = *(vu32*)(Address+AddrCnt) ;

//	 				status = FLASH_WaitForLastOperation(ProgramTimeout); // jsshin 2015.08.18			
 				}
  		}

    	/* Return the Program Status */
 	 return status;
}

/******************************************************************************************************/
/* ReadMasterKey	: 플래시에 저장된 마스터 키를 불러온다.				      */												
/******************************************************************************************************/
void ReadMasterKey(uchar KeyLeng)
{
	FLASH_Unlock();
	FLASH_ReadChar(MasterKeyStorage,MasterKey,KeyLeng);
	FLASH_Lock();
}

/******************************************************************************************************/
/* MasterKeyInit : Des에 사용되는 마스터 키를 초기화 한다.			      */												
/******************************************************************************************************/
void MasterKeyInit()
{
	uchar MasterKeyStat;
	uchar NoData[24];

		
	memset(NoData,0xff,sizeof(uchar) * DES3_USER_KEY_LEN);
	
	ReadMasterKey(DES3_USER_KEY_LEN);

	MasterKeyStat = memcmp(MasterKey,NoData,DES3_USER_KEY_LEN);
	
	if(!MasterKeyStat)
	{
		memcpy(MasterKey,Defaultkey,(sizeof(uchar)*DES3_USER_KEY_LEN));
	}
}
/******************************************************************************************************/
/* ReadMasterKey	: 플래시에 저장된 마스터 키를 불러온다.				      */												
/******************************************************************************************************/
void ReadFlashData(uchar *Data,uchar KeyLeng)
{
//	FLASH_Unlock(); // jsshin 2015.08.18
	FLASH_ReadChar(MasterKeyStorage,Data,KeyLeng);
//	FLASH_Lock(); // 2015.08.18
}

void FlashSettingInit(uchar FlashInit)
{
	#if 1		//pbbch 171212 warning across
	uchar MasterKeyStat, SerialNumState,NoData[FlashDataSize]; //,TempCnt,usbCnt=0;
	#else
	uchar MasterKeyStat,FlashInitState, SerialNumState,NoData[FlashDataSize]; //,TempCnt,usbCnt=0;
	#endif
	uchar *pTempFlashData = TempFlashData.MasterDesKey;	
	#if defined(UseUsb)		//pbbch 180209 UseUsb define add....warning across
	uchar TempCnt,usbCnt=0;
	#endif

	ReadFlashData(pTempFlashData,FlashDataSize); // FlashDataSize is defined 43
	
	memset(NoData,0xff,sizeof(uchar) * DES3_USER_KEY_LEN);
	//memset(NoData,0xff,sizeof(uchar) * FlashDataSize);
#if 1
	MasterKeyStat = memcmp(TempFlashData.MasterDesKey,NoData,DES3_USER_KEY_LEN);
	SerialNumState = memcmp(TempFlashData.SerialNumber,NoData,7);
	
	if(!MasterKeyStat)
	{
		memcpy(MasterKey,Defaultkey,(sizeof(uchar)*DES3_USER_KEY_LEN));
		memcpy(TempFlashData.MasterDesKey,Defaultkey,(sizeof(uchar)*DES3_USER_KEY_LEN));
	}
	else
	{
		memcpy(MasterKey,TempFlashData.MasterDesKey,(sizeof(uchar)*DES3_USER_KEY_LEN));
	}

	if(!SerialNumState)
	{
		memset(TempFlashData.SerialNumber,0x30,sizeof(uchar) * 7);
	}
	#if defined(UseUsb)
	else
	{
		for(TempCnt = 0;TempCnt < 7;TempCnt++)
		{
			CustomHID_StringSerial[2+usbCnt] = TempFlashData.SerialNumber[TempCnt];
			usbCnt +=2;
		}
	}
	#endif
#endif
	if(TempFlashData.ModuleType== 0xff)
	{
		TempFlashData.ModuleType = USEALL;
	}	
	if(TempFlashData.IFM_RetryCnt_INComm == 0xff)
	{
		TempFlashData.IFM_RetryCnt_INComm = 3;
	}
	if(TempFlashData.IFM_RetryCnt_Respons== 0xff)
	{
		TempFlashData.IFM_RetryCnt_Respons = 3;
	}	
	if(TempFlashData.IFM_WatingTime_Respons== 0xffff)
	{
		TempFlashData.IFM_WatingTime_Respons = 30000;//5s
	}
	if(TempFlashData.IFM_WatingTime_INComm == 0xffff)
	{
		TempFlashData.IFM_WatingTime_INComm = 5000;//5s
	}	
	if(TempFlashData.Main_WatingTime_INComm == 0xffff)
	{
		#if 1		//pbbch 180220 when protocol length error or etc erro occure,  time out occure
		TempFlashData.Main_WatingTime_INComm = 0;//2395;//about 3s
		#else
		/// always [2011/12/12] 500ms 로 변경
		TempFlashData.Main_WatingTime_INComm = 5000;//5s
		#endif
	}
	if(TempFlashData.BoardCheckFlag == 0xff)
	{
		TempFlashData.BoardCheckFlag = RESET;
	}
	if(TempFlashData.AutoLocktime == 0xff)
	{
#ifdef Hanmega_USA
		TempFlashData.AutoLocktime = 5;//5분 Lock
#else
		TempFlashData.AutoLocktime = 2;//2분 Lock
#endif
	}
#ifdef Hanmega_Russia
	TempFlashData.AutoLocktime = 5;//5분 Lock
#endif
	SolCnt = TempFlashData.AutoLocktime*60;
	
#ifdef Hanmega_Russia
	cmd_format.SN = TempFlashData.SN_hanmega;
#endif
	
	// add jsshin 2016.01.29
	//  memset(TempFlashData.SerialNumber,0x30,sizeof(uchar) * 7);

	if(FlashInit == SET)
	{
#if 1 
		memcpy(MasterKey,Defaultkey,(sizeof(uchar)*DES3_USER_KEY_LEN));
		memcpy(TempFlashData.MasterDesKey,Defaultkey,(sizeof(uchar)*DES3_USER_KEY_LEN));
#endif
		TempFlashData.IFM_RetryCnt_INComm = 2;
		TempFlashData.IFM_RetryCnt_Respons = 2;
		TempFlashData.IFM_WatingTime_Respons = 30000;// 3s
		TempFlashData.IFM_WatingTime_INComm = 5000;// 3s
		#if 1		//pbbch 180220 when protocol length error or etc erro occure,  time out occure
		TempFlashData.Main_WatingTime_INComm = 0;//2395;//about 3s
		#else
		/// always [2011/12/12] 500ms 로 변경
		TempFlashData.Main_WatingTime_INComm = 5000;//
		#endif
		TempFlashData.ModuleType = USEALL;
		TempFlashData.BoardCheckFlag = RESET;
		
#ifdef Hanmega_USA
		TempFlashData.AutoLocktime = 5;//5분 Lock
#else
		TempFlashData.AutoLocktime = 2;//2분 Lock
#endif
		
#ifdef Hanmega_Russia
		TempFlashData.SN_hanmega = 0;
#endif
		
		memset(TempFlashData.SerialNumber,0x30,sizeof(uchar) * 7);
		
		FlashDataWrite();
	}
}
/******************************************************************************************************/
/* MasterKeyInit : Des에 사용되는 마스터 키를 초기화 한다.			      */												
/******************************************************************************************************/
void FlashDataInit()
{
	
//	uchar TestCnt;
	
	//FlashSettingInit(SET); // SET : 무조건 초기화
	FlashSettingInit(RESET); // RESET : 저장된 값 읽어서 비교, 틀리면 리셋.

	//Uart_DBG_Printf("Main_WatingTime_INComm = %d\n",TempFlashData.Main_WatingTime_INComm);

}
/******************************************************************************************************/
/* SetBaudrate	:	보드레이트 설정					      */												
/******************************************************************************************************/
ushort SetHardwareFlow(uchar HardwareFlowStat)
{
	switch(HardwareFlowStat)
	{
	case 0x00:
		return USART_HardwareFlowControl_None;

	case 0x04:
		return USART_HardwareFlowControl_CTS;
	
	case 0x08:
		return USART_HardwareFlowControl_RTS;

	case 0x0c:
		return USART_HardwareFlowControl_RTS_CTS;
	default:
		return USART_HardwareFlowControl_None;
	}
		
}
/******************************************************************************************************/
/* SetBaudrate	:	보드레이트 설정					      */												
/******************************************************************************************************/
uint SetBaudrate(uchar StatBaud)
{
	switch(StatBaud)
	{
	case 0x00:
		return 9600;

	case 0x01:
		return 19200;
	
	case 0x02:
		return 38400;

	case 0x03:
		return 57600;
	default:
		return 19200;
	}
		
}

void Check_RF_Con()
{
	#ifdef RF_NFC
	uchar CmdData[5] = {0x02,0x00,0x01,0x10,0x11};//듀얼아이,   CmdData[8] = {0x02,0x00,0x04,0x20,0x02,0x64,0x42,0x03};
	ushort CmdDatasize =5;
	#else
	uchar CmdData[8] = {0x02,0x00,0x04,0x20,0x02,0x64,0x42,0x03};
	ushort CmdDatasize =8;
	#endif
	uchar RecieveData[5];
	ushort RecDataSize =0;
	uchar Result =0;
	uchar ResendCnt =0;

	for(ResendCnt = 0;ResendCnt < 2;ResendCnt++)
	{
		#if 1		//pbbch 171212 across warning...RecDataSize 주소값 전달 하면 문제 없는 것으로 판단 됨. 적용.//pbbch 180321 waing time 조정을 위해 수정.
		Result = F_CmdProc(CmdData,CmdDatasize,RecieveData,&RecDataSize,0);	//250ms
		#else
		Result = F_CmdProc(CmdData,CmdDatasize,RecieveData,RecDataSize);
		#endif
		if(Result == PRespons)
		{
			break;
		}
	}
}
/******************************************************************************************************/
/* OptionByte_initial	:	uart 통신 속도, 펌웨어 업데이트, 리셋 명령 플래그 확인			      */												
/******************************************************************************************************/
void OptionByte_initial(void)
{
	uchar OptionByte;
	uchar BaudrateStat;
	uint Baudrate;
	uchar HardwareFlowStat;
	ushort HardwareFlow;
	uchar LockuseFlag;
#if 0		//pbbch 171212 warning across
	uchar ResetFlag;
	uchar DownloadFlag;

	uchar Dummy[1];
#endif	

	

	OptionByte = (uchar)bOptionByte & 0xff  ;

	if(OptionByte == 0xff)
	{
		OptionByte = 0x10;		//option byte : 0x10 0xff
		FLASH_Unlock();
		FLASH_EraseOptionBytes();					
		FLASH_ProgramOptionByteData(0x1ffff804,OptionByte);
	}
	
	BaudrateStat = (OptionByte >> 4)&(0x03);
	HardwareFlowStat = (OptionByte >> 4)&(0x0c);
	LockuseFlag = (OptionByte >> 2) & (0x1);
#if 0		//pbbch 171212 warning across	
	ResetFlag = (OptionByte >> 1) & (0x1);
	DownloadFlag = OptionByte & 0x1;
#endif	

	/// always [2011/3/2] 메인 통신 설정
	Baudrate = SetBaudrate(BaudrateStat);
	HardwareFlow = SetHardwareFlow(HardwareFlowStat);
	if(g_pcb_version == PCB_CR30_R3)
	{
		UART1_initial(Baudrate,HardwareFlow);
	}
	else
	{
		UART3_initial(Baudrate,HardwareFlow);
	}

	LockUse = LockuseFlag;

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
			/// always [2011/5/4] usb 통신시 응답
			//pbbch 180201 option byte init response는 init 명령 이후 발행 하나, usb 연결이 안되면 실행 안함.
			//또한 protocol select는 단말기 자의적이 아닌 host 첫번째 통신 시 선택이 되어야 하므로 변수 초기화 시킴.
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
			/// always [2011/5/4] uart 통신 시 응답.
			Resp_DATA_proc(Dummy,0);
		
			OptionByte &= ~(0x01 <<1);
		}
		if(DownloadFlag == SET)
		{
			OptionByte &= ~(0x01);
		}
		
		FLASH_Unlock();////20140721 hyesun 처리 위치 변경
		FLASH_EraseOptionBytes();////20140721 hyesun 처리 위치 변경					
		FLASH_ProgramOptionByteData(0x1ffff804,OptionByte);////20140721 hyesun 처리 위치 변경
	}
}

/******************************************************************************************************/
/* STAT_initial	:	status 초기 설정 및 주변장치 power on       						      */												
/******************************************************************************************************/
void STAT_initial()
{ 	
	SENSOR_ON;		// SENSOR POWER ON

	//-----------------------------------
	// IFM module POWER ON
	//-----------------------------------
	#if 0		//pbbch 190319 cr30 초기화 위치  수정.		//pbbch 190329 update
	CR30_RESET_ON;	// CR30_nRESET(PB1) set LOW
	IFM_ON; 		// IFM_PWR_CTRL(PC9) set HIGH
	#endif
	
	#if 0		//pbbch 180320 eject 시점에서는 CR30의 Card detect signal을 high를 주어 card가 없음을 알려 detective를 선진행 해야 한다.
	IFM_ICC_PRESENT;
	#endif
	
	delay_ms(1);
	
	STAT_B = 0;
	STAT2_B = 0;

	ErrorFlag = 0;

	InitSensor = SET;
	/// always [2010/11/18] stat 값 변경
	NO_ICC_Reset;
	Non_MS_DATA;

	/// always [2011/1/31] Sub Key 설정 전에는 off상태
	UseDesOFF;		// 확인 ???
	
	//MasterKeyInit();
	

	/// always [2011/5/17] flash 저장 정보 확인
	
}

#ifdef	UseUsb

/*******************************************************************************
* Function Name  : USB_Interrupts_Config.
* Description    : Configures the USB interrupts.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


  NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz).
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
  /* Select USBCLK source */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

  /* Enable USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode.
* Description    : Power-off system clocks and power while entering suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
  bDeviceState = SUSPENDED;
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : Restores system clocks and power while exiting suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else
  {
    bDeviceState = ATTACHED;
  }
}

/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
#if defined(UseUsb)
void Get_SerialNum(void)
{

  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

  Device_Serial0 = *(uint32_t*)(0x1FFFF7E8);
  Device_Serial1 = *(uint32_t*)(0x1FFFF7EC);
  Device_Serial2 = *(uint32_t*)(0x1FFFF7F0);

  if(Device_Serial0 != 0)
  {
     CustomHID_StringSerial[2] = (uint8_t)(Device_Serial0 & 0x000000FF);
     CustomHID_StringSerial[4] = (uint8_t)((Device_Serial0 & 0x0000FF00) >> 8);
     CustomHID_StringSerial[6] = (uint8_t)((Device_Serial0 & 0x00FF0000) >> 16);
     CustomHID_StringSerial[8] = (uint8_t)((Device_Serial0 & 0xFF000000) >> 24);

     CustomHID_StringSerial[10] = (uint8_t)(Device_Serial1 & 0x000000FF);
     CustomHID_StringSerial[12] = (uint8_t)((Device_Serial1 & 0x0000FF00) >> 8);
     CustomHID_StringSerial[14] = (uint8_t)((Device_Serial1 & 0x00FF0000) >> 16);
     CustomHID_StringSerial[16] = (uint8_t)((Device_Serial1 & 0xFF000000) >> 24);

     CustomHID_StringSerial[18] = (uint8_t)(Device_Serial2 & 0x000000FF);
     CustomHID_StringSerial[20] = (uint8_t)((Device_Serial2 & 0x0000FF00) >> 8);
     CustomHID_StringSerial[22] = (uint8_t)((Device_Serial2 & 0x00FF0000) >> 16);
     CustomHID_StringSerial[24] = (uint8_t)((Device_Serial2 & 0xFF000000) >> 24);
  }
}
#endif

#if 1		//pbbch 180102 usb connect status function add
unsigned char Usb_connect_status_check(void)
{
	unsigned char i=0;
	unsigned int ldelay=0;
	unsigned char lret=0;

	for(i=0; i<3;i++)
	{
		if(!USBConStat) break;
		for(ldelay=0; ldelay<36000;ldelay++);		//if cpu clodk is 72Mhz, about 1ms setting
	}

	if(i<3) lret=1;									//usb connect fail

	return lret;
}
#endif
void WaitingUSBCon(uchar RetryMax,ushort Time)
{
	ushort WaitTime;
	uchar ReConnect = 0;
	USB_Con_ON;
	/// always [2011/12/27] 재시도
	if(USBConStat == SET)
	{
		for(ReConnect =0;ReConnect  < RetryMax;ReConnect++)
		{
			
			for(WaitTime = 0;WaitTime < Time; WaitTime++)
			{
				if((bDeviceState == ADDRESSED)&&(USBConStat == SET))
				{
					break;
				}
				delay_ms(10);
			}
			/// always [2011/12/27] USB 초기화 성공
			if(WaitTime < Time)
			{
				break;
			}
			else
			{
				/// always [2011/12/27] 실패시 제 시도
				USB_Con_OFF;
				delay_ms(10);
				#if 1		//pbbch 180129 copy boot code...it is usb reset signal genarate
				USB_Con_ON;
				#endif
			}
		}
	}
	USB_Con_ON;				//pbbch 180129 usb d+ line should connect pull up. so we must trun on tr. 0: turn on, 1: turn off
}

#if 1 		//pbbch 180129 we need to know usb connect or not connect. so function add
void init_usb_configure_exe(void)
{
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
	WaitingUSBCon(3,300);
	gusb_protect.configured_flag=1;
}
#endif

#endif


#if 1		//pbbch 180102 void add
u8 PCB_Check(void)
#else
u8 PCB_Check()
#endif
{
	GPIO_InitTypeDef GPIO_InitStructure;
	int i;
	u8 rev = 0;
	
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB), ENABLE);
	/***********************************
	// USART1 : GPIOA Pin 9 Input pull-down
	***********************************/

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	for(i=0;i<0xff;i++) asm("NOP");
	if(GPIOA->IDR & GPIO_Pin_9) rev = PCB_CR30_R3;
	
	if(rev) return rev;

	/***********************************
	// USART3 : GPIOB Pin 10 Input pull-down
	***********************************/

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	for(i=0;i<0xff;i++) asm("NOP");
	if(GPIOB->IDR & GPIO_Pin_10) rev = PCB_CR30_R2;
	
	if(rev) return rev;

	/***********************************
	// RENEWAL : FRONT SENSOR, OLD : NC
	// GPIOA Pin 4 Input pull-down
	***********************************/
	
	GPIO_InitStructure.GPIO_Pin =  (GPIO_Pin_4);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	for(i=0;i<0xff;i++) asm("NOP");
	if(GPIOA->IDR & GPIO_Pin_4) 
	{
		rev = PCB_CR30_R1;
	}
	else 
	{
		rev = PCB_GEM_POS;
	}
	
	return rev;
  
}

#if 1		//pbbch 180503 sen5 추가로 board에 따른 sensor 처리를 변경 해야 함. 따라서 구분자 추가.  
// 00or etc: //sen1:Lock, sen2: END, Sen3: Front or anti, Sen4: Inner
// 01://sen1:Lock, sen2: END, Sen3: anti, Sen4: Inner, Sen5: Front
unsigned char read_sen_array_type(void)
{
	unsigned char lread_gpio=0;
	unsigned char lsen_array_type=0;
	
	lread_gpio=GPIO_ReadInputData(GPIOD);
	lsen_array_type=(unsigned char)(lread_gpio&0x03);

	return lsen_array_type;
}
#endif

#if defined(USE_IWDG_RESET)
void Watchdog_initial(void)
{
  /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Prescaler_256);

  /* Set counter reload value to obtain 250ms IWDG TimeOut.
     Counter Reload Value = 250ms/IWDG counter clock period
                          = 250ms / (LSI/32)
                          = 0.25s / (LsiFreq/32)
                          = LsiFreq/(32 * 4)
                          = LsiFreq/128
   */
  //IWDG_SetReload(0xFFF);//4095 26214.4ms
  IWDG_SetReload(0x7FF);//2047 13107.2ms

  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
}
#endif




/******************************************************************************************************/
/* CORTEX_initial	:		        						      */												
/******************************************************************************************************/
void CORTEX_initial_org(void)
{
//	ushort Datasize = 0;
	PLL_initial();		// PLL Initial
	NVIC_initial();               // vector initial

#if 1 //pbbch 180129 usb protect logic add
	memset(&gusb_protect.detect,0x00,sizeof(gusb_protect));
	gmain_process_status=0;			//pbbch 180201 reset 이후 초기화 문제로 인하여 추가.
#endif	
	
	g_pcb_version = PCB_Check(); // jsshin 2016.03.03: add New PCB check

	GPIO_port_initial();		// gpio initial

#if 1		//pbbch 180503 sen5 추가로 board에 따른 sensor 처리를 변경 해야 함. 따라서 구분자 추가.  
	gsen_array_type= read_sen_array_type();
	//gsen_array_type=SEN12345_ARRAY;			//pbbch 180510 test용도
#endif
#if 0	//pbbch 180129 usb con on set....DP line must do pullup
	USB_Con_OFF; // USB 사용 안함???
#endif
	
	if(g_pcb_version == PCB_CR30_R3)
	{
		#if defined(USE_REAL_EEPROM)
		IIC_initial();
	 	AT24Cxx_FindDevice();

		#if 0
		uchar Data[8192];
		memset(Data, 0x00, 8192);
		AT24Cxx_Read(0, Data, 8192); // temp
		/*
		__disable_irq();
		memset(Data, 0x55, 1024);
		AT24Cxx_Write(0x1000, Data, 1024); // temp
		memset(Data, 0x00, 1024);
		AT24Cxx_Read(0x1000, Data, 1024); // temp

		memset(Data, 0xAA, 1024);
		AT24Cxx_Write(0x1000, Data, 1024); // temp
		memset(Data, 0x00, 1024);
		AT24Cxx_Read(0x1000, Data, 1024); // temp

		memset(Data, 0xFF, 1024);
		AT24Cxx_Write(0x1000, Data, 1024); // temp
		memset(Data, 0x00, 1024);
		AT24Cxx_Read(0x1000, Data, 1024); // temp
		memset(Data, 0x00, 1024);
		__enable_irq();

		while(1);
		*/
		#endif
		#endif
	}
	STAT_initial();
	
#ifdef USE_RF
	if(g_pcb_version == PCB_CR30_R3)
	{
		
		#ifdef RF_NFC
		UART3_initial(115200, 0);
		#else
		UART3_initial(9600, 0); //PCB == R3 : RF
		#endif
	}
	else
	{
		#ifdef RF_NFC
		UART1_initial(115200, 0);
		#else
		UART1_initial(9600, 0); //PCB == R3 : RF
		#endif
	}
#endif
	UART4_initial(BAUDRATE_9600); //IFM
	UART5_initial(); //DBG
#if defined(DEBUG)		//pbbch 181011 test add
	printf("initqqq\r\n");
#endif	
	
	TIME1_initial();//Track2
	//TIME2_initial();// for Anti-Skimming Sol, reserved!
	TIME3_initial();//Sol
	TIME4_initial();//Track 1
	TIME5_initial();//Track 3
	TIME6_initial();//comm Time Out

	#if 0		//pbbch 180207 ms decoder chip reset signal set move.
	//TR6201_RESET_ON;//TR6201 Reset
	//delay_ms(10);
	TR6201_RESET_OFF;//TR6201 Reset
	#endif
	
	//Gp_initial();
#if 1		//pbbch 180102 usb connect status function add
	#ifdef	UseUsb

	#if 1	//pbbch 180129 usb protect logic add
		protect_usb_detect();
		if(gusb_protect.detect) init_usb_configure_exe();
	#else//pbbch 180102 usb connect status function add
		if(!Usb_connect_status_check())
		{
			USB_Interrupts_Config();
			Set_USBClock();
			USB_Init();
			/// always [2011/12/27] USB 연결 시 연결 될때 까지 대기
			WaitingUSBCon(3,300);
		}
	#endif

	#else		//pbbch 180123 neicp code add
	USB_Con_OFF;
	//USB_Con_ON;
	#endif
#else
	#ifdef	UseUsb
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
	/// always [2011/12/27] USB 연결 시 연결 될때 까지 대기
	WaitingUSBCon(3,300);
	#endif
#endif

	FlashDataInit();
	OptionByte_initial();	//HOST comm initialize
	
#ifdef USE_RF
	Check_RF_Con();
#endif

	/// 20160307 hyesun : DMA_initial 후에 ADC_initial 해야 순서대로 ADC DMA 함.
	DMA_initial();
	if(g_pcb_version != PCB_GEM_POS)
	{
		ADC_initial();
	}
	DMAInit();
	EXTI_initial();
	
	SYSTICK_initial();		// shutter 2sec time out
#if 0		//pbbch 180319 avoid protocol error, move init response posion	
	OptionByte_initial_Response();	//응답
#endif
	
#if defined(USE_IWDG_RESET)
	Watchdog_initial();
#endif

#if defined(USE_ICC_115200)		//pbbch 180123 neicp code add
	#if 0
	SAM_initial();
	#else
		#if 1		//pbbch 190319 cr30 초기화 위치  수정.//pbbch 190329 update//pbbch 190401 power on reset 부분 SAM_initial 함수로 merge
					//pbbch 190412 power reset 최오 1회로 fix.
		IFM_OFF;
		delay_ms(300); // 50ms 지연.
		IFM_ON; 
		delay_ms(300); // 300ms 지연.
		#endif
		u8 ctr=3;
		while(ctr--)
		{
			if(SAM_initial()) break;
			delay_ms(10);
		}
	#endif
#endif  

	#if 1		//pbbch 180207 ms decoder chip reset signal low setting
	delay_ms(1);		//delay need minimum 100us after power on 
	GPIO_SetBits(GPIOD,GPIO_Pin_10);
	#endif
}

// mh1903 system init
void CORTEX_initial(void)
{
//	ushort Datasize = 0;
	// PLL_initial();		// PLL Initial
	// NVIC_initial();               // vector initial

	// System init
	SYSCTRL_SYSCLKSourceSelect(SELECT_INC12M);		// internal clock
	SYSCTRL_PLLConfig(SYSCTRL_PLL_192MHz);
	SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
//	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);	/* PCLK >= 48M */

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE);

	SYSTICK_Init();

	// NVIC_Configuration();

#if 1 //pbbch 180129 usb protect logic add
	memset(&gusb_protect.detect,0x00,sizeof(gusb_protect));
	gmain_process_status=0;			//pbbch 180201 reset 이후 초기화 문제로 인하여 추가.
#endif	
	
	// domyst
	//g_pcb_version = PCB_Check(); // jsshin 2016.03.03: add New PCB check

	GPIO_port_initial();		// gpio initial

#if 1		//pbbch 180503 sen5 추가로 board에 따른 sensor 처리를 변경 해야 함. 따라서 구분자 추가.  
	gsen_array_type= read_sen_array_type();
	//gsen_array_type=SEN12345_ARRAY;			//pbbch 180510 test용도
#endif
#if 0	//pbbch 180129 usb con on set....DP line must do pullup
	USB_Con_OFF; // USB 사용 안함???
#endif
	#if 0 // domyst
	if(g_pcb_version == PCB_CR30_R3)
	{
		#if defined(USE_REAL_EEPROM)
		IIC_initial();
	 	AT24Cxx_FindDevice();

		#if 0
		uchar Data[8192];
		memset(Data, 0x00, 8192);
		AT24Cxx_Read(0, Data, 8192); // temp
		/*
		__disable_irq();
		memset(Data, 0x55, 1024);
		AT24Cxx_Write(0x1000, Data, 1024); // temp
		memset(Data, 0x00, 1024);
		AT24Cxx_Read(0x1000, Data, 1024); // temp

		memset(Data, 0xAA, 1024);
		AT24Cxx_Write(0x1000, Data, 1024); // temp
		memset(Data, 0x00, 1024);
		AT24Cxx_Read(0x1000, Data, 1024); // temp

		memset(Data, 0xFF, 1024);
		AT24Cxx_Write(0x1000, Data, 1024); // temp
		memset(Data, 0x00, 1024);
		AT24Cxx_Read(0x1000, Data, 1024); // temp
		memset(Data, 0x00, 1024);
		__enable_irq();

		while(1);
		*/
		#endif
		#endif
	}
	#endif // domyst
	
	STAT_initial();
	
#ifdef USE_RF
	if(g_pcb_version == PCB_CR30_R3)
	{
		
		#ifdef RF_NFC
		UART3_initial(115200, 0);
		#else
		UART3_initial(9600, 0); //PCB == R3 : RF
		#endif
	}
	else
	{
		#ifdef RF_NFC
		UART1_initial(115200, 0);
		#else
		UART1_initial(9600, 0); //PCB == R3 : RF
		#endif
	}
#endif
	// UART4_initial(BAUDRATE_9600); //IFM
	// UART5_initial(); //DBG

	IFM_Configuration();	// IFM configuration
	SAM_Configuration();
	UART_Configuration();	// UART0,1,2
	TIMER_Configuration();	// Timer0 init
	NVIC_Configuration();	// Timer0 Interrupt enable

#if defined(DEBUG)		//pbbch 181011 test add
	printf("initqqq\r\n");
#endif	
	
	#if 0 // domyst
	TIME1_initial();//Track2
	//TIME2_initial();// for Anti-Skimming Sol, reserved!
	TIME3_initial();//Sol
	TIME4_initial();//Track 1
	TIME5_initial();//Track 3
	TIME6_initial();//comm Time Out
	#endif 
	
	#if 0		//pbbch 180207 ms decoder chip reset signal set move.
	//TR6201_RESET_ON;//TR6201 Reset
	//delay_ms(10);
	TR6201_RESET_OFF;//TR6201 Reset
	#endif
	
	//Gp_initial();
#if 1		//pbbch 180102 usb connect status function add
	#ifdef	UseUsb

	#if 1	//pbbch 180129 usb protect logic add
		protect_usb_detect();
		if(gusb_protect.detect) init_usb_configure_exe();
	#else//pbbch 180102 usb connect status function add
		if(!Usb_connect_status_check())
		{
			USB_Interrupts_Config();
			Set_USBClock();
			USB_Init();
			/// always [2011/12/27] USB 연결 시 연결 될때 까지 대기
			WaitingUSBCon(3,300);
		}
	#endif

	#else		//pbbch 180123 neicp code add
	USB_Con_OFF;
	//USB_Con_ON;
	#endif
#else
	#ifdef	UseUsb
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
	/// always [2011/12/27] USB 연결 시 연결 될때 까지 대기
	WaitingUSBCon(3,300);
	#endif
#endif

	FlashDataInit();
	OptionByte_initial();	//HOST comm initialize
	
#ifdef USE_RF
	Check_RF_Con();
#endif

	/// 20160307 hyesun : DMA_initial 후에 ADC_initial 해야 순서대로 ADC DMA 함.
	DMA_initial();
	if(g_pcb_version != PCB_GEM_POS)
	{
		ADC_initial();
	}
	DMAInit();
	EXTI_initial();
	
	SYSTICK_initial();		// shutter 2sec time out
#if 0		//pbbch 180319 avoid protocol error, move init response posion	
	OptionByte_initial_Response();	//응답
#endif
	
#if defined(USE_IWDG_RESET)
	Watchdog_initial();
#endif

#if defined(USE_ICC_115200)		//pbbch 180123 neicp code add
	#if 0
	SAM_initial();
	#else
		#if 1		//pbbch 190319 cr30 초기화 위치  수정.//pbbch 190329 update//pbbch 190401 power on reset 부분 SAM_initial 함수로 merge
					//pbbch 190412 power reset 최오 1회로 fix.
		IFM_OFF;
		delay_ms(300); // 50ms 지연.
		IFM_ON; 
		delay_ms(300); // 300ms 지연.
		#endif
		u8 ctr=3;
		while(ctr--)
		{
			if(SAM_initial()) break;
			delay_ms(10);
		}
	#endif
#endif  

	#if 1		//pbbch 180207 ms decoder chip reset signal low setting
	delay_ms(1);		//delay need minimum 100us after power on 
	GPIO_SetBits(GPIOD,GPIO_Pin_10);
	#endif
}
/******************* (C) COPYRIGHT 2007 INSEM Inc ***************************************END OF FILE****/
