############################################################
#
#                         Makefile
#
############################################################

PROJECT       = digit4_demo

# hier alle zusaetzlichen Softwaremodule angegeben
SRCS          = ../src/sysf030_init.o
SRCS         += ../src/seg7anz_v3.o

INC_DIR       = -I./ -I../include

LSCRIPT       = stm32f030x6.ld

# FLASHERPROG Auswahl fuer STM32:
# 0 : STLINK-V2, 1 : 1 : stm32flash_rts  2 : stm32chflash 3 : DFU_UTIL
# FLASHERPROG Auswahl fuer LPC
# 4 : flash1114_rts

PROGPORT      = /dev/ttyUSB0
CH340RESET    = 1
ERASEFLASH    = 0
FLASHERPROG   = 1


include ../lib/libopencm3.mk
