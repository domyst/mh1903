#include "ProjectConfig.h"
#include "DecodeLib.h"
#include "beep.h"
#include "ili9488.h"

/**
 *��buff����Demo
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

	 //���屣�������������
    uint8_t result[2100] = {0};
    int32_t resnum;    //���ؽ�����������Ϊ0����ʾ����ʧ�ܣ�Ϊ-1��ʾ��������ƫС
    DecodeConfigTypeDef DecodeCfg = {0};  //���ý�������
	DecodeCfg.cfgGlobal = DECODE_MIRROR;
    DecodeResultTypeDef res = {.result = result, .maxn = 2100}; //��ʼ������ṹ��
    
    //���ý�����Ϣ
    /**
     * ����Ĭ������
     */
    DecodeConfigInit(&DecodeCfg);

	while(button_count == old_count)
	{
        //ʹ�ܲ�ͼ
        DecodeDcmiStart();
        //����ͼ�Ƿ���ɣ���Ҫ�жϵ���DCMI_CallBackFrame()����

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
        //��ʼ����
        resnum = DecodeStart(&DecodeCfg, &res);
        
        if(resnum > 0)
        {
            printf("ID:%d\tAIMID:%s\n", res.id, res.AIM);
            for(int i = 0; i < resnum; i++)
            {
                printf("%c", result[i]);
            }
            printf("\r\n");

            //����ɹ�������buff״̬��Ϊ���У���ֹ�ظ�����         
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
  * ˫buff����demo
  *
  */
void DoubleBuffDecodeDemo(void)
{
     //���屣�������������
    uint8_t result[2100] = {0};
    int32_t resnum;    //���ؽ�����������Ϊ0����ʾ����ʧ�ܣ�Ϊ-1��ʾ��������ƫС
    DecodeConfigTypeDef DecodeCfg = {0};  //���ý�������
    DecodeResultTypeDef res = {.result = result, .maxn = 2100}; //��ʼ������ṹ��
    
    //���ý�����Ϣ
    /**
     * ����Ĭ������
     */
    DecodeConfigInit(&DecodeCfg);

    while(1) 
	{
        //������Ӵ����жϣ���ֹ˫buff����Ч����ɿ�����while��
        if(GetDecodeBuffFlag() == FALSE)
        {
            //ʹ�ܲ�ͼ
            DecodeDcmiStart();
        }
        //����ͼ�Ƿ���ɣ���Ҫ�жϵ���DCMI_CallBackFrame()����
        while(!DecodeDcmiFinish());
        //�ɼ����һ��ͼ�����ǰʹ�ܲɼ���һ��buffͼ
         DecodeDcmiStart();
        //��ʼ����

        resnum = DecodeStart(&DecodeCfg, &res);

        if(resnum > 0)
        {
            printf("ID:%d\tAIMID:%s\r\n", res.id, res.AIM);
            for(int i = 0; i < res.resn; i++)
            {
                printf("%c", res.result[i]);
            }
            printf("\r\n");
            
            //����ɹ�������buff״̬��Ϊ���У���ֹ�ظ�����         
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
