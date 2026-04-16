################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OUT_DIR += /drivers/flash

OBJS += \
$(OUT_PATH)/drivers/flash/flash_common.o \
$(OUT_PATH)/drivers/flash/flash_mid114485.o \
$(OUT_PATH)/drivers/flash/flash_mid1160c8.o \
$(OUT_PATH)/drivers/flash/flash_mid1164c8.o \
$(OUT_PATH)/drivers/flash/flash_mid13325e.o \
$(OUT_PATH)/drivers/flash/flash_mid136085.o \
$(OUT_PATH)/drivers/flash/flash_mid1360c8.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/drivers/flash/%.o: $(TEL_PATH)/drivers/flash/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

$(OUT_PATH)/drivers/flash/%.o: $(TEL_PATH)/drivers/flash/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(BOOT_FLAG) $(INCLUDE_PATHS) -c -o"$@" "$<"


