################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OUT_DIR += /boot

LS_FLAGS := $(TEL_PATH)/boot/boot_b80.link

OBJS += \
$(OUT_PATH)/boot/cstartup_flash.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/boot/%.o: $(TEL_PATH)/boot/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

$(OUT_PATH)/boot/%.o: $(TEL_PATH)/boot/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(BOOT_FLAG) $(INCLUDE_PATHS) -c -o"$@" "$<"


