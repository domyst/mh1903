/*
 * i2c.c
 *
 *  Created on: 2021. 9. 13.
 *      Author: hyesun
 */
//#define USE_REAL_EEPROM
#define MCRW_DIP

#if defined(USE_REAL_EEPROM)
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

//#include "stm32f10x.h"
//#include "stm32f10x_lib.h"
//#include "main.h"
#include "i2c.h"
#include <string.h>

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64		8191
#define AT24C128	16383
#define AT24C256	32767

//#define EE_TYPE		AT24C64
//#define EEP_PAGE_SIZE	32
static u16 EE_TYPE;// = AT24C64;
static u8 EEP_PAGE_SIZE;// = 32;

//#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
//#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
//#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

//#define GPIOB_ODR_Addr    1//(GPIOB_BASE+12) //0x40010C0C

//#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)
#if 0
void PBout(u8 n)
{
	gpio_pin_config_t EEPROM_PIN_config = {
		.direction = kGPIO_DigitalOutput,
		.outputLogic = 0U,
		.interruptMode = kGPIO_NoIntmode
	};
	GPIO_PinInit(GPIO3, n, &EEPROM_PIN_config);
}
#endif
//#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)
#if 0
void PBin(u8 n)
{
	gpio_pin_config_t EEPROM_PIN_config = {
		.direction = kGPIO_DigitalInput,
		.outputLogic = 0U,
		.interruptMode = kGPIO_NoIntmode
	};
	GPIO_PinInit(GPIO3, n, &EEPROM_PIN_config);
}
#endif

// DIP
//#define SDA_IN()  {GPIOB->CRL &= (u32)0X0FFFFFFF; GPIOB->CRL |= (u32)(8<<28);}
//#define SDA_OUT() {GPIOB->CRL &= (u32)0X0FFFFFFF; GPIOB->CRL |= (u32)(3<<28);}
void SDA_IN(void)	
{
	gpio_pin_config_t EEPROM_SDA_config = {
		.direction = kGPIO_DigitalInput,
		.outputLogic = 0U,
		.interruptMode = kGPIO_NoIntmode
	};
	GPIO_PinInit(GPIO3, 23U, &EEPROM_SDA_config);
}

void SDA_OUT(void)	
{
	gpio_pin_config_t EEPROM_SDA_config = {
		.direction = kGPIO_DigitalOutput,
		.outputLogic = 0U,
		.interruptMode = kGPIO_NoIntmode
	};
	GPIO_PinInit(GPIO3, 23U, &EEPROM_SDA_config);
}


#define IIC_SCL(n)	GPIO_PinWrite(GPIO3, 22U, n);//PBout(22U)//PBout(6) //SCL
#define IIC_SDA(n)	GPIO_PinWrite(GPIO3, 23U, n);//PBout(23U)//PBout(7) //SDA
//#define READ_SDA	1///GPIO_PinRead(GPIO3, 23U);//(GPIOB->IDR & GPIO_Pin_7) //SDA

#define IIC_WP(n)	GPIO_PinWrite(GPIO3, 24U, n);//PBout(24U)//PBout(5)

//#define READ_SDA (uint32_t)GPIO_PinRead(GPIO3, 23U);//(GPIOB->IDR & GPIO_Pin_7) //SDA


#if 0
void IIC_initial(void)			// eeprom ��� �߰� 2014.01.14
{
	GPIO_InitTypeDef GPIO_InitStructure;

#if defined(MCRW_SLIM)
	/* Enable GPIOB */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // SCL
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

#elif defined(MCRW_STANDALONE)
	/* Enable GPIOB */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; // SCL
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; // SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

#elif defined(MCRW_DIP)
	/* Enable GPIOB */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; // WP
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // SCL
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; // SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	IIC_WP=1; // write disable
#endif
}
#endif

static void IIC_Start(void)
{
	SDA_OUT();
	IIC_SDA(1);//IIC_SDA=1;
	IIC_SCL(1);//IIC_SCL=1;
	delay_us(2);
 	IIC_SDA(0);//IIC_SDA=0;//START:when CLK is high,DATA change form high to low
	delay_us(2);
	IIC_SCL(0);//IIC_SCL=0;
}

static void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SCL(0);//IIC_SCL=0;
	IIC_SDA(0);//IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(2);
	IIC_SCL(1);//IIC_SCL=1;
	IIC_SDA(1);//IIC_SDA=1;
	delay_us(2);
}

static u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();
	IIC_SDA(1);//IIC_SDA=1;
	delay_us(1);
	IIC_SCL(1);//IIC_SCL=1;
	delay_us(1);
	while(GPIO_PinRead(GPIO3, 23U))//while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL(0);//IIC_SCL=0;
	return 0;
}

static void IIC_Ack(void)
{
	IIC_SCL(0);//IIC_SCL=0;
	SDA_OUT();
	IIC_SDA(0);//IIC_SDA=0;
	delay_us(1);
	IIC_SCL(1);//IIC_SCL=1;
	delay_us(1);
	IIC_SCL(0);//IIC_SCL=0;
}

static void IIC_NAck(void)
{
	IIC_SCL(0);//IIC_SCL=0;
	SDA_OUT();
	IIC_SDA(1);//IIC_SDA=1;
	delay_us(1);
	IIC_SCL(1);//IIC_SCL=1;
	delay_us(1);
	IIC_SCL(0);//IIC_SCL=0;
}

static void IIC_Send_Byte(u8 txd)
{
  u8 t;
	SDA_OUT();
  IIC_SCL(0);//IIC_SCL=0;
  for(t=0;t<8;t++)
  {
    IIC_SDA((txd&0x80)>>7);//IIC_SDA=(txd&0x80)>>7;
    txd<<=1;
		delay_us(1);
		IIC_SCL(1);//IIC_SCL=1;
		delay_us(1);
		IIC_SCL(0);//IIC_SCL=0;
		delay_us(1);
  }
}

static u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();
	for(i=0;i<8;i++ )
	{
		IIC_SCL(0);//IIC_SCL=0;
		delay_us(2);
		IIC_SCL(1);//IIC_SCL=1;
		receive<<=1;
		if(GPIO_PinRead(GPIO3, 23U))receive++;//if(READ_SDA)receive++;
		delay_us(1);
	}
	if (!ack)
		IIC_NAck();
	else
		IIC_Ack();
	return receive;
}

//==================================
// 1 : 24C64
// 0 : 24C16
//==================================
// �⺻������ HW�� �Ѱ��� EEPROM�� ���� �ȴ�.
// 24C64�� ��� 0xA0 ����̽� ������ ȣ�⿡ ACK �����Ѵ�. 0xA1�� NOACK.
// 24C16�� ��� 0xA0 ������ �Բ� ���� �޸� ��巹���� ȣ�� �ϹǷ� 0xA1�� ACK.
void AT24Cxx_FindDevice(void)
{
	u8 temp=0;
	IIC_Start();
	IIC_Send_Byte(0xA0+(((1)&7)<<1));

	temp = IIC_Wait_Ack();
	if(0==temp) IIC_Stop();
	delay_us(1);

	if(temp)
	{
		// 24C64 �� Ȯ��! device ����� �����ϴ���...
		// IIC_Start();
		// IIC_Send_Byte(0xA0+(((1)&7)<<1));
		// temp = IIC_Wait_Ack();
		// if(0==temp) IIC_Stop();
		// delay_us(1);
		// if(0==temp)
		// {
			EE_TYPE		= AT24C64;
			EEP_PAGE_SIZE	 = 32;
		//} else ;//ERROR!!!!
	}
	else
	{
	EE_TYPE		= AT24C16;
	EEP_PAGE_SIZE	 = 16;
	}
	return;
}

u8 AT24Cxx_ReadOneByte(u16 ReadAddr)
{
	u8 temp=0;
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0xA0);
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);
	} else IIC_Send_Byte(0xA0+(((ReadAddr/256)&7)<<1));
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr%256);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(0xA1);
	IIC_Wait_Ack();
	temp=IIC_Read_Byte(0);
	IIC_Stop();
	delay_us(50);
	return temp;
}

void AT24Cxx_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{
#if defined(MCRW_DIP)
IIC_WP(0);//IIC_WP=0; // write enable
#endif
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0xA0);
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);
	}else IIC_Send_Byte(0xA0+((WriteAddr/256)<<1));
	IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr%256);
	IIC_Wait_Ack();
	IIC_Send_Byte(DataToWrite);
	IIC_Wait_Ack();
    IIC_Stop();
#if defined(MCRW_DIP)
IIC_WP(1);//IIC_WP=1; // write disable
#endif
	delay_ms(5); // EEPROM hold(SCL, SDA set low) max 5ms. --> need check SCL, SDA line are high.
}

u8 AT24Cxx_WritePage(u16 WriteAddr, u8 SizeToWrite, u8 *pData)
{
	u8 i;
	u8 Ret = 0;
	// page access violation check
	if(SizeToWrite > EEP_PAGE_SIZE) return -1;
	if(EEP_PAGE_SIZE < (WriteAddr % EEP_PAGE_SIZE)+SizeToWrite) return -1;

#if defined(MCRW_DIP)
IIC_WP(0);//IIC_WP=0; // write enable
#endif
	IIC_Start();
	if(EE_TYPE > AT24C16)
	{
		IIC_Send_Byte(0xA0);
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);
	}else IIC_Send_Byte(0xA0+((WriteAddr/256)<<1));
	IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr%256);
	IIC_Wait_Ack();

	for(i=0; i< SizeToWrite; i++)
	{
		IIC_Send_Byte(pData[i]);
		IIC_Wait_Ack();
	}
    IIC_Stop();
#if defined(MCRW_DIP)
IIC_WP(1);//IIC_WP=1; // write disable
#endif
	delay_ms(5); // EEPROM hold(SCL, SDA set low) max 5ms. --> need check SCL, SDA line are high.
	return Ret;
}


u8 AT24Cxx_ReadPage(u16 ReadAddr, u8 SizeToRead, u8 *pData)
{
	u8 i;
	u8 Ret = 0;

	// page access violation check
	if(SizeToRead > EEP_PAGE_SIZE) return -1;
	if(EEP_PAGE_SIZE < (ReadAddr % EEP_PAGE_SIZE)+SizeToRead) return -1;

	IIC_Start();
	if(EE_TYPE > AT24C16)
	{
		IIC_Send_Byte(0xA0);
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);
	} else IIC_Send_Byte(0xA0+(((ReadAddr/256)&7)<<1));
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr%256);
	IIC_Wait_Ack();

	IIC_Start();
	IIC_Send_Byte(0xA1);
	IIC_Wait_Ack();
	for(i=0;i < SizeToRead; i++)
	{
		pData[i] = IIC_Read_Byte((i<(SizeToRead-1)) ? 1 : 0);
	}
	IIC_Stop();
	delay_us(10);
	return Ret;
}

void AT24Cxx_WriteLongByte(u16 WriteAddr,u32 DataToWrite)
{
	u8 t;
	for(t=0;t<4;t++)
	{
		AT24Cxx_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}
}

u32 AT24Cxx_ReadLongByte(u16 ReadAddr)
{
	u8 t;
	u32 temp=0;
	for(t=0;t<4;t++)
	{
		temp<<=8;
		temp+=AT24Cxx_ReadOneByte(ReadAddr+4-t-1);
	}
	return temp;
}

/*
void AT24Cxx_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24Cxx_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}
}

u32 AT24Cxx_ReadLenByte(u16 ReadAddr,u8 Len)
{
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24Cxx_ReadOneByte(ReadAddr+Len-t-1);
	}
	return temp;
}
*/

void AT24Cxx_WriteLongBitCount(u16 WriteAddr,u8 count)
{
	u8 i, t;
  u32 DataToWrite=0;
  if(count > 32) count = 32;
  for(i=0;i<count;i++)
  {
     DataToWrite |= 1;
     DataToWrite = (DataToWrite << 1);
  }
	for(t=0;t<4;t++)
	{
		AT24Cxx_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}
}

u8 AT24Cxx_ReadLongBitCount(u16 ReadAddr)
{
	u8 i, t;
	u8 temp=0;
  u8 count=0;

	for(t=0;t<4;t++)
	{
		temp = AT24Cxx_ReadOneByte(ReadAddr+4-t-1);
    for(i=0;i<8;i++)
    {
      if(temp&1) count++;
      temp = (temp >> 1);
    }
	}
	return count;
}

// check 24C16 or 24C64
// Write Tag,
u8 AT24Cxx_Check(void)
{
  u8 ret = 0;
  if(EE_TYPE >= AT24C64)
  {
    //printf("[24C64]\r\n");
    ret = 1;
  }
  else
  {
    //printf("[24C16]\r\n");
    ret = 0;
  }
	return ret;
}

//////void AT24Cxx_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
//////{
//////	while(NumToRead)
//////	{
//////		*pBuffer++=AT24Cxx_ReadOneByte(ReadAddr++);
//////		NumToRead--;
//////	}
//////}
//////
//////void AT24Cxx_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
//////{
//////	while(NumToWrite--)
//////	{
//////		AT24Cxx_WriteOneByte(WriteAddr,*pBuffer);
//////		WriteAddr++;
//////		pBuffer++;
//////	}
//////}

u8 AT24Cxx_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
//	while(NumToRead)
//	{
//		*pBuffer++=AT24Cxx_ReadOneByte(ReadAddr++);
//		NumToRead--;
//	}
	u16 i;
	u8 size;
	u8 Ret = 0;
//  while(1)
//  {
//	IIC_SCL=1;
//	delay_us(100); // EEPROM hold(SCL, SDA set low) max 5ms. --> need check SCL, SDA line are high.
//	IIC_SCL=0;
//	delay_us(100); // EEPROM hold(SCL, SDA set low) max 5ms. --> need check SCL, SDA line are high.
//	}


	// EEPROM size limit check!!!
	//if(SizeToWrite > EEP_PAGE_SIZE) return -1;
	//if(EEP_PAGE_SIZE < (WriteAddr % EEP_PAGE_SIZE)+SizeToWrite) return -1;

	if((ReadAddr % EEP_PAGE_SIZE))
	{
		// this means WriteAddr is not start page offset 0
		// write data from WriteAddr to current pasge end address.
		size = EEP_PAGE_SIZE - (ReadAddr % EEP_PAGE_SIZE); // max writable size
		if(NumToRead < size) size = NumToRead;
		Ret = AT24Cxx_ReadPage(ReadAddr, size, pBuffer);
		if(Ret) return -1;
		ReadAddr	+= size;
		pBuffer		+= size;
		NumToRead	-= size;
	}
	for(i = 0; NumToRead;i += size)
	{
		if(NumToRead > EEP_PAGE_SIZE) size = EEP_PAGE_SIZE; else size = NumToRead;
		Ret = AT24Cxx_ReadPage(ReadAddr, size, pBuffer);
		if(Ret) return -1;
		ReadAddr	+= size;
		pBuffer		+= size;
		NumToRead	-= size;
	}
	return Ret;
}

u8 AT24Cxx_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	u16 i;
	u8 size;
	u8 Ret = 0;
	// EEPROM size limit check!!!
	//if(SizeToWrite > EEP_PAGE_SIZE) return -1;
	//if(EEP_PAGE_SIZE < (WriteAddr % EEP_PAGE_SIZE)+SizeToWrite) return -1;
	if((WriteAddr % EEP_PAGE_SIZE))
	{
		// this means WriteAddr is not start page offset 0
		// write data from WriteAddr to current pasge end address.
		size = EEP_PAGE_SIZE - (WriteAddr % EEP_PAGE_SIZE); // max writable size
		if(NumToWrite < size) size = NumToWrite;
		Ret = AT24Cxx_WritePage(WriteAddr, size, pBuffer);
		if(Ret) return -1;
		WriteAddr	+= size;
		pBuffer		+= size;
		NumToWrite	-= size;
	}
	for(i = 0; NumToWrite;i += size)
	{
		if(NumToWrite > EEP_PAGE_SIZE) size = EEP_PAGE_SIZE; else size = NumToWrite;
		Ret = AT24Cxx_WritePage(WriteAddr, size, pBuffer);
		if(Ret) return -1;
		WriteAddr	+= size;
		pBuffer		+= size;
		NumToWrite	-= size;
	}
	return Ret;
}

u8 AT24Cxx_Erase(u16 WriteAddr,u16 NumToWrite)
{
	u16 i;
	u8 size;
	u8 Ret = 0;
	u8 pBuffer[32];
	memset(pBuffer, 0xFF, 32);

	// EEPROM size limit check!!!
	//if(SizeToWrite > EEP_PAGE_SIZE) return -1;
	//if(EEP_PAGE_SIZE < (WriteAddr % EEP_PAGE_SIZE)+SizeToWrite) return -1;
	if((WriteAddr % EEP_PAGE_SIZE))
	{
		// this means WriteAddr is not start page offset 0
		// write data from WriteAddr to current pasge end address.
		size = EEP_PAGE_SIZE - (WriteAddr % EEP_PAGE_SIZE); // max writable size
		if(NumToWrite < size) size = NumToWrite;
		Ret = AT24Cxx_WritePage(WriteAddr, size, pBuffer);
		if(Ret) return -1;
		WriteAddr	+= size;
		//pBuffer		+= size;
		NumToWrite	-= size;
	}
	for(i = 0; NumToWrite;i += size)
	{
		if(NumToWrite > EEP_PAGE_SIZE) size = EEP_PAGE_SIZE; else size = NumToWrite;
		Ret = AT24Cxx_WritePage(WriteAddr, size, pBuffer);
		if(Ret) return -1;
		WriteAddr	+= size;
		//pBuffer		+= size;
		NumToWrite	-= size;
	}
	return Ret;
}

#endif//USE_REAL_EEPROM
