/*
 * uart.c
 *
 *  Created on: Aug 11, 2021
 *      Author: skpark
 */

#include "mhscpu.h"
#include "uart.h"


uint8_t tmpRece = 0;

void Uart0_Initial(void)
{
 	uint8_t str[]="Hello World!!\n";

	Uart0_SendDatas(str,15);	

//	  HAL_UART_Transmit(&huart1, str, sizeof(str), 100);
}

void Uart0_SendDatas(uint8_t* buf,uint16_t len)
{
	uint16_t i=0;
	
	for(i=0;i<len;i++){
		while(!UART_IsTXEmpty(UART0));
		UART_SendData(UART0, (uint8_t) buf[i]);		
	}
}


// module_uart.c

void init_uart(uart_t* u)
{
  u->head = 0;
  u->tail = 0;
  memset(u->Rx_buffer, 0, sizeof(u->Rx_buffer));
  memset(u->Tx_buffer, 0, sizeof(u->Tx_buffer));
}

void push(uart_t* u, uint8_t data)
{
  u->Rx_buffer[u->head] = data;

  u->head++;

  if (u->head >= MAX_BUFFER_SIZE) {
    u->head = 0;
  }
}

uint8_t pop(uart_t* u)
{
  uint8_t data = u->Rx_buffer[u->tail];

  u->tail++;

  if (u->tail >= MAX_BUFFER_SIZE) {
    u->tail = 0;
  }

  return data;
}

uint8_t isEmpty(uart_t* u)
{
  return u->head == u->tail;
}
