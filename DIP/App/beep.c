#include "beep.h"
//#include "ProjectConfig.h"
#define BEEP_GPIO                   GPIOF
#define BEEP_GOIO_PIN               GPIO_Pin_2	

void BEEP_Init(void);

void Delay_Ms(uint32_t num)
{
	uint32_t i = 0;
    
	for (i = 0; i<num*10000; i++);
}

void BEEP_Configuration(void)
{
	BEEP_Init();
}
void BEEP_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = BEEP_GOIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(BEEP_GPIO, &GPIO_InitStruct);
	
	GPIO_SetBits(BEEP_GPIO,BEEP_GOIO_PIN);	
}

void beep(int ms)
{	
	GPIO_ResetBits(BEEP_GPIO,BEEP_GOIO_PIN);
	Delay_Ms(ms);
	GPIO_SetBits(BEEP_GPIO,BEEP_GOIO_PIN);
}
