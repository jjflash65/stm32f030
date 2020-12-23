readme.txt
--------------------------------------------------------------------------------------------

reset_stm32_b.c ist eine Firmware fuer einen ATtiny13 Mikrocontroller, der das Aktivieren
des seriellen Bootloadermodus an einem STM32 Mikrocontroller vornehmen kann.

Die Firmware reset_stm32_b arbeitet zusammen mit dem PC-Programm

                           stm32flash_rts

Dieses Programm ist eine modifizierte Version eines bekannten stm32flash, welches zusaetzlich
beim Aufruf des Programms 3 Impulse auf der RTS - Leitung einer seriellen Schnittstelle
sendet.

Dieses ist fuer den ATtiny13 das Startsignal, den Bootloadermodus an einem STM32 Controller
vorzunehmen.

Beim Beenden des Programms stm32flash_rts sendet dieses 6 Impulse auf der RTS - Leitung, was
den ATtiny13 dazu veranlasst, den Bootloadermodus des STM32 zu deaktiveren und einen Reset
am STM32 vorzunehmen.

Die Fuses-Einstellungen fuer den ATtiny13 sind:

Lo-Fuse: 0x79
Hi-Fuse: 0xFF

Dies ist die Einstellung fuer eine Taktfrequenz von 4,8 MHz.

Die Firmware des ATtiny kann, bei einem anggeschlossenen Programmer (wie z.Bsp. USBASP oder
auch STK500v2) mittels der Eingabe

                              ./firmware_install

erfolgen. Dieses Script uebersetzt die Firmware und ruft einen (interaktiven) Flashvorgang
auf.

Schaltplan:



  +--------------+
  |              |
  |              |                                                 +------------------+
  |              |                                                 |                  |
  |          TxD |-------------------------------------------------| RxD              |
  |              |                                                 |                  |
  |          RxD |-------------------------------------------------| TxD              |
  |              |                                                 |                  |
  |              |        3,3V         3,3V        3,3V            |                  |
  |              |         ^            ^           ^              |                  |
  |   USB2UART   |         |            |           |              |                  |
  |              |         |            |          .-. 10k         |                  |
  |    z.Bsp.    |         |            |          | |             |                  |
  |    CH340     |        .-. 10k       |          '-'             |       STM32      |
  |   FT232RL    |        | |          8|          1|              |                  |
  |              |        '-'       +--------------------+         |                  |
  |              |         |        |  Vcc        /rst   |         |                  |
  |              |         |      3 |                    |7        |                  |
  |         RTS  | --------(------- | PB4            PB2 |---------| boot0            |
  |              |         |        |      ATtiny13      |         |                  |
  |              |         |      2 |                    |6        |                  |
  +--------------+         o------- | PB3            PB1 |---------| /rst             |
                           |        |                    |         |                  |
                           |        |  PB0   GND         |         |                  |
                           |        +--------------------+         +------------------+
                           |           5|    4|
                          \  reset     .-.    |
                           | target    | |390 |
                           |           '-'    |
                           |            |     |
                           |      LED  ---    |
                           |       rt  \ /    |
                           |           ---    |
                           |            |     |
                          ---          ---   ---

