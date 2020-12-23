###############################################################################
#
#                                 Makefile
#
#   Dieses Makefile muss folgende benutzerdefinierte Angaben enthalten:
#
#   PROJECT
#        Der Name des Hauptprogramms (welches u.a. die main - Funktion
#        beinhaltet OHNE jegliche Erweiterung
#
#   SRCS
#        hier sind die zu erzeugenden Dateien anzugeben (Hauptprogramm
#        welches die Funktion Main enthaellt plus zusaetzliche Software-
#        module ohne weiteren "Trenner":
#
#        SRCS    = ../src/serial_demo.o
#        SRCS   += ../src/readint.o
#
#        Die Dateien serial_demo.c und readint.c werden zu Objektdateien
#        uebersetzt und dem Gesamtprogramm hinzugelinkt
#
#
#   PRINT_FL / SCAN_FL
#        = 1 wenn Unterstuetzung fuer Gleitkommazahlen mittels printf / scanf
#            vorhanden sein soll.
#        = 0 ohne unterstuetzung
#
#
#   SCR_DIR
#        Suchverzeichnis, in dem zusaetzliche Programmmodule liegen
#
#
#   MCU
#        Name des zu verwendenden Mikrocontrollers
#
#
#   FREQ
#        Taktfrequenz des Controllers, die Angabe hier ist gleichbedeutend
#        einem
#                 #define F_CPU frequenz
#        im C-Programm
#
#
#   PROGRAMMER / BRATE / SERPORT
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
#   August 2017,  R. Seelig
#
###############################################################################

#  --- Benutzerangaben ---

PROJECT   = reset_stm32_b

SRCS      =

PRINTF_FL = 0
SCANF_FL  = 0
MATH      = 0

# fuer Compiler / Linker
FREQ      = 4800000ul
MCU       = attiny13

# fuer AVRDUDE
PROGRAMMER = usbasp
SERPORT    = /dev/ttyUSB0
BRATE      = 115200


###############################################################################
#
# funktionaler Teil des Makefiles, Aenderungen nur mit Bedacht vornehmen
#
###############################################################################

CC      = avr-gcc
LD      = avr-gcc
OBJCOPY = avr-objcopy
SIZE    = avr-size

INCLUDE_PATHS = -I./ -I../include

OBJECTS   = ./$(PROJECT).o $(SRCS)

CPU       = -mmcu=$(MCU)

CC_FLAGS = -c -Os $(CPU) -std=c99

CC_SYMBOLS = -DF_CPU=$(FREQ)
LD_FLAGS = $(CPU)

ifeq ($(PRINTF_FL), 1)
  LD_FLAGS += -Wl,-u,vprintf -lprintf_flt
endif

ifeq ($(SCANF_FL), 1)
  LD_FLAGS += -Wl,-u,vscanf -lscanf_flt
endif

LD_SYS_LIB =

ifeq ($(MATH), 1)
  LD_SYS_LIB +=-lm
endif


.PHONY: all clean size compile flash fuses

all: clean $(PROJECT).hex size

compile:
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -o $(PROJECT).o $(PROJECT).c

clean:
	rm -f $(PROJECT).o $(PROJECT).elf $(PROJECT).hex $(PROJECT).map
	rm -f $(SRCS)

.c.o:
	$(CC)  $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -o $@ $<


$(PROJECT).elf: $(OBJECTS)
	$(LD) $(LD_FLAGS) $(LD_SYS_LIB)  $^ -o $@

$(PROJECT).hex: $(PROJECT).elf
	@$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(PROJECT).lst: $(PROJECT).elf
	@$(OBJDUMP) -Sdh $< > $@

size: $(PROJECT).elf
	$(SIZE) -C $(PROJECT).elf --mcu=$(MCU)

flash:
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(SERPORT) -b $(BRATE) -B 5 -V -U flash:w:$(PROJECT).hex

fuses:
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(SERPORT) -b $(BRATE)  -B 10 -U lfuse:w:0x79:m -U hfuse:w:0xff:m


