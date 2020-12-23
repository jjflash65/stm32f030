###############################################################################
#
#                                 Makefile
#
#   einfaches Makefile zum "builden" von HEX-Dateien fuer Atmel (c) AVR-
#   Mikrocontroller.
#
#
#
#   Januar 2019,  R. Seelig
#
###############################################################################

# Projectnummer
#                 1 : Demo
#                 2 : Integer Calculator

PROJ_NR           = 2

ifeq ($(PROJ_NR),1)
	PROJECT           = tm1638_demo
endif

# Project 2 funktioniert nur mit Board 2. Umstellen der Boardversion in
# tm1638.h nicht vergessen !!!!!
ifeq ($(PROJ_NR),2)
	PROJECT           = tm1638_calc
endif

SRCS              = ../src/sysf030_init.o
SRCS             += ../src/tm1638.o

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
