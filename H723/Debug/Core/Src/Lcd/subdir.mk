################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Lcd/lcd_io_spi_hal.c \
../Core/Src/Lcd/st7789.c \
../Core/Src/Lcd/stm32_adafruit_lcd.c 

OBJS += \
./Core/Src/Lcd/lcd_io_spi_hal.o \
./Core/Src/Lcd/st7789.o \
./Core/Src/Lcd/stm32_adafruit_lcd.o 

C_DEPS += \
./Core/Src/Lcd/lcd_io_spi_hal.d \
./Core/Src/Lcd/st7789.d \
./Core/Src/Lcd/stm32_adafruit_lcd.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Lcd/%.o Core/Src/Lcd/%.su Core/Src/Lcd/%.cyclo: ../Core/Src/Lcd/%.c Core/Src/Lcd/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_NUCLEO_64 -DUSE_HAL_DRIVER -DSTM32H723xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/BSP/STM32H7xx_Nucleo -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Lcd

clean-Core-2f-Src-2f-Lcd:
	-$(RM) ./Core/Src/Lcd/lcd_io_spi_hal.cyclo ./Core/Src/Lcd/lcd_io_spi_hal.d ./Core/Src/Lcd/lcd_io_spi_hal.o ./Core/Src/Lcd/lcd_io_spi_hal.su ./Core/Src/Lcd/st7789.cyclo ./Core/Src/Lcd/st7789.d ./Core/Src/Lcd/st7789.o ./Core/Src/Lcd/st7789.su ./Core/Src/Lcd/stm32_adafruit_lcd.cyclo ./Core/Src/Lcd/stm32_adafruit_lcd.d ./Core/Src/Lcd/stm32_adafruit_lcd.o ./Core/Src/Lcd/stm32_adafruit_lcd.su

.PHONY: clean-Core-2f-Src-2f-Lcd

