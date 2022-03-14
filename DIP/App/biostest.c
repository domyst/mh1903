#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mhscpu.h"
#include "mhscpu_it.h"
//
#include "mhscpu_qspi.h"

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
