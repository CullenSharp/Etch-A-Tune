/**
 * @file stm32h7xx_nucleo.h
 * @author Cullen Sharp
 * @brief Header for stm32h7xx_nucleo.c
 */

/* Define to prevent recursive inclusion */
#ifndef BSP_STM32H7XX_NUCLEO_STM32H7XX_NUCLEO_H_
#define BSP_STM32H7XX_NUCLEO_STM32H7XX_NUCLEO_H_

#ifdef __cplusplus
	extern "C" {
#endif

/**
 *  @addtogroup BSP
 *  @{
 */

/** @defgroup STM32G0XX_NUCLEO STM32G0XX-NUCLEO
  * @{
  */

/* Includes */
#include "stm32h7xx_hal.h"

/** @defgroup STM32H7XX_NUCLEO_Exported_Types Exported Types
 *  @{
 */

/**
 *  @}
 */

/** @defgroup STM32H7XX_NUCLEO_Exported_Constants Exported Constants
 *  @{
 */

/**
 * @brief Define for STM32H7XX_NUCLEO board
 */
#if !defined (USE_STM32H7XX_NUCLEO)
	#define USE_STM32H7XX_NUCLEO
#endif

/**
 * @defgroup STM32H7XX_NUCLEO_BUS BUS Constants
 * @{
 */

/*###################### SPI1 ###################################*/
#define NUCLEO_SPIx                                 SPI1
#define NUCLEO_SPIx_CLK_ENABLE()                  __HAL_RCC_SPI1_CLK_ENABLE()

#define NUCLEO_SPIx_SCK_AF                          GPIO_AF5_SPI1
#define NUCLEO_SPIx_SCK_GPIO_PORT                   GPIOA
#define NUCLEO_SPIx_SCK_PIN                         GPIO_PIN_5
#define NUCLEO_SPIx_SCK_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define NUCLEO_SPIx_SCK_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOA_CLK_DISABLE()

#define NUCLEO_SPIx_MISO_MOSI_AF                    GPIO_AF5_SPI1
#define NUCLEO_SPIx_MISO_MOSI_GPIO_PORT             GPIOA
#define NUCLEO_SPIx_MISO_MOSI_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define NUCLEO_SPIx_MISO_MOSI_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOA_CLK_DISABLE()
#define NUCLEO_SPIx_MISO_PIN                        GPIO_PIN_6
#define NUCLEO_SPIx_MOSI_PIN                        GPIO_PIN_7
/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define NUCLEO_SPIx_TIMEOUT_MAX                   1000

/**
  * @brief  LCD Control Lines management
  */

/* TODO: These need to be reassigned
#define LCD_CS_LOW()      HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_RESET)
#define LCD_CS_HIGH()     HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET)
#define LCD_DC_LOW()      HAL_GPIO_WritePin(LCD_DC_GPIO_PORT, LCD_DC_PIN, GPIO_PIN_RESET)
#define LCD_DC_HIGH()     HAL_GPIO_WritePin(LCD_DC_GPIO_PORT, LCD_DC_PIN, GPIO_PIN_SET)
*/

/**
  * @brief  SD Control Interface pins (shield D4)
  */
/* TODO: These need to be reassigned
#define SD_CS_PIN                                 GPIO_PIN_5
#define SD_CS_GPIO_PORT                           GPIOB
#define SD_CS_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOB_CLK_ENABLE()
#define SD_CS_GPIO_CLK_DISABLE()                __HAL_RCC_GPIOB_CLK_DISABLE()
*/

/**
  * @brief  LCD Control Interface pins (shield D10)
  */
/* TODO: These need to be reassigned
#define LCD_CS_PIN                                 GPIO_PIN_0
#define LCD_CS_GPIO_PORT                           GPIOB
#define LCD_CS_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOB_CLK_ENABLE()
#define LCD_CS_GPIO_CLK_DISABLE()                __HAL_RCC_GPIOB_CLK_DISABLE()
*/

/**
  * @brief  LCD Data/Command Interface pins
  */
/* TODO: These need to be reassigned
#define LCD_DC_PIN                                 GPIO_PIN_9
#define LCD_DC_GPIO_PORT                           GPIOA
#define LCD_DC_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOA_CLK_ENABLE()
#define LCD_DC_GPIO_CLK_DISABLE()                __HAL_RCC_GPIOA_CLK_DISABLE()
*/

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* BSP_STM32H7XX_NUCLEO_STM32H7XX_NUCLEO_H_ */

/******END OF FILE****/
