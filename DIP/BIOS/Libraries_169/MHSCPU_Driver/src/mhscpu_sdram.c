/************************ (C) COPYRIGHT Megahuntmicro *************************
 * @file                : mhscpu_sdram.c
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 21-October-2014
 * @brief               : This file provides all the SDRAM firmware functions
 *****************************************************************************/

 /* Includes ----------------------------------------------------------------*/
#include "mhscpu_sdram.h"


uint32_t SDRAM_DelayTimeout = 0;


void SDRAM_Init(SDRAM_InitTypeDef *SDRAM_InitStruct)
{
	uint32_t Timeout;
	
	// Update DelayTimeout
	SDRAM_DelayTimeout = SDRAM_InitStruct->SysFreq * 5;	// 10us Timeout
	
	// DISABLE SDRAM
	SDRAM->CONTROL = 0;
	
	// Wait Stable
	Timeout = SDRAM_DelayTimeout;
	while(Timeout--);
	
	/* Endian Setting */
	SDRAM->CONFIG = SDRAM_InitStruct->Endian;					// Endian Mode

	/* FeedBack Clock Delay */
	SYSCTRL->SDRAM_CTRL = (SYSCTRL->SDRAM_CTRL & ~SDRAM_FBCLK_SEL_Mask) | (SDRAM_InitStruct->ClockDelay << SDRAM_FBCLK_SEL_Pos);
	
	SDRAM->DYN_REFRESH = SDRAM_InitStruct->tRefresh;		// DYN_ Refresh Time 46
	SDRAM->DYN_RP	  = SDRAM_InitStruct->tRP & 0x0F;		// DYN_ t(RP) > 15ns [0:3] (default 0x0F + 1 CLK)
	SDRAM->DYN_RAS	  = SDRAM_InitStruct->tRAS & 0x0F;		// DYN_ t(RAS) > 40ns [0:3] (default 0x0F + 1 CLK)
	SDRAM->DYN_SREX	  = SDRAM_InitStruct->tSREX & 0x7F;		// DYN_ t(SREX) > 72ns [0:6] (default 0x0F + 1 CLK)
	SDRAM->DYN_WR	  = SDRAM_InitStruct->tWR & 0x0F;		// DYN_ t(WR) > 2CLK [0:3] (default 0x0F + 1 CLK)
	SDRAM->DYN_RC	  = SDRAM_InitStruct->tRC & 0x1F;		// DYN_ t(RC) > 60ns [0:4] (default 0x1F + 1 CLK)
	SDRAM->DYN_RFC	  = SDRAM_InitStruct->tRFC & 0x1F;		// DYN_ t(RFC) > 60ns [0:4] (default 0x1F + 1 CLK)
	SDRAM->DYN_XSR	  = SDRAM_InitStruct->tXSR & 0xFF;		// DYN_ t(XSR) > 72ns [0:7] (default 0x0F + 1 CLK)
	SDRAM->DYN_RRD	  = SDRAM_InitStruct->tRRD & 0x0F;		// DYN_ t(RRD) > 2CLK [0:3] (default 0x0F + 1 CLK)
	SDRAM->DYN_MRD	  = SDRAM_InitStruct->tMRD & 0x0F;		// DYN_ t(MRD/RSC) > 2CLK [0:3] (default 0x0F + 1 CLK)
	
	// tCCD, read/write to read/write period
	SDRAM->DYN_CDLR	 = SDRAM_InitStruct->tCDLR & 0x0F;		// DYN_ t(CDLR) > 1CLK [0:3] (default 0x0F + 1 CLK)

	SDRAM->DYN_READCONFIG = 0x00001111;						// Postive + FeedBack
	SDRAM->DYN_CONFIG = SDRAM_InitStruct->ModelConfig;		// DYN_ Choice 128M(8Mx16),4 banks,row 12,col 9
	SDRAM->DYN_RASCAS = ((SDRAM_InitStruct->RAS & 0xFF) | (SDRAM_InitStruct->CAS << 8));
}

void SDRAM_InitSequence(uint32_t ModeSet, FunctionalState NewState)
{
	uint32_t ModeReister, Timeout;
	
	/* DISABLE SDRAM */
	SDRAM->CONTROL &= ~SDRAM_CONTROL_ENABLE;
	
	// Wait Stable
	Timeout = SDRAM_DelayTimeout;
	while(Timeout--);
	
	if (NewState == DISABLE)
    {
		return;
    }

	// Start & Update Settings
	SDRAM->DYN_CONTROL = 0x018a;			// SDRAM NOP
	SDRAM->DYN_CONTROL = 0x010a;			// SDRAM PreCharge
	SDRAM->CONTROL = 1;					    // Start SDRAM

	// Wait SDRAM Start Stable
	do
	{
		Timeout = SDRAM_DelayTimeout;
		while(Timeout--);
	}
	while (SDRAM->STATUS);
	
	// Mode Change
	SDRAM->DYN_CONTROL = 0x008a;			// SDRAM Mode Register Setting
	
	Timeout = SDRAM_DelayTimeout;
	while (Timeout--);
	
	// Write Mode Reg to SDRAM
	ModeReister = *((uint32_t *)(0x60000000 | ModeSet));
	
	Timeout = SDRAM_DelayTimeout;
	while (Timeout--);
	
	SDRAM->DYN_CONTROL = 0x000a;			// SDRAM Normal
	
	// Wait SDRAM Work Stable
	do
	{
		Timeout = SDRAM_DelayTimeout;
		while(Timeout--);
	}
	
	while (SDRAM->STATUS);
}
