################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += /vendor/common

OBJS += \
$(OUT_PATH)/vendor/common/app_common.o \
$(OUT_PATH)/vendor/common/battery_check.o \
$(OUT_PATH)/vendor/common/ble_flash.o \
$(OUT_PATH)/vendor/common/blt_fw_sign.o \
$(OUT_PATH)/vendor/common/blt_led.o \
$(OUT_PATH)/vendor/common/blt_soft_timer.o \
$(OUT_PATH)/vendor/common/custom_pair.o \
$(OUT_PATH)/vendor/common/flash_fw_check.o \
$(OUT_PATH)/vendor/common/flash_prot.o \
$(OUT_PATH)/vendor/common/simple_sdp.o \
$(OUT_PATH)/vendor/common/software_uart.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/vendor/common/%.o: $(TEL_PATH)/vendor/common/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

$(OUT_PATH)/vendor/common/%.o: $(TEL_PATH)/vendor/common/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(BOOT_FLAG) $(INCLUDE_PATHS) -c -o"$@" "$<"
