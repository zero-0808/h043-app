################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/TAPD/proj/h043wqcm0301_raaa1_afa1/h043wqcm0301_raaa1_afa1/FATFS/App/fatfs.c 

OBJS += \
./Application/User/FATFS/App/fatfs.o 

C_DEPS += \
./Application/User/FATFS/App/fatfs.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/FATFS/App/fatfs.o: D:/TAPD/proj/h043wqcm0301_raaa1_afa1/h043wqcm0301_raaa1_afa1/FATFS/App/fatfs.c Application/User/FATFS/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../../Core/Inc -I"D:/TAPD/proj/h043wqcm0301_raaa1_afa1/h043wqcm0301_raaa1_afa1/STM32CubeIDE/Drivers/bsp_driver" -I../../Drivers/h043_driver -I../../FATFS/Target -I../../FATFS/App -I../../USB_HOST/App -I../../USB_HOST/Target -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Utilities/JPEG -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Middlewares/Third_Party/FatFs/src -I../../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Drivers/bsp_driver -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Application/User/FATFS/App/fatfs.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

