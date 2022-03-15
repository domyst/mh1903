#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "mhscpu_conf.h"
#include "systick.h"
#include "xpt2046.h"
#include "ili9488.h"
#include "beep.h"
#include "picture.h"
#include "calibration.h"
#include "fnd.h"
#include "ProjectConfig.h"
//#include "DecodeLib.h"
#include "test_emv.h"
#include "emv_core.h"
#include "iso7816_3.h"
#include "rtc.h"
#include "sound.h"
#include "uart.h"
#include "CreditCard_lib.h"

#include "mhscpu_qspi.h"
#define USE_EXTERN_32K

#if QFN88	// domyst
#define VCC_5V_3V_PORT	GPIOH					//GPIOA
#define VCC_1_8V_PORT	GPIOH					//GPIOF
#define VCC_5V_3V_PIN	GPIO_Pin_1				//GPIO_Pin_11
#define VCC_1_8V_PIN	GPIO_Pin_0				//GPIO_Pin_8	
#else
#define VCC_5V_3V_PORT	GPIOA
#define VCC_1_8V_PORT	GPIOF
#define VCC_5V_3V_PIN	GPIO_Pin_11
#define VCC_1_8V_PIN	GPIO_Pin_8
#endif
/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

void UART_Configuration(void);
void GPIO_Configuration(void);
//void KEY_Configuration(void);
//void QR_Configuration(void);
void SCI_Configuration(void);
//void DAC_Configuration(void);
//void RTC_Configuration(void);

//void MSR_test(void);
//void QR_Test(void);
//void DAC_Test(void);
//void RTC_Test(void);

uint8_t			Valid_Credit_Number[200]="";
uint8_t			ValidNumber[5]="";
uint8_t			CreditNumber[100]="";
uint32_t		Valid_Credit_Len=0;

uart_t uart;

unsigned char pool[DECODE_BUFF_SIZE] = {0};

// added by domyst
#define ADDED_BY_DOMYST		1
#if ADDED_BY_DOMYST
uint32_t TIMM0_GetTick(uint32_t TIMx);
volatile uint32_t tick_Timer0[TIM_NUM] = {0};
#endif

#if 0//QFN88
void Select_EMV_VCC(void)
{
/*	
#define VCC_5V_3V_PORT	GPIOH					//GPIOA
#define VCC_1_8V_PORT	GPIOH					//GPIOF
#define VCC_5V_3V_PIN	GPIO_Pin_1				//GPIO_Pin_11
#define VCC_1_8V_PIN	GPIO_Pin_0				//GPIO_Pin_8	
*/	
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;		//GPIO_Pin_1
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);


	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;		//GPIO_Pin_8
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
 	GPIO_SetBits(VCC_1_8V_PORT,VCC_1_8V_PIN);					//In order to use the VCC_5V_3V, you have to set the 1.8V Pin to High.
//	GPIO_ResetBits(VCC_1_8V_PORT,VCC_1_8V_PIN);					//In order to use the 1.8V, you have to set the VCC_5V_3V Pin to High.
	
	GPIO_SetBits(VCC_5V_3V_PORT,VCC_5V_3V_PIN);		//5V select	
//	GPIO_ResetBits(VCC_5V_3V_PORT,VCC_5V_3V_PIN);		//3V select

}

void IFM_Power_On(uint8_t data)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	if (data == 1)
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
	else
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
}
#else
void Select_EMV_VCC(void)
{
/*	
#define VCC_5V_3V_PORT	GPIOA
#define VCC_1_8V_PORT	GPIOF
#define VCC_5V_3V_PIN	GPIO_Pin_11
#define VCC_1_8V_PIN	GPIO_Pin_8	
*/	
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);


	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
 	GPIO_SetBits(VCC_1_8V_PORT,VCC_1_8V_PIN);					//In order to use the VCC_5V_3V, you have to set the 1.8V Pin to High.
//	GPIO_ResetBits(VCC_1_8V_PORT,VCC_1_8V_PIN);					//In order to use the 1.8V, you have to set the VCC_5V_3V Pin to High.
	
	GPIO_SetBits(VCC_5V_3V_PORT,VCC_5V_3V_PIN);		//5V select	
//	GPIO_ResetBits(VCC_5V_3V_PORT,VCC_5V_3V_PIN);		//3V select

}
#endif


#if 0
int main_org(void)
{
	TouchPositionTypeDef press_position;
	TouchPositionTypeDef release_position_pre, release_position;

	uint16_t	i;
	uint16_t	menu_select_press = 0;
	uint16_t	menu_select_release = 0;

	//SYSCTRL_SYSCLKSourceSelect(SELECT_EXT12M);	// SELECT_INC12M
	SYSCTRL_SYSCLKSourceSelect(SELECT_INC12M);		// domyst
	//SYSCTRL_PLLConfig(SYSCTRL_PLL_204MHz);
	SYSCTRL_PLLConfig(SYSCTRL_PLL_192MHz);
	SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
//	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);	/* PCLK >= 48M */
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_GPIO, ENABLE);
	
	SYSTICK_Init();	
	
	UART_Configuration();
	TOUCH_Configuration();
	LCD_Configuration();
	BEEP_Configuration();
	KEY_Configuration();
	FND_Configuration();
//	MSR_Configuration();
	QR_Configuration();
	SCI_Configuration();
	DAC_Configuration();
	RTC_Configuration();

	//By Psk: Led Off
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);

	FND_BIT_Write(0x00);
	printf("CSS Demo V1.1.\r\n");	// domyst

	memcpy(lcd_buf, IMG_CSS_CI, sizeof(lcd_buf));
	LCD_DisplayPicDMA(lcd_buf);			//320x240

	beep(200);
	mdelay(2000);

	LCD_DisplayColor(LCD_DISP_BLACK);

	calibrateTouchScreen();

	button_count = 0;
	old_count = 0;

//	for(i=0;i<8;i++)	LCD_DisplayPicDirect_Partial((uint8_t *)MENU[i].img_normal, IMG_MENU_WIDTH, IMG_MENU_HEIGHT, MENU[i].x, MENU[i].y);
	Display_All_Normal_menu();

	while(1)
	{

		if(touch_irq() == 0)
		{
			press_position = touch_get_position();	//dummy_data;
			press_position = touch_get_position();

			menu_select_press = menu_find(press_position);
/*
			if(menu_select_press < 8)
				LCD_DisplayPicDirect_Partial((uint8_t *)MENU[menu_select_press].img_click, IMG_MENU_WIDTH, IMG_MENU_HEIGHT, MENU[menu_select_press].x, MENU[menu_select_press].y);
*/
			if(menu_select_press < 8){
				memcpy(lcd_Halfbuf, MENU[menu_select_press].img_click, sizeof(lcd_Halfbuf));
				LCD_DisplayPicDMA_Partial(lcd_Halfbuf, IMG_MENU_WIDTH, IMG_MENU_HEIGHT,MENU[menu_select_press].x, MENU[menu_select_press].y);				
			}
			
			i=0;
			while(touch_irq() == 0)
			{
				if(i != 0)
				{
					release_position_pre= release_position;
				}
				release_position = touch_get_position();
				i = 1;
			}
			release_position = release_position_pre;
			menu_select_release = menu_find(release_position);

			if(menu_select_press == menu_select_release)
			{
				//menu_start(menu_select_release);
				FND_HEX_Write(menu_select_release);
				printf("Start PGM [%d]\n\r",menu_select_release);
				beep(200);
				menu_select(menu_select_release);
				beep(200);

			}

		}

//		for(i=0;i<8;i++)	LCD_DisplayPicDirect_Partial((uint8_t *)MENU[i].img_normal, IMG_MENU_WIDTH, IMG_MENU_HEIGHT, MENU[i].x, MENU[i].y);
		Display_All_Normal_menu();
		
	}

}
#endif
#if ADDED_BY_DOMYST
void TIMER_Configuration(void)
{
	TIM_InitTypeDef TIM_InitStruct;
	
	TIM_InitStruct.TIM_Period = SYSCTRL->PCLK_1MS_VAL;
	
	TIM_InitStruct.TIMx = TIM_0;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	#if 1
	TIM_InitStruct.TIMx = TIM_1;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	//#if 0
	TIM_InitStruct.TIMx = TIM_2;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_3;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_4;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_5;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_6;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);

	TIM_InitStruct.TIMx = TIM_7;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	#endif
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	#if 1
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_1_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_2_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_3_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_4_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_5_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_6_IRQn;
 	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM0_7_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	 #endif

	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = UART0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

uint32_t TIMM0_GetTick(uint32_t TIMx)
{
	return tick_Timer0[TIMx];
}

void TIMM0_Mdelay(uint32_t TIMx, uint32_t delay)
{
	uint32_t tick = tick_Timer0[TIMx];
	
	TIM_Cmd(TIMM0, (TIM_NumTypeDef)TIMx, ENABLE);
	if ((tick + delay) < tick_Timer0[TIMx])
	{
        while((tick_Timer0[TIMx] - delay) < tick);
	}
	else
	{
        while((tick + delay) > tick_Timer0[TIMx]);
	}
	
    TIM_Cmd(TIMM0, (TIM_NumTypeDef)TIMx, DISABLE);
}
#endif

#if QFN88
int main(void)
{
	TouchPositionTypeDef press_position;
	TouchPositionTypeDef release_position_pre, release_position;

	uint16_t	i;
	uint16_t	menu_select_press = 0;
	uint16_t	menu_select_release = 0;

	//SYSCTRL_SYSCLKSourceSelect(SELECT_EXT12M);	// SELECT_INC12M
	SYSCTRL_SYSCLKSourceSelect(SELECT_INC12M);		// domyst
	//SYSCTRL_PLLConfig(SYSCTRL_PLL_204MHz);
	SYSCTRL_PLLConfig(SYSCTRL_PLL_192MHz);
	SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
//	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);	/* PCLK >= 48M */
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_GPIO, ENABLE);
	// domyst
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE); //if no, ok
	// org SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
	// end domyst
	
	SYSTICK_Init();	
	
	UART_Configuration();
	#if ADDED_BY_DOMYST
	// added by domyst 2022-02-22
	TIMER_Configuration();
	NVIC_Configuration();
	//
	#endif
	TOUCH_Configuration();
	LCD_Configuration();
	BEEP_Configuration();
	KEY_Configuration();
	FND_Configuration();
//	MSR_Configuration();
	QR_Configuration();
	SCI_Configuration();
	DAC_Configuration();
	RTC_Configuration();

	//By Psk: Led Off
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);

	FND_BIT_Write(0x00);
	printf("CSS Demo V1.1..\r\n");	// domyst
	// added by domsyt
	//#if ADDED_BY_DOMYST
	#if 0
	//while(1)
	for (int i=0; i < 3; i++)
	{
		TIMM0_Mdelay(TIM_0, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_0, TIMM0_GetTick(TIM_0));
		#if 0
		TIMM0_Mdelay(TIM_1, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_1, TIMM0_GetTick(TIM_1));
		
		TIMM0_Mdelay(TIM_2, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_2, TIMM0_GetTick(TIM_2));
		
		TIMM0_Mdelay(TIM_3, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_3, TIMM0_GetTick(TIM_3));
		
		TIMM0_Mdelay(TIM_4, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_4, TIMM0_GetTick(TIM_4));
		#endif
		// TIMM0_Mdelay(TIM_5, 1000);		
		// printf("current Timer0_%d_GetTick tick = %u \n", TIM_5, TIMM0_GetTick(TIM_5));
		
		// TIMM0_Mdelay(TIM_6, 1000);		
		// printf("current Timer0_%d_GetTick tick = %u \n", TIM_6, TIMM0_GetTick(TIM_6));
	
		// TIMM0_Mdelay(TIM_7, 1000);		
		// printf("current Timer0_%d_GetTick tick = %u \n", TIM_7, TIMM0_GetTick(TIM_7));	
	}
	#endif
	//
	memcpy(lcd_buf, IMG_CSS_CI, sizeof(lcd_buf));
	LCD_DisplayPicDMA(lcd_buf);			//320x240

	beep(200);
	mdelay(2000);

	LCD_DisplayColor(LCD_DISP_BLACK);

	calibrateTouchScreen();

	button_count = 0;
	old_count = 0;

//	for(i=0;i<8;i++)	LCD_DisplayPicDirect_Partial((uint8_t *)MENU[i].img_normal, IMG_MENU_WIDTH, IMG_MENU_HEIGHT, MENU[i].x, MENU[i].y);
	Display_All_Normal_menu();

	while(1)
	{

		if(touch_irq() == 0)
		{
			press_position = touch_get_position();	//dummy_data;
			press_position = touch_get_position();

			menu_select_press = menu_find(press_position);
/*
			if(menu_select_press < 8)
				LCD_DisplayPicDirect_Partial((uint8_t *)MENU[menu_select_press].img_click, IMG_MENU_WIDTH, IMG_MENU_HEIGHT, MENU[menu_select_press].x, MENU[menu_select_press].y);
*/
			if(menu_select_press < 8){
				memcpy(lcd_Halfbuf, MENU[menu_select_press].img_click, sizeof(lcd_Halfbuf));
				LCD_DisplayPicDMA_Partial(lcd_Halfbuf, IMG_MENU_WIDTH, IMG_MENU_HEIGHT,MENU[menu_select_press].x, MENU[menu_select_press].y);				
			}
			
			i=0;
			while(touch_irq() == 0)
			{
				if(i != 0)
				{
					release_position_pre= release_position;
				}
				release_position = touch_get_position();
				i = 1;
			}
			release_position = release_position_pre;
			menu_select_release = menu_find(release_position);

			if(menu_select_press == menu_select_release)
			{
				//menu_start(menu_select_release);
				FND_HEX_Write(menu_select_release);
				printf("Start PGM [%d]\n\r",menu_select_release);
				beep(200);
				menu_select(menu_select_release);
				beep(200);

			}

		}

//		for(i=0;i<8;i++)	LCD_DisplayPicDirect_Partial((uint8_t *)MENU[i].img_normal, IMG_MENU_WIDTH, IMG_MENU_HEIGHT, MENU[i].x, MENU[i].y);
		Display_All_Normal_menu();
		
	}

}
#else
int main_ser_timer(void)
{
	uint16_t	i;
	uint8_t rxdata;

	//SYSCTRL_SYSCLKSourceSelect(SELECT_EXT12M);	// SELECT_INC12M
	SYSCTRL_SYSCLKSourceSelect(SELECT_INC12M);		// domyst
	//SYSCTRL_PLLConfig(SYSCTRL_PLL_204MHz);
	SYSCTRL_PLLConfig(SYSCTRL_PLL_192MHz);
	SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
//	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);	/* PCLK >= 48M */
	
	//SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_GPIO, ENABLE);
	// domyst
	//SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE); //if no, ok
	// org SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
	// end domyst
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);

	SYSTICK_Init();	
	
	UART_Configuration();
	UART_ITConfig(UART0, UART_IT_RX_RECVD, ENABLE);	// domyst
	#if ADDED_BY_DOMYST
	// added by domyst 2022-02-22
	TIMER_Configuration();
	NVIC_Configuration();
	//
	#endif
	// TOUCH_Configuration();
	// LCD_Configuration();
	// BEEP_Configuration();
	// KEY_Configuration();
	// FND_Configuration();
//	MSR_Configuration();
	// QR_Configuration();
	// SCI_Configuration();
	// DAC_Configuration();
	// RTC_Configuration();

	//By Psk: Led Off
	// GPIO_ResetBits(GPIOA, GPIO_Pin_3);

	// FND_BIT_Write(0x00);
	printf("CSS Demo V1.1..\r\n");	// domyst
	// added by domsyt
	//#if ADDED_BY_DOMYST
	#if 1
	//while(1)
	for (i=0; i < 10; i++)
	{
		TIMM0_Mdelay(TIM_0, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_0, TIMM0_GetTick(TIM_0));
		#if 1
		TIMM0_Mdelay(TIM_1, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_1, TIMM0_GetTick(TIM_1));
		
		TIMM0_Mdelay(TIM_2, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_2, TIMM0_GetTick(TIM_2));
		
		TIMM0_Mdelay(TIM_3, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_3, TIMM0_GetTick(TIM_3));
		
		TIMM0_Mdelay(TIM_4, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_4, TIMM0_GetTick(TIM_4));
		#endif
		TIMM0_Mdelay(TIM_5, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_5, TIMM0_GetTick(TIM_5));
		
		TIMM0_Mdelay(TIM_6, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_6, TIMM0_GetTick(TIM_6));
	
		TIMM0_Mdelay(TIM_7, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_7, TIMM0_GetTick(TIM_7));	
	}
}
#endif
int main(void)
{
	uint16_t	i;
	uint8_t rxdata;
	uint8_t *p;
	uint8_t buf[256]={0};

	QSPI_Init(NULL);

	//SYSCTRL_SYSCLKSourceSelect(SELECT_EXT12M);	// SELECT_INC12M
	SYSCTRL_SYSCLKSourceSelect(SELECT_INC12M);		// domyst
	//SYSCTRL_PLLConfig(SYSCTRL_PLL_204MHz);
	SYSCTRL_PLLConfig(SYSCTRL_PLL_192MHz);
	SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
//	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);	/* PCLK >= 48M */
	
	//SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_GPIO, ENABLE);
	// domyst
	//SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE); //if no, ok
	// org SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
	// end domyst

	QSPI_SetLatency(0);

	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
	SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);

	SYSTICK_Init();	
	
	UART_Configuration();
	UART_ITConfig(UART0, UART_IT_RX_RECVD, ENABLE);	// domyst
	#if ADDED_BY_DOMYST
	// added by domyst 2022-02-22
	TIMER_Configuration();
	NVIC_Configuration();
	//
	#endif
	// TOUCH_Configuration();
	// LCD_Configuration();
	// BEEP_Configuration();
	// KEY_Configuration();
	// FND_Configuration();
//	MSR_Configuration();
	// QR_Configuration();
	// SCI_Configuration();
	// DAC_Configuration();
	// RTC_Configuration();

	//By Psk: Led Off
	// GPIO_ResetBits(GPIOA, GPIO_Pin_3);

	// FND_BIT_Write(0x00);
	printf("CSS Demo V1.1..\r\n");	// domyst
	// added by domsyt
	//#if ADDED_BY_DOMYST
	#if 1
	//while(1)
	for (i=0; i < 1; i++)
	{
		TIMM0_Mdelay(TIM_0, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_0, TIMM0_GetTick(TIM_0));
		#if 1
		TIMM0_Mdelay(TIM_1, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_1, TIMM0_GetTick(TIM_1));
		
		TIMM0_Mdelay(TIM_2, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_2, TIMM0_GetTick(TIM_2));
		
		TIMM0_Mdelay(TIM_3, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_3, TIMM0_GetTick(TIM_3));
		
		TIMM0_Mdelay(TIM_4, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_4, TIMM0_GetTick(TIM_4));
		#endif
		TIMM0_Mdelay(TIM_5, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_5, TIMM0_GetTick(TIM_5));
		
		TIMM0_Mdelay(TIM_6, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_6, TIMM0_GetTick(TIM_6));
	
		TIMM0_Mdelay(TIM_7, 1000);		
		printf("current Timer0_%d_GetTick tick = %u \n", TIM_7, TIMM0_GetTick(TIM_7));	
	}
	#endif

	//flash_test();
	FLASH_read(0x1091000U, buf, 16);

	//p = (uint8_t *)0x1091000U;
	for (i=0; i<16; i++)
	{
		printf("[%#02X]", buf[i]);
	}
	MSR_test();
	// p = (uint8_t *)0x1091000U;
	// for (i=0; i<16; i++)
	// {
	// 	printf("[%#02X]", *p++);
	// }

	while(1)
	{
		if(!isEmpty(&uart))
		{
			rxdata = pop(&uart);
			Uart0_SendDatas(&rxdata,1);
		}
	}
}
#endif

// main loop
#if 0
int main(void)
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
	OptionByte_initial_Response();	//응답
	#endif

	#if 1//pbbch 181012 host uart init fucntion add....kiss 노래방 대응 용.
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
#endif

#if 0
void GetCreditCardNumber()
{
	int lcd_line = 56;
	uint8_t i;
	
	for(i=0;i<200;i++) Valid_Credit_Number[i]=0;
	Valid_Credit_Len=0;
	
	Valid_Credit_Len=SCI_Test(Valid_Credit_Number);
	
	memcpy(ValidNumber,Valid_Credit_Number,4);
	Valid_Credit_Len-=4;
	memcpy(CreditNumber,Valid_Credit_Number,Valid_Credit_Len);
	
	LCD_DisplayColor(LCD_DISP_BLACK);
	
	LCD_DisplayStr((uint8_t *)"Credit Card Valid Data / Credit Number.. ", 8, 24, LCD_DISP_GREEN,  LCD_DISP_BLACK );	
	
	LCD_DisplayStr((uint8_t *)"Valid Data:", 8, lcd_line, LCD_DISP_CYAN,  LCD_DISP_BLACK );
	lcd_line+=16;
	LCD_DisplayStr(ValidNumber, 8, lcd_line, LCD_DISP_YELLOW,  LCD_DISP_BLACK );
	
	lcd_line+=32;
	LCD_DisplayStr((uint8_t *)"Credit Number:", 8, lcd_line, LCD_DISP_CYAN ,  LCD_DISP_BLACK );
	lcd_line+=16;
	LCD_DisplayStr(&CreditNumber[4], 8, lcd_line, LCD_DISP_YELLOW,  LCD_DISP_BLACK );	
	
	
	lcd_line+=32;
	LCD_DisplayStr((uint8_t *)"Remove and Insert IC Card Again...!! ", 8, lcd_line, LCD_DISP_MAGENTA,  LCD_DISP_BLACK );	
}
#endif
#if 0
void MSR_test(void)
{
	track_data tdata[MAX_TRACK_NUM];
	int ret;
	int i, j;
	uint8_t cfmt, tflag;
	int cnts = 0;
	int lcd_line = 0;
	unsigned char raw_data_buf[256];

	set_wakeup_status(HARD_WAKEUP_WITHOUT_SLEEP);
	init_dpu();

	sc_sleep();
	while(old_count == button_count)
	{
		switch (detect_swiping_card())
		{
		case DETECT_SWIPING_CARD:
			{
				printf("\r\nDetect swiping card,Times: %d \n", ++cnts);
				LCD_DisplayColor(LCD_DISP_BLACK);
				LCD_DisplayStr((uint8_t *)"Detect swiping card,Times:", 8, 8, LCD_DISP_RED,  LCD_DISP_BLACK );
				LCD_DisplayOneInt(cnts, 224, 8, LCD_DISP_GREEN, LCD_DISP_BLACK, 5);

				cfmt = tflag = 0;
				ret = get_decode_data(tdata, TRACK_SELECT_1 | TRACK_SELECT_2 | TRACK_SELECT_3, &cfmt, &tflag);
				printf("T1 = %d, T2 = %d, T3 = %d\ttflag = %02X\n", (int)tdata[0].len, (int)tdata[1].len, (int)tdata[2].len, (int)tflag);

				LCD_DisplayStr((uint8_t *)"T1 = ", 8, 24, LCD_DISP_RED,  LCD_DISP_BLACK );
				LCD_DisplayOneInt((int)tdata[0].len, 48, 24, LCD_DISP_RED, LCD_DISP_BLACK,5);

				LCD_DisplayStr((uint8_t *)"T2 = ", 102, 24, LCD_DISP_RED,  LCD_DISP_BLACK );
				LCD_DisplayOneInt((int)tdata[1].len, 142, 24, LCD_DISP_RED, LCD_DISP_BLACK,5);

				LCD_DisplayStr((uint8_t *)"T3 = ", 196, 24, LCD_DISP_RED,  LCD_DISP_BLACK );
				LCD_DisplayOneInt((int)tdata[2].len, 236, 24, LCD_DISP_RED, LCD_DISP_BLACK,5);
				
				LCD_DisplayStr((uint8_t *)"tFlag = ", 286, 24, LCD_DISP_RED,  LCD_DISP_BLACK );
				LCD_DisplayOneInt((int)tflag, 350, 24, LCD_DISP_RED, LCD_DISP_BLACK,5);

				lcd_line = 56;

				if (ret == SUCCESS)		
				{
					for (i = 0; i < MAX_TRACK_NUM; i++)
					{
						if (tdata[i].len)
						{
							printf("T%d decode data:\n", (int)(i+1));
							LCD_DisplayStr((uint8_t *)"T  decode data:", 8, lcd_line, LCD_DISP_RED,  LCD_DISP_BLACK );
							LCD_DisplayOneChar((unsigned char)(i + 1) & 0x0F, 16, lcd_line, LCD_DISP_RED, LCD_DISP_BLACK);
							lcd_line = lcd_line + 16; //LCD_FONT_H

							for (j = 0; j < tdata[i].len; j++)
							{
								putchar(tdata[i].buf[j]);
							}
							LCD_DisplayStr( &tdata[i].buf[0], 8, lcd_line, LCD_DISP_GREEN,  LCD_DISP_BLACK );

							printf("\r\n");
							lcd_line = lcd_line + 16;
							lcd_line = lcd_line + 16;

						}

						printf("\n\rT%d raw data:\r\n", (int)(i+1));
						get_track_raw_data(raw_data_buf, i);

						for(j=0;j < 256; j++)
						{
							printf("%02X",raw_data_buf[j]);
						}

					}
					beep(200);
				}
			}
			break;

			
		case DETECT_HARD_WAKEUP:
			sc_sleep();			
			break;
			
		case DETECT_NO_SWIPING_CARD:
		default:
			break;
		}
	}
}
#endif
#if 0
/* I2C Pin Config */
void CameraI2CGPIOConfig(void)
{
    SYSCTRL_APBPeriphClockCmd( SYSCTRL_APBPeriph_I2C0, ENABLE);
    I2C_DeInit(I2C0);
    
    GPIO_PinRemapConfig( SI2C_PORT, SI2C_SCL_PIN, SI2C_GPIO_REMAP);
    GPIO_PinRemapConfig( SI2C_PORT, SI2C_SDA_PIN, SI2C_GPIO_REMAP);
}

/* Camera Clock Config */
void Cameraclk_Configuration(void)
{
	uint32_t Period = 0;
	uint32_t PWM_HZ = 24000000;
	SYSCTRL_ClocksTypeDef clocks;
	TIM_PWMInitTypeDef TIM_PWMSetStruct;

	SYSCTRL_APBPeriphClockCmd( SYSCTRL_APBPeriph_TIMM0, ENABLE);
 
	SYSCTRL_GetClocksFreq(&clocks);
		
	/* Check PCLK, need >= 48MHz */
	if(clocks.PCLK_Frequency / 2 < PWM_HZ)
	{
			PWM_HZ = clocks.PCLK_Frequency / 2;
	}

	Period = clocks.PCLK_Frequency / PWM_HZ;																																																																																							 

	TIM_PWMSetStruct.TIM_LowLevelPeriod = (Period / 2 - 1);
	TIM_PWMSetStruct.TIM_HighLevelPeriod = (Period - TIM_PWMSetStruct.TIM_LowLevelPeriod - 2);
		
	TIM_PWMSetStruct.TIMx = CAM_XCK_TIM;
	TIM_PWMInit(TIMM0, &TIM_PWMSetStruct);

	GPIO_PinRemapConfig(CAM_XCK_GPIO, CAM_XCK_GPIO_PIN, GPIO_Remap_2);

	TIM_Cmd(TIMM0, CAM_XCK_TIM, ENABLE);
}

void SCI_NVICConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);
    
    NVIC_InitStructure.NVIC_IRQChannel = SCI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}
#endif

#if QFN88
void SCI_IOConfig(void)
{
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, GPIO_Remap_0); //ALT0
    //card detect
    SYSCTRL->PHER_CTRL &= ~BIT(16); // SCI0 card detection signal -> 0 : active high (1: ative low)
//    SYSCTRL->PHER_CTRL |= BIT(16);
    //Choose active level(Low level active).
    SYSCTRL->PHER_CTRL |= BIT(20);  // SCI0 VCC effective signal level selection -> 1 : active low (0: active high)

}
#else
// void SCI_IOConfig(void)
// {
//     GPIO_PinRemapConfig(GPIOA, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, GPIO_Remap_0);

//    //card detect
//     SYSCTRL->PHER_CTRL &= ~BIT(16);
// //    SYSCTRL->PHER_CTRL |= BIT(16);
//     //Choose active level(Low level active).
//     SYSCTRL->PHER_CTRL |= BIT(20);

// }
#endif

// IFM configuration
// void SCI_Configuration(void)
// {
// 	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_SCI0, ENABLE);
//     SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_SCI0, ENABLE);

// 	SCI_IOConfig();

// 	Select_EMV_VCC();
// 	SCI_ConfigEMV(0x01, 3000000);

// 	SCI_NVICConfig();	
// }

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);	
	
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	
	UART_Init(UART0, &UART_InitStructure);
}

void UART_Configuration_all(void)
{
	UART_InitTypeDef UART_InitStructure;
	// UART0
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);	
	
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	
	UART_Init(UART0, &UART_InitStructure);

	// UART1
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART1, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART1, ENABLE);
	
	GPIO_PinRemapConfig(GPIOB, GPIO_Pin_12 | GPIO_Pin_13, GPIO_Remap_3);	
	
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	
	UART_Init(UART1, &UART_InitStructure);

	// UART2
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART2, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART2, ENABLE);
	
	GPIO_PinRemapConfig(GPIOD, GPIO_Pin_8 | GPIO_Pin_9, GPIO_Remap_2);	// ALT2
	
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	
	UART_Init(UART2, &UART_InitStructure);
}

//Retarget Printf
int SER_PutChar (int ch)
{
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) ch);
	
	return ch;
}

int fputc(int c, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	if (c == '\n')
	{
		SER_PutChar('\r');
	}
	return (SER_PutChar(c));
}

#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	
	/* Infinite loop */
	while (1)
	{
	}
}
#endif
