STM32F03F4P6
------------------------------------------------------------

Hier handelt es sich um ein eingerichtetes Setup fuer
STM32F030 Microcontroller in Verbindung mit lipobencm3.

In jedem Verzeichnis ist ein sehr einfaches Makefile ent-
halten, das den Bedürfnissen angepasst werden kann (oder
muss).

Hier sind lediglich Angaben zu machen bzgl. der Datei,
die die - main - enthaellt. 

Desweiteren koennen mittels 

              SRCS    = sourcedatei.o

                      oder

              SRCS   += sourcedatei.o

eigene Quellcodes eingebunden werden, die im Verzeichnis

                      src
                      
als sourcedatei.c vorliegen müssen

Hier gefolgt gibt es einen Eintrag, wie ein fertiges 
Programm via "make flash" auf den Controller gebracht
wird:

              FLASHERPROG  = 0

verwendet einen ST-Link v2

              FLASHERPROG  = 1

verwendet den seriellen Bootloader des Controllers

Dieses Makefile ruft dann das Makefile der libopencm3 auf.
Ein Blick in ein bereits vorhandenes Makefile in einem
Verzeichnis klaert sicherlich auf.

Im Verzeichnis src sind viele Sourcecodes enthalten zum
einfachen Umgang mit TFT-Displays, SW-Displays, UART,
SPI und divererse Hardwarebauteile
