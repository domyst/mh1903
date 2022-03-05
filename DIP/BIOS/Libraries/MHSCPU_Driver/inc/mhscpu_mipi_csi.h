#ifndef DWC_MIPI_CSI_H_
#define DWC_MIPI_CSI_H_

#include "stdint.h"

/*CSI2 Host register address begin*/
#define VERSION  0x0
#define N_LANES  0x4
#define CSI2_RESETN  0x8
#define INT_ST_MAIN  0xc
#define DATA_IDS_1  0x10
#define DATA_IDS_2  0x14
#define PHY_SHUTDOWNZ  0x40
#define DPHY_RSTZ  0x44
#define PHY_RX  0x48
#define PHY_STOPSTATE  0x4c
#define PHY_TEST_CTRL0  0x50
#define PHY_TEST_CTRL1  0x54
#define PHY2_TEST_CTRL0  0x58
#define PHY2_TEST_CTRL1  0x5c
#define IPI_MODE  0x80
#define IPI_VCID  0x84
#define IPI_DATA_TYPE  0x88
#define IPI_MEM_FLUSH  0x8c
#define IPI_HSA_TIME  0x90
#define IPI_HBP_TIME  0x94
#define IPI_HSD_TIME  0x98
#define IPI_HLINE_TIME  0x9c
#define IPI_VSA_LINES  0xb0
#define IPI_VBP_LINES  0xb4
#define IPI_VFP_LINES  0xb8
#define IPI_VACTIVE_LINES  0xbc

#define INT_ST_PHY_FATAL  0xe0
#define INT_MSK_PHY_FATAL  0xe4
#define INT_FORCE_PHY_FATAL  0xe8

#define INT_ST_PKT_FATAL  0xf0
#define INT_MSK_PKT_FATAL  0xf4
#define INT_FORCE_PKT_FATAL  0xf8

#define INT_ST_FRAME_FATAL 0x100
#define INT_MSK_FRAME_FATAL 0x104
#define INT_FORCE_FRAME_FATAL  0x108

#define INT_ST_PHY  0x110
#define INT_MSK_PHY  0x114
#define INT_FORCE_PHY  0x118

#define INT_ST_PKT  0x120
#define INT_MSK_PKT 0x124
#define INT_FORCE_PKT  0x128

#define INT_ST_LINE  0x130
#define INT_MSK_LINE  0x134
#define INT_FORCE_LINE  0x138

#define INT_ST_IPI 0x140
#define MSK_INT_IPI  0x144
#define FORCE_INT_IPI  0x148
/*CSI2 Host register address end*/

/* PHY rgister address begain */
#define  CLK_LANE_CTRL 		0x34 			//HS RX control of clock lane
#define	 DATA_LANE0_CTRL 	0x44    	//HS RX control of lane 0 
#define  DATA_LANE1_CTRL	0x54			//HS RX control of lane 1
/* PHY register address end  */

// Data Types begin P75
// 0x00 to 0x07      Short    Synchronization Short Packet Data Types
  #define CSI2_HOST_DT_FS         0x00
  #define CSI2_HOST_DT_FE         0x01
  #define CSI2_HOST_DT_LS         0x02
  #define CSI2_HOST_DT_LE         0x03
  #define CSI2_HOST_DT_RSRV1      0x04
  #define CSI2_HOST_DT_RSRV2      0x05
  #define CSI2_HOST_DT_RSRV3      0x06
  #define CSI2_HOST_DT_RSRV4      0x07

// 0x08 to 0x0F      Short    Generic Short Packet Data Types
  #define CSI2_HOST_DT_G_SP1      0x08
  #define CSI2_HOST_DT_G_SP2      0x09
  #define CSI2_HOST_DT_G_SP3      0x0A
  #define CSI2_HOST_DT_G_SP4      0x0B
  #define CSI2_HOST_DT_G_SP5      0x0C
  #define CSI2_HOST_DT_G_SP6      0x0D
  #define CSI2_HOST_DT_G_SP7      0x0E
  #define CSI2_HOST_DT_G_SP8      0x0F

// 0x10 to 0x17      Long     Generic Long Packet Data Types
  #define CSI2_HOST_DT_NULL       0x10
  #define CSI2_HOST_DT_BLANK      0x11
  #define CSI2_HOST_DT_EMBED      0x12
  #define CSI2_HOST_DT_RSRV5      0x13
  #define CSI2_HOST_DT_RSRV6      0x14
  #define CSI2_HOST_DT_RSRV7      0x15
  #define CSI2_HOST_DT_RSRV8      0x16
  #define CSI2_HOST_DT_RSRV9      0x17

// 0x18 to 0x1F      Long     YUV Data
  #define CSI2_HOST_DT_YUV420_8   0x18
  #define CSI2_HOST_DT_YUV420_10  0x19
  #define CSI2_HOST_DT_YUV420_8L  0x1A
  #define CSI2_HOST_DT_RSRV10     0x1B
  #define CSI2_HOST_DT_YUV420_8C  0x1C
  #define CSI2_HOST_DT_YUV420_10C 0x1D
  #define CSI2_HOST_DT_YUV422_8   0x1E
  #define CSI2_HOST_DT_YUV422_10  0x1F

// 0x20 to 0x27      Long     RGB Data
  #define CSI2_HOST_DT_RGB444     0x20
  #define CSI2_HOST_DT_RGB555     0x21
  #define CSI2_HOST_DT_RGB565     0x22
  #define CSI2_HOST_DT_RGB666     0x23
  #define CSI2_HOST_DT_RGB888     0x24
  #define CSI2_HOST_DT_RSRV11     0x25
  #define CSI2_HOST_DT_RSRV12     0x26
  #define CSI2_HOST_DT_RSRV13     0x27

// 0x28 to 0x2F      Long     RAW Data
  #define CSI2_HOST_DT_RAW6       0x28
  #define CSI2_HOST_DT_RAW7       0x29
  #define CSI2_HOST_DT_RAW8       0x2A
  #define CSI2_HOST_DT_RAW10      0x2B
  #define CSI2_HOST_DT_RAW12      0x2C
  #define CSI2_HOST_DT_RAW14      0x2D
  #define CSI2_HOST_DT_RSRV14     0x2E
  #define CSI2_HOST_DT_RSRV15     0x2F

// 0x30 to 0x37      Long     User Defined Byte-based Data
  #define CSI2_HOST_DT_U_DEF1     0x30
  #define CSI2_HOST_DT_U_DEF2     0x31
  #define CSI2_HOST_DT_U_DEF3     0x32
  #define CSI2_HOST_DT_U_DEF4     0x33
  #define CSI2_HOST_DT_U_DEF5     0x34
  #define CSI2_HOST_DT_U_DEF6     0x35
  #define CSI2_HOST_DT_U_DEF7     0x36
  #define CSI2_HOST_DT_U_DEF8     0x37


// 0x38 to 0x3F      --       Reserved
  #define CSI2_HOST_DT_RSRV16     0x38
  #define CSI2_HOST_DT_RSRV17     0x39
  #define CSI2_HOST_DT_RSRV18     0x3A
  #define CSI2_HOST_DT_RSRV19     0x3B
  #define CSI2_HOST_DT_RSRV20     0x3C
  #define CSI2_HOST_DT_RSRV21     0x3D
  #define CSI2_HOST_DT_RSRV22     0x3E
  #define CSI2_HOST_DT_RSRV23     0x3F
//data type end

enum interrupt_type
{
	CSI2_INT_PHY_FATAL = 1<<0,
	CSI2_INT_PKT_FATAL = 1<<1,
	CSI2_INT_FRAME_FATAL = 1<<2,
	CSI2_INT_PHY = 1<<16,
	CSI2_INT_PKT = 1<<17,
	CSI2_INT_LINE = 1<<18,
	CSI2_INT_IPI = 1<<19,
};

/*color mode*/
#define CSI2_COLOR_MODE_48BIT	(0x0)
#define CSI2_COLOR_MODE_16BIT	(0x1)

/*IPI timing */
#define  CSI2_CAMERA_TIMING_MODE	    (0x0)
#define  CSI2_CONTROLLER_TIMING_MODE	(0x1)

/* mipi-dcmi pix clk divider */
#define MIPI_PIX_CLK_DIVIDER_HALF     0   //divider by AHB clk
#define MIPI_PIX_CLK_DIVIDER_QUARTER  1
#define MIPI_PIX_CLK_DIVIDER_SIXTH		2 

/******************************************************************************
* @param  		void
* @function		change dcmio connect to inter dcmi, config dataen for dcmi hsync
							soft reset mipi csi2
* @return     void   
******************************************************************************/
void MIPI_Open(void);

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
void MIPI_DcmiPixClkSet(uint32_t clkDivid);

/******************************************************************************
* @param  		address:csi2 host register
*							data:value
* @function		write mipi csi2 host register
* @return     void   
******************************************************************************/
void MIPI_CSI_WRITE(uint16_t address,uint32_t data);

/******************************************************************************
* @param  		address:mipi csi2 host register address
* @function		read host register 
* @return     void   
******************************************************************************/
uint32_t MIPI_CSI_READ(uint16_t address);

/******************************************************************************
* @param  		void
* @function		MIPI csi reset   
*						  Clear CSI2_RESETN, bit[0] to put the CSI-2 internal logic in the reset state.
*							Set CSI2_RESETN, bit[0] to remove the CSI-2 internal logic from the reset state.
* @return     void   
******************************************************************************/
void MIPI_CSI_Reset(void);

/******************************************************************************
* @param  		laneNums: data lane nums,1 or 2
* @param			frequencyMhz: 80-1000Mhz
* @function		set clock lane and data lane frequency    
* @return     0:ok,1:err    
******************************************************************************/
int32_t MIPI_CSI_SetLaneFrequency(uint32_t laneNums, uint32_t frequencyMhz);

 /******************************************************************************
* @param  		mipi d-phy register address 
* @function		read mipi d-phy register 
* @return     register value    
******************************************************************************/
uint8_t MIPI_CSI_ReadTestCodeCfg(uint8_t address);

/******************************************************************************
* @param  		void
* @function		check lanes status 
* @return     void   
******************************************************************************/
void MIPI_CSI_CheckLaneSts(void);

/******************************************************************************
* @param  		ipi_vcid:to select the virtual channel to be processed by the IPI
*							ipi_data_type:to select the data type to be processed by the IPI
* @function		mipi set virtual channel and data type
* @return     void   
******************************************************************************/
void MIPI_CSI_SetVcidDt(uint32_t ipi_vcid,uint32_t ipi_data_type);

/******************************************************************************
* @param  		ipiColorMode: CSI2_COLOR_MODE_48BIT or CSI2_COLOR_MODE_16BIT 
* @function		Config IPI color mode
* @return     0:ok,1:err    
******************************************************************************/
int32_t MIPI_CSI2_SetIpiColorMode(uint32_t ipiColorMode);

 /******************************************************************************
* @param  		timeMode: CSI2_CAMERA_TIMING_MODE or CSI2_CONTROLLER_TIMING_MODE
* @function		config ipi mode timing    
* @return     0:ok,1:err    
******************************************************************************/
int32_t MIPI_CSI2_SetTimeMode(uint32_t timeMode);

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
*							pix interface transmit ______________________^^^^^^^^pixdata^^^^^^^^^________
* 
* @function		set horizontal information 
* @return     void   
******************************************************************************/
void MIPI_CSI_SetHorizontal(uint32_t ipi_hsa_time,uint32_t ipi_hbp_time,uint32_t ipi_hsd_time,uint32_t ipi_hline_time);

/******************************************************************************
* @param  		ipi_vsa_lines:configure the Vertical Synchronism Active period measured in number of horizontal lines
*							ipi_vbp_lines:configure the Vertical Back Porch period measured in number of horizontal lines
*							ipi_vfp_lines:configure the Vertical Front Porch period measured in number of horizontal lines
*							ipi_vactive_lines:configure the Vertical Active period measured in number of horizontal lines
* @function		set vertical information, only CSI2_CONTROLLER_TIMING_MODE need
* @return     void   
******************************************************************************/
void MIPI_CSI_SetVertical(uint32_t ipi_vsa_lines,uint32_t ipi_vbp_lines,uint32_t ipi_vfp_lines,uint32_t ipi_vactive_lines);

/******************************************************************************
* @param  		void
* @function		set def err mask 
* @return     void   
******************************************************************************/
void MIPI_CSI_DefErrMask(void);

 /******************************************************************************
* @param  		laneNums: data lane nums,1 or 2
* @function		set lane numbers    
* @return     no 
******************************************************************************/
void MIPI_CSI_SetLane(uint32_t laneNum);

#endif
