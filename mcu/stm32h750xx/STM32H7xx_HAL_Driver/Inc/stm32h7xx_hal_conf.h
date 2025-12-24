/**
  ******************************************************************************
  * @file    stm32h7xx_hal_conf.h
  * @brief   HAL configuration file for STM32H750.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32H7xx_HAL_CONF_H
#define STM32H7xx_HAL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* ########################## Module Selection ############################## */
#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_EXTI_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED

/* ########################## HSE/HSI Values adaptation ##################### */
#if !defined  (HSE_VALUE)
  #define HSE_VALUE    25000000U
#endif

#if !defined  (CSI_VALUE)
  #define CSI_VALUE    4000000U
#endif

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    64000000U
#endif

#if !defined  (LSI_VALUE)
  #define LSI_VALUE    32000U
#endif

#if !defined  (LSE_VALUE)
  #define LSE_VALUE    32768U
#endif

#if !defined  (EXTERNAL_CLOCK_VALUE)
  #define EXTERNAL_CLOCK_VALUE    12288000U
#endif

#define LSE_STARTUP_TIMEOUT    5000U
#define HSE_STARTUP_TIMEOUT    100U

/* ########################### System Configuration ######################### */
#define  VDD_VALUE                    3300U
#define  TICK_INT_PRIORITY            0x0FU
#define  USE_RTOS                     0U
#define  USE_SD_TRANSCEIVER           0U
#define  USE_HAL_SDRAM_REGISTER_CALLBACKS   0U
#define  USE_HAL_SDMMC_REGISTER_CALLBACKS   0U
#define  USE_HAL_MMC_REGISTER_CALLBACKS     0U
#define  USE_HAL_DSI_REGISTER_CALLBACKS     0U
#define  USE_HAL_JPEG_REGISTER_CALLBACKS    0U
#define  USE_HAL_LTDC_REGISTER_CALLBACKS    0U
#define  USE_HAL_RNG_REGISTER_CALLBACKS     0U
#define  USE_HAL_SPI_REGISTER_CALLBACKS     0U
#define  USE_HAL_FMPI2C_REGISTER_CALLBACKS  0U
#define  USE_HAL_I2C_REGISTER_CALLBACKS     0U
#define  USE_HAL_UART_REGISTER_CALLBACKS    0U
#define  USE_HAL_USART_REGISTER_CALLBACKS   0U
#define  USE_HAL_IRDA_REGISTER_CALLBACKS    0U
#define  USE_HAL_WWDG_REGISTER_CALLBACKS    0U
#define  USE_HAL_PCD_REGISTER_CALLBACKS     0U
#define  USE_HAL_HCD_REGISTER_CALLBACKS     0U
#define  USE_HAL_DFSDM_REGISTER_CALLBACKS   0U
#define  USE_HAL_CEC_REGISTER_CALLBACKS     0U
#define  USE_HAL_FDCAN_REGISTER_CALLBACKS   0U
#define  USE_HAL_SAI_REGISTER_CALLBACKS     0U
#define  USE_HAL_DAC_REGISTER_CALLBACKS     0U
#define  USE_HAL_MDIOS_REGISTER_CALLBACKS   0U
#define  USE_HAL_EXTI_REGISTER_CALLBACKS    0U

/* ########################## Assert Selection ############################## */
#define USE_FULL_ASSERT    0U

/* ################## Ethernet peripheral configuration ##################### */
#define ETH_TX_DESC_CNT         4
#define ETH_RX_DESC_CNT         4
#define ETH_MAC_ADDR0   2U
#define ETH_MAC_ADDR1   0U
#define ETH_MAC_ADDR2   0U
#define ETH_MAC_ADDR3   0U
#define ETH_MAC_ADDR4   0U
#define ETH_MAC_ADDR5   0U
#define ETH_RX_BUF_SIZE ETH_MAX_PACKET_SIZE
#define ETH_TX_BUF_SIZE ETH_MAX_PACKET_SIZE

#include "stm32h7xx_hal_def.h"

#if defined(HAL_RCC_MODULE_ENABLED)
  #include "stm32h7xx_hal_rcc.h"
  #include "stm32h7xx_hal_rcc_ex.h"
#endif
#if defined(HAL_GPIO_MODULE_ENABLED)
  #include "stm32h7xx_hal_gpio.h"
  #include "stm32h7xx_hal_gpio_ex.h"
#endif
#if defined(HAL_DMA_MODULE_ENABLED)
  #include "stm32h7xx_hal_dma.h"
  #include "stm32h7xx_hal_dma_ex.h"
#endif
#if defined(HAL_EXTI_MODULE_ENABLED)
  #include "stm32h7xx_hal_exti.h"
#endif
#if defined(HAL_CORTEX_MODULE_ENABLED)
  #include "stm32h7xx_hal_cortex.h"
#endif
#if defined(HAL_FLASH_MODULE_ENABLED)
  #include "stm32h7xx_hal_flash.h"
  #include "stm32h7xx_hal_flash_ex.h"
#endif
#if defined(HAL_PWR_MODULE_ENABLED)
  #include "stm32h7xx_hal_pwr.h"
  #include "stm32h7xx_hal_pwr_ex.h"
#endif
#if defined(HAL_UART_MODULE_ENABLED)
  #include "stm32h7xx_hal_uart.h"
  #include "stm32h7xx_hal_uart_ex.h"
#endif

/* ########################## Assert Definition ############################## */
#if (USE_FULL_ASSERT == 1U)
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

#ifdef __cplusplus
}
#endif

#endif /* STM32H7xx_HAL_CONF_H */
