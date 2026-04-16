################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OUT_DIR += /common

OBJS += \
$(OUT_PATH)/common/string.o \
$(OUT_PATH)/common/timer_event.o \
$(OUT_PATH)/common/utility.o \
$(OUT_PATH)/common/div_mod.o

#$(OUT_PATH)/common/sdk_version.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/common/%.o: $(TEL_PATH)/common/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

$(OUT_PATH)/common/%.o: $(TEL_PATH)/common/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(BOOT_FLAG) $(INCLUDE_PATHS) -c -o"$@" "$<"


