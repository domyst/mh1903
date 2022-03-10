#include "mhscpu.h"
#include "systick.h"

tick g_current_tick = 0;

void SYSTICK_Init(void)
{
	SYSCTRL_ClocksTypeDef clocks;	
	
	SYSCTRL_GetClocksFreq(&clocks);
	SysTick_Config(clocks.CPU_Frequency / 1000000);   ///< 1us
}

// void SysTick_Handler(void)
// {
// 	g_current_tick++;
// }

//  domyst
static volatile u32 systick_ctr = 0;
#if 1	//pbbch 181015 add system tick verialble for soft ware uart init.
u32 gsys_tick_cnt=0;
#endif
void SysTick_Handler(void)
{
	uchar ErrorState;
#if 0		//pbbch 171212 warning across	
	uchar WaitTime = 200;
#endif

	g_current_tick++;	// mh1903 source

#if 1	//pbbch 181016 add system tick verialble for soft ware uart init.
	if(systick_ctr>=0xffffffff) systick_ctr=0;
	else		systick_ctr++;

	gsys_tick_cnt=systick_ctr;
#else
	systick_ctr++;
#endif

	if(0 == (systick_ctr % 1))
	{
		if(InDeal == SET)//카드가 리더기안에 잇눈 경우 
		{
			//Green_ON(); // 20140529 hyesun : 삭제, LED 제어 커멘드 추가 루틴
			//if(LED_Control == LED_Default)Green_ON();//LED_Default, LED_Off, LED_On, LED_Toggle
			if(LED_Control == LED_Off)GREEN_OFF;
			else if(LED_Control == LED_On)Green_ON();
			else if(LED_Control == LED_Toggle){
				if(0==(systick_ctr % LEDTime_Control))Green_T();
			}
			else Green_ON();
		}
		else
		{
			ErrorState = ErrorFlag;
			/// always [2011/5/12] 카드의 상태에 따라 LED 색을 조절한다.
			if(ErrorState)
			{
				if(0==(systick_ctr % Toggle_1sec))Red_T();
			}
			else
			{
				//Green_T(); // 20140529 hyesun : 삭제, LED 제어 커멘드 추가 루틴
				if(LED_Control == LED_Off)GREEN_OFF;
				else if(LED_Control == LED_On)Green_ON();
				else if(LED_Control == LED_Toggle){
					if(0==(systick_ctr % LEDTime_Control))Green_T();
				}
				else
				{
					if(0==(systick_ctr % Toggle_1sec))Green_T();
				}
			}
		}
		
		if(0 == (systick_ctr % Toggle_1sec))
		{
			if(StatSol == SET)
			{
				SolCnt--;
			}
		}
	}
}
// end of domyst

tick get_tick(void)
{
	return g_current_tick;
}

void udelay(tick usec)
{
	tick old_tick;

	old_tick = get_tick();
	while (get_diff_tick(get_tick(), old_tick) < usec)
	{
	}
}

void mdelay(tick msec)
{
	tick old_tick;

	old_tick = get_tick();
	while (get_diff_tick(get_tick(), old_tick) < (msec * 1000))
	{
	}
}

tick get_diff_tick(tick cur_tick, tick prior_tick)
{
	if (cur_tick < prior_tick)
	{
		return (cur_tick + (~prior_tick));
	}
	else
	{
		return (cur_tick - prior_tick);
	}
}
