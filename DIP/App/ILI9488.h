#ifndef __ILI9488_H
#define __ILI9488_H

#ifdef __cplusplus
extern "C"
{
#endif 

/* Includes ------------------------------------------------------------------*/
//#include "mhscpu.h"
#include <stdint.h>
	
/************** TFT_LCD 管脚连线 **************
			PF7 --------> CS 
			PD8 --------> RESET
			PC5 --------> DB0
			PC6 --------> DB1
			PC7 --------> DB2
			PC8 --------> DB3
			PC9 --------> DB4
			PC10--------> DB5
			PC11--------> DB6
			PC12--------> DB7
			PC13--------> RD
			PC14--------> WR
			PC15--------> CD
**********************************************/	

/***************** 行、列数 ******************/

//width mode
#define LCD_COL_NUM                     480	
#define LCD_ROW_NUM                     320	

//Vertical mode
#define LCD_COL_NUM90                     320	
#define LCD_ROW_NUM90                     480	

/************** 颜色(RGB 5,6,5) **************/
/*
#define LCD_DISP_RED                    0xF800                
#define LCD_DISP_GREEN                  0x07E0
#define LCD_DISP_BLUE                   0x001F
#define LCD_DISP_WRITE                  0xFFFF
#define LCD_DISP_BLACK                  0x0000
#define LCD_DISP_GRAY                   0xEF5D
#define LCD_DISP_GRAY75                 0x39E7
#define LCD_DISP_GRAY50                 0x7BEF
#define LCD_DISP_GRAY25                 0xADB5
*/
#define LCD_DISP_BLACK           0x0000      /*   0,   0,   0 */
#define LCD_DISP_NAVY            0x000F      /*   0,   0, 128 */
#define LCD_DISP_DARKGREEN       0x03E0      /*   0, 128,   0 */
#define LCD_DISP_DARKCYAN        0x03EF      /*   0, 128, 128 */
#define LCD_DISP_MAROON          0x7800      /* 128,   0,   0 */
#define LCD_DISP_PURPLE          0x780F      /* 128,   0, 128 */
#define LCD_DISP_OLIVE           0x7BE0      /* 128, 128,   0 */
#define LCD_DISP_LIGHTGREY       0xC618      /* 192, 192, 192 */
#define LCD_DISP_GRAY            0xEF5D
#define LCD_DISP_GRAY75          0x39E7
#define LCD_DISP_GRAY50          0x7BEF
#define LCD_DISP_GRAY25          0xADB5
#define LCD_DISP_BLUE            0x001F      /*   0,   0, 255 */
#define LCD_DISP_GREEN           0x07E0      /*   0, 255,   0 */
#define LCD_DISP_CYAN            0x07FF      /*   0, 255, 255 */
#define LCD_DISP_RED             0xF800      /* 255,   0,   0 */
#define LCD_DISP_MAGENTA         0xF81F      /* 255,   0, 255 */
#define LCD_DISP_YELLOW          0xFFE0      /* 255, 255,   0 */
#define LCD_DISP_WRITE           0xFFFF      /* 255, 255, 255 */
#define LCD_DISP_ORANGE          0xFD20      /* 255, 165,   0 */
#define LCD_DISP_GREENYELLOW     0xAFE5      /* 173, 255,  47 */
#define LCD_DISP_PINK            0xF81F


//extern LCD_InitTypeDef LCD_InitStructure;

void LCD_BlockWritePrep(unsigned int xStart, unsigned int xEnd, unsigned int yStart, unsigned int yEnd);

void LCD_DisplayColor(uint32_t color);
void LCD_DisplayGrayHor16(void);
void LCD_DisplayGrayHor32(void);
void LCD_DisplayBand(void);
void LCD_DisplayFrame(void);
void LCD_DisplayScaleHor1(void);
void LCD_DisplayScaleHor2(void);
void LCD_DisplayScaleVer(void);
void LCD_DisplayPicDirect(const uint8_t *picture);
void LCD_DisplayPicDirect_Partial(uint8_t *picture, uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y);
void LCD_DisplayPicDMA(uint8_t *picture);
void LCD_DisplayPicDMA_Partial(uint8_t *picture, uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y);

void LCD_WriteOneDot(uint32_t color);
void LCD_DisplayOneChar(uint8_t ord, uint32_t xStart, uint32_t yStart, uint32_t textColor, uint32_t backColor);
void LCD_DisplayOneInt(uint16_t data, uint32_t xStart, uint32_t yStart, uint32_t textColor, uint32_t backColor,uint8_t digit_cnt);

void LCD_DisplayStr(uint8_t *str, uint32_t xStart, uint32_t yStart, uint32_t textColor, uint32_t backColor);

void LCD_DrawLine(uint32_t xStart, uint32_t xEnd, uint32_t yStart, uint32_t yEnd, uint32_t color);
void LCD_DrawGird(uint32_t color);

void LCD_RD_RDDIDIF(void);

void LCD_Configuration(void);

void LCD_DisplayOneDot(uint32_t xStart, uint32_t yStart, uint16_t Color);

void LCD_InitSequence_PSK(uint8_t sel);

void LCD_DisplayStr90(uint8_t *str, uint32_t xStart, uint32_t yStart, uint32_t textColor, uint32_t backColor);

#ifdef __cplusplus
}
#endif

#endif  ///< __ILI9488_H

