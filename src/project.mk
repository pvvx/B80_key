
OUT_DIR += /src

OBJS += \
$(OUT_PATH)/src/app_main.o \
$(OUT_PATH)/src/app_data.o \
$(OUT_PATH)/src/app_cmd.o \
$(OUT_PATH)/src/app_ble.o \
$(OUT_PATH)/src/app_buffer.o \
$(OUT_PATH)/src/app_att.o \
$(OUT_PATH)/src/app_battery.o \
$(OUT_PATH)/src/app_soc_temp.o \
$(OUT_PATH)/src/bthome_beacon.o \
$(OUT_PATH)/src/ccm.o \
$(OUT_PATH)/src/flash_eep.o \
$(OUT_PATH)/src/app_flash_protect.o \
$(OUT_PATH)/src/main.o \
$(OUT_PATH)/src/sws_printf.o 

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/src/%.o: $(PROJECT_PATH)/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

$(OUT_PATH)/src/%.o: $(PROJECT_PATH)/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(INCLUDE_PATHS) $(BOOT_FLAG) -c -o"$@" "$<"

