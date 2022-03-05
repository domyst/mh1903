#ifndef _DECODELIB_H_
#define _DECODELIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mhscpu.h"

/************************************************************************************************/
/*ͼ��ת��������*/
typedef enum
{
    FLIP_NORMAL,   //ԭʼͼ��
    FLIP_VER,      //��ֱ��ת
    FLIP_HOZ,      //ˮƽ��ת
    FLIP_VERHOZ    //��ֱˮƽ��ת����ԭʼͼ����ת180��
}SensorImageFlipType;

/************************************************************************************************/
/*����CodeID*/
typedef enum
{
    ID_NONE = 0,      /*δ�⵽��*/
    ID_CODE128,       /*code128*/
    ID_CODE39,        /*code39*/
    ID_CODE93,        /*code93*/
    ID_EAN13,         /*EAN 13*/
    ID_EAN8,          /*EAN 8*/
    ID_UPC_A,         /*UPC_A*/
    ID_UPC_E0,        /*UPC_E0*/
    ID_UPC_E1,        /*UPC_E1*/
    ID_ISBN13,        /*ISBN13*/
    ID_ITF25,         /*���������*/
    ID_MTX25,         /*Matrix 2 of 5*/
    ID_IDL25,         /*Industrial 25*/
    ID_CODE11,        /*Code11*/
    ID_CODABAR,       /*Codabar*/
    ID_RSS14,         /*RSS-14*/
    ID_RSSLIM,        /*RSS Limited*/
    ID_RSSEXP,        /*RSS Expanded*/
    ID_MSIPLESSEY,    /*MSI Plessey*/
    ID_STD25,         /*Standard 25*/
    ID_PLESSEY,       /*Plessey*/
    ID_CHINAPOST25,   /*China Post 25*/
    ID_CODE16K,       /*Code16K*/
    ID_GS1Composite,  /*GS1 Composite*/
    ID_QRCODE,        /*QRCode*/
    ID_PDF417,        /*PDF417*/
    ID_DATAMATRIX,    /*DataMatrix*/
    ID_HANXIN,        /*Han Xin*/
    ID_MICPDF417,     /*Micro PDF417*/
    ID_MICQRCODE,     /*Micro QRCode*/
    ID_MAXICODE,      /*MaxiCode*/
    ID_AZTEC,         /*Aztec*/
}DecodeIDType;
/*Code128 FNC3 ��־��¼*/
typedef struct
{
    bool status;       /*Code128 fnc3״̬ true��FNC3��־ false��FNC3��־*/
    uint8_t location;  /*Code128 fnc3λ��*/
}Code128FNC3TypeDef;
/*�������ݽṹ��*/
typedef struct
{
    DecodeIDType id;             /*������id*/
    uint8_t AIM[4];              /*AIM ID*/
    Code128FNC3TypeDef c128fnc3; /*��¼Code128 FNC3��־��λ��*/
    uint8_t *result;             /*��������������׵�ַ*/
    uint32_t resn;               /*���ؽ���������*/
    uint32_t maxn;               /*���������������*/
}DecodeResultTypeDef;
/************************************************************************************************/
/*�����ʼ�����ýṹ��*/
typedef struct
{
    void            *pool;                 /*�ڴ���׵�ַ */
    unsigned int    size;                  /*�ڴ�ش�С   ���ڵ��� 410 * 1024kbyte,ջ�ռ���ڵ���4k */
    GPIO_TypeDef*   CAM_PWDN_GPIOx;        /*camera PWDN gpio */
    uint16_t        CAM_PWDN_GPIO_Pin;     /*camera PWDN gpio pin */
    GPIO_TypeDef*   CAM_RST_GPIOx;         /*camera RESET gpio */
    uint16_t        CAM_RST_GPIO_Pin;      /*camera RESET gpio pin */
    I2C_TypeDef*    CAM_I2Cx;              /*��������ͷʹ�õ�I2C */
    uint32_t        CAM_I2CClockSpeed;     /*I2Cʱ���������ã����ʷ�Χ[100000, 400000]*/
    uint8_t       (*SensorConfig)[2];      /*�Զ�������ͷ���������׵�ַ��ΪNULLʱʹ��Ĭ�����ò���*/
    uint32_t        SensorCfgSize;         /*�Զ�������ͷ���������С*/ 
}DecodeInitTypeDef;

/*������ʼ������״̬��Ϣ*/
typedef enum
{
    DecodeInitSuccess,             /*������ʼ���ɹ�*/            
    DecodeInitCheckError,          /*�����У��ʧ�ܣ�����ϵоƬ����*/
    DecodeInitMemoryError,         /*��������buff��С*/
    DecodeInitSensorError,         /*����ͷ��ʼ��ʧ��*/
}DecodeFlagTypeDef;

/*DeocdeStart ���󷵻�ֵ״̬��Ϣ*/
enum
{
    DecodeResOverflowError = -1,   /*����������buff��С����buff��СС��256byte*/            
    DecodeImageNoDoneError = -2,   /*ͼƬδ�ɼ����*/
    DecodeLibNoInitError   = -3,   /*�����δ��ʼ��*/
};
/*�������ýṹ��*/
typedef struct
{
    /*һά������*/
    uint32_t cfgCODE128;            /*code 128��������      */
    uint32_t cfgCODE39;             /*code 39��������       */
    uint32_t cfgCODE93;             /*code 93��������       */
    uint32_t cfgEAN13;              /*EAN13 ��������        */
    uint32_t cfgEAN8;               /*EAN8 ��������         */
    uint32_t cfgUPC_A;              /*UPC_A ��������        */
    uint32_t cfgUPC_E0;             /*UPC_E0 ��������(UPC_Eǰ����Ϊ0)*/
    uint32_t cfgUPC_E1;             /*UPC_E1 ��������(UPC_Eǰ����Ϊ1)*/
    uint32_t cfgISBN13;             /*ISBN13 ��������       */
    uint32_t cfgInterleaved2of5;    /*����������������    */
    uint32_t cfgMatrix2of5;         /*MATRIX 2 OF 5 ��������*/
    uint32_t cfgIndustrial25;       /*INDUSTRIAL 25 ��������*/
    uint32_t cfgCode11;             /*CODE11 ��������       */
    uint32_t cfgCodabar;            /*Codabar ��������      */
    uint32_t cfgRSS14;              /*RSS 14 ��������       */
    uint32_t cfgRSSLimited;         /*RSS Limited ��������  */
    uint32_t cfgRSSExpanded;        /*RSS Expanded �������� */
    uint32_t cfgMSIPlessey;         /*MSI Plessey ��������  */
    uint32_t cfgStandard2of5;       /*Standard 2of5 ��������*/
    uint32_t cfgPlessey;            /*Plessey ��������      */
    uint32_t cfgChinaPost25;        /*China Post ��������   */
    uint32_t cfgCode16K;            /*Code16K ��������      */
    /*����������*/
    uint32_t cfgGS1Composite;       /*GS1 Compoiste ��������*/ 
    /*��ά������*/
    uint32_t cfgQRCode;             /*qr code ��������      */
    uint32_t cfgPDF417;             /*pdf 417 ��������      */
    uint32_t cfgDataMatrix;         /*data matrix ��������  */
    uint32_t cfgHanXin;             /*���� ��������         */
    uint32_t cfgMicroPDF417;        /*MicroPDF417 ��������  */
    uint32_t cfgMicroQRCode;        /*MicroQRCode ��������  */
    uint32_t cfgMaxiCode;           /*MaxiCode ��������     */
    uint32_t cfgAztec;              /*Aztec ��������        */
    /*ȫ�ֽ�������*/
    uint32_t cfgGlobal;             /*����ȫ������*/
}DecodeConfigTypeDef;

/*����״̬*/
typedef enum
{
    DECODE_DISABLE = 0,               /*ʧ������*/
    DECODE_ENABLE = !DECODE_DISABLE    /*ʹ�ܵ�ǰ����*/
}DecodeStatus_t;

/*�������״̬����*/
typedef enum
{
    DE_STATUS  = 0,             /*0bit ����״̬λ ����һά�룬��ά�룬��λ����ʹ����ʧ��*/
    //ͨ������ �󲿷���֧��
    DE_EMIT_CHECK = 1,          /*1bit ʹ���������У��λ*/
    DE_ADD_CHECK = 2,           /*2bit ʹ������У��      */
    //code128����
    DE_CODE128_ENHANCE = 7,     /*7bit ʹ��code128ʶ����ǿ����Ҫ����code128���ձȲ���׼����*/
    //code39����
    DE_ASCII         = 3,       /*3bit ʹ������full ASCIIģʽ��� (code39)*/
    DE_CODE32        = 4,       /*4bit ʹ��Code39תCode32*/
    DE_CODE32_PREFIX = 5,       /*5bit Code32ǰ׺'A'����*/
    DE_EMIT_START    = 6,       /*6bit ʹ��code39��ʼ�����('*')*/
    DE_EMIT_END      = 7,       /*7bit ʹ��code39���������('*')*/
    //codabar����
    DE_CODABAR_MOD16 = 2,       /*2bit Codabarģ16У��*/
    DE_CODABAR_MOD10 = 3,       /*3bit Codabarģ10У��*/
    DE_CODABAR_ABCD  = 7,       /*7bit Codabar�����ʼ������ABCD*/
    //Code11����
    DE_CODE11_CHECKC = 2,       /*2bit Code11 CУ�飨1��У��λ��*/
    DE_CODE11_CHECKK = 3,       /*3bit Code11 KУ�� ��1��У��λ��*/
    DE_CODE11_CHECKCK = 4,      /*4bit Code11 C+KУ�� ��2��У��Ϊ����������ʹ�ô�У�飩*/
    //MSI Plessey ���� �����뿪��һ��У�飬���������
    DE_MSIPLS_MOD10  = 2,       /*2bit MSI Plessey ģ10У��*/
    DE_MSIPLS_MOD1010 = 3,      /*3bit MSI Plessey ˫ģ10У��*/
    //RSS����
    DE_RSS_AI_NO_BRACKETS = 7,  /*7bit AI��Ŀ��ʶ���Ƿ�����ţ�����Ϊ0�����ţ�*/
    //EAN UPC
    DE_UPC2EAN   = 4,           /*4bit UPC_AתEAN13���ʹ��(UPC_A)*/
    DE_MUSTADDN  = 5,           /*5bit ����������������������� (EAN13,UPC_A,EAN8,UPC_E0,UPC_E1)*/
    DE_EANADDON2 = 6,           /*6bit ���2λ������ (EAN13,UPC_A,EAN8,UPC_E0,UPC_E1)*/
    DE_EANADDON5 = 7,           /*7bit ���5λ������ (EAN13,UPC_A,EAN8,UPC_E0,UPC_E1)*/
    
    DE_MIN_LEN   = 16,          /*16bit-23bit ��8bit ������С����*/
    DE_MAX_LEN   = 24,          /*24bit-31bit ��8bit ���������*/  

    //QR�����״̬λ����
    DEQR_MISSINGCORNER = 1,     /*1bit QRCodeȱ���㷨ʹ��*/
    DEQR_BLUR          = 2,     /*2bit QRCode�������㷨ʹ��*/
    DEQR_CURVE         = 3,     /*3bit QRCode��������㷨ʹ��*/
    DEQR_MODE1         = 5,     /*5bit QRCodeģʽ1ʹ��*/
    DEQR_ECICONFIG     = 7,     /*7bit QRCode���ECI����*/

    //DM ״̬����
    DEDM_MULTIPLE      = 1,     /*1bit DataMatrix����ʹ��*/
    DEDM_ECICONFIG     = 7,     /*7bit DataMatrix���ECI����*/
    
    //PDF417����
    DEPDF417_ECICONFIG  = 7,     /*7bit PDF417���ECI����*/
    
    //MicroPDF417 ����
    DEMICPDF417_ECICONFIG = 7,   /*7bit MicroPDF417���ECI����*/
    
    //MaxiCode ����
    DEMAXICODE_ECICONFIG  = 7,   /*7bit MaxiCode ���ECI����*/
    
    //���� ����
    DEHANXIN_ECICONFIG = 7,      /*7bit �������ECI����*/
    
    //Aztec ����
    DEAZTEC_ECICONFIG = 7,       /*7bit Aztec ���ECI����*/
}DeBarCodeStatus_t;

/*�������ȫ������*/
typedef enum
{
    DEBAR_PRECISION   = 0,     /*0bit ���뾫�����������λ*/
    DECODE_ANTICOLOR  = 1,     /*1bit ֧�ַ�ɫ���������λ*/
    DEBAR_DAMAGE      = 2,     /*2bit ���������߽�������λ*/
    DECODE_MIRROR     = 3,     /*3-4bit ���루DM��������������0/1Ϊ�������룬2Ϊ�����룬3Ϊ�����붼���Խ�*/
    DEBAR_DISABLE     = 24,    /*24bit ȫ������ʧ��*/
    DE_SETUP_C128     = 31,    /*31bit Code128 8λ������ʹ��*/
}DecodeGlobalStatus_t; 
/****************************************************************************************************/
/**
 * �����ýṹ���ʼ������
 *
 */
void DecodeConfigInit(DecodeConfigTypeDef *cfg);
/**
 * BCTC��������Ƽ����ýṹ���ʼ������
 *
 */
void BCTCTestConfigInit(DecodeConfigTypeDef *cfg);
/****************************************************************************************************/
/**
 *�����ʼ�����ڴ�س�ʼ��
 * ��ʼ���ṹ��
 *����ֵΪ 1 �����ʼ���ɹ���0 �����ʼ��ʧ��
 */
DecodeFlagTypeDef DecodeInit(DecodeInitTypeDef* DecodeInitStruct);

/**
 *  ��ʼ���뺯��
 *  ����ֵΪ����������
 *  ���� 0 ����ʧ��
 *       DecodeResOverflowError �������������������buffer��Χ�����߸�������buffС��256byte
 *       DecodeImageNoDoneError ��ͼδ���
 *       DecodeLibNoInitError δ��ʼ�����߳�ʼ��ʧ��
 */
int DecodeStart(DecodeConfigTypeDef *cfg, DecodeResultTypeDef *res);
    
/**
 * ʹ��DCMI��ͼ
 *
 *
 */
void DecodeDcmiStart(void);

/**
 * �жϲ�ͼ�Ƿ���� 
 * ����������жϵ��ô˺���(DCMI_CallBackFrame())
 * ����ֵ 0δ�ɼ���� 1 �ɼ���ɣ��Ǳ�����ú���
 */
int DecodeDcmiFinish(void);

/**
 * ���ȫ������buff��־�����õ�ǰ����ͼ��buff����ʧЧ
 *
 */
void CleanDecodeBuffFlag(void);

/**
 * ��ȡ��ǰ����buff�Ƿ��ǿ���״̬
 * ����FALSE��ʾ��ǰ����ͼ��buff��Ϊ��Ч״̬����Ҫ���²�ͼ
 * ����TRUE��ʾ��ǰͼ��buff������һ��buff������Ч״̬
 */
Boolean GetDecodeBuffFlag(void);

/**
 * DCMI_IT_FRAME �жϻص�����
 */
void DCMI_CallBackFrame(void);

/**
 * ͼ��������
 * TRUE ����ͷ��ͼΪ����ͼƬ FALSE ����ͷ�������ͼ��
 */
void SensorImageMirror(Boolean status);

/**
 * ����ͼ��ת
 * 
 */
void SetSensorImageFlip(SensorImageFlipType fliptype);

/**
 * AEC���ں���
 *
 */
bool SensorAECRun(void);

/**
 * �ͷŽ�������Ҫ����Ӳ����Դ
 * 
 */
void CloseDecode(void);
     
/**
  * ��ȡ��ǰ�㷨�汾��
  * 16-31bitΪ���汾�ţ�8-15bitΪ�ΰ汾�ţ�0-7bitΪ�����汾��
  */
uint32_t GetDecodeLibVerison(void);

unsigned char * GetImageBuffAddr(void);

#ifdef __cplusplus
}
#endif

#endif 
