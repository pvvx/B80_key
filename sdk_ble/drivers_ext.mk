################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += /drivers/driver_ext

OBJS += \
$(OUT_PATH)/drivers/driver_ext/ext_calibration.o \
$(OUT_PATH)/drivers/driver_ext/ext_misc.o \
$(OUT_PATH)/drivers/driver_ext/rf_pa.o \
$(OUT_PATH)/drivers/driver_ext/rf_private_pa.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/drivers/driver_ext/%.o: $(TEL_PATH)/drivers/driver_ext/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

$(OUT_PATH)/drivers/driver_ext/%.o: $(TEL_PATH)/drivers/driver_ext/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(BOOT_FLAG) $(INCLUDE_PATHS) -c -o"$@" "$<"
