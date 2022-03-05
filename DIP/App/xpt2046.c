#include <stdint.h>
#include <stdio.h>
#include "systick.h"
#include "xpt2046.h"
#include "mhscpu_spi.h"
#include "mhscpu_gpio.h"

#define	TS_IRQ_PORT		GPIOE
#define	TS_IRQ_PIN		GPIO_Pin_11
#define	TS_BUSY_PORT	GPIOE
#define	TS_BUSY_PIN		GPIO_Pin_10

#define SPI_CS_PORT		GPIOE
#define SPI_CS_PIN		GPIO_Pin_13

#define SPI_CLK_PORT	GPIOE
#define SPI_CLK_PIN		GPIO_Pin_12

#define SPI_MOSI_PORT	GPIOE
#define SPI_MOSI_PIN	GPIO_Pin_14

#define SPI_MISO_PORT	GPIOE
#define SPI_MISO_PIN	GPIO_Pin_15

#define SPI_CS_LOW		GPIO_ResetBits(SPI_CS_PORT, SPI_CS_PIN);
#define	SPI_CS_HIGH		GPIO_SetBits(SPI_CS_PORT, SPI_CS_PIN);

#define SPI_CLK_LOW		GPIO_ResetBits(SPI_CLK_PORT, SPI_CLK_PIN);
#define	SPI_CLK_HIGH	GPIO_SetBits(SPI_CLK_PORT, SPI_CLK_PIN);

#define SPI_MOSI_LOW	GPIO_ResetBits(SPI_MOSI_PORT, SPI_MOSI_PIN);
#define	SPI_MOSI_HIGH	GPIO_SetBits(SPI_MOSI_PORT, SPI_MOSI_PIN);

#define	SPI_MISO_GET	

/* XPT2046 Control Bit
bit7	Start	(1 is start)
bit6	A2		
bit5	A1
bit4	A0
bit3	mode
bit2	SFT/DFR	(SFR(Single-Ended) = 1, DFR(Differential) = 0)
bit1	PD1
bit0	PD0

A2..A0	111 Temperature
A2..A0	010	Battery 

*/
#define	XPT2046_X_DATA	0x00D2
#define	XPT2046_Y_DATA	0x0092
#define	XPT2046_TEMP	0x00F0
#define	XPT2046_BAT		0x00A0


/* SPI4 Pin and Alt
PE12	SPI_CLK		ALT2
PE13	SPI_CSN		ALT2
PE14	SPI_MOSI	ALT2
PE15	SPI_MISO	ALT2
*/

#define	GPIO_MODE	1

void TOUCH_IO_Configuration(void);
void SPI_Configuration(void);

void SPI_GPIO_Init(void);
void SPI_GPIO_TX(uint16_t data);
uint16_t SPI_GPIO_RX(void);

void TOUCH_Configuration(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_SPI4, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_SPI4, ENABLE);

	SPI_Configuration();
	TOUCH_IO_Configuration();
}

void TOUCH_IO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = TS_IRQ_PIN;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(TS_IRQ_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = TS_BUSY_PIN;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(TS_BUSY_PORT, &GPIO_InitStruct);

	//PE13	SPI_CSN		ALT2
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = SPI_CS_PIN;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(SPI_CS_PORT, &GPIO_InitStruct);

	//PE12	SPI_CLK		ALT2
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOE, &GPIO_InitStruct);

	//PE14	SPI_MOSI	ALT2
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;		//modified by domyst GPIO_Remap_1
	GPIO_Init(GPIOE, &GPIO_InitStruct);

	//PE15	SPI_MISO	ALT2
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;		//modified by domyst GPIO_Remap_1
	GPIO_Init(GPIOE, &GPIO_InitStruct);

#if GPIO_MODE
#else
    GPIO_PinRemapConfig(GPIOE, GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_Remap_2);
#endif
}

uint8_t  touch_irq(void)
{
	return GPIO_ReadInputDataBit(TS_IRQ_PORT, TS_IRQ_PIN);
}

uint8_t  touch_busy(void)
{
	return GPIO_ReadInputDataBit(TS_BUSY_PORT, TS_BUSY_PIN);
}

//#define	TOUCH_X_MAX	2000		
//#define	TOUCH_X_MIN	150

//#define	TOUCH_Y_MAX	
//#define	TOUCH_Y_MIN

TouchPositionTypeDef touch_get_raw_position(void)
{
	TouchPositionTypeDef position;
	
	position.x = touch_get_x();
	position.y = touch_get_y();
	
//	printf("touch data : [%04d] [%04d]\n\r",position.x, position.y);
	
	return position;
}

uint16_t  touch_get_x(void)
{
	uint16_t	result;

    if(touch_irq())	  	return 0;

#if GPIO_MODE

	SPI_CS_LOW;
	SPI_GPIO_TX(XPT2046_Y_DATA);
    while(touch_busy());
	result = SPI_GPIO_RX() >> 4;
	SPI_CS_HIGH;

#else
	SPI_CS_LOW;

    SPI_SendData(SPIM4, XPT2046_Y_DATA);
	while(RESET == SPI_GetFlagStatus(SPIM4, SPI_FLAG_RXNE));

	result = SPI_ReceiveData(SPIM4);
	SPI_CS_HIGH;

#endif

	return result;
}

uint16_t  touch_get_y(void)
{
	uint16_t	result;

    if(touch_irq())	  	return 0;

#if GPIO_MODE

	SPI_CS_LOW;
	SPI_GPIO_TX(XPT2046_X_DATA);
	while(touch_busy());
	result = SPI_GPIO_RX() >> 4;
	SPI_CS_HIGH;

	
#else
	SPI_CS_LOW;

    SPI_SendData(SPIM4, XPT2046_X_DATA);
	while(RESET == SPI_GetFlagStatus(SPIM4, SPI_FLAG_RXNE));

	result = SPI_ReceiveData(SPIM4);
	SPI_CS_HIGH;

#endif
	return result;
}

uint16_t  touch_temp(void)
{
	uint16_t	result;

    if(touch_irq())	  	return 0;

#if GPIO_MODE

	SPI_CS_LOW;
	SPI_GPIO_TX(XPT2046_TEMP);
	result = SPI_GPIO_RX() >> 4;
	SPI_CS_HIGH;

#else
	SPI_CS_LOW;

    SPI_SendData(SPIM4, XPT2046_TEMP);
	while(RESET == SPI_GetFlagStatus(SPIM4, SPI_FLAG_RXNE));

	result = SPI_ReceiveData(SPIM4);
	SPI_CS_HIGH;

#endif
	return result;

}

void SPI_GPIO_Init(void)
{
	SPI_CS_HIGH;
	SPI_CLK_HIGH;
	SPI_MOSI_HIGH;
	
}

void SPI_GPIO_TX(uint16_t data)
{
	uint16_t	mask;
	uint16_t 	i;

	mask = 0x8000;

	for(i=0;i<16;i++)
	{
		SPI_CLK_LOW;

		if(data & mask)
		{
			SPI_MOSI_HIGH;
		}
		else
		{
			SPI_MOSI_LOW;
		}

		SPI_CLK_HIGH;
		mask = mask >> 1;
	}
}

uint16_t SPI_GPIO_RX(void)
{
	uint16_t	result;
	uint16_t	mask;
	uint16_t	i;
	
	result = 0;
	mask = 0x8000;
	
	for(i=0;i<16;i++)
	{
		SPI_CLK_LOW;
		SPI_CLK_HIGH;
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_15))
		{
			result |= mask;
		}

		mask = mask >> 1;
	}

	return result;
}

void SPI_Configuration(void)
{
	SPI_InitTypeDef SPI_InitStructure;
    
    //SPI4
//    GPIO_PinRemapConfig(GPIOE, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_Remap_2);
    GPIO_PinRemapConfig(GPIOE, GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_Remap_2);
	
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_0;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_RXFIFOFullThreshold = SPI_RXFIFOFullThreshold_1;
	SPI_InitStructure.SPI_TXFIFOEmptyThreshold = SPI_TXFIFOEmptyThreshold_10;
	
	SPI_Init(SPIM4, &SPI_InitStructure);
    SPI_Cmd(SPIM4, ENABLE);
}
