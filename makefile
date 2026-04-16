
PGM_PORT?=COM5

PROJECT_NAME ?= ble_test

MCU?=b80

ifeq ($(MCU),b80)

SDK := sdk_ble

TEL_CHIP := -DMCU_CORE_B80=1 
#-DSWS_PRINTF_MODE=1
#-DALL_SRAM_CODE=1 
#-DSRAM_OTP_FLASH_HANDLE=1

BOOT_FLAG := $(TEL_CHIP) -DMCU_STARTUP_FLASH=1 -DMCU_STARTUP_B80=1

LIBS := -llt_8208 -lfirmware_encrypt

MemInfo := TlsrMemInfo_b80.py

TEL_PATH ?= ./$(SDK)

else

SDK := sdk_ble

TEL_CHIP := -DMCU_CORE_B80B=1  
#-DSWS_PRINTF=1
#-DALL_SRAM_CODE=1 
#-DSRAM_OTP_FLASH_HANDLE=1

BOOT_FLAG := $(TEL_CHIP) -DMCU_STARTUP_FLASH=1 -DMCU_STARTUP_B80B=1

LIBS := -llt_8208B -lfirmware_encrypt

MemInfo := TlsrMemInfo_b80.py

TEL_PATH ?= ./$(SDK)

endif

PROJECT_PATH := ./src
OUT_PATH :=./out
PYTOOLS_PATH ?= ./tools

INCLUDE_PATHS := -I$(PROJECT_PATH) -I$(TEL_PATH) -I$(TEL_PATH)/lib -I$(TEL_PATH)/drivers -I$(TEL_PATH)/common -I$(TEL_PATH)/vendor/common
LS_INCLUDE := -L$(TEL_PATH)/proj_lib

ifneq ($(TEL_PATH)/drivers/gpio.c, $(wildcard $(TEL_PATH)/drivers/gpio.c))
$(error "Please check SDK Path and set TEL_PATH.")
endif

COMPILEOS = $(shell uname -o)
LINUX_OS = GNU/Linux

ifeq ($(COMPILEOS),$(LINUX_OS))
	PYTHON ?= python3
	TOOLS_PATH := ./tools/linux/
	TC32_PATH := $(TOOLS_PATH)tc32/bin/
	TC32_ELF_NM := $(TC32_PATH)tc32-elf-nm
else
	PYTHON ?= python
	TOOLS_PATH := ./tools/windows/
ifeq ($(TOOLS_PATH)tc32/bin/tc32-elf-gcc.exe, $(wildcard $(TOOLS_PATH)tc32/bin/tc32-elf-gcc.exe))
	TC32_PATH := $(TOOLS_PATH)tc32/bin/
	TC32_ELF_NM := $(TC32_PATH)tc32-elf-nm.exe
endif
endif

OBJ_SRCS := 
S_SRCS := 
ASM_SRCS := 
C_SRCS := 
S_UPPER_SRCS := 
O_SRCS := 
FLASH_IMAGE := 
ELFS := 
OBJS := 
LST := 
SIZEDUMMY := 
OUT_DIR :=

GCC_FLAGS := \
-ffunction-sections \
-fdata-sections \
-Wall \
-O2 \
-fpack-struct \
-fshort-enums \
-finline-small-functions \
-std=gnu99 \
-funsigned-char \
-fshort-wchar \
-fms-extensions

LNK_FLAGS := --gc-sections -nostartfiles

GCC_FLAGS += $(TEL_CHIP)
GCC_FLAGS += $(POJECT_DEF)

#include Project makefile
-include $(PROJECT_PATH)/project.mk

#include SDK makefile
-include $(TEL_PATH)/application.mk
-include $(TEL_PATH)/boot.mk
-include $(TEL_PATH)/common.mk
-include $(TEL_PATH)/drivers.mk
-include $(TEL_PATH)/drivers_ext.mk
-include $(TEL_PATH)/vendor_common.mk
-include $(TEL_PATH)/flash.mk

# Add inputs and outputs from these tool invocations to the build variables 
LST_FILE := $(OUT_PATH)/$(PROJECT_NAME).lst
BIN_FILE := $(OUT_PATH)/../$(PROJECT_NAME).bin
ELF_FILE := $(OUT_PATH)/$(PROJECT_NAME).elf

SIZEDUMMY := sizedummy

# All Target
all: clean pre-build main-build

build: pre-build main-build


flash: $(BIN_FILE)
	@$(PYTHON) $(PYTOOLS_PATH)/TlsrPgm.py -p$(PGM_PORT) -z11 -a-70 -m -w we 0 $(BIN_FILE)

erase:
	@$(PYTHON) $(PYTOOLS_PATH)/TlsrPgm.py -p$(PGM_PORT) -t50 -a2750 -s ea

reset:
	@$(PYTHON) $(PYTOOLS_PATH)/TlsrPgm.py -p$(PGM_PORT) -t50 -a2750 -m -w i

stop:
	@$(PYTHON) $(PYTOOLS_PATH)/TlsrPgm.py -p$(PGM_PORT) -z11 -s i

go:
	@$(PYTHON) $(PYTOOLS_PATH)/TlsrPgm.py -p$(PGM_PORT) -w -m

TADDR?=0x00847E00
TLEN?=512
test_dump:
	@$(PYTHON) $(PYTOOLS_PATH)/TlsrPgm.py -p$(PGM_PORT) -z11 -c -g ds $(TADDR) $(TLEN)

SWSBUF?=0x00847F00	
sws_printf:
	@$(PYTHON) $(PYTOOLS_PATH)/TlsrPgm.py -p$(PGM_PORT) -b1500000 sws $(SWSBUF)
	
rst_sws_pr:		
	@$(PYTHON) $(PYTOOLS_PATH)/TlsrPgm.py -p$(PGM_PORT) -b1500000 -z15 -s -m -o $(SWSBUF) ds $(SWSBUF) 2 

flash_printf: $(BIN_FILE)
	@$(PYTHON) $(PYTOOLS_PATH)/TlsrPgm.py -p$(PGM_PORT) -z11 -a-70 -m -w -o $(SWSBUF) we 0 $(BIN_FILE)


# Main-build Target
main-build: $(ELF_FILE) secondary-outputs

# Tool invocations
$(ELF_FILE): $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@$(TC32_PATH)tc32-elf-ld $(LNK_FLAGS) $(LS_INCLUDE) -T$(LS_FLAGS) -o $(ELF_FILE) $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

$(LST_FILE): $(ELF_FILE)
	@echo 'Invoking: TC32 Create Extended Listing'
	@$(TC32_PATH)tc32-elf-objdump -x -l -S --headers $(ELF_FILE) > $(LST_FILE)
	@echo 'Finished building: $@'
	@echo ' '


$(BIN_FILE): $(ELF_FILE)
	@echo 'Create Flash image (binary format)'
	@$(TC32_PATH)tc32-elf-objcopy -v -O binary $(ELF_FILE)  $(BIN_FILE)
	@$(PYTHON) $(PYTOOLS_PATH)/tl_check_fw.py $(BIN_FILE)
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: $(ELF_FILE)
	@$(PYTHON) $(PYTOOLS_PATH)/$(MemInfo) -t $(TC32_ELF_NM) $(ELF_FILE)

clean:
	-$(RM) $(FLASH_IMAGE) $(ELFS) $(OBJS) $(LST) $(SIZEDUMMY) $(ELF_FILE) $(BIN_FILE) $(LST_FILE)
	-@echo 'Clean...  '

pre-build:
	@mkdir -p $(foreach s,$(OUT_DIR),$(OUT_PATH)$(s))
	-@echo 'Create OUT-dir '

secondary-outputs: $(BIN_FILE) $(LST_FILE) $(SIZEDUMMY)

install:
	@echo "Install TC32..."
ifeq ($(COMPILEOS),$(LINUX_OS))
	@tar -xvjf $(TOOLS_PATH)/tc32_gcc_v2.0.tar.bz2 -C $(TOOLS_PATH)
else
	@unzip -o $(TOOLS_PATH)/tc32.zip -d $(TOOLS_PATH)
endif
	@echo "TC32 installed."
	@echo "Use: make clean"
	@echo "     make all"

.PHONY: all clean
.SECONDARY: main-build