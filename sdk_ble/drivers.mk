################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += /drivers

OBJS += \
$(OUT_PATH)/drivers/adc.o \
$(OUT_PATH)/drivers/aes.o \
$(OUT_PATH)/drivers/analog.o \
$(OUT_PATH)/drivers/bsp.o \
$(OUT_PATH)/drivers/clock.o \
$(OUT_PATH)/drivers/flash.o \
$(OUT_PATH)/drivers/gpio.o \
$(OUT_PATH)/drivers/i2c.o \
$(OUT_PATH)/drivers/ir_learn.o \
$(OUT_PATH)/drivers/keyscan.o \
$(OUT_PATH)/drivers/printf.o \
$(OUT_PATH)/drivers/qdec.o \
$(OUT_PATH)/drivers/s7816.o \
$(OUT_PATH)/drivers/spi.o \
$(OUT_PATH)/drivers/timer.o \
$(OUT_PATH)/drivers/uart.o \
$(OUT_PATH)/drivers/uart_b80b.o \
$(OUT_PATH)/drivers/usbhw.o \
$(OUT_PATH)/drivers/watchdog.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/drivers/%.o: $(TEL_PATH)/drivers/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

$(OUT_PATH)/drivers/%.o: $(TEL_PATH)/drivers/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(BOOT_FLAG) $(INCLUDE_PATHS) -c -o"$@" "$<"
