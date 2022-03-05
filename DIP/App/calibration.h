#ifndef __CALIBRATION_H
#define	__CALIBRATION_H

#ifdef __cplusplus
extern "C"
{
#endif 
#include "xpt2046.h"

void calibrateTouchScreen(void);
TouchPositionTypeDef touch_get_position(void);

#ifdef __cplusplus
}
#endif
	
#endif   ///< __CALIBRATION_H
