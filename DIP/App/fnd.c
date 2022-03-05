#include <stdint.h>
#include "fnd.h"
#include "systick.h"
#include "mhscpu_gpio.h"

#define FND_GPIO_PORT	GPIOG
#define FND_GPIO_PIN	GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6 | GPIO_Pin_7

void FND_Configuration(void)
{
	FND_IOConfig();
}

void FND_IOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = FND_GPIO_PIN;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(FND_GPIO_PORT, &GPIO_InitStruct);
}

void FND_HEX_Write(uint8_t data)
{
	uint8_t		fnd_data;
	uint8_t 	FND_HEX_TABLE[16] = {FND_0, FND_1, FND_2, FND_3, FND_4, FND_5, FND_6, FND_7, FND_8, FND_9, FND_A, FND_B, FND_C, FND_D, FND_E, FND_F};
	uint16_t	gpio_data;

	data &= 0x0F;
	fnd_data = FND_HEX_TABLE[data];

	gpio_data = GPIO_ReadOutputData(FND_GPIO_PORT);
	gpio_data &= 0x00;
	gpio_data |= fnd_data;
	
	GPIO_Write(FND_GPIO_PORT, gpio_data);
}


void FND_BIT_Write(uint8_t data)
{
	uint16_t	gpio_data;

	gpio_data = GPIO_ReadOutputData(FND_GPIO_PORT);
	gpio_data &= 0x00;
	gpio_data |= data & 0x0F;
	
	GPIO_Write(FND_GPIO_PORT, gpio_data);
}

void FND_Blink(uint8_t data,uint32_t delay)
{
	FND_BIT_Write(0x00);
	mdelay(delay);
	FND_HEX_Write(data);
	mdelay(delay);
}	

