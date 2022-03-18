#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "uart.h"

extern uart_t uart;

void pwm_start(void);
void pwm_stop(void);

typedef struct
{
	uint32_t PWM_HZ;
	uint32_t DutyCycleHighPluse;
	uint32_t DutyCycleLowPluse;
	SYSCTRL_ClocksTypeDef clocks;
	TIM_PWMInitTypeDef TIM_PWMSetStruct;
}timer_pwm_set;

typedef struct
{
    uint8_t PinName[5];
    GPIO_TypeDef *GPIOx;
    uint16_t Pin_Num;
    uint16_t Ramp_Select;
}TypeRampPinDef;


typedef struct
{
    uint8_t PwmName[5];
    TypeRampPinDef RampPin[5];
}Type_RampDef;

void UART_Configuration(void);
void TIMER_Configuration3(void);
void PrintfTimerRemapList(void);
void TimerPWMSetStructInit3(void);
void TIMER_PWMSet3(void);
void PrintSet3(void);
void PrintList2(void);
void UART0_Set(void);
void PWM_Pin_Set(void);
void PrintfCurrentTimerRemap(void);

uint16_t PWM_CurrentPin = 1;

Type_RampDef Ramp_List[] =
{
    {"PWM0", "PA0", NULL, NULL, NULL,                "PB0", GPIOB, GPIO_Pin_0, GPIO_Remap_2, "---",  NULL,  NULL,        NULL,          "PD4",  GPIOD, GPIO_Pin_4, GPIO_Remap_2, "PF13",  GPIOF, GPIO_Pin_13, GPIO_Remap_0},
    {"PWM1", "PA1", NULL, NULL, NULL,                "PB1", GPIOB, GPIO_Pin_1, GPIO_Remap_2, "---",  NULL,  NULL,        NULL,          "PD5",  GPIOD, GPIO_Pin_5, GPIO_Remap_2, "PF14",  GPIOF, GPIO_Pin_14, GPIO_Remap_0},
    {"PWM2", "PA2", GPIOA, GPIO_Pin_2, GPIO_Remap_2, "PB2", GPIOB, GPIO_Pin_2, GPIO_Remap_2, "---",  NULL,  NULL,        NULL,          "PD6",  GPIOD, GPIO_Pin_6, GPIO_Remap_2, "PF15",  GPIOF, GPIO_Pin_15, GPIO_Remap_0},
    {"PWM3", "PA3", GPIOA, GPIO_Pin_3, GPIO_Remap_2, "PB3", GPIOB, GPIO_Pin_3, GPIO_Remap_2, "PB12", GPIOB, GPIO_Pin_12, GPIO_Remap_2,  "PD7",  GPIOD, GPIO_Pin_7, GPIO_Remap_2, "PF0",   GPIOF, GPIO_Pin_0,  GPIO_Remap_0},
    {"PWM4", "PA4", GPIOA, GPIO_Pin_4, GPIO_Remap_2, "PB4", GPIOB, GPIO_Pin_4, GPIO_Remap_2, "PB13", GPIOB, GPIO_Pin_13, GPIO_Remap_2,  "PF0",  GPIOF, GPIO_Pin_0, GPIO_Remap_2, "PG1",   GPIOG, GPIO_Pin_1,  GPIO_Remap_0},
    {"PWM5", "PA5", GPIOA, GPIO_Pin_5, GPIO_Remap_2, "PB5", GPIOB, GPIO_Pin_5, GPIO_Remap_2, "PB14", GPIOB, GPIO_Pin_14, GPIO_Remap_2,  "PF1",  GPIOF, GPIO_Pin_1, GPIO_Remap_2, "PG2",   GPIOG, GPIO_Pin_2,  GPIO_Remap_0},
    {"PWM6", "PA6", GPIOA, GPIO_Pin_6, GPIO_Remap_2, "PB6", GPIOB, GPIO_Pin_6, GPIO_Remap_2, "PB15", GPIOB, GPIO_Pin_15, GPIO_Remap_2,  "PF2",  GPIOF, GPIO_Pin_2, GPIO_Remap_2, "PG3",   GPIOG, GPIO_Pin_3,  GPIO_Remap_0},
    {"PWM7", "PA7", GPIOA, GPIO_Pin_7, GPIO_Remap_2, "___", NULL , NULL      , GPIO_Remap_2, "---",  NULL,  NULL,        NULL,          "PF3",  GPIOF, GPIO_Pin_3, GPIO_Remap_2, "PG4",   GPIOG, GPIO_Pin_4,  GPIO_Remap_0},
};
timer_pwm_set timerPWMSet;


// int main(int argc , char *argv[])
void pwm_test1(void)
{
    uint32_t controlReg;
    printf("-----------------\n");
	// SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	// SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
	

    
    // UART_Configuration();
	TimerPWMSetStructInit3();
	TIMER_Configuration3();
	
	printf("MegaHunt SCPU Timer PWM Demo V1.0.\r\n");
	PrintSet3();
	PrintList2();

    controlReg = TIMM0->TIM[TIM_3].ControlReg;

    printf(">>> controlReg : [%02X]\n", controlReg);

	while(1)
	{
		UART0_Set();
	}
}

void pwm_test(void)
{   
    uint8_t cmd;
    printf("pwm test..\n");

    while (1)
    {
        if(!isEmpty(&uart))
        {
            // cmd = UART_ReceiveData(UART0);
            cmd = pop(&uart);
            Uart0_SendDatas(&cmd,1);	
            switch(cmd)
            {
            case '0':
                pwm_stop();
                break;
            case '1':
                pwm_start();
                break;
            case 'x':
                return;
            }
        }

    }
}

void PrintList2()
{
    printf("\r\n\r\n");
	printf("============Option List===========\n");
    printf("\"s\" Set PWM Pin\n");
	printf("\">\" Inc PWM Width 1KHZ(Max 10MZ)\n");
	printf("\"<\" Dec PWM Width 1KHZ(Min 1KZ)\n");
	printf("\"+\" Inc PWM HighPeriod DutyCycle\n");
	printf("\"-\" Dec PWM HighPeriod DutyCycle\n");
	printf("\"0\" Timer0 Enable or Disable\n");
	printf("\"1\" Timer1 Enable or Disable\n");
    printf("\"2\" Timer2 Enable or Disable\n");
    printf("\"3\" Timer3 Enable or Disable\n");
    printf("\"4\" Timer4 Enable or Disable\n");
    printf("\"5\" Timer5 Enable or Disable\n");
    printf("\"6\" Timer6 Enable or Disable\n");
    printf("\"7\" Timer7 Enable or Disable\n");
    printf("\"o\" Timer single pulse Enable\n");
    printf("\"c\" Timer single pulse Disable\n");
    printf("ESC return to the previous screen\n");
	printf("============ List  End ===========\n");
}

void PrintSet3(void)
{
	printf("PWM Width %dKHZ\n", timerPWMSet.PWM_HZ / 1000);
	printf("DutyCycle HighPluse : LowPluse = %d\n", timerPWMSet.DutyCycleHighPluse);
}

void UART0_Set(void)
{
	uint8_t cmd = 0;
    
	// if (UART_GetLineStatus(UART0) & UART_LINE_STATUS_RX_RECVD)
    if(!isEmpty(&uart))
	{
		// cmd = UART_ReceiveData(UART0);
        cmd = pop(&uart);
        Uart0_SendDatas(&cmd,1);	
		switch(cmd)
		{
        case 's':
			{
                PrintfTimerRemapList();
				PWM_Pin_Set();
			}
			break;
		//'>'
		case '.': 
			{
				if (timerPWMSet.PWM_HZ < 24000000)
				{
					timerPWMSet.PWM_HZ += 1000;
					TIMER_PWMSet3();
                    printf("PWM_HZ = %d\r\n", timerPWMSet.PWM_HZ);
				}
			}
			break;
			
		//'<'
		case ',':
			{
				if (timerPWMSet.PWM_HZ > 1000)
				{
					timerPWMSet.PWM_HZ -= 1000;
					TIMER_PWMSet3();
                    printf("PWM_HZ = %d\r\n", timerPWMSet.PWM_HZ);
				}
			}
			break;
		//'+'
		case '=':
			{
				timerPWMSet.DutyCycleHighPluse ++;
				TIMER_PWMSet3();
                printf("DutyCycle = %d\r\n", timerPWMSet.DutyCycleHighPluse);
			}
			break;
		//'-'
		case '-':
			{
				if (timerPWMSet.DutyCycleHighPluse > 1)
				{
					timerPWMSet.DutyCycleHighPluse--;
					TIMER_PWMSet3();
                    printf("DutyCycle = %d\r\n", timerPWMSet.DutyCycleHighPluse);
				}
			}
			break;
		case '0':
			{
				if (TIMM0->TIM[TIM_0].ControlReg & 0x01)
				{
					TIM_Cmd(TIMM0, TIM_0, DISABLE);
					printf("TIMER0_0 PWM DISABLE\n");
				}
				else
				{
					TIM_Cmd(TIMM0, TIM_0, ENABLE);
					printf("TIMER0_0 PWM ENABLE\n");
				}
			}
			break;
		case '1':
			{
				if (TIMM0->TIM[TIM_1].ControlReg & 0x01)
				{
					TIM_Cmd(TIMM0, TIM_1, DISABLE);
					printf("TIMER0_1 PWM DISABLE\n");
				}
				else
				{
					TIM_Cmd(TIMM0, TIM_1, ENABLE);
					printf("TIMER0_1 PWM ENABLE\n");
				}
			}
			break;
        case '2':
			{
				if (TIMM0->TIM[TIM_2].ControlReg & 0x01)
				{
					TIM_Cmd(TIMM0, TIM_2, DISABLE);
					printf("TIMER0_2 PWM DISABLE\n");
				}
				else
				{
					TIM_Cmd(TIMM0, TIM_2, ENABLE);
					printf("TIMER0_2 PWM ENABLE\n");
				}
			}
			break;
        case '3':
			{
				if (TIMM0->TIM[TIM_3].ControlReg & 0x01)
				{
					TIM_Cmd(TIMM0, TIM_3, DISABLE);
					printf("TIMER0_3 PWM DISABLE\n");
				}
				else
				{
					TIM_Cmd(TIMM0, TIM_3, ENABLE);
					printf("TIMER0_3 PWM ENABLE\n");
				}
			}
			break;
        case '4':
			{
				if (TIMM0->TIM[TIM_4].ControlReg & 0x01)
				{
					TIM_Cmd(TIMM0, TIM_4, DISABLE);
					printf("TIMER0_4 PWM DISABLE\n");
				}
				else
				{
					TIM_Cmd(TIMM0, TIM_4, ENABLE);
					printf("TIMER0_4 PWM ENABLE\n");
				}
			}
			break;
		case '5':
			{
				if (TIMM0->TIM[TIM_5].ControlReg & 0x01)
				{
					TIM_Cmd(TIMM0, TIM_5, DISABLE);
					printf("TIMER0_5 PWM DISABLE\n");
				}
				else
				{
					TIM_Cmd(TIMM0, TIM_5, ENABLE);
					printf("TIMER0_5 PWM ENABLE\n");
				}
			}
			break;
        case '6':
			{
				if (TIMM0->TIM[TIM_6].ControlReg & 0x01)
				{
					TIM_Cmd(TIMM0, TIM_6, DISABLE);
					printf("TIMER0_6 PWM DISABLE\n");
				}
				else
				{
					TIM_Cmd(TIMM0, TIM_6, ENABLE);
					printf("TIMER0_6 PWM ENABLE\n");
				}
			}
			break;
        case '7':
			{
				if (TIMM0->TIM[TIM_7].ControlReg & 0x01)
				{
					TIM_Cmd(TIMM0, TIM_7, DISABLE);
					printf("TIMER0_7 PWM DISABLE\n");
				}
				else
				{
					TIM_Cmd(TIMM0, TIM_7, ENABLE);
					printf("TIMER0_7 PWM ENABLE\n");
				}
			}
			break;
        case 'o':
			{
                TIM_PWMSinglePulseConfig(TIMM0, TIM_0, ENABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_1, ENABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_2, ENABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_3, ENABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_4, ENABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_5, ENABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_6, ENABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_7, ENABLE);
				printf("timer enable one pulse.\n");
			}
			break;
        case 'c':
            {
                TIM_PWMSinglePulseConfig(TIMM0, TIM_0, DISABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_1, DISABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_2, DISABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_3, DISABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_4, DISABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_5, DISABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_6, DISABLE);
                TIM_PWMSinglePulseConfig(TIMM0, TIM_7, DISABLE);
                printf("timer disable one pulse.\n");
            }
            break;
        
        //ESC
		case 0x1B:   
			{
				PrintSet3();
                PrintList2();
			}
			break;
            
		case 'p':
			{
				PrintSet3();
			}
			break;
		}
	}
}

void PrintfTimerRemapList(void)
{
    uint16_t i;
    printf("\r\n==============Set Alt Pin==============\r\n");
    printf("   C\\R\t 1\t 2\t 3\t 4\t 5\r\n");
    for (i = 0; i < sizeof(Ramp_List)/sizeof(Ramp_List[0]); i++)
    {
        printf("%d.%s\t%s\t%s\t%s\t%s\t%s\r\n", i, Ramp_List[i].PwmName, Ramp_List[i].RampPin[0].PinName, Ramp_List[i].RampPin[1].PinName, \
                                                                     Ramp_List[i].RampPin[2].PinName, Ramp_List[i].RampPin[3].PinName, \
                                                                     Ramp_List[i].RampPin[4].PinName);
    }
    printf("==============+++++++++++==============\r\n");
}

void PWM_Pin_Set__(void)
{
    uint8_t index,i;
    printf("Please Input Pin index:(R:1/2/3/4/5) ");
    do
    {
        if (UART_GetLineStatus(UART0) & UART_LINE_STATUS_RX_RECVD)
        {
            index = UART_ReceiveData(UART0) - 48;
            if (index == 0x1B)  //esc
            {
                return;
            }
            printf("%d\n", index);
        }
        
    }while (index < 1 || index > 5);   
      
    PWM_CurrentPin = index - 1;
    printf("PWM_CurrentPin %d\r\n", PWM_CurrentPin);
    for (i = 0; i < sizeof(Ramp_List)/sizeof(Ramp_List[0]); i++)
    {
        
        if (Ramp_List[i].RampPin[PWM_CurrentPin].GPIOx != NULL)
        {
            GPIO_PinRemapConfig(Ramp_List[i].RampPin[PWM_CurrentPin].GPIOx, \
                                Ramp_List[i].RampPin[PWM_CurrentPin].Pin_Num, \
                                Ramp_List[i].RampPin[PWM_CurrentPin].Ramp_Select);
            printf("GPIOx is %d,Pin_Num is %d,Ramp select %d\r\n",Ramp_List[i].RampPin[PWM_CurrentPin].GPIOx, \
                                Ramp_List[i].RampPin[PWM_CurrentPin].Pin_Num, \
                                Ramp_List[i].RampPin[PWM_CurrentPin].Ramp_Select);
        }
    } 
    PrintfCurrentTimerRemap();
    PrintList2();
}

void PWM_Pin_Set(void)
{
    uint8_t index,i;
    printf("Please Input Pin index:(R:1/2/3/4/5) ");
    do
    {
        //if (UART_GetLineStatus(UART0) & UART_LINE_STATUS_RX_RECVD)
        if(!isEmpty(&uart))
        {
            //index = UART_ReceiveData(UART0) - 48;
            index = pop(&uart) - 48;
            if (index == 0x1B)  //esc
            {
                return;
            }
            printf("%d\n", index);
        }
        
    }while (index < 1 || index > 5);   
      
    PWM_CurrentPin = index - 1;
    printf("PWM_CurrentPin %d\r\n", PWM_CurrentPin);
    for (i = 0; i < sizeof(Ramp_List)/sizeof(Ramp_List[0]); i++)
    {
        
        if (Ramp_List[i].RampPin[PWM_CurrentPin].GPIOx != NULL)
        {
            GPIO_PinRemapConfig(Ramp_List[i].RampPin[PWM_CurrentPin].GPIOx, \
                                Ramp_List[i].RampPin[PWM_CurrentPin].Pin_Num, \
                                Ramp_List[i].RampPin[PWM_CurrentPin].Ramp_Select);
            printf("GPIOx is %d,Pin_Num is %d,Ramp select %d\r\n",Ramp_List[i].RampPin[PWM_CurrentPin].GPIOx, \
                                Ramp_List[i].RampPin[PWM_CurrentPin].Pin_Num, \
                                Ramp_List[i].RampPin[PWM_CurrentPin].Ramp_Select);
        }
    } 
    PrintfCurrentTimerRemap();
    PrintList2();
}

void PrintfCurrentTimerRemap(void)
{
    uint16_t i;
    printf("\r\n====PWM Current Pin====\r\n");
    for (i = 0; i < sizeof(Ramp_List)/sizeof(Ramp_List[0]); i++)
    {
        printf("%d.%s\t%s\t\r\n", i, Ramp_List[i].PwmName, Ramp_List[i].RampPin[PWM_CurrentPin].PinName);
    }
    printf("=====+++++++++++=====\r\n");
}

void TimerPWMSetStructInit3()
{

	timerPWMSet.PWM_HZ = (SYSCTRL->PCLK_1MS_VAL * 500);
	timerPWMSet.DutyCycleHighPluse = 1;
	timerPWMSet.DutyCycleLowPluse = 1;
    
	SYSCTRL_GetClocksFreq(&timerPWMSet.clocks);
    
    if(timerPWMSet.clocks.PCLK_Frequency < (timerPWMSet.PWM_HZ * 2))
    {
        printf("You set PWM frequency is %dHz.\n",timerPWMSet.PWM_HZ);
        printf("The highest frequency of PWM is PCLK_Frequency/2,is %dHz.\n",timerPWMSet.clocks.PCLK_Frequency / 2);
        while(1);
    }
    // domyst
    printf("timerPWMSet.PWM_HZ = %d\n", timerPWMSet.PWM_HZ);
}

void TIMER_PWMSet3(void)
{
	uint32_t Period = 0;
	uint32_t DutyCyclePeriod = 0;

	Period = timerPWMSet.clocks.PCLK_Frequency / timerPWMSet.PWM_HZ;

	DutyCyclePeriod = Period / (timerPWMSet.DutyCycleLowPluse + timerPWMSet.DutyCycleHighPluse);

	timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod = (DutyCyclePeriod * timerPWMSet.DutyCycleLowPluse - 1);
	timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod = (DutyCyclePeriod * timerPWMSet.DutyCycleHighPluse - 1);

	TIM_SetPWMPeriod(TIMM0, TIM_0,
						timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod,
						timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod);
	TIM_SetPWMPeriod(TIMM0, TIM_1,
						timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod,
						timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod);
    TIM_SetPWMPeriod(TIMM0, TIM_2,
						timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod,
						timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod);
    TIM_SetPWMPeriod(TIMM0, TIM_3,
						timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod,
						timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod);
    TIM_SetPWMPeriod(TIMM0, TIM_4,
						timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod,
						timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod);
    TIM_SetPWMPeriod(TIMM0, TIM_5,
						timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod,
						timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod);
    TIM_SetPWMPeriod(TIMM0, TIM_6,
						timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod,
						timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod);
    TIM_SetPWMPeriod(TIMM0, TIM_7,
						timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod,
						timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod);
}

void TIMER_Configuration3(void)
{
	uint32_t Period = 0;
	uint32_t DutyCyclePeriod = 0;
	
    //domyst
    printf("timerPWMSet.clocks.PCLK_Frequency : %d,  Period :%d\n", timerPWMSet.clocks.PCLK_Frequency, Period);
	Period = timerPWMSet.clocks.PCLK_Frequency / timerPWMSet.PWM_HZ;
	printf("_Period :%d\n", Period);
	DutyCyclePeriod = Period / (timerPWMSet.DutyCycleLowPluse + timerPWMSet.DutyCycleHighPluse);
	printf("DutyCyclePeriod :%d\n", DutyCyclePeriod);
    printf("timerPWMSet.DutyCycleLowPluse : %d,  timerPWMSet.DutyCycleHighPluse :%d\n", timerPWMSet.DutyCycleLowPluse, timerPWMSet.DutyCycleHighPluse);

	timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod = (DutyCyclePeriod * timerPWMSet.DutyCycleLowPluse - 1);
	timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod = (DutyCyclePeriod * timerPWMSet.DutyCycleHighPluse - 1);

    printf("timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod : %d,  timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod :%d\n", timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod, timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod);

    // timerPWMSet.TIM_PWMSetStruct.TIMx = TIM_0;
	// TIM_PWMInit(TIMM0, &timerPWMSet.TIM_PWMSetStruct);
    
    // timerPWMSet.TIM_PWMSetStruct.TIMx = TIM_1;
    // TIM_PWMInit(TIMM0, &timerPWMSet.TIM_PWMSetStruct);
    
    // timerPWMSet.TIM_PWMSetStruct.TIMx = TIM_2;
    // TIM_PWMInit(TIMM0, &timerPWMSet.TIM_PWMSetStruct);
    
    timerPWMSet.TIM_PWMSetStruct.TIMx = TIM_3;
    TIM_PWMInit(TIMM0, &timerPWMSet.TIM_PWMSetStruct);
    
    // timerPWMSet.TIM_PWMSetStruct.TIMx = TIM_4;
    // TIM_PWMInit(TIMM0, &timerPWMSet.TIM_PWMSetStruct);
    
    // timerPWMSet.TIM_PWMSetStruct.TIMx = TIM_5;
    // TIM_PWMInit(TIMM0, &timerPWMSet.TIM_PWMSetStruct);
    
    // timerPWMSet.TIM_PWMSetStruct.TIMx = TIM_6;
    // TIM_PWMInit(TIMM0, &timerPWMSet.TIM_PWMSetStruct);
    
    // timerPWMSet.TIM_PWMSetStruct.TIMx = TIM_7;
    // TIM_PWMInit(TIMM0, &timerPWMSet.TIM_PWMSetStruct);

    // Ãß°¡
    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_3, GPIO_Remap_2);
}

//
//void pwm_start(period, duty)
void pwm_start(void)
{
    uint32_t Period = 0;
	uint32_t DutyCyclePeriod = 0;

    timerPWMSet.PWM_HZ = 1000;
	timerPWMSet.DutyCycleHighPluse = 1;
	timerPWMSet.DutyCycleLowPluse = 1;
    
	SYSCTRL_GetClocksFreq(&timerPWMSet.clocks);
    
    printf("PCLK : %d\n", timerPWMSet.clocks.PCLK_Frequency);   // 48000000
    if(timerPWMSet.clocks.PCLK_Frequency < (timerPWMSet.PWM_HZ * 2))
    {
        printf("You set PWM frequency is %dHz.\n",timerPWMSet.PWM_HZ);
        printf("The highest frequency of PWM is PCLK_Frequency/2,is %dHz.\n",timerPWMSet.clocks.PCLK_Frequency / 2);
        while(1);
    }
    //
    // uint32_t Period = 0;
	// uint32_t DutyCyclePeriod = 0;
	
	Period = timerPWMSet.clocks.PCLK_Frequency / timerPWMSet.PWM_HZ;        // 48000
	
	DutyCyclePeriod = Period / (timerPWMSet.DutyCycleLowPluse + timerPWMSet.DutyCycleHighPluse); // 24000
	
	timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod = (DutyCyclePeriod * timerPWMSet.DutyCycleLowPluse - 1);    // 23999
	timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod = (DutyCyclePeriod * timerPWMSet.DutyCycleHighPluse - 1);  // 23999

    printf("Period : %d, DutyCyclePeriod : %d, low : %d, high : %d\n", Period, DutyCyclePeriod, timerPWMSet.TIM_PWMSetStruct.TIM_LowLevelPeriod, timerPWMSet.TIM_PWMSetStruct.TIM_HighLevelPeriod);
    timerPWMSet.TIM_PWMSetStruct.TIMx = TIM_3;
    TIM_PWMInit(TIMM0, &timerPWMSet.TIM_PWMSetStruct);
    
    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_3, GPIO_Remap_2);
    //
    TIM_Cmd(TIMM0, TIM_3, ENABLE);
}

void pwm_stop(void)
{
    TIM_Cmd(TIMM0, TIM_3, DISABLE);
}
//
#if 0
void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
	
    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
    
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;

	UART_Init(UART0, &UART_InitStructure);	
}

//Retarget Printf
int SER_PutChar (int ch)
{
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) ch);
	
	return ch;
}

int fputc(int c, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	if (c == '\n')
    {
		SER_PutChar('\r');
	}
	return (SER_PutChar(c));
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
        printf("%s\t%s\t%d\r\n", __func__, file, line);
	}
}
#endif

#endif