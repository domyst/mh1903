#include <string.h>
#include <stdio.h>

#include "mhscpu.h"
#include "mhscpu_it.h"
#include "mhscpu_dcmi.h"
#include "DecodeLib.h"
#include "ili9488.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#define DATA_BUF_SIZE				256

extern uint16_t send_buf[DATA_BUF_SIZE];
extern uint16_t rece_buf[DATA_BUF_SIZE];

extern uint32_t send_buf_index;
extern uint32_t rece_buf_index;

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  printf("HardFault_Handler\n");

  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//	printf("SysTick_Handler\r\n");
//}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */



//void UART0_IRQHandler(void)
//{
//}
/**
 * DCMI 中断处理函数
 *
 */
void DCMI_IRQHandler(void)
{  
	if (DCMI_GetITStatus(DCMI_IT_LINE) != RESET) 
	{
		DCMI_ClearITPendingBit(DCMI_IT_LINE); 			  
	}
	
	if (DCMI_GetITStatus(DCMI_IT_VSYNC) != RESET) 
	{
		DCMI_ClearITPendingBit(DCMI_IT_VSYNC);	  
	}

	if (DCMI_GetITStatus(DCMI_IT_FRAME) != RESET) 
	{
        //调用回调函数
        DCMI_CallBackFrame();
		DCMI_ClearITPendingBit(DCMI_IT_FRAME);
	} 
	
	if (DCMI_GetITStatus(DCMI_IT_OVF) != RESET)
	{
		DCMI_ClearITPendingBit(DCMI_IT_OVF); 
	}
    
	if (DCMI_GetITStatus(DCMI_IT_ERR) != RESET) 
	{
		DCMI_ClearITPendingBit(DCMI_IT_ERR);
	}
}

void EXTI0_IRQHandler(void)
{
	extern uint8_t	button_count;
	uint8_t	i;
	uint8_t	chattering_count;
	uint8_t	dummy;
//	printf("EXTI0_IRQHandler In\r\n");
//	printf("EXTI0_GPIO_Status %08X\r\n", EXTI_GetITStatus());
//	printf("EXTI0_GPIO_Status %08X\r\n", EXTI_GetITLineStatus(EXTI_Line0));
	EXTI_ClearITPendingBit(EXTI_Line0);
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
	
	dummy = 0;
	i = 0;
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0x00)
	{
		dummy = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
		for(chattering_count=0;chattering_count<100;chattering_count++)
		{
			if(dummy == GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)) i++;
		}
		
		if(chattering_count == i)
		{
			//By Psk: Led Off
			GPIO_ResetBits(GPIOA, GPIO_Pin_3);			
			button_count++;
//			if(button_count >= 0x10) button_count = 0;
		}

	}

	
}
void MSR_IRQHandler(void)
{
	
}

void SPI4_IRQHandler(void)
{
	
}
