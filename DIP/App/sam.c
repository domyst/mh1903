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

//
#define delay_us(usec)		udelay(usec)
#define delay_ms(msec)		mdelay(msec)

#define SAM_VCC_5V_3V_PORT	GPIOH					
#define SAM_VCC_1_8V_PORT	GPIOH					
#define SAM_VCC_5V_3V_PIN	GPIO_Pin_3				
#define SAM_VCC_1_8V_PIN	GPIO_Pin_2				

void Select_SAM_VCC(void)
{
/*	
#define VCC_5V_3V_PORT	GPIOH					
#define VCC_1_8V_PORT	GPIOH					
#define VCC_5V_3V_PIN	GPIO_Pin_1
#define VCC_1_8V_PIN	GPIO_Pin_0					
*/	
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);


	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);
	
 	GPIO_SetBits(SAM_VCC_1_8V_PORT, SAM_VCC_1_8V_PIN);					//In order to use the VCC_5V_3V, you have to set the 1.8V Pin to High.
//	GPIO_ResetBits(VCC_1_8V_PORT,VCC_1_8V_PIN);					//In order to use the 1.8V, you have to set the VCC_5V_3V Pin to High.
	
	GPIO_SetBits(SAM_VCC_5V_3V_PORT, SAM_VCC_5V_3V_PIN);		//5V select	
//	GPIO_ResetBits(VCC_5V_3V_PORT,VCC_5V_3V_PIN);		//3V select

}

void Select_SAM_VCC_x(void)
{
/*	
#define VCC_5V_3V_PORT	GPIOH					
#define VCC_1_8V_PORT	GPIOH					
#define VCC_5V_3V_PIN	GPIO_Pin_1
#define VCC_1_8V_PIN	GPIO_Pin_0					
*/	
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);


	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
 	GPIO_SetBits(SAM_VCC_1_8V_PORT, SAM_VCC_1_8V_PIN);					//In order to use the VCC_5V_3V, you have to set the 1.8V Pin to High.
//	GPIO_ResetBits(VCC_1_8V_PORT,VCC_1_8V_PIN);					//In order to use the 1.8V, you have to set the VCC_5V_3V Pin to High.
	
	GPIO_SetBits(SAM_VCC_5V_3V_PORT, SAM_VCC_5V_3V_PIN);		//5V select	
//	GPIO_ResetBits(VCC_5V_3V_PORT,VCC_5V_3V_PIN);		//3V select

}

void IFM_Power_On(uint8_t data)		// active high
{
	//printf("%d, %s \n\r",data, __func__);
	//printf("------------------\n");
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

void IC_CARD_detection(uint8_t data)		// active high
{
	//printf("%d, %s \n\r",data, __func__);
	//printf("------------------\n");
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);

	if (data == 1)
		GPIO_SetBits(GPIOH, GPIO_Pin_13);
	else
		GPIO_ResetBits(GPIOH, GPIO_Pin_13);
}

void SAM_IOConfig(void)
{
	GPIO_PinRemapConfig(GPIOE, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12, GPIO_Remap_0); //ALT0, SCI2
    //card detect SCI2
    SYSCTRL->PHER_CTRL &= ~BIT(18); // SCI0 card detection signal -> 0 : active high (1: ative low)
//    SYSCTRL->PHER_CTRL |= BIT(18);
    //Choose active level(Low level active).
    SYSCTRL->PHER_CTRL |= BIT(22);  // SCI0 VCC effective signal level selection -> 1 : active low (0: active high)
}


#if 0
#define CLK_DIV         (1)

int32_t SAM_ConfigEMV(uint8_t SCI_Bitmap, uint32_t SCIx_Clk)
{
    uint8_t i;
    uint32_t SCIx_RefClock;
    uint32_t SCIx_ClkReg;
    int32_t tmp, SICx_RealClock;

    if (0 == SCI_Bitmap || SCI_Bitmap > 7 || SCIx_Clk < FREQ_Mhz(1) || SCIx_Clk > FREQ_Mhz(5))
    {
        return -1;
    }
//#define PCLK_KHZ        (*(uint32_t *)(0x40000024))
    //SCIx reference clock is same as system PCLK(Khz).
    SCIx_RefClock = SYSCTRL->PCLK_1MS_VAL;
    SCIx_Clk *=CLK_DIV;

    //clk = refClk / (2 *( CLkReg + 1)) MHz.
    //tmp = clk * (CLkReg + 1) = refClk * 1000 / 2 Hz
    tmp = SCIx_RefClock * 500UL;
    //The real value in Reg SCICLKICC, round up to SCIx_Clk / 2.
    SCIx_ClkReg = (tmp - (SCIx_Clk >> 1)) / SCIx_Clk;
    //realClock is the real ICC clock(Hz).
    SICx_RealClock = tmp / (SCIx_ClkReg + 1);

    for (i = 0; i < SLOT_COUNT; i++)
    {
        if (SCI_Bitmap & (1 << i))
        {
            aEMV_Param[i].SCIx_IdealClk = SCIx_Clk;
            aEMV_Param[i].SCIx_RealClk = SICx_RealClock;
            aEMV_Param[i].SCIx_RefClock = SCIx_RefClock;
            aEMV_Param[i].SCIx_ClkReg = SCIx_ClkReg;
            //Stable time is 10ms(ref clock), use 16-bit counter feed 8-bit counter.
            //aEMV_Param[i].SCIx_StableReg = (10 * SCIx_RefClock) >> 16;
            //Use NCN8025 stable time is 0ms(ref clock), NCN has already dedounced.
            aEMV_Param[i].SCIx_StableReg = 0;
            //Deacticve time is 100ns(ref clock) for each satage of deactive card.(Total three)
            //aEMV_Param[i].SCIx_DeactiveReg = SCIx_RefClock / 10;
            //Use NCN 8025 Deacticve time should be 0(ref clock).It means shut down immediately.
            aEMV_Param[i].SCIx_DeactiveReg = 1000;
            aEMV_Param[i].SCI_IntStatus = 0;
            SYSCTRL->CG_CTRL1 |= aSCI_CG_Table[i];
            emv_sci_reset(i);
        }
    }
    
    //output ICC clock deviation percent.
//    {
//        float32_t deviation = (SICx_RealClock - (int32_t)SCIx_Clk) * 100.0f / (float32_t)SCIx_Clk;
//        
//        dprintf("deviation percent %+1.2f%%\n", deviation);
//    }
    //check_dev(SCI_Bitmap);
    
    return 0;
}
#endif

void SAM_NVICConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);
    
    NVIC_InitStructure.NVIC_IRQChannel = SCI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

void SAM_Configuration(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_SCI2, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_SCI2, ENABLE);

	SAM_IOConfig();

	Select_SAM_VCC();
	//SAM_Config(0x01, 3000000);
    SCI_ConfigEMV(0x01, 3000000);

	SAM_NVICConfig();	
}

void LED_YELLOW_ON(uint8_t data)
{
	// mh1903_init.c 哪颇老 饶 力芭贸府
    GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);

	if (data == 1)
        GPIO_ResetBits(GPIOH, GPIO_Pin_4);
	else
        GPIO_SetBits(GPIOH, GPIO_Pin_4);
}

void LED_RED_ON(uint8_t data)
{
	// mh1903_init.c 哪颇老 饶 力芭贸府
    GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);

	if (data == 1)
        GPIO_ResetBits(GPIOH, GPIO_Pin_5);
	else
        GPIO_SetBits(GPIOH, GPIO_Pin_5);
}

void MST_Enable(uint8_t data)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);

	if (data == 1)
        GPIO_SetBits(GPIOH, GPIO_Pin_6);      
	else
        GPIO_ResetBits(GPIOH, GPIO_Pin_6);
}
// CPU_TXD2, CPU_RXD2 for debug, barcode
// 0 for debug, 1 for barcode
void UART_select(uint8_t data)
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

//gpio_data = GPIO_ReadInputData(GPIOC);
//	printf("gpio read data  : [%04X]\n\r",gpio_data);

uint8_t IC_Card_Detection(void)
{
    return GPIO_ReadInputDataBit(GPIOH, GPIO_Pin_13);
}

uint8_t Read_MST_SIG(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
}

uint8_t Read_CardEject(void)
{
    // GPIO_InitTypeDef  GPIO_InitStruct;
	// GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	// GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	// GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	// GPIO_Init(GPIOG, &GPIO_InitStruct);

    return GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_3);
}

uint8_t Read_BardcodeGoodLED(void)
{
    // GPIO_InitTypeDef  GPIO_InitStruct;
	// GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	// GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	// GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	// GPIO_Init(GPIOF, &GPIO_InitStruct);

    return GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_10);
}

void barcode_trigger(uint8_t data)
{
    // GPIO_InitTypeDef  GPIO_InitStruct;
	// GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	// GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	// GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	// GPIO_Init(GPIOF, &GPIO_InitStruct);

	if (data == 1)
        GPIO_SetBits(GPIOF, GPIO_Pin_9);        // barcode      
	else        
        GPIO_ResetBits(GPIOF, GPIO_Pin_9);      // debug
}

void barcode_rst()
{
    GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	GPIO_ResetBits(GPIOF, GPIO_Pin_8); 
    Delay_Ms(40);
    GPIO_SetBits(GPIOF, GPIO_Pin_8);     
}

void sam_mux_io(uint8_t port)
{
    // GPIO_InitTypeDef  GPIO_InitStruct;
	// GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	// GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	// GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	// GPIO_Init(GPIOD, &GPIO_InitStruct);

    switch (port)
    {
        case 0:
            GPIO_ResetBits(GPIOF, GPIO_Pin_10 | GPIO_Pin_11);
            break;
        case 1:
            GPIO_SetBits(GPIOD, GPIO_Pin_10);
            GPIO_ResetBits(GPIOD, GPIO_Pin_11);
            break;
        case 2:
            GPIO_ResetBits(GPIOD, GPIO_Pin_10);
            GPIO_SetBits(GPIOD, GPIO_Pin_11);
            break;
        case 3: 
            GPIO_SetBits(GPIOD, GPIO_Pin_10 | GPIO_Pin_11);
            break;
    }
}

uint8_t Read_solenoid_lock_sensor(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11);
}

uint8_t Read_card_end_sensor(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12);
}

uint8_t Read_anti_skimming_detect_sensor(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);
}

uint8_t read_inner_sensor(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);
}

uint8_t read_front_sensor(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);
}

uint8_t read_sam_card_detection(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
}

void MSR_Configuration(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStruct;

    //SPI2
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_SPI2, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_SPI2, ENABLE);

    GPIO_PinRemapConfig(GPIOE, GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5, GPIO_Remap_0); // GPIOE ??

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_0;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_RXFIFOFullThreshold = SPI_RXFIFOFullThreshold_1;
	SPI_InitStructure.SPI_TXFIFOEmptyThreshold = SPI_TXFIFOEmptyThreshold_10;
	
	SPI_Init(SPIM2, &SPI_InitStructure);
    SPI_Cmd(SPIM2, ENABLE);

	//TOUCH_IO_Configuration();
    // io init

	//PE12	SPI_CLK		ALT2
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_0;
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

void msr_rst(void)
{
    // GPIO_InitTypeDef  GPIO_InitStruct;
	// GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	// GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	// GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	// GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_ResetBits(GPIOB, GPIO_Pin_3); 
    Delay_Ms(40);
    GPIO_SetBits(GPIOB, GPIO_Pin_3);     
}

void sensor_power_on(uint8_t data)
{
//printf("%d, %s \n\r",data, __func__);
	//printf("------------------\n");
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	if (data == 1)
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
	else
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);

}
