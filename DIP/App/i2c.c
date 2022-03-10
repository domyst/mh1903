/*******************************(C) COPYRIGHT 2007 INSEM Inc.****************************************/
/* processor 	  : CORETEX-M3(STM32F10X)         		    				    */
/* compiler       : EWARM Compiler								    */
/* program by	  : JK.Won									    */
/* History:											    */
/* 04/13/2007     : Version 1.0									    */
/* copy right	  : Insem Inc.									    */
/****************************************************************************************************/
#if defined(USE_REAL_EEPROM)

#include "stm32f10x.h"
//#include "stm32f10x_lib.h"
//#include "main.h"
#include "i2c.h"

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
static u16 EE_TYPE = AT24C64;
static u8 EEP_PAGE_SIZE = 32;

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)


#if defined(MCRW_SLIM)

// SLIM
#define SDA_IN()  {GPIOB->CRH&=0XFFFFF0FF;GPIOB->CRH|=8<<8;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFFF0FF;GPIOB->CRH|=3<<8;}

#define IIC_SCL    PBout(11) //SCL
#define IIC_SDA    PBout(10) //SDA	
#define READ_SDA   (GPIOB->IDR & GPIO_Pin_10) //SDA

#elif defined(MCRW_STANDALONE)

// STANDALONE
#define SDA_IN()  {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=8<<24;}
#define SDA_OUT() {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=3<<24;}

#define IIC_SCL    PBout(15) //SCL
#define IIC_SDA    PBout(14) //SDA	
#define READ_SDA   (GPIOB->IDR & GPIO_Pin_14) //SDA

#elif defined(MCRW_DIP)

#if 1	// 20190219 - kjhfw : 함수로 변경 및 중간값 버퍼이용 (2번 관련)
void SDA_IN(void)
{
	unsigned int temp=0;
	
	temp = GPIOB->CRL;
	
	temp &= 0x0FFFFFFF;
	temp |= 8<<28;
	
	GPIOB->CRL = temp;
}
void SDA_OUT(void)
{
	unsigned int temp=0;
	
	temp = GPIOB->CRL;
	
	temp &= 0x0FFFFFFF;
	temp |= 3<<28;
	
	GPIOB->CRL = temp;
}
#else
// DIP
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=8<<28;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=3<<28;}
#endif

#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	
#define READ_SDA   (GPIOB->IDR & GPIO_Pin_7) //SDA

#define IIC_WP    PBout(5)
#else
#pragma error "DEVICE TYPE not selected!"
#endif


void IIC_initial(void)			// eeprom 기능 추가 2014.01.14
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
#if 1	//20190219 - kjhfw : I2C Init 변경 (2번 관련)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
#else
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
#endif
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	IIC_WP=1; // write disable
#endif
}	

static void IIC_Start(void)
{
	SDA_OUT();
	IIC_SDA=1;	  	
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low
	delay_us(4);
	IIC_SCL=0;
}

static void IIC_Stop(void)
{
	// 20190219 - kjhfw : STOP Timing 개선 (5번 관련)
#if 1
	SDA_OUT();
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1;
	delay_us(4);
	IIC_SDA=1;
	delay_us(4);
#else
	SDA_OUT();
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1;
	IIC_SDA=1;
	delay_us(4);
#endif
}

static u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();
	IIC_SDA=1;delay_us(1);	
	IIC_SCL=1;delay_us(1);	
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;	
	return 0;
}

static void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

static void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}		

static void IIC_Send_Byte(u8 txd)
{
  u8 t;
	SDA_OUT(); 	
  IIC_SCL=0;
  for(t=0;t<8;t++)
  {
	IIC_SDA=(txd&0x80)>>7;
	txd<<=1; 	
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;	
	delay_us(2);
  }	
} 	

static u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();
	for(i=0;i<8;i++ )
	{
		IIC_SCL=0;
		delay_us(2);
		IIC_SCL=1;
		receive<<=1;
		if(READ_SDA)receive++;
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
// 기본적으로 HW에 한개의 EEPROM만 장착 된다.
// 24C64의 경우 0xA0 디바이스 번지만 호출에 ACK 응답한다. 0xA1은 NOACK.
// 24C16의 경우 0xA0 번지와 함께 내부 메모리 어드레스를 호출 하므로 0xA1은 ACK.
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
		// 24C64 재 확인! device 제대로 응답하는지...
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
	delay_us(100);	
	return temp;
}

void AT24Cxx_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																
#if defined(MCRW_DIP)
IIC_WP=0; // write enable
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
IIC_WP=1; // write disable
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
IIC_WP=0; // write enable
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
IIC_WP=1; // write disable
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
  if(EE_TYPE == AT24C64)
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
