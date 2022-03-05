#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include "ili9488.h"
#include "xpt2046.h"
#include "calibration.h"

// calibration values
float xCalM = 0.0, yCalM = 0.0; // gradients
float xCalC = 0.0, yCalC = 0.0; // y axis crossing points

int8_t blockWidth = 20; // block size
int8_t blockHeight = 20;
int16_t blockX = 0, blockY = 0; // block position (pixels)

void calibrateTouchScreen(void)
{
	int16_t x1,y1,x2,y2;
	int16_t	xDist, yDist;
	uint16_t	i;

	TouchPositionTypeDef position;
	TouchPositionTypeDef position_pre;
	
	LCD_DisplayColor(LCD_DISP_BLACK);
	
	LCD_DrawLine(10, 30, 20, 20, LCD_DISP_RED);
	LCD_DrawLine(20, 20, 10, 30, LCD_DISP_RED);

	x1 = 0;
	y1 = 0;
	
	while(touch_irq() == 1);
	i=0;
	while(touch_irq() == 0)
	{
		if(i != 0)
		{
			position_pre= position;
		}
		position = touch_get_raw_position();
		i = 1;
	}

	x1 = position_pre.x;
	y1 = position_pre.y;
	
	LCD_DrawLine(10, 30, 20, 20, LCD_DISP_BLACK);
	LCD_DrawLine(20, 20, 10, 30, LCD_DISP_BLACK);

	printf("Cal 1 [%04d], [%04d]\n\r",x1, y1);


	LCD_DrawLine(LCD_COL_NUM - 30, LCD_COL_NUM - 10, LCD_ROW_NUM - 20, LCD_ROW_NUM - 20, LCD_DISP_RED);
	LCD_DrawLine(LCD_COL_NUM - 20, LCD_COL_NUM - 20, LCD_ROW_NUM - 30, LCD_ROW_NUM - 10, LCD_DISP_RED);

	while(touch_irq() == 1);

	i=0;
	while(touch_irq() == 0)
	{
		if(i != 0)
		{
			position_pre= position;
		}
		position = touch_get_raw_position();
		i = 1;
	}

	x2 = position_pre.x;
	y2 = position_pre.y;

	LCD_DrawLine(LCD_COL_NUM - 30, LCD_COL_NUM - 10, LCD_ROW_NUM - 20, LCD_ROW_NUM - 20, LCD_DISP_BLACK);
	LCD_DrawLine(LCD_COL_NUM - 20, LCD_COL_NUM - 20, LCD_ROW_NUM - 30, LCD_ROW_NUM - 10, LCD_DISP_BLACK);

	printf("Cal 2 [%04d], [%04d]\n\r",x2, y2);
	
	xDist = LCD_COL_NUM - 40;
	yDist = LCD_ROW_NUM - 40;
	
	// x
	xCalM = (float)xDist / (float)(x2 - x1);
	xCalC = 20.0 - ((float)x1 * xCalM);
	// y
	yCalM = (float)yDist / (float)(y2 - y1);
	yCalC = 20.0 - ((float)y1 * yCalM);

}

TouchPositionTypeDef touch_get_position(void)
{
	TouchPositionTypeDef position;
	
	position = touch_get_raw_position();

	position.x = position.x * xCalM + xCalC;
	position.y = position.y * yCalM + yCalC;
	
	if(position.x < 0) position.x = 0;
	if(position.y < 0) position.y = 0;
	
	return position;
}

