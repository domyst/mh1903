#ifndef __SYSTICK_H
#define	__SYSTICK_H

#ifdef __cplusplus
extern "C"
{
#endif 

#include "mhscpu.h"
#include "stdint.h"

typedef uint32_t tick;
	

void SYSTICK_Init(void);

tick get_tick(void);
tick get_diff_tick(tick cur_tick, tick prior_tick);
	
void udelay(tick usec);
void mdelay(tick msec);

int is_timeout(tick start_time, tick interval);


#ifdef __cplusplus
}
#endif
	
#endif   ///< __SYSTICK_H

