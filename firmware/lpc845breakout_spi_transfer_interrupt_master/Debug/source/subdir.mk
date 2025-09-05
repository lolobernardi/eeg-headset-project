################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/semihost_hardfault.c \
../source/spi_transfer_interrupt_master.c 

C_DEPS += \
./source/semihost_hardfault.d \
./source/spi_transfer_interrupt_master.d 

OBJS += \
./source/semihost_hardfault.o \
./source/spi_transfer_interrupt_master.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DMCUXPRESSO_SDK -DCPU_LPC845M301JBD64 -DSDK_DEBUGCONSOLE=1 -DMCUX_META_BUILD -DCPU_LPC845M301JBD48 -DCPU_LPC845M301JBD48_cm0plus -DCR_INTEGER_PRINTF -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_spi_transfer_interrupt_master\source" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_spi_transfer_interrupt_master\drivers" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_spi_transfer_interrupt_master\CMSIS" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_spi_transfer_interrupt_master\CMSIS\m-profile" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_spi_transfer_interrupt_master\device" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_spi_transfer_interrupt_master\device\periph2" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_spi_transfer_interrupt_master\utilities" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_spi_transfer_interrupt_master\utilities\str" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_spi_transfer_interrupt_master\utilities\debug_console_lite" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_spi_transfer_interrupt_master\component\uart" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_spi_transfer_interrupt_master\board" -O0 -fno-common -g3 -gdwarf-4 -c -ffunction-sections -fdata-sections -fno-builtin -imacros "C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_spi_transfer_interrupt_master\source\mcux_config.h" -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/semihost_hardfault.d ./source/semihost_hardfault.o ./source/spi_transfer_interrupt_master.d ./source/spi_transfer_interrupt_master.o

.PHONY: clean-source

