#ifndef _PROJECTCONFIG_H_
#define _PROJECTCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*单buff解码最小内存*/
#define SINGLE_BUFF_MIN_SIZE          (550 * 1024)
/*双buff解码最小内存*/
#define DOUBLE_BUFF_MIN_SIZE          (850 * 1024)
/*解码buff大小*/
//#define DECODE_BUFF_SIZE              (DOUBLE_BUFF_MIN_SIZE)
#define DECODE_BUFF_SIZE              (SINGLE_BUFF_MIN_SIZE)

/*蜂鸣器管脚设置*/
#define BEEP_GPIO                  GPIOF
#define BEEP_GOIO_PIN              GPIO_Pin_2
/*摄像头XCK管脚设置*/
#define CAM_XCK_GPIO               GPIOA
#define CAM_XCK_GPIO_PIN           GPIO_Pin_5
#define CAM_XCK_TIM                TIM_5
#define CAM_XCK_REMAP              GPIO_Remap_2
/*摄像头I2C管脚设置*/
#define SI2C_PORT                   GPIOB
#define SI2C_SCL_PIN                GPIO_Pin_0
#define SI2C_SDA_PIN                GPIO_Pin_1
#define SI2C_GPIO_REMAP             GPIO_Remap_0
/*摄像头PWDN管脚定义*/
#define CAM_PWDN_GPIO               GPIOH
#define CAM_PWDN_GOIO_PIN           GPIO_Pin_7
/*摄像头RST管脚定义*/
#define CAM_RST_GPIO                GPIOH
#define CAM_RST_GOIO_PIN            GPIO_Pin_9

// VSYNC
#define DCMI_VSYNC_GPIO_PORT    	GPIOH
#define DCMI_VSYNC_GPIO_PIN     	GPIO_Pin_11
#define DCMI_VSYNC_AF				GPIO_Remap_0
// HSYNC
#define DCMI_HSYNC_GPIO_PORT    	GPIOH
#define DCMI_HSYNC_GPIO_PIN     	GPIO_Pin_12
#define DCMI_HSYNC_AF				GPIO_Remap_0
// PIXCLK
#define DCMI_PIXCLK_GPIO_PORT   	GPIOH
#define DCMI_PIXCLK_GPIO_PIN    	GPIO_Pin_13
#define DCMI_PIXCLK_AF				GPIO_Remap_0
// DATA 0 ~ 7
#define DCMI_D0_GPIO_PORT       	GPIOG
#define DCMI_D0_GPIO_PIN        	GPIO_Pin_13
#define DCMI_D0_AF					GPIO_Remap_0
#define DCMI_D1_GPIO_PORT       	GPIOG
#define DCMI_D1_GPIO_PIN        	GPIO_Pin_14
#define DCMI_D1_AF					GPIO_Remap_0
#define DCMI_D2_GPIO_PORT       	GPIOG
#define DCMI_D2_GPIO_PIN        	GPIO_Pin_15
#define DCMI_D2_AF					GPIO_Remap_0
#define DCMI_D3_GPIO_PORT        	GPIOH
#define DCMI_D3_GPIO_PIN         	GPIO_Pin_0
#define DCMI_D3_AF					GPIO_Remap_0
#define DCMI_D4_GPIO_PORT        	GPIOH
#define DCMI_D4_GPIO_PIN         	GPIO_Pin_1
#define DCMI_D4_AF					GPIO_Remap_0
#define DCMI_D5_GPIO_PORT        	GPIOH
#define DCMI_D5_GPIO_PIN         	GPIO_Pin_2
#define DCMI_D5_AF					GPIO_Remap_0
#define DCMI_D6_GPIO_PORT        	GPIOH
#define DCMI_D6_GPIO_PIN         	GPIO_Pin_3
#define DCMI_D6_AF					GPIO_Remap_0
#define DCMI_D7_GPIO_PORT       	GPIOH
#define DCMI_D7_GPIO_PIN        	GPIO_Pin_4
#define DCMI_D7_AF					GPIO_Remap_0

/**
 *单buff解码Demo
 *
 */
 void SingleBuffDecodeDemo(void);
  /**
  * 双buff解码demo
  *
  */
 void DoubleBuffDecodeDemo(void);

#ifdef __cplusplus
}
#endif

#endif
