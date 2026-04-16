################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OUT_DIR += /application/keyboard

OBJS += $(OUT_PATH)/application/keyboard/keyboard.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/application/keyboard/%.o: $(TEL_PATH)/application/keyboard/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

$(OUT_PATH)/application/keyboard/%.o: $(TEL_PATH)/application/keyboard/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(BOOT_FLAG) $(INCLUDE_PATHS) -c -o"$@" "$<"


