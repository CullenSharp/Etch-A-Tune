/**
 * @file stm32h7xx_nucleo.c
 * @author Cullen Sharp
  * @brief   This file provides set of firmware functions to manage:
  *          - LCD and microSD available on the Hosyond 4.0inch SPI Module (SKU MSP4021)
 */

/* Includes  */
#include "stm32h7xx_nucleo.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32H7XX_NUCLEO
  * @{
  */

/** @defgroup STM32H7XX_NUCLEO_Private_Defines Private Defines
  * @{
  */

/**
  * @brief STM32G0XX NUCLEO BSP Driver version number
  */
#define __STM32H7XX_NUCLEO_BSP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define __STM32H7XX_NUCLEO_BSP_VERSION_SUB1   (0x01U) /*!< [23:16] sub1 version */
#define __STM32H7XX_NUCLEO_BSP_VERSION_SUB2   (0x02U) /*!< [15:8]  sub2 version */
#define __STM32H7XX_NUCLEO_BSP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */
#define __STM32H7XX_NUCLEO_BSP_VERSION        ((__STM32H7XX_NUCLEO_BSP_VERSION_MAIN << 24)\
                                              |(__STM32H7XX_NUCLEO_BSP_VERSION_SUB1 << 16)\
                                              |(__STM32H7XX_NUCLEO_BSP_VERSION_SUB2 << 8 )\
                                              |(__STM32H7XX_NUCLEO_BSP_VERSION_RC))

/**
  * @brief LINK SD Card
  */
#define SD_DUMMY_BYTE            0xFF
#define SD_NO_RESPONSE_EXPECTED  0x80

/**
  * @}
  */

/** @defgroup STM32H7XX_NUCLEO_Private_Variables Private Variables
  * @{
  */

/**
 * @brief BUS variables
 */

// TODO: This is where the hspi handle will go later


/**
  * @}
  */

/** @defgroup STM32H7XX_NUCLEO_Private_Functions Private Functions
  * @{
  */

// TODO: Add private functions

/**
  * @}
  */
/** @defgroup STM32H7XX_NUCLEO_Exported_Functions Exported Functions
  * @{
  */

/** @addtogroup STM32H7XX_NUCLEO_generic_functions
  * @{
  */
/**
  * @brief  This method returns the STM32H7XX NUCLEO BSP Driver revision
  * @retval version : 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t BSP_GetVersion(void)
{
  return __STM32H7XX_NUCLEO_BSP_VERSION;
}
/**
  * @}
  */

/**
  * @}
  */
