#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mhscpu.h"
#include "mhscpu_it.h"
#include "uart.h"
//
#include "mhscpu_qspi.h"
#include "mhscpu_msr.h"
#include "CreditCard_lib.h"

extern uart_t uart;
extern void loop_back(uint8_t u8Slot);

// program star address 0x1001000 (0x80000)
#define FLASH_SIZE_BYTES        (1024 * 1024 - 64 * 1024)

#define FLASH_START_ADDR 	    0x1091000U            //0x1010000U
#define FLASH_SECTOR_SIZE 	    (0x1000)
#define FLASH_SECTOR_NUM 	    (FLASH_SIZE_BYTES / FLASH_SECTOR_SIZE)
#define FLASH_PAGE_NUM 	        (FLASH_SECTOR_NUM * 16)         

#define FLASH_ARRAY_ADDR		0x1008000U

typedef enum
{
	DATA_TYPE_ALL_ZERO = 0x0,
	DATA_TYPE_ALL_ONE,
	DATA_TYPE_A5A5A5A5,
	DATA_TYPE_ADDRESS_SELF,
    DATA_TYPE_00TOFF,
}FLASH_TEST_DATA_TYPE;


void UART_Configuration(void);

static void DataPrintf(void *buf, uint32_t bufsize);
static int DataCheck(void *src, void *dst, uint32_t size);
static int DataCheckReverse(void *src, void *dst, uint32_t size);
static int EraseCheck(uint32_t addr, uint32_t pagNum);
static int FlashTest(uint32_t pagNum, uint32_t dataType);

// main.c
uint8_t			Valid_Credit_Number[200]="";
uint8_t			ValidNumber[5]="";
uint8_t			CreditNumber[100]="";
uint32_t		Valid_Credit_Len=0;
//

static void DataPrintf(void *buf, uint32_t bufsize)
{
	uint32_t i = 0;
	uint8_t *pBuf = (uint8_t *)buf;
	
	if (0 != bufsize)
	{
		for (i = 0; i < bufsize; i++)
		{
			if (0 != i && 0 == i%16)
			{
				printf(" \n");			
			}

			printf("%02X ", pBuf[i]);
		}
	}
	printf("\n");
}

static int DataCheck(void *src, void *dst, uint32_t size)
{
	uint8_t *pSrc = (uint8_t *)src, *pDst = (uint8_t *)dst;
	
	if (memcmp(pDst, pSrc, size))
	{
		DataPrintf(pDst, size);
		return -1;
	}

	return 0;
}

static int DataCheckReverse(void *src, void *dst, uint32_t size)
{
	uint32_t i;
	uint8_t *pSrc = (uint8_t *)src, *pDst = (uint8_t *)dst;

	for (i = size - 1; i > 0; i--)
	{
		if (pSrc[i] != pDst[i])
		{
			return -1;
		}
	}
	
	return 0;
}

static int EraseCheck(uint32_t addr, uint32_t pagNum)
{
	uint32_t i = 0;
	uint8_t erase_Buf[X25Q_PAGE_SIZE];

	memset(erase_Buf, 0xFF, X25Q_PAGE_SIZE);	
 
	CACHE_CleanAll(CACHE);
	for (i = 0; i < pagNum; i++)
	{	
		if (-1 == DataCheck(erase_Buf, (uint8_t *)(addr + i * X25Q_PAGE_SIZE), sizeof(erase_Buf)))
		{
			printf("Erase Check failed!\n");
            while(1);
			return -1;
		}
	}	
	
	return 0;
}

int FLASH_read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    CACHE_CleanAll(CACHE);
    memcpy(buf, (uint8_t *)addr, len);

    return 0;
}

void flash_test_ok(void)
{
    uint32_t i = 0, j = 0;
	uint32_t write_Buf[X25Q_PAGE_SIZE/4] = {0};
    uint8_t data_buf_00toFF[X25Q_PAGE_SIZE] = {0};
    uint8_t read_buf[X25Q_PAGE_SIZE] = {0};
	uint32_t program_addr = FLASH_START_ADDR;

    printf("\n>>>  flash test..\n");

	//FlashTest(FLASH_PAGE_NUM, DATA_TYPE_A5A5A5A5);

    FLASH_EraseSector(FLASH_START_ADDR);
	EraseCheck(FLASH_START_ADDR, 16);

    memset(write_Buf, 0xA5, sizeof(write_Buf));

    FLASH_ProgramPage(program_addr, sizeof(write_Buf), (uint8_t*)(write_Buf));

    // FLASH_read(program_addr, read_buf, 16);

    // for (i=0; i<16; i++)
    // {
    //     printf("[%#02]", read_buf[i]);
    // }

    printf("Flash Read Data From Cache Forward Start\n");
	//for (i = 0; i < pagNum; i++)
	//{
		// if (DATA_TYPE_ADDRESS_SELF == dataType)
		// {
		// 	for (j = 0; j < X25Q_PAGE_SIZE/4; j++)
		// 	{
		// 		write_Buf[j] = program_addr + i * X25Q_PAGE_SIZE + j;
		// 	}
		// }
		
		CACHE_CleanAll(CACHE);
		if (-1 == DataCheck(write_Buf, (uint8_t *)(program_addr), sizeof(write_Buf)))
		{
			printf("Addr %#x data[Cache] check error!\n", program_addr);
            while(1);
			//return -1;
		}
	//}	
}

void flash_test(void)
{
    uint32_t i = 0, j = 0;
    uint8_t Data = 0x12;
	uint32_t write_Buf[X25Q_PAGE_SIZE/4] = {0};
    uint8_t data_buf_00toFF[X25Q_PAGE_SIZE] = {0};
    uint8_t read_buf[X25Q_PAGE_SIZE] = {0};
	uint32_t program_addr = FLASH_START_ADDR;

    printf("\n>>>  flash test..\n");

	//FlashTest(FLASH_PAGE_NUM, DATA_TYPE_A5A5A5A5);

    FLASH_EraseSector(FLASH_START_ADDR);
	EraseCheck(FLASH_START_ADDR, 16);

    //memset(write_Buf, 0x5A, sizeof(write_Buf));
    memset(write_Buf, 0x12, sizeof(write_Buf));

    // ok FLASH_ProgramPage(program_addr, /*sizeof(write_Buf)*/16, (uint8_t*)(write_Buf));
    // 1byte test
    //FLASH_ProgramPage(program_addr, /*sizeof(write_Buf)*/1, &Data);
    FLASH_ProgramPage(program_addr, /*sizeof(write_Buf)*/4, (uint8_t*)(write_Buf));
    // FLASH_read(program_addr, read_buf, 16);

    // for (i=0; i<16; i++)
    // {
    //     printf("[%#02]", read_buf[i]);
    // }
    #if 0
    printf("Flash Read Data From Cache Forward Start\n");
	//for (i = 0; i < pagNum; i++)
	//{
		// if (DATA_TYPE_ADDRESS_SELF == dataType)
		// {
		// 	for (j = 0; j < X25Q_PAGE_SIZE/4; j++)
		// 	{
		// 		write_Buf[j] = program_addr + i * X25Q_PAGE_SIZE + j;
		// 	}
		// }
		
		CACHE_CleanAll(CACHE);
		if (-1 == DataCheck(write_Buf, (uint8_t *)(program_addr), sizeof(write_Buf)))
		{
			printf("Addr %#x data[Cache] check error!\n", program_addr);
            while(1);
			//return -1;
		}
	//}
    #endif
}

static int FlashTest(uint32_t pagNum, uint32_t dataType)
{
	uint32_t i = 0, j = 0;
	uint32_t write_Buf[X25Q_PAGE_SIZE/4] = {0};
    uint8_t data_buf_00toFF[X25Q_PAGE_SIZE] = {0};
	uint32_t program_addr = FLASH_START_ADDR;
	
    for (i = 0; i < X25Q_PAGE_SIZE; i++)
    {
        data_buf_00toFF[i] = i;
    }
	
	switch (dataType)
	{
		case DATA_TYPE_ALL_ONE:
			memset(write_Buf, 0xFF, sizeof(write_Buf));			
			break;
	
		case DATA_TYPE_A5A5A5A5:
			memset(write_Buf, 0xA5, sizeof(write_Buf));
			break;
		
        case DATA_TYPE_00TOFF:
			memset(write_Buf, 0x00, sizeof(write_Buf));
            memcpy(write_Buf, data_buf_00toFF, sizeof(data_buf_00toFF));
            break;
        
		case DATA_TYPE_ADDRESS_SELF:
			break;
			
		case DATA_TYPE_ALL_ZERO:
		default:			
			memset(write_Buf, 0x00, sizeof(write_Buf));
			break;
	}

	printf("Flash Programe data: \n");
	for (i = 0; i < pagNum; i++)
	{
		if (DATA_TYPE_ADDRESS_SELF == dataType)
		{
			for (j = 0; j < X25Q_PAGE_SIZE/4; j++)
			{
				write_Buf[j] = program_addr + i * X25Q_PAGE_SIZE + j;
			}
		}

		FLASH_ProgramPage(program_addr + i * X25Q_PAGE_SIZE, sizeof(write_Buf), (uint8_t*)(write_Buf));	
	}

	printf("Flash Read Data From Cache Forward Start\n");
	for (i = 0; i < pagNum; i++)
	{
		if (DATA_TYPE_ADDRESS_SELF == dataType)
		{
			for (j = 0; j < X25Q_PAGE_SIZE/4; j++)
			{
				write_Buf[j] = program_addr + i * X25Q_PAGE_SIZE + j;
			}
		}
		
		CACHE_CleanAll(CACHE);
		if (-1 == DataCheck(write_Buf, (uint8_t *)(program_addr + i * X25Q_PAGE_SIZE), sizeof(write_Buf)))
		{
			printf("Addr %#x data[Cache] check error!\n", program_addr + i * X25Q_PAGE_SIZE);
            while(1);
			return -1;
		}
	}	
	
	printf("Flash Read Data From Cache Reverse Start\n");
	for (i = 0; i < pagNum; i++)
	{
		if (DATA_TYPE_ADDRESS_SELF == dataType)
		{
			for (j = 0; j < X25Q_PAGE_SIZE/4; j++)
			{
				write_Buf[j] = program_addr + i * X25Q_PAGE_SIZE + j;
			}
		}

		CACHE_CleanAll(CACHE);
		if (-1 == DataCheckReverse(write_Buf, (uint8_t *)(program_addr + i * X25Q_PAGE_SIZE), sizeof(write_Buf)))
		{
			printf("Addr %#x data[Cache] check error!\n", program_addr + i * X25Q_PAGE_SIZE);
            while(1);
			return -1;
		}
	}		
	
	printf("Flash Read Data Check OK \n");	
	return 0;
}

// msr test
void MSR_test_ok(void)
{
	track_data tdata[MAX_TRACK_NUM];
	int ret;
	int i, j;
	uint8_t cfmt, tflag;
	int cnts = 0;
	int lcd_line = 0;
	unsigned char raw_data_buf[256];
    uint8_t rxdata;
    int dir;

	set_wakeup_status(HARD_WAKEUP_WITHOUT_SLEEP);
	init_dpu();

	sc_sleep();

    dir = get_swipe_dir();
    printf("\nmsr dir[%d]\n", dir);
    // set_swipe_dir(SD_FORWARD_DIRECTION);
    // dir = get_swipe_dir();
    // printf("\nmsr dir[%d]\n", dir);

	//while(old_count == button_count)
    while (1)
	{
		switch (detect_swiping_card())
		{
		case DETECT_SWIPING_CARD:
			{
				printf("\r\nDetect swiping card,Times: %d \n", ++cnts);
				// LCD_DisplayColor(LCD_DISP_BLACK);
				// LCD_DisplayStr((uint8_t *)"Detect swiping card,Times:", 8, 8, LCD_DISP_RED,  LCD_DISP_BLACK );
				// LCD_DisplayOneInt(cnts, 224, 8, LCD_DISP_GREEN, LCD_DISP_BLACK, 5);

				cfmt = tflag = 0;
				#if 0
                ret = get_decode_data(tdata, TRACK_SELECT_1 | TRACK_SELECT_2 | TRACK_SELECT_3, &cfmt, &tflag);
				printf("T1 = %d, T2 = %d, T3 = %d\ttflag = %02X\n", (int)tdata[0].len, (int)tdata[1].len, (int)tdata[2].len, (int)tflag);
                #endif
                ret = get_decode_data(tdata, TRACK_SELECT_2 | TRACK_SELECT_3, &cfmt, &tflag);
				//printf("ret[%d], T2 = %d, T3 = %d\ttflag = %02X\n", ret, (int)tdata[1].len, (int)tdata[2].len, (int)tflag);
                printf("\n");
                switch (ret)
                {
                    case SUCCESS:
                        printf("success[0]\n");
                        break;
                    case INVALID_ADDR:
                        printf("invalid address[1]\n");
                        break;
                    case PARITY_ERR:
                        printf("parity error[2]\n");
                        break;
                    case LENGTH_ERR:
                        printf("length error[3]\n");
                        break;
                    case TRACK_NO_DATA:
                        printf("track no data[4]\n");
                        break;
                    case HAVE_NO_ZERO:
                        printf("have no zero[5]\n");
                        break;
                }
                printf("T2 = %d, T3 = %d\ttflag = %02X\n",(int)tdata[1].len, (int)tdata[2].len, (int)tflag);

				// LCD_DisplayStr((uint8_t *)"T1 = ", 8, 24, LCD_DISP_RED,  LCD_DISP_BLACK );
				// LCD_DisplayOneInt((int)tdata[0].len, 48, 24, LCD_DISP_RED, LCD_DISP_BLACK,5);

				// LCD_DisplayStr((uint8_t *)"T2 = ", 102, 24, LCD_DISP_RED,  LCD_DISP_BLACK );
				// LCD_DisplayOneInt((int)tdata[1].len, 142, 24, LCD_DISP_RED, LCD_DISP_BLACK,5);

				// LCD_DisplayStr((uint8_t *)"T3 = ", 196, 24, LCD_DISP_RED,  LCD_DISP_BLACK );
				// LCD_DisplayOneInt((int)tdata[2].len, 236, 24, LCD_DISP_RED, LCD_DISP_BLACK,5);
				
				// LCD_DisplayStr((uint8_t *)"tFlag = ", 286, 24, LCD_DISP_RED,  LCD_DISP_BLACK );
				// LCD_DisplayOneInt((int)tflag, 350, 24, LCD_DISP_RED, LCD_DISP_BLACK,5);

				// lcd_line = 56;

				if (ret == SUCCESS)		
				{
					for (i = 0; i < MAX_TRACK_NUM; i++)
					{
						if (tdata[i].len)
						{
							printf("T%d decode data:\n", (int)(i+1));
							// LCD_DisplayStr((uint8_t *)"T  decode data:", 8, lcd_line, LCD_DISP_RED,  LCD_DISP_BLACK );
							// LCD_DisplayOneChar((unsigned char)(i + 1) & 0x0F, 16, lcd_line, LCD_DISP_RED, LCD_DISP_BLACK);
							// lcd_line = lcd_line + 16; //LCD_FONT_H

							for (j = 0; j < tdata[i].len; j++)
							{
								putchar(tdata[i].buf[j]);
							}
							// LCD_DisplayStr( &tdata[i].buf[0], 8, lcd_line, LCD_DISP_GREEN,  LCD_DISP_BLACK );

							printf("\r\n");
							// lcd_line = lcd_line + 16;
							// lcd_line = lcd_line + 16;

						}

						printf("\n\rT%d raw data:\r\n", (int)(i+1));
						get_track_raw_data(raw_data_buf, i);

						for(j=0;j < 256; j++)
						{
							printf("%02X",raw_data_buf[j]);
						}

					}
					// beep(200);
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

        if(!isEmpty(&uart))
		{
			rxdata = pop(&uart);
			Uart0_SendDatas(&rxdata,1);
		}
	}
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
    uint8_t rxdata;
    int dir;

	set_wakeup_status(HARD_WAKEUP_WITHOUT_SLEEP);
	init_dpu();

	sc_sleep();

    dir = get_swipe_dir();
    printf("\nmsr dir[%d]\n", dir);
    // set_swipe_dir(SD_FORWARD_DIRECTION);
    // dir = get_swipe_dir();
    // printf("\nmsr dir[%d]\n", dir);

	//while(old_count == button_count)
    while (1)
	{
        //
        if(!isEmpty(&uart))
        {
            rxdata = pop(&uart);
            Uart0_SendDatas(&rxdata,1);
            switch (rxdata)
            {
                case 'x': return;
            }
        }
        //
		switch (detect_swiping_card())
		{
		case DETECT_SWIPING_CARD:
			{
				printf("\r\nDetect swiping card,Times: %d \n", ++cnts);
				// LCD_DisplayColor(LCD_DISP_BLACK);
				// LCD_DisplayStr((uint8_t *)"Detect swiping card,Times:", 8, 8, LCD_DISP_RED,  LCD_DISP_BLACK );
				// LCD_DisplayOneInt(cnts, 224, 8, LCD_DISP_GREEN, LCD_DISP_BLACK, 5);

				cfmt = tflag = 0;
				#if 0
                ret = get_decode_data(tdata, TRACK_SELECT_1 | TRACK_SELECT_2 | TRACK_SELECT_3, &cfmt, &tflag);
				printf("T1 = %d, T2 = %d, T3 = %d\ttflag = %02X\n", (int)tdata[0].len, (int)tdata[1].len, (int)tdata[2].len, (int)tflag);
                #endif
                ret = get_decode_data(tdata, TRACK_SELECT_2 | TRACK_SELECT_3, &cfmt, &tflag);
				//printf("ret[%d], T2 = %d, T3 = %d\ttflag = %02X\n", ret, (int)tdata[1].len, (int)tdata[2].len, (int)tflag);
                printf("\n");
                switch (ret)
                {
                    case SUCCESS:
                        printf("success[0]\n");
                        break;
                    case INVALID_ADDR:
                        printf("invalid address[1]\n");
                        break;
                    case PARITY_ERR:
                        printf("parity error[2]\n");
                        break;
                    case LENGTH_ERR:
                        printf("length error[3]\n");
                        break;
                    case TRACK_NO_DATA:
                        printf("track no data[4]\n");
                        break;
                    case HAVE_NO_ZERO:
                        printf("have no zero[5]\n");
                        break;
                }
                printf("T2 = %d, T3 = %d\ttflag = %02X\n",(int)tdata[1].len, (int)tdata[2].len, (int)tflag);

				// LCD_DisplayStr((uint8_t *)"T1 = ", 8, 24, LCD_DISP_RED,  LCD_DISP_BLACK );
				// LCD_DisplayOneInt((int)tdata[0].len, 48, 24, LCD_DISP_RED, LCD_DISP_BLACK,5);

				// LCD_DisplayStr((uint8_t *)"T2 = ", 102, 24, LCD_DISP_RED,  LCD_DISP_BLACK );
				// LCD_DisplayOneInt((int)tdata[1].len, 142, 24, LCD_DISP_RED, LCD_DISP_BLACK,5);

				// LCD_DisplayStr((uint8_t *)"T3 = ", 196, 24, LCD_DISP_RED,  LCD_DISP_BLACK );
				// LCD_DisplayOneInt((int)tdata[2].len, 236, 24, LCD_DISP_RED, LCD_DISP_BLACK,5);
				
				// LCD_DisplayStr((uint8_t *)"tFlag = ", 286, 24, LCD_DISP_RED,  LCD_DISP_BLACK );
				// LCD_DisplayOneInt((int)tflag, 350, 24, LCD_DISP_RED, LCD_DISP_BLACK,5);

				// lcd_line = 56;

				if (ret == SUCCESS)		
				{
					for (i = 0; i < MAX_TRACK_NUM; i++)
					{
						if (tdata[i].len)
						{
							printf("T%d decode data:\n", (int)(i+1));
							// LCD_DisplayStr((uint8_t *)"T  decode data:", 8, lcd_line, LCD_DISP_RED,  LCD_DISP_BLACK );
							// LCD_DisplayOneChar((unsigned char)(i + 1) & 0x0F, 16, lcd_line, LCD_DISP_RED, LCD_DISP_BLACK);
							// lcd_line = lcd_line + 16; //LCD_FONT_H

							for (j = 0; j < tdata[i].len; j++)
							{
								putchar(tdata[i].buf[j]);
							}
							// LCD_DisplayStr( &tdata[i].buf[0], 8, lcd_line, LCD_DISP_GREEN,  LCD_DISP_BLACK );

							printf("\r\n");
							// lcd_line = lcd_line + 16;
							// lcd_line = lcd_line + 16;

						}

						// printf("\n\rT%d raw data:\r\n", (int)(i+1));
						// get_track_raw_data(raw_data_buf, i);

						// for(j=0;j < 256; j++)
						// {
						// 	printf("%02X",raw_data_buf[j]);
						// }

					}
					// beep(200);
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

        // if(!isEmpty(&uart))
		// {
		// 	rxdata = pop(&uart);
		// 	Uart0_SendDatas(&rxdata,1);
		// }
	}
}
// end of msr test

// IFM test
// mh1903_init.c

#define IFM_VCC_5V_3V_PORT	GPIOH					
#define IFM_VCC_1_8V_PORT	GPIOH					
#define IFM_VCC_5V_3V_PIN	GPIO_Pin_1				
#define IFM_VCC_1_8V_PIN	GPIO_Pin_0	

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

//--------------------------
#if 0
// org
#define VCC_5V_3V_PORT	GPIOA
#define VCC_1_8V_PORT	GPIOF
#define VCC_5V_3V_PIN	GPIO_Pin_11
#define VCC_1_8V_PIN	GPIO_Pin_8

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

#define VCC_5V_3V_PORT	GPIOH					
#define VCC_1_8V_PORT	GPIOH					
#define VCC_5V_3V_PIN	GPIO_Pin_1				
#define VCC_1_8V_PIN	GPIO_Pin_0	

// void Select_EMV_VCC(void)
// {
// /*	
// #define VCC_5V_3V_PORT	GPIOA
// #define VCC_1_8V_PORT	GPIOF
// #define VCC_5V_3V_PIN	GPIO_Pin_11
// #define VCC_1_8V_PIN	GPIO_Pin_8	
// */	
// 	GPIO_InitTypeDef  GPIO_InitStruct;
// 	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
// 	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
// 	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
// 	GPIO_Init(GPIOH, &GPIO_InitStruct);


// 	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
// 	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
// 	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
// 	GPIO_Init(GPIOH, &GPIO_InitStruct);
	
//  	GPIO_SetBits(VCC_1_8V_PORT,VCC_1_8V_PIN);					//In order to use the VCC_5V_3V, you have to set the 1.8V Pin to High.
// //	GPIO_ResetBits(VCC_1_8V_PORT,VCC_1_8V_PIN);					//In order to use the 1.8V, you have to set the VCC_5V_3V Pin to High.
	
// 	GPIO_SetBits(VCC_5V_3V_PORT,VCC_5V_3V_PIN);		//5V select	
// //	GPIO_ResetBits(VCC_5V_3V_PORT,VCC_5V_3V_PIN);		//3V select

// }

void SCI_IOConfig(void)
{
    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, GPIO_Remap_0);

    //card detect
    SYSCTRL->PHER_CTRL &= ~BIT(16);
//    SYSCTRL->PHER_CTRL |= BIT(16);
    //Choose active level(Low level active).
    SYSCTRL->PHER_CTRL |= BIT(20);

}

void SCI_Configuration(void)
{
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_SCI0, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_SCI0, ENABLE);

	SCI_IOConfig();

	Select_EMV_VCC();
	SCI_ConfigEMV(0x01, 3000000);

	SCI_NVICConfig();	
}
#endif
//--------------------------

// SAM configuration
// void SAM_Configuration(void)
// {
// 	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_SCI2, ENABLE);
//     SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_SCI2, ENABLE);

// 	SAM_SCI2_IOConfig();

// 	Select_SAM_VCC();
// 	SCI_ConfigEMV(0x01, 3000000);

// 	//SCI_NVICConfig();	
// 	SAM_SCI2_NVICConfig();
// }
//

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
	
    printf("Valid_Credit_Len %d\n", Valid_Credit_Len);
    // for (i=0; i<4; i++)
    //     printf("[%d]", ValidNumber[i]);

    // for (i=0; i<(Valid_Credit_Len-4); i++)
    //     printf("[%d]", CreditNumber[i]);
    
	// LCD_DisplayColor(LCD_DISP_BLACK);
	
	// LCD_DisplayStr((uint8_t *)"Credit Card Valid Data / Credit Number.. ", 8, 24, LCD_DISP_GREEN,  LCD_DISP_BLACK );	
	
	// LCD_DisplayStr((uint8_t *)"Valid Data:", 8, lcd_line, LCD_DISP_CYAN,  LCD_DISP_BLACK );
	// lcd_line+=16;
	// LCD_DisplayStr(ValidNumber, 8, lcd_line, LCD_DISP_YELLOW,  LCD_DISP_BLACK );
	
	// lcd_line+=32;
	// LCD_DisplayStr((uint8_t *)"Credit Number:", 8, lcd_line, LCD_DISP_CYAN ,  LCD_DISP_BLACK );
	// lcd_line+=16;
	// LCD_DisplayStr(&CreditNumber[4], 8, lcd_line, LCD_DISP_YELLOW,  LCD_DISP_BLACK );	
	
	
	// lcd_line+=32;
	// LCD_DisplayStr((uint8_t *)"Remove and Insert IC Card Again...!! ", 8, lcd_line, LCD_DISP_MAGENTA,  LCD_DISP_BLACK );	
}

void IFM_test(void)
{
	uint8_t rxdata;
    printf("\nIFM test..\n");
    //IFM_Power_On(0);
    IC_CARD_detection(0);       //domyst
    while (1)
    //while(old_count == button_count)
	{
		if(!isEmpty(&uart))
        {
            rxdata = pop(&uart);
            Uart0_SendDatas(&rxdata,1);
            switch (rxdata)
            {
                case 'x': return;
				case 'i': IC_CARD_detection(0); break;
            }
        }
		GetCreditCardNumber();
		IC_CARD_detection(1);
		printf("---\n");
	}
}
// end of IFM test 

void sensor_test(void)
{
    uint8_t rxdata, ret;

    printf("\r\n sensor test..\n");
    while(1)
    {
        if(!isEmpty(&uart))
        {
            rxdata = pop(&uart);
            Uart0_SendDatas(&rxdata,1);
            switch (rxdata)
            {
                case 'f':
                    printf("\n front sensor");
                    ret = read_front_sensor();
                    printf("read : %d\n", ret);
                    break;
                case 'r':
                    printf("\n rear sensor");
                    ret = Read_card_end_sensor();
                    printf("read : %d\n", ret);
                    break;
                case 'i':
                    printf("\n inner sensor");
                    ret = read_inner_sensor();
                    printf("read : %d\n", ret);
                    break;
                case 's':
                    printf("\n solenoid lock sensor");
                    ret = Read_solenoid_lock_sensor();
                    printf("read : %d\n", ret);
                    break;
                case 'a':
                    printf("\n anti skimming detect sensor");
                    ret = Read_anti_skimming_detect_sensor();
                    printf("read : %d\n", ret);
                    break;
                case 'x':
                    return;
                case '?':
                    printf("\n 'f' : front sensor");
                    printf("\n 'r' : rear sensor");
                    printf("\n 'i' : inner sensor");
                    printf("\n 's' : solenoid lock sensor");
                    printf("\n 'a' : anti skimming detect sensor");
                    break;
            }
        }
    }
}

void led_test(void)
{
    uint8_t rxdata, ret;

    printf("\r\n led test..\n");
    while(1)
    {
        if(!isEmpty(&uart))
        {
            rxdata = pop(&uart);
            Uart0_SendDatas(&rxdata,1);
            switch (rxdata)
            {
                case '0':
                    printf("\n red off");
                    LED_RED_ON(0);
                    break;
                case '1':
                    printf("\n red on");
                    LED_RED_ON(1);
                    break;
                case '2':
                    printf("\n yellow off");
                    LED_YELLOW_ON(0);
                    break;
                case '3':
                    printf("\n yellow on");
                    LED_YELLOW_ON(1);
                    break;
                case 'x':
                    return;
            }
        }
    }
}

void external_int_test(void)
{
	// rear sensor, pc12, card remove
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStruct;

	printf("\n ext int test");
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	EXTI_LineConfig(EXTI_Line2, EXTI_PinSource12, EXTI_Trigger_Falling);

	//
	//GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}
// pwd test



// end of pwm
// void pwm_test1(void)
// {
// 	printf("pwm test\n");
// 	TimerPWMSetStructInit();
// 	TIMER_Configuration1();

// 	TIM_PWMSinglePulseConfig(TIMM0, TIM_3, ENABLE);
//     TIM_Cmd(TIMM0, TIM_3, ENABLE);

// 	PrintSet();
// }

// void pwm_test2(void)
// {
// 	printf("pwm test\n");
// 	TimerPWMSetStructInit();
// 	TIMER_Configuration1();

// 	//TIM_PWMSinglePulseConfig(TIMM0, TIM_3, ENABLE);
// 	//TIMER_PWMSet();
//     TIM_Cmd(TIMM0, TIM_3, ENABLE);

// 	PrintSet();
// }

void sol_power_on(uint8_t data)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	if (data == 1)
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
	else
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}

void eeprom_test(void)
{
	uint8_t rxdata, ret;
	printf("eeprom test..\n");

	while (1)
	{
		if(!isEmpty(&uart))
        {
            rxdata = pop(&uart);
            Uart0_SendDatas(&rxdata,1);
            switch (rxdata)
            {
                case 'r':
                    printf("\n eeprom read");
                    ret = AT24Cxx_ReadOneByte(0x00);
					printf("ret[%02X]\n", ret);
                    break;
				case 'w':
					printf("\n eeprom write");
					AT24Cxx_WriteOneByte(0x00, 0xA5);
					break;
                case 'i':
                    printf("\n find device");
                    AT24Cxx_FindDevice();
                    break;
				case 'x':
					return;
			}
		}
	}
	
}

void bios_test(void)
{
    uint8_t rxdata;

    printf("\r\n bios test..\n");
    while(1)
    {
        if(!isEmpty(&uart))
        {
            rxdata = pop(&uart);
            Uart0_SendDatas(&rxdata,1);
            switch (rxdata)
            {
                case 'm':
                    printf("\n msr test");
                    MSR_test();
                    break;
                case 'i':
                    printf("\n ifm test");
                    IFM_Power_On(1);
                    IFM_test();
                    break;
                case 'l':
                    printf("\n loopback test");
                    IFM_Power_On(1);
                    loop_back(0);
                    break;
                case '0':
                    printf("\n ifm_power_off");
                    // IC_CARD_detection(0);
                    IFM_Power_On(0);
                    break;
                case '1':
                    printf("\n ifm_power_on");
                    // IC_CARD_detection(0);
                    IFM_Power_On(1);
                    break;
                case 's':
                    printf("\n sensor test");
                    sensor_power_on(1);
                    sensor_test();
                    break;
                case '3':
                    //printf("\n led test");
                    led_test();
                    break;
				case '5':
					sensor_power_on(1);
					external_int_test();
					break;
				case '6':
					pwm_test();
					break;
				case '7':
					sol_power_on(0);
					break;
				case '8':
					sol_power_on(1);
					break;
				case 'e':
					eeprom_test();
					break;
                case '?':
                    printf("\n 'm' : msr test");
                    printf("\n 'i' : ifm test");
                    printf("\n 'l' : ifm loopback test");
                    printf("\n '0' : ifm power off test");
                    printf("\n '1' : ifm power on test");
                    printf("\n 's' : sensor test");
                    printf("\n '3' : led test");
					printf("\n '5' : external int");
					printf("\n '6' : pwm test");
                    // printf("\n 1 : msr test");
                    // printf("\n 1 : msr test");
					printf("\n 'e' : eeprom test");
                    break;
            }
        }
    }
}