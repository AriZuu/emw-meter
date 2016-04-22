/*
 * Copyright (c) 2015-2016, Ari Suutari <ari@stonepile.fi>.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <picoos.h>
#include <wiced-driver.h>

void portSystemInit()
{
  wdSystemInit();
}

void Default_Handler(void);

void Default_Handler()
{
  __disable_irq();
  while (1)
    ;
}

extern unsigned int __stack[];

PORT_WEAK_HANDLER(SysTick_Handler);
PORT_WEAK_HANDLER(WWDG_irq);
PORT_WEAK_HANDLER(PVD_irq);
PORT_WEAK_HANDLER(TAMP_STAMP_irq);
PORT_WEAK_HANDLER(RTC_WKUP_IRQHandler);
PORT_WEAK_HANDLER(FLASH_irq);
PORT_WEAK_HANDLER(RCC_irq);
PORT_WEAK_HANDLER(EXTI0_irq);
PORT_WEAK_HANDLER(EXTI1_irq);
PORT_WEAK_HANDLER(EXTI2_irq);
PORT_WEAK_HANDLER(EXTI3_irq);
PORT_WEAK_HANDLER(EXTI4_irq);
PORT_WEAK_HANDLER(DMA1_Stream0_irq);
PORT_WEAK_HANDLER(DMA1_Stream1_irq);
PORT_WEAK_HANDLER(DMA1_Stream2_irq);
PORT_WEAK_HANDLER(DMA1_Stream3_irq);
PORT_WEAK_HANDLER(DMA1_Stream4_irq);
PORT_WEAK_HANDLER(DMA1_Stream5_IRQHandler);
PORT_WEAK_HANDLER(DMA1_Stream6_IRQHandler);
PORT_WEAK_HANDLER(ADC_irq);
PORT_WEAK_HANDLER(CAN1_TX_irq);
PORT_WEAK_HANDLER(CAN1_RX0_irq);
PORT_WEAK_HANDLER(CAN1_RX1_irq);
PORT_WEAK_HANDLER(CAN1_SCE_irq);
PORT_WEAK_HANDLER(EXTI9_5_irq);
PORT_WEAK_HANDLER(TIM1_BRK_TIM9_irq);
PORT_WEAK_HANDLER(TIM1_UP_TIM10_irq);
PORT_WEAK_HANDLER(TIM1_TRG_COM_TIM11_irq);
PORT_WEAK_HANDLER(TIM1_CC_irq);
PORT_WEAK_HANDLER(TIM2_irq);
PORT_WEAK_HANDLER(TIM3_irq);
PORT_WEAK_HANDLER(TIM4_irq);
PORT_WEAK_HANDLER(I2C1_EV_irq);
PORT_WEAK_HANDLER(I2C1_ER_irq);
PORT_WEAK_HANDLER(I2C2_EV_irq);
PORT_WEAK_HANDLER(I2C2_ER_irq);
PORT_WEAK_HANDLER(SPI1_irq);
PORT_WEAK_HANDLER(SPI2_irq);
PORT_WEAK_HANDLER(USART1_IRQHandler);
PORT_WEAK_HANDLER(USART2_IRQHandler);
PORT_WEAK_HANDLER(USART3_IRQHandler);
PORT_WEAK_HANDLER(EXTI15_10_irq);
PORT_WEAK_HANDLER(RTC_Alarm_irq);
PORT_WEAK_HANDLER(OTG_FS_WKUP_irq);
PORT_WEAK_HANDLER(TIM8_BRK_TIM12_irq);
PORT_WEAK_HANDLER(TIM8_UP_TIM13_irq);
PORT_WEAK_HANDLER(TIM8_TRG_COM_TIM14_irq);
PORT_WEAK_HANDLER(TIM8_CC_irq);
PORT_WEAK_HANDLER(DMA1_Stream7_irq);
PORT_WEAK_HANDLER(FSMC_irq);
PORT_WEAK_HANDLER(SDIO_irq);
PORT_WEAK_HANDLER(TIM5_irq);
PORT_WEAK_HANDLER(SPI3_irq);
PORT_WEAK_HANDLER(UART4_IRQHandler);
PORT_WEAK_HANDLER(UART5_IRQHandler);
PORT_WEAK_HANDLER(TIM6_DAC_irq);
PORT_WEAK_HANDLER(TIM7_irq);
PORT_WEAK_HANDLER(DMA2_Stream0_irq);
PORT_WEAK_HANDLER(DMA2_Stream1_irq);
PORT_WEAK_HANDLER(DMA2_Stream2_IRQHandler);
PORT_WEAK_HANDLER(DMA2_Stream3_irq);
PORT_WEAK_HANDLER(DMA2_Stream4_irq);
PORT_WEAK_HANDLER(ETH_irq);
PORT_WEAK_HANDLER(ETH_WKUP_irq);
PORT_WEAK_HANDLER(CAN2_TX_irq);
PORT_WEAK_HANDLER(CAN2_RX0_irq);
PORT_WEAK_HANDLER(CAN2_RX1_irq);
PORT_WEAK_HANDLER(CAN2_SCE_irq);
PORT_WEAK_HANDLER(OTG_FS_irq);
PORT_WEAK_HANDLER(DMA2_Stream5_irq);
PORT_WEAK_HANDLER(DMA2_Stream6_irq);
PORT_WEAK_HANDLER(DMA2_Stream7_IRQHandler);
PORT_WEAK_HANDLER(USART6_IRQHandler);
PORT_WEAK_HANDLER(I2C3_EV_irq);
PORT_WEAK_HANDLER(I2C3_ER_irq);
PORT_WEAK_HANDLER(OTG_HS_EP1_OUT_irq);
PORT_WEAK_HANDLER(OTG_HS_EP1_IN_irq);
PORT_WEAK_HANDLER(OTG_HS_WKUP_irq);
PORT_WEAK_HANDLER(OTG_HS_irq);
PORT_WEAK_HANDLER(DCMI_irq);
PORT_WEAK_HANDLER(CRYP_irq);
PORT_WEAK_HANDLER(HASH_RNG_irq);
PORT_WEAK_HANDLER(FPU_irq);

PortExcHandlerFunc vectorTable[] __attribute__ ((section(".vectors"))) =
{ (PortExcHandlerFunc) __stack,        // stack pointer
    Reset_Handler,                     // code entry point
    Reset_Handler,                     // NMI handler (not really)
    HardFault_Handler,                 // hard fault handler (let's hope not)
    Reset_Handler,                     // MemManage failt
    Reset_Handler,                     // Bus fault
    UsageFault_Handler,                // Usage fault
    0,                                 // Reserved
    0,                                 // Reserved
    0,                                 // Reserved
    0,                                 // Reserved
    SVC_Handler,                       // SVC
    Reset_Handler,                     // Debug monitor
    0,                                 // Reserved
    PendSV_Handler,                    // Context switch
    SysTick_Handler,
// STM32F2xx handlers
    WWDG_irq,                   // Window WatchDog
    PVD_irq,                    // PVD through EXTI Line detection
    TAMP_STAMP_irq,             // Tamper and TimeStamps through the EXTI line
    RTC_WKUP_IRQHandler,        // RTC Wakeup through the EXTI line
    FLASH_irq,                  // FLASH
    RCC_irq,                    // RCC
    EXTI0_irq,                  // EXTI Line0
    EXTI1_irq,                  // EXTI Line1
    EXTI2_irq,                  // EXTI Line2
    EXTI3_irq,                  // EXTI Line3
    EXTI4_irq,                  // EXTI Line4
    DMA1_Stream0_irq,           // DMA1 Stream 0
    DMA1_Stream1_irq,           // DMA1 Stream 1
    DMA1_Stream2_irq,           // DMA1 Stream 2
    DMA1_Stream3_irq,           // DMA1 Stream 3
    DMA1_Stream4_irq,           // DMA1 Stream 4
    DMA1_Stream5_IRQHandler,    // DMA1 Stream 5
    DMA1_Stream6_IRQHandler,    // DMA1 Stream 6
    ADC_irq,                    // ADC1, ADC2 and ADC3s
    CAN1_TX_irq,                // CAN1 TX
    CAN1_RX0_irq,               // CAN1 RX0
    CAN1_RX1_irq,               // CAN1 RX1
    CAN1_SCE_irq,               // CAN1 SCE
    EXTI9_5_irq,                // External Line[9:5]s
    TIM1_BRK_TIM9_irq,          // TIM1 Break and TIM9
    TIM1_UP_TIM10_irq,          // TIM1 Update and TIM10
    TIM1_TRG_COM_TIM11_irq,     // TIM1 Trigger and Commutation and TIM11
    TIM1_CC_irq,                // TIM1 Capture Compare
    TIM2_irq,                   // TIM2
    TIM3_irq,                   // TIM3
    TIM4_irq,                   // TIM4
    I2C1_EV_irq,                // I2C1 Event
    I2C1_ER_irq,                // I2C1 Error
    I2C2_EV_irq,                // I2C2 Event
    I2C2_ER_irq,                // I2C2 Error
    SPI1_irq,                   // SPI1
    SPI2_irq,                   // SPI2
    USART1_IRQHandler,          // USART1
    USART2_IRQHandler,          // USART2
    USART3_IRQHandler,          // USART3
    EXTI15_10_irq,              // External Line[15:10]s
    RTC_Alarm_irq,              // RTC Alarm (A and B) through EXTI Line
    OTG_FS_WKUP_irq,            // USB OTG FS Wakeup through EXTI line
    TIM8_BRK_TIM12_irq,         // TIM8 Break and TIM12
    TIM8_UP_TIM13_irq,          // TIM8 Update and TIM13
    TIM8_TRG_COM_TIM14_irq,     // TIM8 Trigger and Commutation and TIM14
    TIM8_CC_irq,                // TIM8 Capture Compare
    DMA1_Stream7_irq,           // DMA1 Stream7
    FSMC_irq,                   // FSMC
    SDIO_irq,                   // SDIO
    TIM5_irq,                   // TIM5
    SPI3_irq,                   // SPI3
    UART4_IRQHandler,           // UART4
    UART5_IRQHandler,           // UART5
    TIM6_DAC_irq,               // TIM6 and DAC1&2 underrun errors
    TIM7_irq,                   // TIM7
    DMA2_Stream0_irq,           // DMA2 Stream 0
    DMA2_Stream1_irq,           // DMA2 Stream 1
    DMA2_Stream2_IRQHandler,    // DMA2 Stream 2
    DMA2_Stream3_irq,           // DMA2 Stream 3
    DMA2_Stream4_irq,           // DMA2 Stream 4
    ETH_irq,                    // Ethernet
    ETH_WKUP_irq,               // Ethernet Wakeup through EXTI line
    CAN2_TX_irq,                // CAN2 TX
    CAN2_RX0_irq,               // CAN2 RX0
    CAN2_RX1_irq,               // CAN2 RX1
    CAN2_SCE_irq,               // CAN2 SCE
    OTG_FS_irq,                 // USB OTG FS
    DMA2_Stream5_irq,           // DMA2 Stream 5
    DMA2_Stream6_irq,           // DMA2 Stream 6
    DMA2_Stream7_IRQHandler,    // DMA2 Stream 7
    USART6_IRQHandler,          // USART6
    I2C3_EV_irq,                // I2C3 event
    I2C3_ER_irq,                // I2C3 error
    OTG_HS_EP1_OUT_irq,         // USB OTG HS End Point 1 Out
    OTG_HS_EP1_IN_irq,          // USB OTG HS End Point 1 In
    OTG_HS_WKUP_irq,            // USB OTG HS Wakeup through EXTI
    OTG_HS_irq,                 // USB OTG HS
    DCMI_irq,                   // DCMI
    CRYP_irq,                   // CRYP crypto
    HASH_RNG_irq,               // Hash and Rng
    FPU_irq
};
