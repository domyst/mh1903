/*
 * uart.h
 *
 *  Created on: Aug 11, 2021
 *      Author: skpark
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include <stdint.h>

//#include "sys_def.h"
//#include "define.h"
//#include "stm32f1xx_hal.h"


// module_uart.h

#define MAX_BUFFER_SIZE    (1024)

typedef struct{
	uint8_t rx_flag;
	uint16_t head;
	uint16_t tail;
	uint8_t Rx_buffer[MAX_BUFFER_SIZE];
	uint8_t Tx_buffer[MAX_BUFFER_SIZE];
}uart_t;

extern uart_t uart;

void Uart0_Initial(void);

void init_uart(uart_t* u);
void push(uart_t*, uint8_t);
uint8_t pop(uart_t*);
uint8_t isEmpty(uart_t*);
void Uart0_SendDatas(uint8_t* buf,uint16_t len);




#endif /* INC_UART_H_ */
