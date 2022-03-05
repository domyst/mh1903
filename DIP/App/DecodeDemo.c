#include "ProjectConfig.h"
#include "DecodeLib.h"
#include "beep.h"
#include "ili9488.h"

/**
 *单buff解码Demo
 *
 */
 
extern uint8_t		lcd_buf[480*320*2];
extern uint16_t		button_count;
extern uint16_t		old_count;

#define	CAM_BUF_X		640
#define	CAM_BUF_Y		480

#define SCALE_X			CAM_BUF_X/LCD_COL_NUM
#define SCALE_Y			CAM_BUF_Y/LCD_ROW_NUM


 void SingleBuffDecodeDemo(void)
 {
    unsigned int i,j;
	uint8_t *img_addr;
	uint16_t r,g,b;
	uint16_t cam_data;

	 //定义保存解码结果的数组
    uint8_t result[2100] = {0};
    int32_t resnum;    //返回解码结果数量，为0，表示解码失败，为-1表示解码数组偏小
    DecodeConfigTypeDef DecodeCfg = {0};  //设置解那种码
	DecodeCfg.cfgGlobal = DECODE_MIRROR;
    DecodeResultTypeDef res = {.result = result, .maxn = 2100}; //初始化译码结构体
    
    //配置解码信息
    /**
     * 解码默认配置
     */
    DecodeConfigInit(&DecodeCfg);

	while(button_count == old_count)
	{
        //使能采图
        DecodeDcmiStart();
        //检测采图是否完成，需要中断调用DCMI_CallBackFrame()函数

        while( !DecodeDcmiFinish());

		img_addr = GetImageBuffAddr();

#define LCD_DISP_RED                    0xF800                
#define LCD_DISP_GREEN                  0x07E0
#define LCD_DISP_BLUE                   0x001F

		memset(lcd_buf,0xFF,307200);

#if 1
		for(j=0;j<LCD_ROW_NUM;j++)
		{
			for(i=0;i<LCD_COL_NUM;i++)
			{

			cam_data = (uint16_t) *(img_addr + (CAM_BUF_X - (unsigned int)(i*SCALE_X)) + (CAM_BUF_X * (unsigned int)(j * SCALE_Y)) ) >> 3;

			r = 0;
			g = 0;
			b = 0;
			r = (cam_data << 11) & LCD_DISP_RED;
			g = (cam_data << 6) & LCD_DISP_GREEN;
			b = (cam_data << 0)	& LCD_DISP_BLUE;
			cam_data = r | g | b;

			
//		 Depends on libraly, deffernt the line as below.
//			lcd_buf[((LCD_COL_NUM - i) + ((LCD_ROW_NUM - j) * LCD_COL_NUM)) * 2 ] = cam_data >> 8;
//			lcd_buf[((LCD_COL_NUM - i) + ((LCD_ROW_NUM - j) * LCD_COL_NUM)) * 2 + 1] = cam_data;
				lcd_buf[i*2 + j*LCD_COL_NUM * 2 ] = cam_data >> 8;
				lcd_buf[i*2 + j*LCD_COL_NUM * 2 + 1] = cam_data;			
			}
		}

		LCD_DisplayPicDMA(lcd_buf);
 #endif      
        //开始解码
        resnum = DecodeStart(&DecodeCfg, &res);
        
        if(resnum > 0)
        {
            printf("ID:%d\tAIMID:%s\n", res.id, res.AIM);
            for(int i = 0; i < resnum; i++)
            {
                printf("%c", result[i]);
            }
            printf("\r\n");

            //解码成功将两个buff状态置为空闲，防止重复解码         
            CleanDecodeBuffFlag();

            LCD_DisplayStr(result,180,16,LCD_DISP_BLUE,0x0000);
            beep(200);       
        }
        else if(resnum < 0)
        {
            if(resnum == DecodeResOverflowError)
            {
                printf("DecodeResOverflowError\r\n");
            }
            else if(resnum == DecodeImageNoDoneError)
            {
                printf("DecodeImageNoDoneError\r\n");
            }
            else if(resnum == DecodeLibNoInitError)
            {
                printf("DecodeLibNoInitError\r\n");
            }
        }
        else
        {
            printf("Decoding failed\r\n");
        }	
    }
 }

 /**
  * 双buff解码demo
  *
  */
void DoubleBuffDecodeDemo(void)
{
     //定义保存解码结果的数组
    uint8_t result[2100] = {0};
    int32_t resnum;    //返回解码结果数量，为0，表示解码失败，为-1表示解码数组偏小
    DecodeConfigTypeDef DecodeCfg = {0};  //设置解那种码
    DecodeResultTypeDef res = {.result = result, .maxn = 2100}; //初始化译码结构体
    
    //配置解码信息
    /**
     * 解码默认配置
     */
    DecodeConfigInit(&DecodeCfg);

    while(1) 
	{
        //建议添加此条判断，防止双buff都无效，造成卡死在while里
        if(GetDecodeBuffFlag() == FALSE)
        {
            //使能采图
            DecodeDcmiStart();
        }
        //检测采图是否完成，需要中断调用DCMI_CallBackFrame()函数
        while(!DecodeDcmiFinish());
        //采集完第一张图后解码前使能采集下一个buff图
         DecodeDcmiStart();
        //开始解码

        resnum = DecodeStart(&DecodeCfg, &res);

        if(resnum > 0)
        {
            printf("ID:%d\tAIMID:%s\r\n", res.id, res.AIM);
            for(int i = 0; i < res.resn; i++)
            {
                printf("%c", res.result[i]);
            }
            printf("\r\n");
            
            //解码成功将两个buff状态置为空闲，防止重复解码         
            CleanDecodeBuffFlag();

            beep(200);
        }
        else if(resnum < 0)
        {
            if(resnum == DecodeResOverflowError)
            {
                printf("DecodeResOverflowError\r\n");
            }
            else if(resnum == DecodeImageNoDoneError)
            {
                printf("DecodeImageNoDoneError\r\n");
            }
            else if(resnum == DecodeLibNoInitError)
            {
                printf("DecodeLibNoInitError\r\n");
            }
        }
        else
        {
            printf("Decoding failed\r\n");
        }
	}
 }
