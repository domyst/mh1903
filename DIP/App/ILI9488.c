#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "mhscpu_lcdi.h"
#include "mhscpu_dma.h"
#include "ILI9488.h"

//2.4 inch : 320*240
//3.5 inch : 480*320

//#define	GPIO_MODE		1
extern uint8_t fontAscii[];

LCD_InitTypeDef LCD_InitStructure; 

/******************* ?? ********************/
#define LCD_CMD_SWRESET					0x01
#define LCD_CMD_RDDIDIF					0x04
#define	LCD_CMD_READID1					0xDA
#define	LCD_CMD_READID2					0xDB
#define	LCD_CMD_READID3					0xDC
#define	LCD_CMD_IDMOFF					0x38
#define	LCD_CMD_IDMON					0x39
#define	LCD_CMD_PTLON					0x12
#define	LCD_CMD_NORON					0x13
#define LCD_CMD_SLPOUT                  0x11    ///< Sleep Out
#define LCD_CMD_DISPOFF                 0x28    ///< Display Off
#define LCD_CMD_DISPON                  0x29    ///< Display On
#define LCD_CMD_CASET                   0x2A    ///< Column Address Set
#define LCD_CMD_RASET                   0x2B    ///< Row Address Set
#define LCD_CMD_RAMWR                   0x2C    ///< Memory Write
#define LCD_CMD_RAMWRC                  0x3C    ///< Memory Write Continue
#define LCD_CMD_MADCTL                  0x36    ///< Memory Data Access Control
#define LCD_CMD_COLMOD                  0x3A    ///< Interface Pixel Format
#define LCD_CMD_IFMODE					0xB0	///< Interface Mode Control
#define LCD_CMD_FRMCTR1					0xB1	///< Frame Rate Control
#define LCD_CMD_PORCTRL                 0xB2    ///< Porch Setting
#define LCD_CMD_INVTR					0xB4	///< Display Inversion Control
#define LCD_CMD_DISCTRL					0xB5	///< Display Function Control
#define LCD_CMD_ETMOD					0xB6	///< Entry Mode Set
#define LCD_CMD_GCTRL                   0xB7    ///< Gate Control
#define LCD_CMD_VCOMS                   0xBB    ///< VCOM Setting
#define LCD_CMD_PWRCTRL1                0xC0    ///< Power Control 1
#define LCD_CMD_PWRCTRL2                0xC1    ///< Power Control 2
#define LCD_CMD_PWRCTRL3                0xC2    ///< Power Control 3
#define LCD_CMD_PWRCTRL4                0xC3    ///< Power Control 4
#define LCD_CMD_PWRCTRL5                0xC4    ///< Power Control 5
#define LCD_CMD_VMCTRL					0xC5	///< VCOM Control
#define LCD_CMD_VDVVRHEN                0xC2    ///< VDV and VRH Command Enable
#define LCD_CMD_FRCTRL2                 0xC6    ///< Frame Rate Control in Normal Mode
#define LCD_CMD_PWCTRL1                 0xD0    ///< Power Control 1
#define LCD_CMD_PVGAMCTRL               0xE0    ///< Positive Voltage Gamma Control
#define LCD_CMD_NVGAMCTRL               0xE1    ///< Negative Voltage Gamma Control
#define LCD_CMD_SETIMAGE				0xE9	///< Set Image Function
#define LCD_CMD_LCMCTRL                 0xC0    ///< LCM Control
#define LCD_CMD_CABCCTRL1               0xC6    ///< Frame Rate Control in Normal Mode
#define LCD_CMD_ADJCTRL3				0xF7	///< Adjust Control 3

/*************** CS?RESET?? ***************/
#define LCD_CS_HIGH()                   GPIO_SetBits(GPIOH, GPIO_Pin_6)
#define LCD_CS_LOW()                    GPIO_ResetBits(GPIOH, GPIO_Pin_6)

#define LCD_RST_HIGH()                  GPIO_SetBits(GPIOH, GPIO_Pin_8)
#define LCD_RST_LOW()                   GPIO_ResetBits(GPIOH, GPIO_Pin_8)

#define LCD_RD_HIGH()                   GPIO_SetBits(GPIOC, GPIO_Pin_13)
#define LCD_RD_LOW()                    GPIO_ResetBits(GPIOC, GPIO_Pin_13)

#define LCD_WR_HIGH()                   GPIO_SetBits(GPIOC, GPIO_Pin_14)
#define LCD_WR_LOW()                    GPIO_ResetBits(GPIOC, GPIO_Pin_14)

#define LCD_CD_HIGH()                   GPIO_SetBits(GPIOC, GPIO_Pin_15)
#define LCD_CD_LOW()                    GPIO_ResetBits(GPIOC, GPIO_Pin_15)

#define LCD_FONT_W                      8
#define LCD_FONT_H                      16


void LCD_DMAInit(uint8_t *base,uint32_t pageLenth);
void LCD_DMAWriteEnable(uint8_t *buf);

void LCD_WriteEx(uint8_t u8CD, uint8_t u8Value);
uint8_t LCD_ReadEx(uint8_t u8CD);

void	LCD_Write_GPIO(uint8_t u8CD, uint8_t u8Value);
uint8_t	LCD_Read_GPIO(void);

uint16_t HexToAscii(uint8_t value);

void LCD_IOConfig(void);
void LCD_Reset(void);
void LCD_InitSequence(void);
void LCD_InitSequence2(void);

void LCD_Configuration(void)
{
//2.4 inch : 320*240
//3.5 inch : 480*320
	
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_LCD | SYSCTRL_AHBPeriph_DMA, ENABLE);
    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_LCD | SYSCTRL_AHBPeriph_DMA, ENABLE); 

	LCD_IOConfig();

    LCD_InitStructure.LCD_BusMode = LCDI_MODE_8080;
	LCD_InitStructure.LCD_IntRead = 2;
    LCD_InitStructure.LCD_IntWrite = 0;
#ifdef DMA_FLUSH_SCREEN		
    LCD_InitStructure.LCD_MaxQTR = 0;
#else
    LCD_InitStructure.LCD_MaxQTR = 2;
#endif
    LCD_InitStructure.fifo_level = 16;   
    LCD_InitStructure.fifo_rst_enable = ENABLE;
    LCD_InitStructure.wr_dma_enable = ENABLE;
    LCD_InitStructure.wr_fifo_enable = ENABLE;

#if DIRECT_FLUSH_SCREEN	  
    LCD_InitStructure.ring_buf.rbRead.u32Head = 0;
    LCD_InitStructure.ring_buf.rbRead.u32Tail = 1;
    LCD_InitStructure.ring_buf.rbRead.u32BuffSize = LCD_READ_BUFF_SIZE;
    LCD_InitStructure.ring_buf.rbRead.pu8Buff = LCD_InitStructure.ring_buf.lcd_ReadBuff;

    LCD_InitStructure.ring_buf.rbWrite.u32Head = 0;
    LCD_InitStructure.ring_buf.rbWrite.u32Tail = 1;
    LCD_InitStructure.ring_buf.rbWrite.u32BuffSize = LCD_WRITE_BUFF_SIZE;
    LCD_InitStructure.ring_buf.rbWrite.pu8Buff = LCD_InitStructure.ring_buf.lcd_WriteBuff;

    LCD_InitStructure.ring_buf.rbCmd.u32Head = 0;
    LCD_InitStructure.ring_buf.rbCmd.u32Tail = 1;
    LCD_InitStructure.ring_buf.rbCmd.u32BuffSize = LCD_CMD_BUFF_SIZE;
    LCD_InitStructure.ring_buf.rbCmd.pu32Buff = LCD_InitStructure.ring_buf.lcd_CmdBuff;
    LCD_InitStructure.ring_buf.u32OpCount = 0;

    LCD_InitStructure.ring_buf.prbCmd = &LCD_InitStructure.ring_buf.rbCmd;
    LCD_InitStructure.ring_buf.prbRead = &LCD_InitStructure.ring_buf.rbRead;
    LCD_InitStructure.ring_buf.prbWrite = &LCD_InitStructure.ring_buf.rbWrite; 
#endif
    
	LCD_Init(LCD, &LCD_InitStructure);

	LCD_Reset();

	LCD_InitSequence();

//	LCD_RD_RDDIDIF();

}


void LCD_IOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /* LCD Remap
	   PC5~PC12:  LCD_DATA0~LCD_DATA7
	   PC13:      LCD_RD
	   PC14:      LCD_WR
	   PC15:      LCD_CD
	 */
#if GPIO_MODE
	GPIO_InitStruct.GPIO_Mode = 	GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin 	= 	GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | \
									GPIO_Pin_9 | GPIO_Pin_10| GPIO_Pin_11 | GPIO_Pin_12 | \
									GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Remap = 	GPIO_Remap_1;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	LCD_CD_HIGH();
	LCD_RD_HIGH();
	LCD_WR_HIGH();
#else
    GPIO_PinRemapConfig(GPIOC, GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | \
                               GPIO_Pin_9 | GPIO_Pin_10| GPIO_Pin_11 | GPIO_Pin_12 | \
                               GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15 , GPIO_Remap_0);
#endif
	
	/* PH6:       CS
	 */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);

	LCD_CS_HIGH();
    /* PH8:       RESET
	 */	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOH, &GPIO_InitStruct);	


/* PA2:       LED_EN
	 */	

GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
//	GPIO_ResetBits(GPIOA, GPIO_Pin_2); //LCD LED OFF

	GPIO_SetBits(GPIOA, GPIO_Pin_2);		//LCD LED ON

}

void LCD_DMAInit(uint8_t *base,uint32_t pageLenth)
{
	DMA_InitTypeDef DMA_InitStruct;
	
	//memory to peripheral
	DMA_InitStruct.DMA_DIR = DMA_DIR_Memory_To_Peripheral;
	
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)base;
	DMA_InitStruct.DMA_MemoryInc = DMA_Inc_Increment;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_MemoryBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_BlockSize = pageLenth;

	DMA_InitStruct.DMA_Peripheral = (uint32_t)LCD;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(LCD->lcdi_data);
	DMA_InitStruct.DMA_PeripheralInc = DMA_Inc_Nochange;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_DataSize_Byte;
	DMA_InitStruct.DMA_PeripheralBurstSize = DMA_BurstSize_1;
	DMA_InitStruct.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;
	DMA_Init(DMA_Channel_0, &DMA_InitStruct);
}

void LCD_DMAWriteEnable(uint8_t *buf)
{
	DMA_ClearITPendingBit(DMA_Channel_0, DMA_IT_DMATransferComplete);
	DMA_Channel_0->SAR_L = (uint32_t)buf;
	
	DMA_Cmd(ENABLE);
	DMA_ChannelCmd(DMA_Channel_0,ENABLE);
	
	while (!(DMA_GetRawStatus(DMA_Channel_0, DMA_IT_DMATransferComplete)));
	DMA_ClearITPendingBit(DMA_Channel_0, DMA_IT_DMATransferComplete);
}

void LCD_Write_GPIO(uint8_t u8CD, uint8_t u8Value)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	uint16_t	gpio_data;
	uint16_t	lcd_data;
	
	lcd_data = u8Value << 5;

	GPIO_InitStruct.GPIO_Mode = 	GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin 	= 	GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | \
									GPIO_Pin_9 | GPIO_Pin_10| GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Remap = 	GPIO_Remap_1;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	
	gpio_data = GPIO_ReadOutputData(GPIOC) & 0xE01F; //1110 0000 0001 1111
//	printf("Masked gpio_data  : [%04X]\n\r",gpio_data);
//	printf("Original lcd data : [%04X]\n\r",u8Value);
//	printf("Shfited lcd data  : [%04X]\n\r",lcd_data);
	
	gpio_data |= lcd_data;
//	printf("Output lcd data   : [%04X]\n\r",gpio_data);
//	printf("\n\r");

	
	GPIO_Write(GPIOC, gpio_data);
//	udelay(1);

	LCD_WR_LOW();

//	LCD_CS_LOW();
//	udelay(1);

	if(u8CD == LCDI_CMD)	
		LCD_CD_LOW();			//CMD
	else
		LCD_CD_HIGH();			//Data

//	udelay(1);

	LCD_WR_HIGH();
//	LCD_CS_HIGH();

}

void LCD_WriteEx(uint8_t u8CD, uint8_t u8Value)
{
#if GPIO_MODE
	LCD_Write_GPIO(u8CD, u8Value);
#else
//	LCD_CS_LOW();
	LCD_Write(LCD, u8CD, u8Value);
//	LCD_CS_HIGH();
#endif
}


uint8_t	LCD_Read_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	uint16_t	gpio_data;
	uint8_t result;

	GPIO_InitStruct.GPIO_Mode = 	GPIO_Mode_IPU;;
	GPIO_InitStruct.GPIO_Pin 	= 	GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | \
									GPIO_Pin_9 | GPIO_Pin_10| GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Remap = 	GPIO_Remap_1;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	LCD_CD_HIGH();

	LCD_RD_LOW();
	udelay(1);
	LCD_RD_HIGH();

	gpio_data = GPIO_ReadInputData(GPIOC);
//	printf("gpio read data  : [%04X]\n\r",gpio_data);

	result = (gpio_data >> 5) & 0xff;
//	printf("lcd  read data  : [%04X]\n\r",result);
	udelay(1);
	printf("\n\r");
	
	return result;
}

uint8_t LCD_ReadEx(uint8_t u8CD)
{
	uint8_t result;

#if GPIO_MODE
	result = LCD_Read_GPIO();
#else
	LCD_Read(LCD, u8CD, &result);
#endif
//	printf("result : [%02X]\n\r",result);
	return result;
}

void LCD_RD_RDDIDIF(void)
{
	uint8_t	data[8];
	
	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_RDDIDIF);
	mdelay(1);
	data[0] = LCD_ReadEx(LCDI_DAT);
	data[1] = LCD_ReadEx(LCDI_DAT);
	data[2] = LCD_ReadEx(LCDI_DAT);
	data[3] = LCD_ReadEx(LCDI_DAT);
	LCD_CS_HIGH();

	printf(" LCD module's manufacturer ID : [%02X]\n\r",data[1]);
	printf(" LCD module/driver version ID : [%02X]\n\r",data[2]);
	printf(" LCD module/driver ID         : [%02X]\n\r",data[3]);
	printf("\n\r");

	LCD_DisplayStr("LCD Module's Manufacturer ID : ", 8, 8,LCD_DISP_BLUE,  LCD_DISP_WRITE );
	LCD_DisplayOneChar((data[1] >> 4), 256, 8, LCD_DISP_BLUE,  LCD_DISP_WRITE );
	LCD_DisplayOneChar(data[1]&0x0F, 264, 8, LCD_DISP_BLUE,  LCD_DISP_WRITE );

	LCD_DisplayStr("LCD Module/driver version ID : ", 8, 32,LCD_DISP_BLUE,  LCD_DISP_WRITE );
	LCD_DisplayOneChar((data[2] >> 4), 256, 32, LCD_DISP_BLUE,  LCD_DISP_WRITE );
	LCD_DisplayOneChar(data[2]&0x0F, 264, 32, LCD_DISP_BLUE,  LCD_DISP_WRITE );

	LCD_DisplayStr("LCD module/driver ID         : ", 8, 56,LCD_DISP_BLUE,  LCD_DISP_WRITE );
	LCD_DisplayOneChar((data[3] >> 4), 256, 56, LCD_DISP_BLUE,  LCD_DISP_WRITE );
	LCD_DisplayOneChar(data[3]&0x0F, 264, 56, LCD_DISP_BLUE,  LCD_DISP_WRITE );

	mdelay(2000);
}

void LCD_Reset(void)
{

	LCD_RST_HIGH();
	udelay(100);
	LCD_RST_LOW();
    mdelay(100);
	LCD_RST_HIGH();
    mdelay(100);

}

void LCD_InitSequence_PSK(uint8_t sel)
{
	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_MADCTL);   //Memory Access 
	
	if(sel==90) LCD_WriteEx(LCDI_DAT, 0x88); 		//MY: Page Address order =1.		
	else LCD_WriteEx(LCDI_DAT, 0x28); 		//MV: Vertical Refresh Order bit =1.
//	LCD_WriteEx(LCDI_DAT, 0x28); 		//MV: Vertical Refresh Order bit =1.
//	LCD_WriteEx(LCDI_DAT, 0xE8); 		//MV,MX,MY :Row and Column Address order =1,  Row/Column Exchange = 1,   Vertical Refresh Order bit =1.
//	LCD_WriteEx(LCDI_DAT, 0x48); 		//MX: Column Address order =1.	
//	LCD_WriteEx(LCDI_DAT, 0x88); 		//MY: Page Address order =1.		

	LCD_CS_HIGH();
}
void LCD_InitSequence(void)
{

	/************* P-Gamma Setting **************/
	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_PVGAMCTRL);	
    LCD_WriteEx(LCDI_DAT, 0x00);
    LCD_WriteEx(LCDI_DAT, 0x03);
    LCD_WriteEx(LCDI_DAT, 0x0C);
    LCD_WriteEx(LCDI_DAT, 0x09);
    LCD_WriteEx(LCDI_DAT, 0x17);
    LCD_WriteEx(LCDI_DAT, 0x09);
    LCD_WriteEx(LCDI_DAT, 0x3E);
    LCD_WriteEx(LCDI_DAT, 0x89);
    LCD_WriteEx(LCDI_DAT, 0x49);
    LCD_WriteEx(LCDI_DAT, 0x08);
    LCD_WriteEx(LCDI_DAT, 0x0D);
    LCD_WriteEx(LCDI_DAT, 0x0A);
    LCD_WriteEx(LCDI_DAT, 0x13);
    LCD_WriteEx(LCDI_DAT, 0x15);
    LCD_WriteEx(LCDI_DAT, 0x0F);
	LCD_CS_HIGH();

	/************* N-Gamma Setting **************/
	LCD_CS_LOW();
    LCD_WriteEx(LCDI_CMD, LCD_CMD_NVGAMCTRL);
    LCD_WriteEx(LCDI_DAT, 0x00);
    LCD_WriteEx(LCDI_DAT, 0x11);
    LCD_WriteEx(LCDI_DAT, 0x15);
    LCD_WriteEx(LCDI_DAT, 0x03);
    LCD_WriteEx(LCDI_DAT, 0x0F);
    LCD_WriteEx(LCDI_DAT, 0x05);
    LCD_WriteEx(LCDI_DAT, 0x2D);
    LCD_WriteEx(LCDI_DAT, 0x34);
    LCD_WriteEx(LCDI_DAT, 0x41);
    LCD_WriteEx(LCDI_DAT, 0x02);
    LCD_WriteEx(LCDI_DAT, 0x0B);
    LCD_WriteEx(LCDI_DAT, 0x0A);
    LCD_WriteEx(LCDI_DAT, 0x33);
    LCD_WriteEx(LCDI_DAT, 0x37);
    LCD_WriteEx(LCDI_DAT, 0x0F);
	LCD_CS_HIGH();

	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_PWRCTRL1);	//Power Control 1   
	LCD_WriteEx(LCDI_DAT, 0x17);		//Vreg1out 
	LCD_WriteEx(LCDI_DAT, 0x15);		//Verg2out  
	LCD_CS_HIGH();

	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_PWRCTRL2);   //Power Control 2   
	LCD_WriteEx(LCDI_DAT, 0x41);  //VGH,VGL 
	LCD_CS_HIGH();
 
	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_VMCTRL);   //Power Control 3 
	LCD_WriteEx(LCDI_DAT, 0x00); 
	LCD_WriteEx(LCDI_DAT, 0x12);  //Vcom 
	LCD_WriteEx(LCDI_DAT, 0x80);
	LCD_CS_HIGH();

	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_MADCTL);   //Memory Access 
	LCD_WriteEx(LCDI_DAT, 0x28); 		//MV: Vertical Refresh Order bit =1.

// psk test
//	LCD_WriteEx(LCDI_DAT, 0xE8); 		//MV,MX,MY :Row and Column Address order =1,  Row/Column Exchange = 1,   Vertical Refresh Order bit =1.
//	LCD_WriteEx(LCDI_DAT, 0x48); 		//MX: Column Address order =1.	
//	LCD_WriteEx(LCDI_DAT, 0x88); 		//MY: Page Address order =1.		

LCD_CS_HIGH();
 
	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_COLMOD);   // Interface Pixel Format RGB565 16bit
	LCD_WriteEx(LCDI_DAT, 0x55); 
	LCD_CS_HIGH();
 
	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_IFMODE);   // Interface Mode Control //COLMOD
	LCD_WriteEx(LCDI_DAT, 0x00);   
	LCD_CS_HIGH();

	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_FRMCTR1);   //Frame rate 
	LCD_WriteEx(LCDI_DAT, 0xA0);  //60Hz 
	LCD_CS_HIGH();
 
	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_INVTR);   //Display Inversion Control 
	LCD_WriteEx(LCDI_DAT, 0x02);  //2-dot 
	LCD_CS_HIGH();

/* 
	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_ETMOD);   //RGB/MCU Interface Control 
	LCD_WriteEx(LCDI_DAT, 0x20);  //MCU 02
	LCD_WriteEx(LCDI_DAT, 0x02);  //Source,Gate scan dieection 
	LCD_CS_HIGH();
*/

	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_SETIMAGE);   // Set Image Function  
	LCD_WriteEx(LCDI_DAT, 0x00);  // Disable 24 bit data input 
	LCD_CS_HIGH();

	LCD_CS_LOW();
	LCD_WriteEx(LCDI_CMD, LCD_CMD_ADJCTRL3);    // Adjust Control 
	LCD_WriteEx(LCDI_DAT, 0xA9);   
	LCD_WriteEx(LCDI_DAT, 0x51);   
	LCD_WriteEx(LCDI_DAT, 0x2C);   
	LCD_WriteEx(LCDI_DAT, 0x82);   // D7 stream, loose 
	LCD_CS_HIGH();

	LCD_CS_LOW();
    LCD_WriteEx(LCDI_CMD, LCD_CMD_SLPOUT);     ///< Exit Sleep 
	LCD_CS_HIGH();
    udelay(120);

	LCD_CS_LOW();
    LCD_WriteEx(LCDI_CMD, LCD_CMD_DISPON);     ///< Display on 
	LCD_CS_HIGH();
}

void LCD_BlockWritePrep(unsigned int xStart, unsigned int xEnd, unsigned int yStart, unsigned int yEnd)
{
	LCD_CS_LOW();
    LCD_WriteEx(LCDI_CMD, LCD_CMD_CASET);
    LCD_WriteEx(LCDI_DAT, xStart >> 8);
    LCD_WriteEx(LCDI_DAT, xStart & 0xff);
    LCD_WriteEx(LCDI_DAT, xEnd >> 8);
    LCD_WriteEx(LCDI_DAT, xEnd & 0xff);
	LCD_CS_HIGH();
    
	LCD_CS_LOW();
    LCD_WriteEx(LCDI_CMD, LCD_CMD_RASET);
    LCD_WriteEx(LCDI_DAT, yStart >> 8);
    LCD_WriteEx(LCDI_DAT, yStart & 0xff);
    LCD_WriteEx(LCDI_DAT, yEnd >> 8);
    LCD_WriteEx(LCDI_DAT, yEnd & 0xff);
	LCD_CS_HIGH();

	LCD_CS_LOW();
    LCD_WriteEx(LCDI_CMD, LCD_CMD_RAMWR);
	LCD_CS_HIGH();
}

void LCD_DisplayColor(uint32_t color)
{
	uint32_t i,j;

	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);

	LCD_CS_LOW();
	for (i = 0; i < LCD_ROW_NUM; i++)
	{
	    for (j=0; j < LCD_COL_NUM; j++)
		{    
            LCD_Write(LCD, LCDI_DAT, color >> 8);
            LCD_Write(LCD, LCDI_DAT, color);
		}
	}
	LCD_CS_HIGH();
}



void LCD_DisplayGrayHor16(void)	 
{
	int i, j, k;

   	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);																								  

	LCD_CS_LOW();
    LCD_WriteEx(LCDI_CMD, LCD_CMD_RAMWR);
	for (i = 0; i < LCD_ROW_NUM; i++)
	{
		for (j = 0; j < LCD_COL_NUM % 16; j++)
		{
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
		}
		
		for (j = 0; j < 16; j++)
		{
	        for (k = 0;k < LCD_COL_NUM / 16; k++)
			{		               
                LCD_Write(LCD, LCDI_DAT, ((j*2) << 3) | ((j*4) >> 3));
                LCD_Write(LCD, LCDI_DAT, ((j*4) << 5) | (j*2));
			} 
		}
	}
	LCD_CS_HIGH();
}

void LCD_DisplayGrayHor32(void)	 
{
	unsigned int i, j, k;

   	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);																								  

	LCD_CS_LOW();
    LCD_WriteEx(LCDI_CMD, LCD_CMD_RAMWR);

	for (i = 0; i < LCD_ROW_NUM; i++)
	{
		for (j = 0; j < LCD_COL_NUM % 32; j++)
		{
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
		}
		
		for (j = 0; j < 32; j++)
		{
	        for (k = 0; k < LCD_COL_NUM / 32; k++)
			{		
                LCD_Write(LCD, LCDI_DAT, (j << 3) | ((j*2) >> 3));
                LCD_Write(LCD, LCDI_DAT, ((j*2) << 5) | j);
			} 
		}
	}
	LCD_CS_HIGH();	
}

void LCD_DisplayBand(void)	 
{
	int i, j, k;
	unsigned int color[8]={0xf800,0xf800,0x07e0,0x07e0,0x001f,0x001f,0xffff,0xffff};

   	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);

    LCD_CS_LOW();
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < LCD_ROW_NUM / 8; j++)
		{
	        for (k = 0; k < LCD_COL_NUM; k++)
			{
                LCD_Write(LCD, LCDI_DAT, color[i] >> 8);
                LCD_Write(LCD, LCDI_DAT, color[i]);
			} 
		}
	}
    
	for (j = 0; j < LCD_ROW_NUM % 8; j++)
	{
        for (k = 0; k < LCD_COL_NUM; k++)
		{
            LCD_Write(LCD, LCDI_DAT, color[7] >> 8);
            LCD_Write(LCD, LCDI_DAT, color[7]); 
		} 
	}
    LCD_CS_HIGH();
}

void LCD_DisplayFrame(void)
{
    int i;
	
	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1); 
    
    LCD_CS_LOW();
	
    LCD_Write(LCD, LCDI_DAT, 0xf8);
    LCD_Write(LCD, LCDI_DAT, 0x00);
    
    for (i = 0; i < LCD_COL_NUM - 2; i++)
    {
        LCD_Write(LCD, LCDI_DAT, 0xf8);
        LCD_Write(LCD, LCDI_DAT, 0x00);
        for (i = 0; i < LCD_COL_NUM - 2; i++)
        {
            LCD_Write(LCD, LCDI_DAT, 0x00);
            LCD_Write(LCD, LCDI_DAT, 0x00);
        }
        LCD_Write(LCD, LCDI_DAT, 0x00);
        LCD_Write(LCD, LCDI_DAT, 0x1F);
    }
    
    LCD_Write(LCD, LCDI_DAT, 0xf8);
    LCD_Write(LCD, LCDI_DAT, 0x00);
    for (i = 0; i < LCD_COL_NUM - 2; i++)
    {
        LCD_Write(LCD, LCDI_DAT, 0xFF);
        LCD_Write(LCD, LCDI_DAT, 0xFF);
    }
    
    LCD_Write(LCD, LCDI_DAT, 0x00);
    LCD_Write(LCD, LCDI_DAT, 0x1F);

    LCD_CS_HIGH();	
}

void LCD_DisplayScaleHor1(void)
{
	int i, j, k;
	
	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);
	
    LCD_CS_LOW();
	/************** black -> red **************/	
	for (k = 0; k < LCD_ROW_NUM / 4; k++)
	{
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < LCD_COL_NUM / 32; j++)	 
			{
                LCD_Write(LCD, LCDI_DAT, i<<3);
                LCD_Write(LCD, LCDI_DAT, 0);
			}
		}
	}

	/************** black -> green **************/	
	for (k = 0; k < LCD_ROW_NUM / 4; k++)
	{
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < LCD_COL_NUM / 32; j++)
			{
                LCD_Write(LCD, LCDI_DAT, (i*2) >> 3);
                LCD_Write(LCD, LCDI_DAT, (i*2) << 5);
			}
		}
	}

	/************** black -> blue **************/	
	for (k = 0; k < LCD_ROW_NUM / 4; k++)
	{
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < LCD_COL_NUM / 32; j++)
			{
                LCD_Write(LCD, LCDI_DAT, 0);
                LCD_Write(LCD, LCDI_DAT, i);
			}
		}
	}

	/************** black -> write **************/
	for (k = 0; k < (LCD_ROW_NUM / 4 + LCD_ROW_NUM % 4); k++)
	{
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < LCD_COL_NUM / 32; j++)
			{
                LCD_Write(LCD, LCDI_DAT, (i << 3) | ((i*2) >> 3));
                LCD_Write(LCD, LCDI_DAT, ((i*2) << 5) | i);
			}
		}
	}
	
	LCD_CS_HIGH();	
}

void LCD_DisplayScaleHor2(void)
{
	int i, j, k;
	
	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);
	
    LCD_CS_LOW();
	/************** black -> red **************/		
	for (k = 0; k < LCD_ROW_NUM / 6; k++)
	{
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {                
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < LCD_COL_NUM/32; j++)	 
			{
                LCD_Write(LCD, LCDI_DAT, i << 3);
                LCD_Write(LCD, LCDI_DAT, 0);
			}
		}
	}

	/************** red -> black **************/
	for (k = 0; k < LCD_ROW_NUM / 6; k++)
	{	
		for (i = 32; i > 0; i--)
		{
			for (j = 0; j < LCD_COL_NUM/32; j++)
			{
                LCD_Write(LCD, LCDI_DAT, (i-1) << 3);
                LCD_Write(LCD, LCDI_DAT, 0);
			}
		}
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
	}
	
	/************** black -> green **************/
	for (k = 0; k < LCD_ROW_NUM / 6; k++)
	{
		for (i = 0; i < LCD_COL_NUM % 64; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 64; i++)
		{
			for (j = 0; j < LCD_COL_NUM / 64; j++)
			{
                LCD_Write(LCD, LCDI_DAT, i >> 3);
                LCD_Write(LCD, LCDI_DAT, i << 5);
			}
		}
	}

	/************** green -> black **************/
	for (k = 0; k < LCD_ROW_NUM / 6; k++)
	{
		for (i = 64; i > 0; i--)
		{
			for (j = 0; j < LCD_COL_NUM/64; j++)
			{
                LCD_Write(LCD, LCDI_DAT, (i-1) >> 3);
                LCD_Write(LCD, LCDI_DAT, (i-1) << 5);
			}
		}
		for (i = 0; i < LCD_COL_NUM % 64; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
	}	

	/************** black -> blue **************/
	for (k = 0; k < LCD_ROW_NUM / 6; k++)
	{
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < LCD_COL_NUM / 32; j++)
			{
                LCD_Write(LCD, LCDI_DAT, 0);
                LCD_Write(LCD, LCDI_DAT, i);
			}
		}
	}

	/************** blue -> black **************/
	for (k = 0; k < (LCD_ROW_NUM / 6 + LCD_ROW_NUM % 6); k++)
	{	
		for (i = 32; i > 0; i--)
		{
			for (j = 0; j < LCD_COL_NUM / 32; j++)
			{
                LCD_Write(LCD, LCDI_DAT, 0);
                LCD_Write(LCD, LCDI_DAT, (i-1));  
			}
		}
		for (i = 0; i < LCD_COL_NUM % 32; i++)
        {            
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
        }
	}
	
	LCD_CS_HIGH();		
}

void LCD_DisplayScaleVer(void)
{
	int i, j, k;
	
	LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);

	LCD_CS_LOW();
	/*********** LCD_DISP_BLACK ***********/
	for (k = 0; k < LCD_ROW_NUM % 32; k++)
	{
		for (j = 0; j < LCD_COL_NUM; j++)
		{
            LCD_Write(LCD, LCDI_DAT, 0);
            LCD_Write(LCD, LCDI_DAT, 0);
		}			
	}

	for (k = 0; k < 32; k++)
	{
		for (i = 0; i < LCD_ROW_NUM / 32; i++)
		{	
			/******** LCD_DISP_RED ********/
			for (j = 0; j < LCD_COL_NUM / 4; j++)
			{
                LCD_Write(LCD, LCDI_DAT, k<<3);
                LCD_Write(LCD, LCDI_DAT, 0);
			}
			/******* LCD_DISP_GREEN *******/
			for (j = 0; j < LCD_COL_NUM / 4; j++)
			{
                LCD_Write(LCD, LCDI_DAT, (k*2) >> 3);
                LCD_Write(LCD, LCDI_DAT, (k*2) << 5);
			}
			/******* LCD_DISP_BLUE *******/
			for (j = 0; j < LCD_COL_NUM / 4; j++)
			{
                LCD_Write(LCD, LCDI_DAT, 0);
                LCD_Write(LCD, LCDI_DAT, k);
			}
			/******* LCD_DISP_GRAY *******/										  
			for (j = 0; j < LCD_COL_NUM / 4; j++)
			{
                LCD_Write(LCD, LCDI_DAT, (k << 3) | ((k*2) >> 3));
                LCD_Write(LCD, LCDI_DAT, ((k*2) << 5) | k);
			}
		}
	}
	
	LCD_CS_HIGH();	
}

void LCD_DisplayPicDirect(const uint8_t *picture)
{
    uint32_t i;
	
    LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);
    
	LCD_CS_LOW();
    for (i = 0; i < LCD_COL_NUM*LCD_ROW_NUM*2; i++)
	{
        LCD_Write(LCD, LCDI_DAT, (*(picture + i)));
    }
	LCD_CS_HIGH();		
}

void LCD_DisplayPicDirect_Partial(uint8_t *picture, uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y)
{
    uint32_t i;
	
    LCD_BlockWritePrep(offset_x, size_x+offset_x-1, offset_y, size_y+offset_y-1);
   
	LCD_CS_LOW();	
    for (i = 0; i < size_x * size_y * 2; i++)
    {
        LCD_Write(LCD, LCDI_DAT, (*(picture + i)));
    }
  	LCD_CS_HIGH();  

    LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);
}

void LCD_DisplayPicDMA(uint8_t *picture)
{
    uint32_t i;
	
    LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);
    
    LCD_DMAInit(picture, (uint32_t)(LCD_COL_NUM*2)); 
    LCD_FIFODMAconfig(LCD, &LCD_InitStructure);
	
	LCD_CS_LOW();	
    for (i = 0; i < LCD_ROW_NUM; i++)
    {
        LCD->lcdi_ctrl |= LCDI_CTRL_CD;
        LCD_DMAWriteEnable(picture + i*(LCD_COL_NUM)*2);
    }
  	LCD_CS_HIGH();  
}

void LCD_DisplayPicDMA_Partial(uint8_t *picture, uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y)
{
    uint32_t i;
/*	
    LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);
    
    LCD_DMAInit(picture, (uint32_t)(LCD_COL_NUM*2)); 
    LCD_FIFODMAconfig(LCD, &LCD_InitStructure);
	
	LCD_CS_LOW();	
    for (i = 0; i < LCD_ROW_NUM; i++)
    {
        LCD->lcdi_ctrl |= LCDI_CTRL_CD;
        LCD_DMAWriteEnable(picture + i*(LCD_COL_NUM)*2);
    }
  	LCD_CS_HIGH();  	
*/	
	
    LCD_BlockWritePrep(offset_x, size_x+offset_x-1, offset_y, size_y+offset_y-1);

    LCD_DMAInit(picture, (uint32_t)(size_x*2)); 
    LCD_FIFODMAconfig(LCD, &LCD_InitStructure);
	
	LCD_CS_LOW();	
    for (i = 0; i < size_y; i++)
    {
        LCD->lcdi_ctrl |= LCDI_CTRL_CD;
        LCD_DMAWriteEnable(picture + i*(size_x)*2); 
    }
  	LCD_CS_HIGH();  

	
//    LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);
}

void LCD_WriteOneDot(uint32_t color)
{
	LCD_CS_LOW();
	
	LCD_Write(LCD, LCDI_DAT, color >> 8);
	LCD_Write(LCD, LCDI_DAT, color);
	
  	LCD_CS_HIGH(); 
}

void LCD_DisplayOneDot(uint32_t xStart, uint32_t yStart, uint16_t Color)
{
    LCD_BlockWritePrep(xStart, LCD_COL_NUM-1, yStart, LCD_ROW_NUM-1);
	LCD_WriteOneDot(Color);
}

void LCD_DisplayOneChar(uint8_t ord, uint32_t xStart, uint32_t yStart, uint32_t textColor, uint32_t backColor)	///< ord: 0~95
{													  
	uint8_t i, j;
	uint8_t *p, data;
	uint32_t index;

	LCD_BlockWritePrep(xStart, xStart + (LCD_FONT_W - 1), yStart, yStart+(LCD_FONT_H - 1));

	index = ord;
	if (index > 95)	   //95:ASCII CHAR NUM
	{
		index = 95; 
	}

	index = index * ((LCD_FONT_W/8) * LCD_FONT_H);	 

	p = fontAscii + index;
	for (i = 0; i < (LCD_FONT_W/8*LCD_FONT_H); i++)
	{
		data = *p++;
		for (j = 0; j < 8; j++)
		{
			if ((data << j) & 0x80)
			{
				LCD_WriteOneDot(textColor);
			}      
			else 
			{
				LCD_WriteOneDot(backColor);	  
			}
		}
	}
}

void LCD_DisplayOneInt(uint16_t data, uint32_t xStart, uint32_t yStart, uint32_t textColor, uint32_t backColor,uint8_t digit_cnt)
{
	uint8_t	digit_4, digit_3, digit_2, digit_1, digit_0;
	uint32_t x_pos;

	x_pos = xStart;
	
	digit_4 = data / 10000;
	digit_3 = (data - digit_4 * 10000) / 1000;
	digit_2 = (data - digit_4 * 10000 - digit_3 * 1000) / 100;
	digit_1 = (data - digit_4 * 10000 - digit_3 * 1000 - digit_2 * 100) / 10;
	digit_0 = (data - digit_4 * 10000 - digit_3 * 1000 - digit_2 * 100 - digit_1 * 10);
	
//	printf("%05d [%d] [%d] [%d] [%d] [%d]\n\r",data, digit_4, digit_3, digit_2, digit_1, digit_0);

	if(digit_cnt >= 5 )
	{
		LCD_DisplayOneChar(digit_4, x_pos, yStart, textColor, backColor);
		x_pos = x_pos + LCD_FONT_W;
	}
	
	if(digit_cnt >= 4 )
	{
		LCD_DisplayOneChar(digit_3, x_pos, yStart, textColor, backColor);
		x_pos = x_pos + LCD_FONT_W;
	}
	
	if(digit_cnt >= 3 )
	{
		LCD_DisplayOneChar(digit_2, x_pos, yStart, textColor, backColor);
		x_pos = x_pos + LCD_FONT_W;
	}
	
	if(digit_cnt >= 2 )
	{
		LCD_DisplayOneChar(digit_1, x_pos, yStart, textColor, backColor);
		x_pos = x_pos + LCD_FONT_W;
	}

	LCD_DisplayOneChar(digit_0, x_pos, yStart, textColor, backColor);
}

/*
 * ascii 32~90(!~Z), (32~47)??~/,(48~57)0~9,(58~64):~@,(65~126)A~~
 * ord 0~95, (48~57)0~9,(65~126)A~z,(33~47)!~/,(58~64):~@
 */
static uint8_t s_toOrd(uint8_t ch)
{
	if (ch < 32)
	{
		ch = 95;
	}
	else if ((ch >= 32) && (ch <= 47))  ///< (32~47)??~/
	{
		ch = (ch - 32) + 10 + 62;
	}
	else if ((ch >= 48) && (ch <= 57))  ///< (48~57)0~9
	{
		ch = ch - 48;
	}
	else if ((ch >= 58) && (ch <= 64))  ///< (58~64):~@
	{
		ch = (ch - 58) + 10 + 62 + 16;
	}
	else if ((ch >= 65) && (ch <= 126)) ///< (65~126)A~~
	{
		ch = (ch - 65) + 10;
	}
	else if (ch > 126)
	{		
		ch = 95;
	}

	return ch;
}

void LCD_DisplayStr(uint8_t *str, uint32_t xStart, uint32_t yStart, uint32_t textColor, uint32_t backColor)
{
	while (!(*str == '\0'))
	{
		LCD_DisplayOneChar(s_toOrd(*str++), xStart, yStart, textColor, backColor);

		if (xStart > ((LCD_COL_NUM-1) - LCD_FONT_W))
		{
			xStart = 0;
		    yStart = yStart + LCD_FONT_H;
		}
		else
		{
			xStart = xStart + LCD_FONT_W;
		}

		if (yStart > ((LCD_ROW_NUM - 1) - LCD_FONT_H))
		{
			yStart=0;
		}
	}
	
    LCD_BlockWritePrep(0, LCD_COL_NUM-1, 0, LCD_ROW_NUM-1);
}


void LCD_DisplayStr90(uint8_t *str, uint32_t xStart, uint32_t yStart, uint32_t textColor, uint32_t backColor)
{
	while (!(*str == '\0'))
	{
		LCD_DisplayOneChar(s_toOrd(*str++), xStart, yStart, textColor, backColor);

		if (xStart > ((LCD_COL_NUM90-1) - LCD_FONT_W))
		{
			xStart = 0;
		    yStart = yStart + LCD_FONT_H;
		}
		else
		{
			xStart = xStart + LCD_FONT_W;
		}

		if (yStart > ((LCD_ROW_NUM90 - 1) - LCD_FONT_H))
		{
			yStart=0;
		}
	}
	
    LCD_BlockWritePrep(0, LCD_COL_NUM90-1, 0, LCD_ROW_NUM90-1);
}




void LCD_DrawLine(uint32_t xStart, uint32_t xEnd, uint32_t yStart, uint32_t yEnd, uint32_t color)
{
	uint32_t i, j;

	LCD_BlockWritePrep(xStart, xEnd, yStart, yEnd);

	for (i = yStart; i < yEnd + 1; i++)
	{
		for (j = xStart; j < xEnd + 1; j++)
		{
			LCD_WriteOneDot(color);
		}
	}
}

void LCD_DrawGird(uint32_t color)
{
	uint32_t i;

	for (i = 15; i < LCD_ROW_NUM-1; i = i+16)
	{
		LCD_DrawLine(0, LCD_COL_NUM-1, i, i, color);
	}

	for (i = 15; i < LCD_COL_NUM-1; i = i+16)
	{
		LCD_DrawLine(i, i, 0, LCD_ROW_NUM-1, color);
	}
}

uint16_t HexToAscii(uint8_t value)
{
	uint16_t result;

	if ((value & 0x0F) >9)
	{
		result = ((value & 0x0F) - 10 + 'A') << 8;
	}
	else
	{
		result = ((value & 0x0F) + '0') << 8;
	}
//	printf("HexToAscii : %d\n\r",result);

	if (((value & 0xF0) >> 4) >9)
	{
		result |= (((value & 0xF0) >> 4) - 10 + 'A');
	}
	else
	{
		result |= (((value & 0xF0) >> 4) + '0');
	}
    
//	printf("HexToAscii : %s\n\r",&result);
    return result;
}