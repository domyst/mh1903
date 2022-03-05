#ifndef __XPT2046_H
#define	__XPT2046_H

#ifdef __cplusplus
extern "C"
{
#endif 
#include <stdint.h>

typedef struct{
	int16_t x;
	int16_t y;
}TouchPositionTypeDef;

void TOUCH_Configuration(void);

uint16_t  touch_get_x(void);
uint16_t  touch_get_y(void);
uint16_t  touch_temp(void);

uint8_t  touch_irq(void);
uint8_t  touch_busy(void);

TouchPositionTypeDef touch_get_raw_position(void);

#ifdef __cplusplus
}
#endif
	
#endif   ///< __XPT2046_H
