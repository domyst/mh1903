
#ifndef TEST_EMV_DEBUG
    #define TEST_EMV_DEBUG

    #ifdef __cplusplus
    extern "C"
    {
    #endif

    #include <stdint.h>


    #define VERSION_410                 (0x410)
    #define VERSION_43A                 (0x43A)

    #define EMV_VERSION                 VERSION_43A
    
    
    #define SCI_DBG
    
    #ifdef SCI_DBG
        #define DBG_PRINT(format, args...)\
                printf(format, ##args)
                
        #define DBG_PRINT_HEX(s, buf, len)\
                print_hex(s, buf, len)
    #else
        #define DBG_PRINT(fmt, args...)
        #define DBG_PRINT_HEX(s, buf, len)
    #endif
    
    void loop_back(uint8_t u8Slot);
    void print_hex(char *s, uint8_t  *buf, int len);
    void delay_Ms(uint32_t u32Us);
    void wait_Ns(uint8_t u8N);
    





    #ifdef __cplusplus
    }
    #endif

#endif

