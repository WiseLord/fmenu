# Output file name
PROJECT     = avr_fmenu

MCU      = atmega328p
F_CPU    = 16000000

C_DEFS += -D_$(MCU)
C_DEFS += -DPLACE_CONST_IN_FLASH

FMENU = ../fmenu

# Source files
C_SOURCES     += main.c
C_SOURCES     += com_io.c
C_SOURCES     += terminal_out.c
C_SOURCES     += $(FMENU)/fmenu.c
C_SOURCES     += $(FMENU)/fproperty.c

C_INCLUDES += \
  -I$(FMENU) \
  -I.


# Build options
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

OPT = -Os
WARN = -Wall

MCU_FLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU)
OPT_FLAGS = $(OPT) -mcall-prologues -fshort-enums -ffunction-sections -fdata-sections -ffreestanding
#OPT_FLAGS += -flto
CFLAGS += $(MCU_FLAGS) $(C_DEFS) $(C_INCLUDES) $(OPT_FLAGS) $(WARN)

# Dependency information
CFLAGS += -MMD -MP -MT $(OBJ_DIR)/$(*F).o -MF $(OBJ_DIR)/$(*D)/$(*F).d

LDFLAGS =  $(MCU_FLAGS) $(WARN) -Wl,--gc-sections -Wl,--relax

# Toolchain definitions
PREFIX = avr-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
OD = $(PREFIX)objdump
AR = $(PREFIX)ar
SZ = $(PREFIX)size

# Avrdude parameters
AVRDUDE  = avrdude
AD_MCU   = -p $(MCU)
#AD_PROG = -c stk500v2
#AD_PORT = -P avrdoper

AD_CMDLINE = $(AD_MCU) $(AD_PROG) $(AD_PORT) -V -B 0.5

OBJS = $(addprefix $(OBJ_DIR)/, $(C_SOURCES:.c=.o))
ELF = $(BUILD_DIR)/$(PROJECT).elf
HEX = flash/$(PROJECT).hex

all: $(HEX) size

$(HEX): $(ELF)
	$(CP) -O ihex -R .eeprom -R .nwram $(ELF) $(HEX)

$(ELF): $(OBJS)
	@mkdir -p $(BUILD_DIR) flash
	$(CC) $(LDFLAGS) -o $(ELF) $(OBJS)
	$(OD) -h -S $(ELF) > $(BUILD_DIR)/$(PROJECT).lss

size: $(ELF)
	@sh ./size.sh $(ELF)

$(OBJ_DIR)/%.o: %.c Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
		rm -rf $(BUILD_DIR)

.PHONY: flash
flash: $(ELF)
	$(AVRDUDE) $(AD_CMDLINE) -U flash:w:$(HEX):i

fuse:
	$(AVRDUDE) $(AD_CMDLINE) -U lfuse:w:0xFF:m -U hfuse:w:0xD8:m -U efuse:w:0xCB:m

# Dependencies
-include $(OBJS:.o=.d)
