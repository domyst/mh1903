#ifndef __FND_H
#define __FND_H

#ifdef __cplusplus
extern "C"
{
#endif 

#define	FND_BIT_0	0x01
#define	FND_BIT_1	0x02
#define	FND_BIT_2	0x04
#define	FND_BIT_3	0x08
#define	FND_BIT_4	0x10
#define	FND_BIT_5	0x20
#define	FND_BIT_6	0x40
#define	FND_BIT_7	0x80

#define FND_0	0x3F
#define FND_1	0x06
#define FND_2	0x5B
#define FND_3	0x4F
#define FND_4	0x66
#define FND_5	0x6D
#define FND_6	0x7D
#define FND_7	0x27
#define FND_8	0x7F
#define FND_9	0x67
#define FND_A	0xF7
#define FND_B	0xFC
#define FND_C	0xB9
#define FND_D	0xDE
#define FND_E	0xF9
#define FND_F	0xF1

void FND_IOConfig(void);
void FND_Configuration(void);
void FND_HEX_Write(uint8_t data);
void FND_BIT_Write(uint8_t data);
void FND_Blink(uint8_t data,uint32_t delay);

#ifdef __cplusplus
}
#endif

#endif  ///< __FND_H
