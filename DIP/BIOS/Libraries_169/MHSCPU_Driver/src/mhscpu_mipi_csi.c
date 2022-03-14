/************************ (C) COPYRIGHT Megahuntmicro *************************
 * File Name            : mhscpu_mipi_csi.c
 * Author               : wangxin@megahunt
 * Version              : V2.0.0
 * Date                 : 8/04/2018
 * Description          : configure csi-2 host 
 *****************************************************************************/
 #include  "Mhscpu.h"
 #include  "stdint.h"
 #include "mhscpu_mipi_csi.h"


/******************************************************************************
* @param  		void
* @function		change dcmio connect to inter dcmi, config dataen for dcmi hsync
							soft reset mipi csi2
* @return     void   
******************************************************************************/
void MIPI_Open(void)
{
	/*  dmci_sel 1:DCMI外部输入 0:内部与csi2相连 */
	SYSCTRL->DBG_CR &= ~(1 << 13);   
	/* 置1后，MIPI的dataen作为DCMI的HSYNC输入 */
	SYSCTRL->DBG_CR |= 1<<14;
	/* csi2_dcmi_sel 置0后，pc~pf的部分IO复用为CSI2-DCMI接口 */
	SYSCTRL->DBG_CR &= ~(1<<12);
	/* srst_csi2 软复位信号 */
	SYSCTRL->SOFT_RST1 |= 1<<22;
}

/******************************************************************************
* @param  		clkDivid:MIPI_PIX_CLK_DIVIDER_HALF ,MIPI_PIX_CLK_DIVIDER_QUARTER,
							MIPI_PIX_CLK_DIVIDER_SIXTH 
							用于DCMI及MIPI的pix_clk生成,clk_dcmis来自Hclk
							0: (clk_dcmis)/2 （不建议使用）
							1: (clk_dcmis)/4
							2: (clk_dcmis)/6
* @function		config mipi-dcmi pix clk  
* @return     void   
******************************************************************************/
void MIPI_DcmiPixClkSet(uint32_t clkDivid)
{
	uint32_t tmpValue;
	tmpValue = SYSCTRL->FREQ_SEL;
	SYSCTRL->FREQ_SEL = (0x1FFFFFFF&tmpValue) | (clkDivid << 29);
}

/******************************************************************************
* @param  		nTime:cycles
* @function		delay for mipi csi2 config interface
* @return     void   
******************************************************************************/
static void MipiDelay( uint32_t nTime) //cycle(*2)
{
	do{
		__asm("nop");
	}while(nTime-->0);
}

/******************************************************************************
* @param  		address:csi2 host register
*							data:value
* @function		write mipi csi2 host register
* @return     void   
******************************************************************************/
void MIPI_CSI_WRITE(uint16_t address,uint32_t data)
{
	*((uint32_t *)(CSI2_BASE+address)) = data;
}

/******************************************************************************
* @param  		address:mipi csi2 host register address
* @function		read host register 
* @return     void   
******************************************************************************/
uint32_t MIPI_CSI_READ(uint16_t address)
{
	uint32_t data;
	
	data = *((uint32_t *)(address + CSI2_BASE));
 	return data;
}
 
/******************************************************************************
* @param  		void
* @function		MIPI csi reset   
*						  Clear CSI2_RESETN, bit[0] to put the CSI-2 internal logic in the reset state.
*							Set CSI2_RESETN, bit[0] to remove the CSI-2 internal logic from the reset state.
* @return     void   
******************************************************************************/
static void MIPI_CSI_HostReset(void)
{
	MIPI_CSI_WRITE(CSI2_RESETN,0);
	MipiDelay(1000);
}

/******************************************************************************
* @param  		void
* @function		mipi csi2 host  release
* @return     void   
******************************************************************************/
static void MIPI_CSI_HostRelease(void)
{
	MIPI_CSI_WRITE(CSI2_RESETN,0x1);
	MipiDelay(1000);
}

/******************************************************************************
* @param  		void
* @function		d-phy reset 
*							1. Clear PHY_SHUTDOWNZ, bit[0] and  DPHY_RSTZ, bit[0] to put the D-PHY in the reset state (powen down).
*							2. Set   PHY_SHUTDOWNZ, bit[0] and DPHY_RSTZ, bit[0] to remove the D-PHY from reset state.
*
* @return     void   
******************************************************************************/
static void MIPI_CSI_PhyReset(void)
{
	MIPI_CSI_WRITE(PHY_SHUTDOWNZ,0);
	MipiDelay(1000);
	MIPI_CSI_WRITE(DPHY_RSTZ,0);
	MipiDelay(1000);
}

/******************************************************************************
* @param  		void
* @function		d-phy release  
* @return     void   
******************************************************************************/
static void MIPI_CSI_PhyRelease(void)
{
	MIPI_CSI_WRITE(PHY_SHUTDOWNZ,0x1);
	MipiDelay(1000);
	MIPI_CSI_WRITE(DPHY_RSTZ,0x1);
	MipiDelay(1000);
}

/******************************************************************************
* @param  		void
* @function		d-phy test code reset 
*							1. Set PHY_TST_CTRL0, bit[0].
*							2. Clear PHY_TST_CTRL0, bit[0]. 
* @return     void   
******************************************************************************/
void MIPI_CSI_PhyTestCodeReset(void)
{
	MIPI_CSI_WRITE(PHY_TEST_CTRL0,0x1);
	__asm("nop");
	MIPI_CSI_WRITE(PHY_TEST_CTRL0,0x0);
}

/******************************************************************************
* @param  		void
* @function		MIPI CSI reset 
* @return     void   
******************************************************************************/
void MIPI_CSI_Reset(void)
{
	MIPI_CSI_HostReset();
	MipiDelay(1000);
	MIPI_CSI_HostRelease();
	MipiDelay(1000);
	MIPI_CSI_PhyReset();
	MipiDelay(1000);
	MIPI_CSI_PhyRelease();
	MipiDelay(1000);
	MIPI_CSI_PhyTestCodeReset();
	MipiDelay(1000);
}

/******************************************************************************
* @param  		void
* @function		set def err mask 
* @return     void   
******************************************************************************/
void MIPI_CSI_DefErrMask(void)
{
	MIPI_CSI_WRITE(INT_MSK_PHY_FATAL,0xffffffff);
	MIPI_CSI_WRITE(INT_MSK_PKT_FATAL,0xffffffff);
	MIPI_CSI_WRITE(INT_MSK_FRAME_FATAL,0xffffffff);
	MIPI_CSI_WRITE(INT_MSK_PHY,0xffffffff);
	MIPI_CSI_WRITE(INT_MSK_PKT,0xffffffff);
	MIPI_CSI_WRITE(INT_MSK_LINE,0xffffffff);
	MIPI_CSI_WRITE(MSK_INT_IPI,0xffffffff);
}

/******************************************************************************
* @param  		ipi_vcid:to select the virtual channel to be processed by the IPI
*							ipi_data_type:to select the data type to be processed by the IPI
* @function		mipi set virtual channel and data type
* @return     void   
******************************************************************************/
void MIPI_CSI_SetVcidDt(uint32_t ipi_vcid,uint32_t ipi_data_type)
{
	MIPI_CSI_WRITE(IPI_VCID,ipi_vcid);
	MIPI_CSI_WRITE(IPI_DATA_TYPE,ipi_data_type);
}

/******************************************************************************
* @param  		ipi_hsa_time:configure Horizontal Synchronism Active period in pixclk cycles
*							ipi_hbp_time:configure Horizontal Back Porch period in pixclk cycles
*							ipi_hsd_time:configure Horizontal Sync Porch delay period in pixclk cycles
*							ipi_hline_time: configure the size of the line time counted in pixclk cycles
*							config these params by Camera param , generally config as below:
*
*							Line Transmission without Horizontal Timing Information from the Camera
*																		  ____________________				
*							time to start video____|		 |					    |_________________________
*																		 |     |________	    |
*							hsync							_____|_____|				|_____|_________________________
*																		 |-HSD-|- HSA -	|-HBP-|________________________
*							data_en           __________________________|                        |_______
*
*							pix interface transmit ______________________^^^^^^^^^pixdata^^^^^^^^________
* 
* @function		set horizontal information 
* @return     void   
******************************************************************************/
void MIPI_CSI_SetHorizontal(uint32_t ipi_hsa_time,uint32_t ipi_hbp_time,uint32_t ipi_hsd_time,uint32_t ipi_hline_time)
{
	MIPI_CSI_WRITE(IPI_HSA_TIME,ipi_hsa_time);
	MIPI_CSI_WRITE(IPI_HBP_TIME,ipi_hbp_time);
	MIPI_CSI_WRITE(IPI_HSD_TIME,ipi_hsd_time);
	MIPI_CSI_WRITE(IPI_HLINE_TIME,ipi_hline_time);
}

/******************************************************************************
* @param  		ipi_vsa_lines:configure the Vertical Synchronism Active period measured in number of horizontal lines
*							ipi_vbp_lines:configure the Vertical Back Porch period measured in number of horizontal lines
*							ipi_vfp_lines:configure the Vertical Front Porch period measured in number of horizontal lines
*							ipi_vactive_lines:configure the Vertical Active period measured in number of horizontal lines
* @function		set vertical information, only CSI2_CONTROLLER_TIMING_MODE need
* @return     void   
******************************************************************************/
void MIPI_CSI_SetVertical(uint32_t ipi_vsa_lines,uint32_t ipi_vbp_lines,uint32_t ipi_vfp_lines,uint32_t ipi_vactive_lines)
{
	MIPI_CSI_WRITE(IPI_VSA_LINES,ipi_vsa_lines);
	MIPI_CSI_WRITE(IPI_VBP_LINES,ipi_vbp_lines);
	MIPI_CSI_WRITE(IPI_VFP_LINES,ipi_vfp_lines);
	MIPI_CSI_WRITE(IPI_VACTIVE_LINES,ipi_vactive_lines);
}

/******************************************************************************
* @param  		void
* @function		mask all irq and power off CSI2 HOST
* @return     void   
******************************************************************************/
void MIPI_CSI_MaskIrqPowerOff(void)
{
	MIPI_CSI_WRITE(N_LANES,0);
	MIPI_CSI_WRITE(INT_MSK_PHY_FATAL,0x0);
	MIPI_CSI_WRITE(INT_MSK_PKT_FATAL,0x0);
	MIPI_CSI_WRITE(INT_MSK_FRAME_FATAL,0x0);
	MIPI_CSI_WRITE(INT_MSK_PHY,0x0);
	MIPI_CSI_WRITE(INT_MSK_PKT,0x0);
	MIPI_CSI_WRITE(INT_MSK_LINE,0x0);
	MIPI_CSI_WRITE(MSK_INT_IPI,0x0);
	MIPI_CSI_WRITE(CSI2_RESETN,0x0);
}

/******************************************************************************
* @param  		void
* @function		check clock lane status 
* @return     void   
******************************************************************************/
void MIPI_CSI_CheckClockLaneSts(void)
{
	uint32_t data=0;
	data=MIPI_CSI_READ(PHY_STOPSTATE);
	if(data&(1<<16))
	{
		printf("clock lane is detected in stop state \r\n");
	}else
	{
		printf("clock lane is detected in normal state \r\n");
	}
}

/******************************************************************************
* @param  		void
* @function		check data lane status 
* @return     void   
******************************************************************************/
void MIPI_CSI_CheckDataLaneSts(void)
{
	uint32_t data=0,i;
	data=MIPI_CSI_READ(PHY_STOPSTATE);
	for(i=0;i<7;i++)
	{
		if(data&(1<<i))
		{
			printf("data lane%d is detected in stop state \r\n",i);
		}else
		{
			printf("data lane%d is detected in normal state \r\n",i);
		}
	}
}

/******************************************************************************
* @param  		void
* @function		check clock lane in high speed mode
* @return     void   
******************************************************************************/
void MIPI_CSI_CheckClkLaneInHsMode(void)
{
	uint32_t data=0;
	data=MIPI_CSI_READ(PHY_RX);
	if(data&(1<<17))
	{
		printf("high-speed clock signal \r\n");
	}else
	{
		printf("not high-speed clock signal \r\n");
	}
}

/******************************************************************************
* @param  		void
* @function		check ulpm
* @return     void   
******************************************************************************/
void MIPI_CSI_CheckULPM(void)
{
	uint32_t data=0,i;
	data=MIPI_CSI_READ(PHY_RX);
	if(data&(1<<16))
	{
		printf("clk lane is detected in not ULPM\r\n");
	}else
	{
		printf("clk lane is detected in ULPM\r\n");
	}
	for(i=0;i<7;i++)
	{
		if(data&(1<<i))
		{
			printf("data lane%d is detected in ULPM\r\n",i);
		}else
		{
			printf("data lane%d is detected in not ULPM  \r\n",i);
		}
	}
}

/******************************************************************************
* @param  		void
* @function		check lanes status 
* @return     void   
******************************************************************************/
void MIPI_CSI_CheckLaneSts(void)
{
	MIPI_CSI_CheckClockLaneSts();
	MIPI_CSI_CheckDataLaneSts();
	MIPI_CSI_CheckClkLaneInHsMode();
	MIPI_CSI_CheckULPM();
	
	//dw_csi_irq(); //检查是否有中断
}

 /******************************************************************************
* @param  		address: d-phy register address
							content: write value
* @function		write d-phy register 
* @return     void   
******************************************************************************/
void MIPI_CSI_WriteTestCodeCfg(uint8_t address, uint8_t content)
{
//	MIPI_CSI_WRITE(PHY_TEST_CTRL0,0);
//	MipiDelay(1000);
//	MIPI_CSI_WRITE(PHY_TEST_CTRL0,0);
//	MipiDelay(1000);
	MIPI_CSI_WRITE(PHY_TEST_CTRL1,(1<<16)|address);
	MipiDelay(1000);
	MIPI_CSI_WRITE(PHY_TEST_CTRL0,0x2);
	MipiDelay(1000);
	MIPI_CSI_WRITE(PHY_TEST_CTRL0,0x0);
	MipiDelay(1000);
	MIPI_CSI_WRITE(PHY_TEST_CTRL1,0xFF&content);
	MipiDelay(1000);
	MIPI_CSI_WRITE(PHY_TEST_CTRL0,0x2);
	MipiDelay(1000);
	MIPI_CSI_WRITE(PHY_TEST_CTRL0,0x0);
	MipiDelay(1000);
}

 /******************************************************************************
* @param  		mipi d-phy register address 
* @function		read mipi d-phy register 
* @return     register value    
******************************************************************************/
uint8_t MIPI_CSI_ReadTestCodeCfg(uint8_t address)
{
	uint32_t data;
//	MIPI_CSI_WRITE(PHY_TEST_CTRL0,0);
//	MipiDelay(1000);
//	MIPI_CSI_WRITE(PHY_TEST_CTRL0,0);
//	MipiDelay(1000);
	MIPI_CSI_WRITE(PHY_TEST_CTRL1,(1<<16)|address);
	MipiDelay(1000);
	MIPI_CSI_WRITE(PHY_TEST_CTRL0,0x2);
	MipiDelay(1000);
	MIPI_CSI_WRITE(PHY_TEST_CTRL0,0x0);
	MipiDelay(1000);
	data=MIPI_CSI_READ(PHY_TEST_CTRL1);
	MipiDelay(1000);
	MIPI_CSI_WRITE(PHY_TEST_CTRL1,0x0);
	MipiDelay(1000);
	
	return ((data&0xff00)>>8);
}

 /******************************************************************************
* @param  		ipiColorMode: CSI2_COLOR_MODE_48BIT or CSI2_COLOR_MODE_16BIT 
* @function		Config IPI color mode
* @return     0:ok,1:err    
******************************************************************************/
int32_t MIPI_CSI2_SetIpiColorMode(uint32_t ipiColorMode)
{
	uint32_t regTmp;
	regTmp = MIPI_CSI_READ(IPI_MODE);
	if(CSI2_COLOR_MODE_48BIT == ipiColorMode)
	{
		regTmp = regTmp & (~(1<<8) ); 
	}
	else if(CSI2_COLOR_MODE_16BIT == ipiColorMode)
	{
		regTmp = regTmp|(1<<8);
	}
	{
		return -1;
	}
	
	MIPI_CSI_WRITE(IPI_MODE, regTmp);
	return 0;
}

 /******************************************************************************
* @param  		timeMode: CSI2_CAMERA_TIMING_MODE or CSI2_CONTROLLER_TIMING_MODE
* @function		config ipi mode timing    
* @return     0:ok,1:err    
******************************************************************************/
int32_t MIPI_CSI2_SetTimeMode(uint32_t timeMode)
{
	uint32_t regTmp;
	regTmp = MIPI_CSI_READ(IPI_MODE);
	
	if(CSI2_CAMERA_TIMING_MODE == timeMode)
	{
		regTmp = regTmp & (~(1<<0) ); 
	}
	else if(CSI2_CONTROLLER_TIMING_MODE == timeMode)
	{
		regTmp = regTmp|(1<<0);
	}
	{
		return -1;
	}
	
	MIPI_CSI_WRITE(IPI_MODE, regTmp);
	return 0;
}

 /******************************************************************************
* @param  		laneNums: data lane nums,1 or 2
* @function		set lane numbers    
* @return     no 
******************************************************************************/
void MIPI_CSI_SetLane(uint32_t laneNums)
{
	MIPI_CSI_WRITE(N_LANES,laneNums - 1);   
}

 /******************************************************************************
* @param  		laneNums: data lane nums,1 or 2
* @param			frequencyMhz: 80-1000Mhz
* @function		set clock lane and data lane frequency    
* @return     0:ok,1:err    
******************************************************************************/
int32_t MIPI_CSI_SetLaneFrequency(uint32_t laneNums, uint32_t frequencyMhz)
{
	uint8_t hsrxfreqrange;
	
	if( (1>laneNums) || (2<laneNums))
		return -1;
	
	if( (80<frequencyMhz) && (110>= frequencyMhz))
	{
		hsrxfreqrange= 0;
	}
	else if( (110<frequencyMhz) && (150>= frequencyMhz))
	{
		hsrxfreqrange= 1;
	}
	else if( (150<frequencyMhz) && (200>= frequencyMhz))
	{
		hsrxfreqrange= 2;
	}
	else if( (200<frequencyMhz) && (250>= frequencyMhz))
	{
		hsrxfreqrange= 3;
	}
	else if( (250<frequencyMhz) && (300>= frequencyMhz))
	{
		hsrxfreqrange= 4;
	}
	else if( (300<frequencyMhz) && (400>= frequencyMhz))
	{
		hsrxfreqrange= 5;
	}
	else if( (400<frequencyMhz) && (500>= frequencyMhz))
	{
		hsrxfreqrange= 6;
	}
	else if( (500<frequencyMhz) && (600>= frequencyMhz))
	{
		hsrxfreqrange= 7;
	}
	else if( (600<frequencyMhz) && (700>= frequencyMhz))
	{
		hsrxfreqrange= 8;
	}
	else if( (700<frequencyMhz) && (800>= frequencyMhz))
	{
		hsrxfreqrange= 9;
	}
	else if( (800<frequencyMhz) && (1000>= frequencyMhz))
	{
		hsrxfreqrange= 10;
	}
	else
	{
		return -1;
	}
		
	MIPI_CSI_WriteTestCodeCfg(CLK_LANE_CTRL, hsrxfreqrange<<1);  //HS RX control of clock lane 
	MipiDelay(1000);
	MIPI_CSI_WriteTestCodeCfg(DATA_LANE0_CTRL, hsrxfreqrange<<1);  //HS RX control of lane 0 
	MipiDelay(1000);
	if(2 == laneNums)
	{
		MIPI_CSI_WriteTestCodeCfg(DATA_LANE1_CTRL, hsrxfreqrange<<1);  //HS RX control of lane 1 
		MipiDelay(1000);
	}
	return 0;
}


	


