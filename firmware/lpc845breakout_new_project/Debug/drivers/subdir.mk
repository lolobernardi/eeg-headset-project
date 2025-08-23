################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_common_arm.c \
../drivers/fsl_gpio.c \
../drivers/fsl_inputmux.c \
../drivers/fsl_power.c \
../drivers/fsl_reset.c \
../drivers/fsl_swm.c \
../drivers/fsl_syscon.c \
../drivers/fsl_usart.c 

C_DEPS += \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_common_arm.d \
./drivers/fsl_gpio.d \
./drivers/fsl_inputmux.d \
./drivers/fsl_power.d \
./drivers/fsl_reset.d \
./drivers/fsl_swm.d \
./drivers/fsl_syscon.d \
./drivers/fsl_usart.d 

OBJS += \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_common_arm.o \
./drivers/fsl_gpio.o \
./drivers/fsl_inputmux.o \
./drivers/fsl_power.o \
./drivers/fsl_reset.o \
./drivers/fsl_swm.o \
./drivers/fsl_syscon.o \
./drivers/fsl_usart.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DMCUXPRESSO_SDK -DCPU_LPC845M301JBD64 -DSDK_DEBUGCONSOLE=1 -DMCUX_META_BUILD -DCPU_LPC845M301JBD48 -DCPU_LPC845M301JBD48_cm0plus -DCR_INTEGER_PRINTF -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_new_project\source" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_new_project\drivers" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_new_project\CMSIS" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_new_project\CMSIS\m-profile" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_new_project\device" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_new_project\device\periph2" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_new_project\utilities" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_new_project\utilities\str" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_new_project\utilities\debug_console_lite" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_new_project\component\uart" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_new_project\board" -O0 -fno-common -g3 -gdwarf-4 -c -ffunction-sections -fdata-sections -fno-builtin -imacros "C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_new_project\source\mcux_config.h" -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-drivers

clean-drivers:
	-$(RM) ./drivers/fsl_clock.d ./drivers/fsl_clock.o ./drivers/fsl_common.d ./drivers/fsl_common.o ./drivers/fsl_common_arm.d ./drivers/fsl_common_arm.o ./drivers/fsl_gpio.d ./drivers/fsl_gpio.o ./drivers/fsl_inputmux.d ./drivers/fsl_inputmux.o ./drivers/fsl_power.d ./drivers/fsl_power.o ./drivers/fsl_reset.d ./drivers/fsl_reset.o ./drivers/fsl_swm.d ./drivers/fsl_swm.o ./drivers/fsl_syscon.d ./drivers/fsl_syscon.o ./drivers/fsl_usart.d ./drivers/fsl_usart.o

.PHONY: clean-drivers

