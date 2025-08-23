################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utilities/fsl_assert.c 

C_DEPS += \
./utilities/fsl_assert.d 

OBJS += \
./utilities/fsl_assert.o 


# Each subdirectory must supply rules for building sources it contributes
utilities/%.o: ../utilities/%.c utilities/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DMCUXPRESSO_SDK -DCPU_LPC845M301JBD64 -DSDK_DEBUGCONSOLE=1 -DMCUX_META_BUILD -DCPU_LPC845M301JBD48 -DCPU_LPC845M301JBD48_cm0plus -DCR_INTEGER_PRINTF -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_ctimer_match_example\source" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_ctimer_match_example\drivers" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_ctimer_match_example\CMSIS" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_ctimer_match_example\CMSIS\m-profile" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_ctimer_match_example\device" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_ctimer_match_example\device\periph2" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_ctimer_match_example\utilities" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_ctimer_match_example\utilities\str" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_ctimer_match_example\utilities\debug_console_lite" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_ctimer_match_example\component\uart" -I"C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_ctimer_match_example\board" -O0 -fno-common -g3 -gdwarf-4 -c -ffunction-sections -fdata-sections -fno-builtin -imacros "C:\Users\Usuario\Desktop\ESCRITORIO\Facultad\Tesis\Proyecto_Casco_EEG\eeg-headset-project\firmware\lpc845breakout_ctimer_match_example\source\mcux_config.h" -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-utilities

clean-utilities:
	-$(RM) ./utilities/fsl_assert.d ./utilities/fsl_assert.o

.PHONY: clean-utilities

