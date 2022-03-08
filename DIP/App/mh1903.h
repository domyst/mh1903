/*******************************(C) COPYRIGHT 2007 INSEM Inc.****************************************/
/* processor 	  : CORETEX-M3(STM32F10X)         		    				    */
/* compiler       : EWARM Compiler								    */
/* program by	  : JK.Won									    */
/* History:											    */
/* 04/13/2007     : Version 1.0									    */
/* copy right	  : Insem Inc.									    */
/****************************************************************************************************/

#ifndef _MH1903_INIT_H_
#define _MH1903_INIT_H_


void Internal_timer_Proc(void);
void timer_setup(uchar tmno, ushort timer);
void CORTEX_initial(void);
/* must be selection (101 or 102 or 103) */
//#define STM32F100_ENABLE
//#define STM32F101_ENABLE
//#define STM32F102_ENABLE
#define STM32F103_ENABLE

/* GPIOC offset */
#define GPIOC_OFFSET                 (GPIOC_BASE - PERIPH_BASE)

/* GPIOC ODR offset */
#define GPIOC_ODR_OFFSET                 (GPIOC_OFFSET + 0x0c)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LAB_GPIO_ODR           // used for odr register-modify point
#define USART2_REMAP                *(u32 *)(0x40010004)

#define IFM_COM_SERIAL  (*(volatile unsigned long *)(0x4221019C)) =  1
#define IFM_COM_USB  (*(volatile unsigned long *)(0x4221019C)) =  0
#define IFM_COM_STAT  (*(volatile unsigned long *)(0x4221011C))

#define IFM_SERIAL_ON  (*(volatile unsigned long *)(0x422101A0)) =  0
#define IFM_USB_ON  (*(volatile unsigned long *)(0x422101A0)) =  1
#define IFM_SERIAL_STAT  (*(volatile unsigned long *)(0x42210120))

#define TIM3_ARR3 350
//#define TIM3_ARR3 700

#define SOL_CON_ON  (*(volatile unsigned long *)(0x4000043c)) =  32;//duty 10%
#define SOL_CON_OFF  (*(volatile unsigned long *)(0x4000043c)) =  0;
#define SOL_CON  (*(volatile unsigned long *)(0x4000043c))		//TIM3_CCR3

#define IFM_ICC_ABSENT		do { if(g_pcb_version == PCB_CR30_R3) pPB9_1; else pPB7_1; } while(0) 
#define IFM_ICC_PRESENT		do { if(g_pcb_version == PCB_CR30_R3) pPB9_0; else pPB7_0; } while(0)
//#define IFM_STAT  (*(volatile unsigned long *)(0x4221811C))

/// always [2010/11/17] test
#define SENSOR_ON		pPB8_1
#define SENSOR_OFF		pPB8_0
//#define SENSOR_STAT  (*(volatile unsigned long *)(0x42218120))

/// always [2010/11/17] test
#define IFM_ON			GPIO_SetBits(GPIOA, GPIO_Pin_5)         // active high         //pPC9_1          
#define IFM_OFF			GPIO_ResetBits(GPIOA, GPIO_Pin_5)                              //pPC9_0
#define IFM_STATE (*(volatile unsigned long *)(0x422201A4))

#define CR30_RESET_ON	pPB1_0
#define CR30_RESET_OFF	pPB1_1


//#define COMM_IFM  pPB5_1
//#define COMM_RF  pPB5_0

#define TR6201_RESET_ON		pPD10_0
#define TR6201_RESET_OFF	pPD10_1

#define GPIO_LOCK_SENSOR_STAT  	pPE0_IN
#define GPIO_FRONT_SENSOR_STAT	pPE1_IN
#define GPIO_REAR_SENSOR_STAT	pPE2_IN
#define GPIO_BOARD_CHECK		pPE3_IN

#define RED_ON  pPE7_0
#define RED_OFF  pPE7_1
#define RED_TOG  pPE7_T
//#define RED_STAT  (*(volatile unsigned long *)(0x4223019C))

#define GREEN_ON  pPE5_0
#define GREEN_OFF  pPE5_1
#define GREEN_TOG  pPE5_T
//#define GREEN_STAT  (*(volatile unsigned long *)(0x42230194))

#define AllLedOff	RED_OFF;GREEN_OFF

#define TIM1_CH1_CCR  (volatile unsigned long)(0x40012C34)
#define TIM1_CH2_CCR  (volatile unsigned long)(0x40012C38)
#define TIM1_CH3_CCR  (volatile unsigned long)(0x40012C3C)

#define PWM_4_EN   (*(volatile unsigned long *)(0x40000414)) |= 0x1

#define ADC_1_DR  (volatile unsigned long)(0x4001244C)
#define ADC_3_DR  (volatile unsigned long)(0x40013C4C)

#define uart1_DR1  (volatile unsigned long)(0x40013804)
#define uart3_DR1  (volatile unsigned long)(0x40004804)

#define uart5_cr1  (*(volatile unsigned long *)(0x40005000))

//#define EXTI_1_EN 	(*(volatile unsigned int *)0x40010400) |= 0x1<<1
//#define EXTI_1_DIS	(*(volatile unsigned int *)0x40010400) &= ~(0x1<<1)

#define AFIO_EN  (*(volatile unsigned long *)(0x40021018)) |= 1

//#define EXTI_1_EN  (*(volatile unsigned long *)(0x40010008)) |= (0x4<<4)
#define EXTI_EN  (*(volatile unsigned long *)(0x40010400)) |= (0x01 << 10)||(0x01 << 15)
#define EXTI_DIS  (*(volatile unsigned long *)(0x40010400)) &= ~(0x01 << 10)||(0x01 << 15)

#define uart4_5_ON  (*(volatile unsigned long *)(0x4002101c)) |= 0x00180000

#define  ResetUsb	(*(volatile unsigned long *)(0x40005c40)) |= (0x01)

#if 1		//pbbch 180503 sen5 추가로 ADC Channel 추가.  
#define Number_of_ADC	4
#else
#define Number_of_ADC	3
#endif
#define Number_of_Buf	10

#define BAUDRATE_9600		9600
#define BAUDRATE_19200		19200
#define BAUDRATE_115200		115200

#if 1		//pbbch 180503 sen5 추가로 board에 따른 sensor 처리를 변경 해야 함. 따라서 구분자 추가.  
#define SEN1234_DEFAULT_ARRAY		0	//sen1:Lock, sen2: END, Sen3: Front or anti, Sen4: Inner
#define SEN12345_ARRAY				1	//sen1:Lock, sen2: END, Sen3: anti, Sen4: Inner, Sen5: Front

extern unsigned char gsen_array_type;
#endif


/*****************************************************************************************************/
#ifdef __STM32F10X_H__

//ushort timer_buf[8];
//__bits flag0;
//__tm tm;	
//

#else

//extern ushort timer_buf[8];
//
//extern __bits flag0;
//extern __tm tm;

/// always [2011/3/21] USB
extern uint32_t bDeviceState; /* USB device status */

#endif

/******************************** bit flag define ******************************************************/
#define   tm1ms_f	flag0.bit.b0

#if 1		//pbbch 171212 warning across
extern uint SetBaudrate(uchar StatBaud);
extern ushort SetHardwareFlow(uchar HardwareFlowStat);
extern void UART1_initial(uint Baudrate,ushort HardFlow);
extern void UART3_initial(uint Baudrate,ushort HardFlow);
extern void UART4_initial(u32 Type);
extern void FlashSettingInit(uchar FlashInit);

extern void OptionByte_initial_Response(); //pbbch 180208 neicp 수정

#if 1		//pbbch 180102 usb connect status function add
extern unsigned char Usb_connect_status_check(void);
#endif

//pbbch 180129 we need to know usb connect or not connect. so function add
extern void init_usb_configure_exe(void); 

#if 1	//pbbch 181015 add extern function for software uart init.
extern void OptionByte_initial(void);
#endif

#endif

#endif  /* _MH1903_INIT_H_ */

/******************* (C) COPYRIGHT 2007 INSEM Inc ***************************************END OF FILE****/

