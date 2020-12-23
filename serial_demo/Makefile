PROJECT = serial_demo

# hier alle zusaetzlichen Softwaremodule angeben
SRCS            = ../src/sysf030_init.o
SRCS           += ../src/my_printf.o
SRCS           += ../src/uart.o

SRC_DIR         = -I./ -I../src -I../include

LSCRIPT         = stm32f030x6.ld

PROGPORT        = /dev/ttyUSB0
ERASEFLASH      = 0
FLASHERPROG     = 1

include ../lib/libopencm3.mk
