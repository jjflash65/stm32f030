############################################################
#
#                         Makefile
#
############################################################

PROJECT       = tft_spiro

# hier alle zusaetzlichen Softwaremodule angegeben
SRCS          = ../src/sysf030_init.o
SRCS         += ../src/my_printf.o
SRCS         += ../src/tftdisplay.o
SRCS         += ../src/gfx_pictures.o
SRCS         += ../src/math_fixed.o

INC_DIR       = -I./ -I../include

LDSCRIPT      = stm32f030x8.ld

# FLASHERPROG Auswahl fuer STM32:
# 0 : STLINK-V2, 1 : 1 : stm32flash_rts  2 : stm32chflash 3 : DFU_UTIL
# FLASHERPROG Auswahl fuer LPC
# 4 : flash1114_rts

PROGPORT      = /dev/ttyUSB0
CH340RESET    = 1
ERASEFLASH    = 0
FLASHERPROG   = 1


include ../lib/libopencm3.mk
