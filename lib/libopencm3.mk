# ---------------------------------------------------------------
#                          makefile.mk
#     funktionaler Bestandteil eines Makefiles zum Uebersetzen
#     von Programmen fuer einen STM32F030
#
#     Diese Datei sollte vom Makefile des Projektes aufgerufen
#     werden
# ---------------------------------------------------------------

LIBOPENCM3DIR ?= ../lib/libopencm3

SOC ?= STM32F0

CFLAGS += -std=gnu99 -Wall -Os -g
CFLAGS += -mcpu=cortex-m0 -mthumb
CFLAGS += -nostartfiles -ffreestanding
CFLAGS += -ffunction-sections -fdata-sections -flto

ifeq ($(INC_DIR),)
	INC_DIR    := -I./ -I../include
endif

CFLAGS += -I$(LIBOPENCM3DIR)/include
# CFLAGS += -I/usr/local/gcc-arm-none-eabi/arm-none-eabi/include
CFLAGS += $(INC_DIR)

CFLAGS += -L$(LIBOPENCM3DIR)/lib
CFLAGS += -L$(LIBOPENCM3DIR)/../
CFLAGS += -D$(SOC)

AS      = arm-none-eabi-as
CC      = arm-none-eabi-gcc
CPP     = arm-none-eabi-g++
LD      = arm-none-eabi-gcc
SIZE    = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump

LDFLAGS += -lopencm3_stm32f0 -Wl,--gc-sections -flto -lm -lc -lgcc -lnosys

# Variable PROGPORT setzen wenn nicht vorgegeben
ifeq ($(PROGPORT),)
	PROGPORT   := /dev/ttyUSB0
endif

# Linkerscript auf STM32F030x6 setzen wenn nicht vorgegeben
ifeq ($(LSCRIPT),)
	LSCRIPT    := stm32f030x6.ld
endif


.PHONY: all lib size flash list clean cleanlib

all: clean $(PROJECT).elf size

lib:
	make -C $(LIBOPENCM3DIR)

$(PROJECT).elf: $(PROJECT).c $(SRCS) | lib
	$(CC) $(CFLAGS)  -o $@ -T $(LSCRIPT) $^ $(LDFLAGS)
	$(OBJCOPY) -O binary $(PROJECT).elf $(PROJECT).bin

$(PROJECT).list: $(PROJECT).elf
	$(OBJDUMP) -D $< > $(PROJECT).list

size: $(PROJECT).elf
	$(SIZE)  $(PROJECT).elf 1>&2

flash: $(PROJECT).bin

####################################################################

# STLINK-V2


ifeq ($(FLASHERPROG), 0)

ifeq ($(ERASEFLASH), 1)
	st-flash erase
endif
	st-flash write $< 0x8000000
endif

####################################################################

# serieller Bootloader mit Aktivierung ueber Reset-Controller (ATtiny13)
# RTS Leitung pulst 3 mal zum Aktivieren des Bootloadermodus

ifeq ($(FLASHERPROG), 1)

ifeq ($(CH340RESET), 1)
	ch340reset
endif

	stm32flash_rts -w $< -b 115200 -g 0 $(PROGPORT)
#	stm32flash_rts -w $< -g 0 $(PROGPORT)
endif

####################################################################

# serieller Bootloader mit Aktivierung ueber direkte RTS-Leitung
# der seriellen Schnittstelle

ifeq ($(FLASHERPROG), 2)

ifeq ($(CH340RESET), 1)
	ch340reset
endif

#	stm32chflash -w $< -b 460800 -g 0 $(PROGPORT)
	stm32chflash -w $< -g 0 $(PROGPORT)

ifeq ($(CH340RESET), 1)
	ch340reset
endif

endif

####################################################################

# DFU-UTIL Bootloader

ifeq ($(FLASHERPROG), 3)
	dfu-util -a0 -s 0x8000000 -D $<
endif

####################################################################

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f ../src/*.o

cleanlib: clean
	rm -f $(LIBOPENCM3DIR)/lib/stm32/f0/*.d
	rm -f $(LIBOPENCM3DIR)/lib/stm32/f0/*.o
