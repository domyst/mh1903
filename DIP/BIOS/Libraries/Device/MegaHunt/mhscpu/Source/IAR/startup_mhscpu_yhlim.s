;/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
;* File Name          : startup_stm32f4xx.s
;* Author             : MCD Application Team
;* Version            : V1.0.2
;* Date               : 05-March-2012
;* Description        : STM32F4xx devices vector table for EWARM toolchain.
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == _iar_program_start,
;*                      - Set the vector table entries with the exceptions ISR 
;*                        address.
;*                      - Configure the system clock and the external SRAM mounted on 
;*                        STM324xG-EVAL board to be used as data memory (optional, 
;*                        to be enabled by user)
;*                      - Branches to main in the C library (which eventually
;*                        calls main()).
;*                      After Reset the Cortex-M4 processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;********************************************************************************
;* 
;* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
;* You may not use this file except in compliance with the License.
;* You may obtain a copy of the License at:
;* 
;*        http://www.st.com/software_license_agreement_liberty_v2
;* 
;* Unless required by applicable law or agreed to in writing, software 
;* distributed under the License is distributed on an "AS IS" BASIS, 
;* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;* See the License for the specific language governing permissions and
;* limitations under the License.
;* 
;*******************************************************************************/
;
;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler             ; Reset Handler

        DCD     NMI_Handler               ; NMI Handler
        DCD     HardFault_Handler         ; Hard Fault Handler
        DCD     MemManage_Handler         ; MPU Fault Handler
        DCD     BusFault_Handler          ; Bus Fault Handler
        DCD     UsageFault_Handler        ; Usage Fault Handler
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     SVC_Handler               ; SVCall Handler
        DCD     DebugMon_Handler          ; Debug Monitor Handler
        DCD     0                         ; Reserved
        DCD     PendSV_Handler            ; PendSV Handler
        DCD     SysTick_Handler           ; SysTick Handler

         ; External Interrupts
	DCD     DMA0_IRQHandler
	DCD     USB_IRQHandler
	DCD     USBDMA_IRQHandler
	DCD     LCD_IRQHandler
	DCD     SCI0_IRQHandler
	DCD     UART0_IRQHandler
	DCD     UART1_IRQHandler
	DCD     SPI0_IRQHandler
	DCD     CRYPT0_IRQHandler
	DCD     TIM0_0_IRQHandler
	DCD     TIM0_1_IRQHandler
	DCD     TIM0_2_IRQHandler
	DCD     TIM0_3_IRQHandler
	DCD     EXTI0_IRQHandler
	DCD     EXTI1_IRQHandler
	DCD     EXTI2_IRQHandler
	DCD     RTC_IRQHandler
	DCD     SENSOR_IRQHandler
	DCD     TRNG_IRQHandler
	DCD     ADC0_IRQHandler
	DCD     SSC_IRQHandler
	DCD     TIM0_4_IRQHandler
	DCD     TIM0_5_IRQHandler
	DCD     KEYBOARD_IRQHandler
	DCD     MSR_IRQHandler
	DCD     EXTI3_IRQHandler
	DCD     SPI1_IRQHandler
	DCD     SPI2_IRQHandler
	DCD     SCI1_IRQHandler
	DCD     SCI2_IRQHandler
	DCD     SPI3_IRQHandler
	DCD     SPI4_IRQHandler
	DCD     UART2_IRQHandler
	DCD     UART3_IRQHandler
	DCD     0
	DCD     QSPI_IRQHandler
	DCD     I2C0_IRQHandler
	DCD     EXTI4_IRQHandler
	DCD     EXTI5_IRQHandler
	DCD     TIM0_6_IRQHandler
	DCD     TIM0_7_IRQHandler
	DCD     CSI2_IRQHandler
	DCD     DCMI_IRQHandler
	DCD     EXTI6_IRQHandler
	DCD     EXTI7_IRQHandler
	DCD     SDIOM_IRQHandler
	DCD     QR_IRQHandler
	DCD     GPU_IRQHandler
	DCD     0
	DCD     0	
	DCD     DAC_IRQHandler
__Vectors_End
__Vectors       EQU   __vector_table
__Vectors_Size  EQU   __Vectors_End - __Vectors

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB
        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
__Reset_Handler
Reset_Handler
        LDR     R0, =__Vectors
        LDR     R1, =0xE000ED08
        STR     R0, [R1]
        LDR     R0, [R0]
        MOV     SP, R0

        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0
/********************************************************/

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK DMA0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA0_IRQHandler  
        B DMA0_IRQHandler

        PUBWEAK USB_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USB_IRQHandler  
        B USB_IRQHandler

        PUBWEAK USBDMA_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
USBDMA_IRQHandler  
        B USBDMA_IRQHandler

        PUBWEAK LCD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)  
LCD_IRQHandler  
        B LCD_IRQHandler

        PUBWEAK SCI0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SCI0_IRQHandler  
        B SCI0_IRQHandler

        PUBWEAK UART0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART0_IRQHandler  
        B UART0_IRQHandler

        PUBWEAK UART1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART1_IRQHandler  
        B UART1_IRQHandler

        PUBWEAK SPI0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI0_IRQHandler  
        B SPI0_IRQHandler

        PUBWEAK CRYPT0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CRYPT0_IRQHandler  
        B CRYPT0_IRQHandler

        PUBWEAK TIM0_0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM0_0_IRQHandler
        B TIM0_0_IRQHandler

        PUBWEAK TIM0_1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
TIM0_1_IRQHandler  
        B TIM0_1_IRQHandler

        PUBWEAK TIM0_2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
TIM0_2_IRQHandler  
        B TIM0_2_IRQHandler

        PUBWEAK TIM0_3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
TIM0_3_IRQHandler  
        B TIM0_3_IRQHandler

        PUBWEAK EXTI0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
EXTI0_IRQHandler  
        B EXTI0_IRQHandler

        PUBWEAK EXTI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
EXTI1_IRQHandler  
        B EXTI1_IRQHandler

        PUBWEAK EXTI2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
EXTI2_IRQHandler  
        B EXTI2_IRQHandler

        PUBWEAK RTC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
RTC_IRQHandler  
        B RTC_IRQHandler

        PUBWEAK SENSOR_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
SENSOR_IRQHandler  
        B SENSOR_IRQHandler

        PUBWEAK TRNG_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TRNG_IRQHandler  
        B TRNG_IRQHandler

        PUBWEAK ADC0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1) 
ADC0_IRQHandler  
        B ADC0_IRQHandler

        PUBWEAK SSC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)  
SSC_IRQHandler  
        B SSC_IRQHandler

        PUBWEAK TIM0_4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)  
TIM0_4_IRQHandler  
        B TIM0_4_IRQHandler

        PUBWEAK TIM0_5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)  
TIM0_5_IRQHandler  
        B TIM0_5_IRQHandler

        PUBWEAK KEYBOARD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1) 
KEYBOARD_IRQHandler  
        B KEYBOARD_IRQHandler

        PUBWEAK MSR_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
MSR_IRQHandler  
        B MSR_IRQHandler

        PUBWEAK EXTI3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
EXTI3_IRQHandler  
        B EXTI3_IRQHandler

        PUBWEAK SPI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
SPI1_IRQHandler  
        B SPI1_IRQHandler
        
        PUBWEAK SPI2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
SPI2_IRQHandler  
        B SPI2_IRQHandler

        PUBWEAK SCI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SCI1_IRQHandler  
        B SCI1_IRQHandler

        PUBWEAK SCI2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SCI2_IRQHandler  
        B SCI2_IRQHandler

        PUBWEAK SPI3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI3_IRQHandler  
        B SPI3_IRQHandler

        PUBWEAK SPI4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI4_IRQHandler  
        B SPI4_IRQHandler

        PUBWEAK UART2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1) 
UART2_IRQHandler  
        B UART2_IRQHandler

        PUBWEAK UART3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1) 
UART3_IRQHandler  
        B UART3_IRQHandler

        PUBWEAK QSPI_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1) 
QSPI_IRQHandler  
        B QSPI_IRQHandler

        PUBWEAK I2C0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1) 
I2C0_IRQHandler  
        B I2C0_IRQHandler

        PUBWEAK EXTI4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI4_IRQHandler  
        B EXTI4_IRQHandler

        PUBWEAK EXTI5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI5_IRQHandler  
        B EXTI5_IRQHandler

        PUBWEAK TIM0_6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM0_6_IRQHandler  
        B TIM0_6_IRQHandler

        PUBWEAK TIM0_7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM0_7_IRQHandler  
        B TIM0_7_IRQHandler

        PUBWEAK CSI2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CSI2_IRQHandler  
        B CSI2_IRQHandler

        PUBWEAK DCMI_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)   
DCMI_IRQHandler  
        B DCMI_IRQHandler

        PUBWEAK EXTI6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)   
EXTI6_IRQHandler  
        B EXTI6_IRQHandler

        PUBWEAK EXTI7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
EXTI7_IRQHandler  
        B EXTI7_IRQHandler
      
        PUBWEAK SDIOM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
SDIOM_IRQHandler  
        B SDIOM_IRQHandler

        PUBWEAK QR_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
QR_IRQHandler  
        B QR_IRQHandler

        PUBWEAK GPU_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)    
GPU_IRQHandler  
        B GPU_IRQHandler

        PUBWEAK DAC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1) 
DAC_IRQHandler  
        B DAC_IRQHandler

        END
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
