# ---------------------------------------------------------------
#     einfaches Makefile zum Compilieren und Linken von
#     AVR Programmen mittels AVR-GCC
# ---------------------------------------------------------------

SRC_NAME    = reset_f103_b

ADD_LIBS    =

PRINTF_FL = 0
SCANF_FL  = 0
SRC_DIR = -I../src/

MCU =attiny13
FREQUENCY = 6000000

MY_DEFINES = -Dtestmode=0

INCLUDE_PATHS  = -I. -I../include
INCLUDE_PATHS += $(SRC_DIR)

PROGRAMMER = usbasp
BAUD = 115200
PORT = /dev/ttyUSB0

DEBUG = 0


###############################################################################
#
#                                 Makefile
#
#   einfaches Makefile zum "builden" von HEX-Dateien fuer Atmel (c) AVR-
#   Mikrocontroller.
#
#   Benutzerdefinierte Angaben fuer:
#
#   PROJECT
#        Der Name des Hauptprogramms OHNE jegliche Erweiterung
#
#   OBJECTS
#        hier sind die zu erzeugenden Dateien anzugeben (Hauptprogramm
#        welches die Funktion Main enthaellt plus zusaetzliche Software-
#        module ohne weiteren "Trenner":
#
#        OBJECTS = ./serial_demo.o ./readint.o
#
#        hier muessen die Dateien serial_demo.c und readint.c vorhanden
#        sein
#
#   PRINT_FL / SCAN_FL
#        = 1 wenn Unterstuetzung fuer Gleitkommazahlen mittels printf / scanf
#            vorhanden sein soll.
#        = 0 ohne unterstuetzung
#
#   SCR_DIR
#        Suchverzeichnis, in dem zusaetzliche Programmmodule liegen
#
#   MCU
#        Name des zu verwendenden Mikrocontrollers
#
#   FREQUENCY
#        Taktfrequenz des Controllers, die Angabe hier ist gleichbedeutend
#        einem
#                 #define F_CPU frequenz
#        im C-Programm
#
#   ADD_LIBS
#        einzubindendes zu erstellendes Softwaremodul
#
#               ADD_LIBS = ../src/softwaremodul.o
#
#        hier muss der Quellcode softwaremodul.c vorhanden sein
#
#   MY_DEFINES
#        defines, wie sie in einem C-Programm angegeben wuerden mit zu-
#        saetzlich vorangestelltem "-D". (Somit sind die Defines in
#        allen Softwaremodulen verfuegbar)
#
#   PROGRAMMER / BAUD / PORT
#        gibt den Programmernamen, den Port und die Baudrate an, an den
#        ein Programmer angeschlossen ist.
#
#
#   Aufrufoption des Makefiles:
#        make oder make all : erstellt alle .o und Hexdateien
#        make compile       : compiliert nur das Hauptprogramm ohne weitere
#                             Bibliotheken
#        make clean         : loescht alle erstellten Dateien (sinnvoll,
#                             wenn Softwaremodule / Bibliotheken veraendert
#                             wurden)
#        make size          : zeigt die Groesse der erstellten Hex-Datei an
#        make flash         : flasht den Zielcontroller
#
#
#   September 2016,  R. Seelig
#
###############################################################################



AS      = avr-as
CC      = avr-gcc
CPP     = avr-g++
LD      = avr-gcc
OBJCOPY = avr-objcopy
SIZE    = avr-size


OBJECTS     = $(SRC_NAME).o


CPU = -mmcu=$(MCU)
CC_SYMBOLS = -DF_CPU=$(FREQUENCY)
CC_SYMBOLS += $(MY_DEFINES)

CC_FLAGS = $(CPU) -c

LD_FLAGS = $(CPU)

ifeq ($(PRINTF_FL), 1)
  LD_FLAGS += -Wl,-u,vprintf -lprintf_flt -lm
endif

ifeq ($(SCANF_FL), 1)
  LD_FLAGS += -Wl,-u,vscanf -lscanf_flt -lm
endif

LD_FLAGS += -Wl,-Map=$(SRC_NAME).map,--cref
LD_SYS_LIBS =


ifeq ($(DEBUG), 1)
  CC_FLAGS += -DDEBUG -O0
else
  CC_FLAGS += -DNODEUG -Os
endif

.PHONY: all clean size flash fuses

all: $(SRC_NAME).bin $(SRC_NAME).hex size


clean:
	rm -f $(SRC_NAME).bin $(SRC_NAME).elf $(SRC_NAME).hex $(SRC_NAME).map $(SRC_NAME).lst $(OBJECTS) $(DEPS) $(ADD_LIBS)


.asm.o:

.c.o:
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu99  $(INCLUDE_PATHS) -o $@ $<

.cpp.o:
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++98 $(INCLUDE_PATHS) -o $@ $<



$(SRC_NAME).elf: $(OBJECTS) $(ADD_LIBS)
	$(LD) $(LD_FLAGS) $(LIBRARY_PATHS) -o $@ $^ $(LIBRARIES) $(LD_SYS_LIBS)


$(SRC_NAME).bin: $(SRC_NAME).elf
	$(OBJCOPY) -O binary $< $@

$(SRC_NAME).hex: $(SRC_NAME).elf
	@$(OBJCOPY) -O ihex $< $@

lst: $(SRC_NAME).lst

size: $(SRC_NAME).elf
	$(SIZE) -C $(SRC_NAME).elf --mcu=$(MCU) 1>&2

DEPS = $(OBJECTS:.o=.d) $(ADD_LIBS:.o=.d)
-include $(DEPS)

flash:

	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -V -B 10 -U flash:w:$(SRC_NAME).hex
