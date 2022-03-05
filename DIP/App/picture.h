#include <stdint.h>

//uint8_t	lcd_buf[480 * 320 * 2];
#define	IMG_CI_WIDTH		480
#define	IMG_CI_HEIGHT		320
#define IMT_CI_DEPTH		2

#define	IMG_MENU_WIDTH		240
#define	IMG_MENU_HEIGHT		80
#define IMT_MENU_DEPTH		2

#define	IMG_CI_SIZE			IMG_CI_WIDTH * IMG_CI_HEIGHT * IMT_CI_DEPTH
#define	IMG_MENU_SIZE		IMG_MENU_WIDTH * IMG_MENU_HEIGHT * IMT_MENU_DEPTH

extern const unsigned char IMG_CSS_CI[];
extern const unsigned char IMG_barcode_click[];
extern const unsigned char IMG_barcode_normal[];
extern const unsigned char IMG_barcode_normal[];
extern const unsigned char IMG_EMV_click[];
extern const unsigned char IMG_EMV_normal[];
extern const unsigned char IMG_MSR_click[];
extern const unsigned char IMG_MSR_normal[];
extern const unsigned char IMG_Sign_click[];
extern const unsigned char IMG_Sign_normal[];
extern const unsigned char IMG_FND_click[];
extern const unsigned char IMG_FND_normal[];
extern const unsigned char IMG_DAC_click[];
extern const unsigned char IMG_DAC_normal[];
extern const unsigned char IMG_RTC_click[];
extern const unsigned char IMG_RTC_normal[];
extern const unsigned char IMG_LCD_click[];
extern const unsigned char IMG_LCD_normal[];
extern const unsigned char IMG_CreditNum_click[];
extern const unsigned char IMG_CreditNum_normal[];
extern const unsigned char IMG_LookBack_click[];
extern const unsigned char IMG_LookBack_normal[];


