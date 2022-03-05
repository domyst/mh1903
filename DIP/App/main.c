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
#include "DecodeLib.h"
#include "test_emv.h"
#include "emv_core.h"
#include "iso7816_3.h"
#include "rtc.h"
#include "sound.h"
#include "uart.h"
#include "CreditCard_lib.h"

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
void KEY_Configuration(void);
void QR_Configuration(void);
void SCI_Configuration(void);
void DAC_Configuration(void);
void RTC_Configuration(void);

void MSR_test(void);
void QR_Test(void);
void DAC_Test(void);
void RTC_Test(void);

void SingleBuffDecodeDemo(void);
void DoubleBuffDecodeDemo(void);

void menu_select(uint8_t menu);
void select_menu_0(void);	//
void select_menu_1(void);	//LCD Test
void select_menu_2(void);	//MSR Test
void select_menu_3(void);	//QR Test with LED
void select_menu_4(void);	//MSR Test
void select_menu_5(void);	//EMV Test
void select_menu_6(void);
void select_menu_7(void);

uint8_t menu_find(TouchPositionTypeDef position);

#define CAM_ADDR_WRITE		0x42
#define CAM_ADDR_READ		0x43

#define	MENU_0_OFFSET_X		0
#define	MENU_0_OFFSET_Y		0

#define	MENU_1_OFFSET_X		240
#define	MENU_1_OFFSET_Y		0

#define	MENU_2_OFFSET_X		0
#define	MENU_2_OFFSET_Y		80

#define	MENU_3_OFFSET_X		240
#define	MENU_3_OFFSET_Y		80

#define	MENU_4_OFFSET_X		0
#define	MENU_4_OFFSET_Y		160

#define	MENU_5_OFFSET_X		240
#define	MENU_5_OFFSET_Y		160

#define	MENU_6_OFFSET_X		0
#define	MENU_6_OFFSET_Y		240

#define	MENU_7_OFFSET_X		240
#define	MENU_7_OFFSET_Y		240

typedef struct{
	int16_t x;
	int16_t y;
	uint8_t *img_normal;
	uint8_t *img_click;
}MENU_InitTypeDef;

MENU_InitTypeDef MENU[] = {
		{MENU_0_OFFSET_X, MENU_0_OFFSET_Y, (uint8_t *)IMG_barcode_normal, 	(uint8_t *)IMG_barcode_click,	},
		{MENU_1_OFFSET_X, MENU_1_OFFSET_Y, (uint8_t *)IMG_EMV_normal, 		(uint8_t *)IMG_EMV_click,		},
		{MENU_2_OFFSET_X, MENU_2_OFFSET_Y, (uint8_t *)IMG_MSR_normal, 		(uint8_t *)IMG_MSR_click,		},
		{MENU_3_OFFSET_X, MENU_3_OFFSET_Y, (uint8_t *)IMG_Sign_normal, 		(uint8_t *)IMG_Sign_click,		},
		{MENU_4_OFFSET_X, MENU_4_OFFSET_Y, (uint8_t *)IMG_FND_normal, 		(uint8_t *)IMG_FND_click,		},
		{MENU_5_OFFSET_X, MENU_5_OFFSET_Y, (uint8_t *)IMG_LCD_normal, 		(uint8_t *)IMG_LCD_click,		},
		{MENU_6_OFFSET_X, MENU_6_OFFSET_Y, (uint8_t *)IMG_DAC_normal, 		(uint8_t *)IMG_DAC_click,		},
		{MENU_7_OFFSET_X, MENU_7_OFFSET_Y, (uint8_t *)IMG_RTC_normal, 		(uint8_t *)IMG_RTC_click,		},
};

uint16_t		button_count;
uint16_t		old_count;
uint8_t			lcd_buf[480*320*2];
uint8_t			lcd_Halfbuf[240*80*2];

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


void Display_All_Normal_menu(void)
{
	uint8_t i;
	
//	for(i=0;i<8;i++)	LCD_DisplayPicDirect_Partial((uint8_t *)MENU[i].img_normal, IMG_MENU_WIDTH, IMG_MENU_HEIGHT, MENU[i].x, MENU[i].y);
	
	for(i=0;i<8;i++){
//		LCD_DisplayPicDirect_Partial((uint8_t *)MENU[i].img_normal, IMG_MENU_WIDTH, IMG_MENU_HEIGHT, MENU[i].x, MENU[i].y);

		
		memcpy(lcd_Halfbuf, MENU[i].img_normal, sizeof(lcd_Halfbuf));
		LCD_DisplayPicDMA_Partial(lcd_Halfbuf, IMG_MENU_WIDTH, IMG_MENU_HEIGHT,MENU[i].x, MENU[i].y);		
		
	}	
}


void Display_All_Click_menu(void)
{
		memcpy(lcd_Halfbuf, IMG_barcode_click, sizeof(lcd_Halfbuf));
		LCD_DisplayPicDMA_Partial(lcd_Halfbuf, IMG_MENU_WIDTH, IMG_MENU_HEIGHT,MENU[0].x, MENU[0].y);	

		memcpy(lcd_Halfbuf, IMG_EMV_click, sizeof(lcd_Halfbuf));
		LCD_DisplayPicDMA_Partial(lcd_Halfbuf, IMG_MENU_WIDTH, IMG_MENU_HEIGHT,MENU[1].x, MENU[1].y);	

		memcpy(lcd_Halfbuf, IMG_MSR_click, sizeof(lcd_Halfbuf));
		LCD_DisplayPicDMA_Partial(lcd_Halfbuf, IMG_MENU_WIDTH, IMG_MENU_HEIGHT,MENU[2].x, MENU[2].y);	
	
//		memcpy(lcd_Halfbuf, IMG_Sign_click, sizeof(lcd_Halfbuf));
//		LCD_DisplayPicDMA_Partial(lcd_Halfbuf, IMG_MENU_WIDTH, IMG_MENU_HEIGHT,MENU[3].x, MENU[3].y);	

		memcpy(lcd_Halfbuf, IMG_CreditNum_normal, sizeof(lcd_Halfbuf));
		LCD_DisplayPicDMA_Partial(lcd_Halfbuf, IMG_MENU_WIDTH, IMG_MENU_HEIGHT,MENU[3].x, MENU[3].y);	
	
		memcpy(lcd_Halfbuf, IMG_FND_click, sizeof(lcd_Halfbuf));
		LCD_DisplayPicDMA_Partial(lcd_Halfbuf, IMG_MENU_WIDTH, IMG_MENU_HEIGHT,MENU[4].x, MENU[4].y);	

//		memcpy(lcd_Halfbuf, IMG_LCD_click, sizeof(lcd_Halfbuf));
//		LCD_DisplayPicDMA_Partial(lcd_Halfbuf, IMG_MENU_WIDTH, IMG_MENU_HEIGHT,MENU[5].x, MENU[5].y);	

		memcpy(lcd_Halfbuf, IMG_LookBack_normal, sizeof(lcd_Halfbuf));
		LCD_DisplayPicDMA_Partial(lcd_Halfbuf, IMG_MENU_WIDTH, IMG_MENU_HEIGHT,MENU[5].x, MENU[5].y);	

		memcpy(lcd_Halfbuf, IMG_DAC_click, sizeof(lcd_Halfbuf));
		LCD_DisplayPicDMA_Partial(lcd_Halfbuf, IMG_MENU_WIDTH, IMG_MENU_HEIGHT,MENU[6].x, MENU[6].y);	

		memcpy(lcd_Halfbuf, IMG_RTC_click, sizeof(lcd_Halfbuf));
		LCD_DisplayPicDMA_Partial(lcd_Halfbuf, IMG_MENU_WIDTH, IMG_MENU_HEIGHT,MENU[7].x, MENU[7].y);	
}


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
#if ADDED_BY_DOMYST
void TIMER_Configuration(void)
{
	TIM_InitTypeDef TIM_InitStruct;
	
	TIM_InitStruct.TIM_Period = SYSCTRL->PCLK_1MS_VAL;
	
	TIM_InitStruct.TIMx = TIM_0;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	#if 0
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
	#if 0
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
#endif
void menu_select(uint8_t menu)
{
		switch(menu)
		{
		case 0x00:
			select_menu_0();
			break;
		case 0x01:
			select_menu_1();
			break;
		case 0x02:
			select_menu_2();
			break;
		case 0x03:
			select_menu_3();
			break;
		case 0x04:
			select_menu_4();
			break;
		case 0x05:
			select_menu_5();
			break;
		case 0x06:
			select_menu_6();
			break;
		case 0x07:
			select_menu_7();
			break;
		default:
			printf("non exist menu\n\r");
		break;
		}

	
}

//by Psk LED Config
void LED_IOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}


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



void select_menu_CreditNum(void)
{
	printf("%s \n\r",__func__);

//    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, GPIO_Remap_0);

//	SCI_NVICConfig();
	LCD_DisplayColor(LCD_DISP_BLACK);
//	LCD_DisplayStr((uint8_t *)"EMV Loop Back ", 8, 24, LCD_DISP_GREEN,  LCD_DISP_BLACK );
	LCD_DisplayStr((uint8_t *)"Credit Card Valid Data / Credit Number.. ", 8, 24, LCD_DISP_GREEN,  LCD_DISP_BLACK );	
	
	LCD_DisplayStr((uint8_t *)"Remove and Insert IC Card Again...!! ", 8, 56, LCD_DISP_MAGENTA,  LCD_DISP_BLACK );	
  
//	loop_back(0);

	while(old_count == button_count)
	{
		GetCreditCardNumber();
	}

	FND_BIT_Write(0x00);
	old_count = button_count;
}


void select_menu_LoopBack(void)
{
	printf("%s \n\r",__func__);

//    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, GPIO_Remap_0);

//	SCI_NVICConfig();
	LCD_DisplayColor(LCD_DISP_BLACK);
	LCD_DisplayStr((uint8_t *)"EMV Loop Back ", 8, 24, LCD_DISP_GREEN,  LCD_DISP_BLACK );
  
	loop_back(0);

	FND_BIT_Write(0x00);
	old_count = button_count;
}


void select_menu_0(void)		//QR_TEST
{
	printf("%s \n\r",__func__);

	//by Psk, LED ON
	LED_IOConfig();
	GPIO_SetBits(GPIOA, GPIO_Pin_3);

	
	QR_Test();

	FND_BIT_Write(0x00);
	old_count = button_count;
}

void select_menu_1(void)
{
	TouchPositionTypeDef press_position;
	TouchPositionTypeDef release_position_pre, release_position;

	uint8_t i=0;
	uint16_t	menu_select_press = 0;
	uint16_t	menu_select_release = 0;
	
	Display_All_Click_menu();
	
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
			if(menu_select_press==3||menu_select_press==5){
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
				if(menu_select_release==3||menu_select_release==5){
						beep(200);
					if(menu_select_release==3) select_menu_CreditNum();
					else if(menu_select_release==5) select_menu_LoopBack();
						beep(200);
					return;
				}

			}

		}

//		for(i=0;i<8;i++)	LCD_DisplayPicDirect_Partial((uint8_t *)MENU[i].img_normal, IMG_MENU_WIDTH, IMG_MENU_HEIGHT, MENU[i].x, MENU[i].y);
			Display_All_Click_menu();
	}
}

void select_menu_2(void)	//MSR Test
{
	printf("%s \n\r",__func__);
	LCD_DisplayColor(LCD_DISP_BLACK);
	LCD_DisplayStr((uint8_t *)"Card Swipe Ready ........", 8, 24, LCD_DISP_GREEN,  LCD_DISP_BLACK );

	MSR_test();

	FND_BIT_Write(0x00);
	old_count = button_count;
}

void select_menu_3(void)	//Touch Test
{
	TouchPositionTypeDef position;

	printf("%s \n\r",__func__);

	LCD_DisplayColor(LCD_DISP_BLACK);

	while(button_count == old_count)
	{
		position = touch_get_position();
		LCD_DisplayOneDot(position.x, position.y,LCD_DISP_RED); 
	}
	
	FND_BIT_Write(0x00);
	old_count = button_count;
}

void select_menu_4(void)	//FND Test
{
	int i;
	
	printf("%s \n\r",__func__);

	while(button_count == old_count)
	{
		for(i=0;i<0x10;i++)
		{
			FND_HEX_Write(i);
			if(button_count != old_count) break;
			mdelay(200);
		}
	}
	
	old_count = button_count;
	FND_BIT_Write(0x00);
}

void select_menu_5(void)	//LCD Test
{
	printf("%s \n\r",__func__);

	while(old_count == button_count)
	{
		printf("LCD_DisplayGrayHor16\r\n");
		LCD_DisplayGrayHor16();
		if(old_count != button_count) break;
		mdelay(1000);
		
		printf("LCD_DisplayGrayHor32\r\n");
		LCD_DisplayGrayHor32();
		if(old_count != button_count) break;
		mdelay(1000);

		printf("LCD_DisplayBand\r\n");
		LCD_DisplayBand();
		if(old_count != button_count) break;
		mdelay(1000);
		
		printf("LCD_DisplayScaleHor1\r\n");
		LCD_DisplayScaleHor1();
		if(old_count != button_count) break;
		mdelay(1000);

		printf("LCD_DisplayScaleHor2\r\n");
		LCD_DisplayScaleHor2();
		if(old_count != button_count) break;
		mdelay(1000);

		printf("LCD_LCD_DisplayScaleVer\r\n");
		LCD_DisplayScaleVer();
		if(old_count != button_count) break;
		mdelay(1000);
	}

	old_count = button_count;
	FND_BIT_Write(0x00);
}

void select_menu_6(void)
{
	printf("%s \n\r",__func__);

	DAC_Test();
	
	FND_BIT_Write(0x00);
	old_count = button_count;
}

void select_menu_7(void)
{
	
	printf("%s \n\r",__func__);

	RTC_Test();
	
	FND_BIT_Write(0x00);
	old_count = button_count;
}


#define MENU_SELECT_OFFSET_X	10
#define MENU_SELECT_OFFSET_Y	5

uint8_t menu_find(TouchPositionTypeDef position)
{
	uint8_t menu;
	
	if((MENU_0_OFFSET_X + MENU_SELECT_OFFSET_X < position.x)  & (MENU_0_OFFSET_X + IMG_MENU_WIDTH - MENU_SELECT_OFFSET_X > position.x))
	{
		menu = 0;
	}
	else if((MENU_1_OFFSET_X + MENU_SELECT_OFFSET_X < position.x)  & (MENU_1_OFFSET_X + IMG_MENU_WIDTH - MENU_SELECT_OFFSET_X > position.x))
	{
		menu = 1;
	}
	else
	{
		menu = 0x10;
	}
	
	if(     (MENU_0_OFFSET_Y + MENU_SELECT_OFFSET_Y < position.y) & (MENU_0_OFFSET_Y + IMG_MENU_HEIGHT - MENU_SELECT_OFFSET_Y > position.y))
	{
		menu = menu + 0;
	}
	else if((MENU_2_OFFSET_Y + MENU_SELECT_OFFSET_Y < position.y) & (MENU_2_OFFSET_Y + IMG_MENU_HEIGHT - MENU_SELECT_OFFSET_Y > position.y))
	{
		menu = menu + 2;
	}
	else if((MENU_4_OFFSET_Y + MENU_SELECT_OFFSET_Y < position.y) & (MENU_4_OFFSET_Y + IMG_MENU_HEIGHT - MENU_SELECT_OFFSET_Y > position.y))
	{
		menu = menu + 4;
	}
	else if((MENU_6_OFFSET_Y + MENU_SELECT_OFFSET_Y < position.y) & (MENU_6_OFFSET_Y + IMG_MENU_HEIGHT - MENU_SELECT_OFFSET_Y > position.y))
	{
		menu = menu + 6;
	}
	else
	{
		menu = 0x10;
	}

	return menu;
}

void Sign_test(void)
{
	TouchPositionTypeDef position;

	while(old_count == button_count)
	{
		position = touch_get_position();
		LCD_DisplayOneDot(position.x, position.y,LCD_DISP_RED); 
	}
		LCD_DisplayColor(LCD_DISP_BLACK);
}

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

void DAC_Test(void)
{
	int i;

	DAC->DAC_TIMER = 240;				

	while(old_count == button_count)
	{
		for (i = 0; i < hello_wav_len; i++)
		{
			while (SET == DAC_GetFlagStatus(DAC_FLAG_RUNNING));
			DAC_SetData(hello_wav[i]);
		}
		mdelay(500);
	}

	DAC_SetData(0x00);
}

void RTC_Test(void)
{
	DateTime date;
//	DateTime set_date;
	uint32_t	currentUtc;

	//By Psk: 2022-01-01	01:22:00 setting value
	RTC_SetRefRegister(1641000000);	
	
	LCD_DisplayColor(LCD_DISP_BLACK);

	while(old_count == button_count)
	{
		currentUtc = RTC_GetRefRegister() + RTC_GetCounter();
		convertUnixTimeToDate(currentUtc, &date);

		LCD_DisplayOneInt(date.year,	  8, 172, LCD_DISP_GREEN, LCD_DISP_BLACK,4);
		LCD_DisplayStr((uint8_t *)"-",	 40, 172, LCD_DISP_GREEN,  LCD_DISP_BLACK );
		LCD_DisplayOneInt(date.month,	 48, 172, LCD_DISP_GREEN, LCD_DISP_BLACK,2);
		LCD_DisplayStr((uint8_t *)"-", 	 64, 172, LCD_DISP_GREEN,  LCD_DISP_BLACK );
		LCD_DisplayOneInt(date.day,		 72, 172, LCD_DISP_GREEN, LCD_DISP_BLACK,2);

		LCD_DisplayOneInt(date.hours,	168, 172, LCD_DISP_GREEN, LCD_DISP_BLACK,2);
		LCD_DisplayStr((uint8_t *)":", 	184, 172, LCD_DISP_GREEN,  LCD_DISP_BLACK );
		LCD_DisplayOneInt(date.minutes,	192, 172, LCD_DISP_GREEN, LCD_DISP_BLACK,2);
		LCD_DisplayStr((uint8_t *)":", 	208, 172, LCD_DISP_GREEN,  LCD_DISP_BLACK );
		LCD_DisplayOneInt(date.seconds,	216, 172, LCD_DISP_GREEN, LCD_DISP_BLACK,2);
		
//		printf("%d-%d-%d %d:%d:%d \r", date.year,date.month,date.day,date.hours,date.minutes,date.seconds);
		mdelay(500);
	
/*
		if (UART_LINE_STATUS_RX_RECVD & UART_GetLineStatus(UART0))
		{
			printf("Enter Year :\n\r");
			scanf(&set_date.year);
			
			printf("Enter Month : \n\r");
			scanf(&set_date.month);
			
			printf("Enter Day : \n\r");
			scanf(&set_date.day);
			
			printf("Enter Hours : \n\r");
			scanf(&set_date.hours);
			
			printf("Enter Min : \n\r");
			scanf(&set_date.minutes);
			
		}
*/
		
	}


	
}

void QR_Test(void)
{    

 	printf("Decode Demo V%d.%d.%d\n\r", (GetDecodeLibVerison() >> 16) & 0xff, (GetDecodeLibVerison() >> 8) & 0xff, GetDecodeLibVerison() & 0xff);

    if (DECODE_BUFF_SIZE >= SINGLE_BUFF_MIN_SIZE && DECODE_BUFF_SIZE < DOUBLE_BUFF_MIN_SIZE)
    {
        /* Single Buffer Example */
        SingleBuffDecodeDemo();
    }
    else if (DECODE_BUFF_SIZE >= DOUBLE_BUFF_MIN_SIZE)
    {
        /* Double Buffer Example */
        DoubleBuffDecodeDemo();
    }
    
    /* Release hardware resources */
//    CloseDecode();
//    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_I2C0, DISABLE);
//    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, DISABLE);
}

/* DCMI Interrupt Config */
void DCMI_NVICConfig(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
   	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
    
    NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure); 
    
//  DCMI_ITConfig(DCMI_IT_VSYNC, ENABLE);
    DCMI_ITConfig(DCMI_IT_OVF, ENABLE);
//  DCMI_ITConfig(DCMI_IT_LINE, ENABLE);
    DCMI_ITConfig(DCMI_IT_FRAME, ENABLE);
    DCMI_ITConfig(DCMI_IT_ERR, ENABLE);

	DCMI_ClearITPendingBit(DCMI_IT_VSYNC);
	DCMI_ClearITPendingBit(DCMI_IT_OVF);
	DCMI_ClearITPendingBit(DCMI_IT_LINE);
	DCMI_ClearITPendingBit(DCMI_IT_FRAME);
	DCMI_ClearITPendingBit(DCMI_IT_ERR);
}

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

void DAC_Configuration(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_ADC, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_ADC, ENABLE);

	DAC_InitTypeDef DAC_InitStruct;
	
	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_1, GPIO_Remap_2);
	
	DAC_InitStruct.DAC_CurrSel = DAC_CURR_SEL_2K;
	DAC_InitStruct.DAC_FIFOThr = 0x8;
	DAC_InitStruct.DAC_TimerExp = 0x18;
	
	DAC_Init(&DAC_InitStruct);

	DAC_Cmd(ENABLE);

}
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
void SCI_IOConfig(void)
{
    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, GPIO_Remap_0);

   //card detect
    SYSCTRL->PHER_CTRL &= ~BIT(16);
//    SYSCTRL->PHER_CTRL |= BIT(16);
    //Choose active level(Low level active).
    SYSCTRL->PHER_CTRL |= BIT(20);

}
#endif
void SCI_Configuration(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_SCI0, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_SCI0, ENABLE);

	SCI_IOConfig();

	Select_EMV_VCC();
	SCI_ConfigEMV(0x01, 3000000);

	SCI_NVICConfig();	
}

void DCMI_IOConfig(void) 
{
	/* �����ź�  */
	GPIO_PinRemapConfig(DCMI_VSYNC_GPIO_PORT, DCMI_VSYNC_GPIO_PIN, DCMI_VSYNC_AF);
	GPIO_PinRemapConfig(DCMI_HSYNC_GPIO_PORT, DCMI_HSYNC_GPIO_PIN, DCMI_HSYNC_AF);
	GPIO_PinRemapConfig(DCMI_PIXCLK_GPIO_PORT, DCMI_PIXCLK_GPIO_PIN, DCMI_PIXCLK_AF);
	/* �����ź� */
	GPIO_PinRemapConfig(DCMI_D0_GPIO_PORT, DCMI_D0_GPIO_PIN, DCMI_D0_AF);
	GPIO_PinRemapConfig(DCMI_D1_GPIO_PORT, DCMI_D1_GPIO_PIN, DCMI_D1_AF);
	GPIO_PinRemapConfig(DCMI_D2_GPIO_PORT, DCMI_D2_GPIO_PIN, DCMI_D2_AF);
	GPIO_PinRemapConfig(DCMI_D3_GPIO_PORT, DCMI_D3_GPIO_PIN, DCMI_D3_AF);
	GPIO_PinRemapConfig(DCMI_D4_GPIO_PORT, DCMI_D4_GPIO_PIN, DCMI_D4_AF);
	GPIO_PinRemapConfig(DCMI_D5_GPIO_PORT, DCMI_D5_GPIO_PIN, DCMI_D5_AF);
	GPIO_PinRemapConfig(DCMI_D6_GPIO_PORT, DCMI_D6_GPIO_PIN, DCMI_D6_AF);
	GPIO_PinRemapConfig(DCMI_D7_GPIO_PORT, DCMI_D7_GPIO_PIN, DCMI_D7_AF);
}

void QR_Configuration(void)
{
	DecodeInitTypeDef DecodeInitStruct;
    DecodeFlagTypeDef ret;

    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);

	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_OTP, ENABLE);	//For Primeum Library
    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_OTP, ENABLE);
	
//	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_QR, ENABLE);
//    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_QR, ENABLE);

//	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_DCMIS, ENABLE);
//    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_DCMIS, ENABLE);
	
//	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_TIMM0, ENABLE);
//    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_TIMM0, ENABLE);

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_I2C0, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_I2C0, ENABLE);
	
    /* I2C Pin Config */
    CameraI2CGPIOConfig();
	
    /* Camera CLK enable, should before DecodeInit */
    Cameraclk_Configuration();

	/*DCMI GPIO Config*/
	DCMI_IOConfig();

    /* DecodeLib & MEM Pool Init */
    DecodeInitStruct.pool = pool;
    DecodeInitStruct.size = DECODE_BUFF_SIZE;
    DecodeInitStruct.CAM_PWDN_GPIOx = CAM_PWDN_GPIO;
    DecodeInitStruct.CAM_PWDN_GPIO_Pin = CAM_PWDN_GOIO_PIN;
    DecodeInitStruct.CAM_RST_GPIOx = CAM_RST_GPIO;
    DecodeInitStruct.CAM_RST_GPIO_Pin = CAM_RST_GOIO_PIN;
    DecodeInitStruct.CAM_I2Cx = I2C0;
    DecodeInitStruct.CAM_I2CClockSpeed = I2C_ClockSpeed_400KHz;
    DecodeInitStruct.SensorConfig = NULL;
    DecodeInitStruct.SensorCfgSize = 0;
    
    ret = DecodeInit(&DecodeInitStruct);

	if (ret != DecodeInitSuccess)
    {
        switch (ret)
        {
            case DecodeInitCheckError:
                printf("Decoding Library Check Failure!\r\n");
            break;
            
            case DecodeInitMemoryError:
                printf("Insufficient memory in decoding library!\r\n");
            break;
            
            case DecodeInitSensorError:
                printf("Camera initialization failed!\r\n");
            break;
			
            default:
                break;
        }
        while(1);
    }

    /* Should invoked after DecodeInit() */
    DCMI_NVICConfig();
	
}

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

void KEY_IOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void KEY_NVICConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	EXTI_LineConfig(EXTI_Line0, EXTI_PinSource4, EXTI_Trigger_Falling);
}    


void KEY_Configuration(void)
{
	KEY_IOConfig();
	KEY_NVICConfig();
}

void RTC_Configuration(void)
{
#ifdef USE_EXTERN_32K
	BPU->SEN_ANA0 |= BIT(10);
#endif
	//RTC_ResetCounter();		//rtc stop counting and counter cleared to 0
	//RTC_SetRefRegister(1641997920);			//���� �ð� : GMT/2022-01-12 PM 2:32:00
	//printf("\nGET RTC RefRegister value: %d\n", RTC_GetRefRegister());
	//currentUtc = RTC_GetRefRegister() + RTC_GetCounter();
	//printf("\r\n current UTC: %d \r\n", currentUtc);
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
