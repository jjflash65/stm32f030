/* -------------------------------------------------------
                        seg7anz_v3.h

     Headerdatei fuer max. 8-stellige 7-Segmentanzeige
     mittels zwei 74HC595 Schieberegistern

     Anmerkung: die Anzeigesegmente des werden ge-
                multiplext werden, da nur
                2 Schieberegister verwendet werden
                SR sind kaskadiert, zuerst ist der
                Datenwert der Ziffer, danach die
                Multiplexstelle auszuschieben.

                Zum Multiplexen wird Timer3 als
                interruptbetriebene Taktquelle
                eingesetzt.

                Dieser Interrupt toggelt zusaetzlich die
                globalen Variablen halfsec und zaehlt
                tim3_zsek hoch

     Hardware : 7-Segment LED Anzeigen
                2 Stck. SN74HC595 Schiebberegister
                               oder
                Chinamodul "4-Bit LED Digital Tube Modul"

     MCU       : STM32f030
     Takt      :

     02.03.2020  R. Seelig
   ------------------------------------------------------ */

/*
   Anschluesse:
 ------------------------------------------------------
 ------------------------------------------------------
   Pinbelegung:

   4 Bit LED Digital Tube Module                 STM32F030F4P6
   -----------------------------------------------------------

       (shift-clock)   Sclk   -------------------- PA5
       (strobe-clock)  Rclk   -------------------- PA0
       (ser. data in)  Dio    -------------------- PA7
       (+Ub)           Vcc
                       Gnd


   Anzeigenposition 0 ist das rechte Segment des Moduls

            +-----------------------------+
            |  POS3   POS2   POS1   POS0  |
    Vcc  o--|   --     --     --     --   |
    Sclk o--|  |  |   |  |   |  |   |  |  |
    Rclk o--|  |  |   |  |   |  |   |  |  |
    Dio  o--|   -- o   -- o   -- o   -- o |
    GND  o--|                             |
            |   4-Bit LED Digital Tube    |
            +-----------------------------+

   Segmentbelegung der Anzeige:

       a
      ---
   f | g | b            Segment | dp |  g  |  f  |  e  |  d  |  c  |  b  |  a  |
      ---               --------------------------------------------------------
   e |   | c            Bit-Nr. |  7 |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
      ---
       d

   Segmente leuchten bei einer logischen 0 (gemeinsame Kathode) !!!

*/

#ifndef in_seg7anz_v3
  #define in_seg7anz_v3

  #include <libopencm3.h>
  #include "sysf030_init.h"

  #define seg7_common           1         // Flag fuer Auswahl des 7-Segment Typs
                                          //    0 : 7-Segment mit gemeinsamer Kathode
                                          //    1 : 7-Segment mit gemeinsamer Anode

  #define  mirror               0         // 0: Ausgabe fuer das "digital tube modul"
                                          // 1: Ausgabe fuer das "multifunction shield"

  //  Anschlusspins des Moduls
  #define srdata_init()       PA7_output_init()
  #define srdata_set()        PA7_set()
  #define srdata_clr()        PA7_clr()

  #define srclock_init()      PA5_output_init()
  #define srclock_set()       PA5_set()
  #define srclock_clr()       PA5_clr()

  #define srstrobe_init()     PA0_output_init()
  #define srstrobe_set()      PA0_set()
  #define srstrobe_clr()      PA0_clr()

  // globale Variable
  extern uint8_t  seg7_4digit[8];               // Buffer, der die Bitmuster der 7-Segmentanzeige aufnimmt
  extern uint8_t  led7sbmp[16];                 // "Bildmuster" fuer Hex-Ziffern

  extern volatile uint32_t my_ticker;
  extern volatile uint32_t tim3_zsek;
  extern volatile char     halfsek;


  /* ----------------------------------------------------------
     Prototypen
     ---------------------------------------------------------- */

  void digit4_delay(void);
  void digit4_ckpuls(void);
  void digit4_stpuls(void);
  void digit4_outbyte(uint8_t value);
  void digit4_setdez(uint32_t value, uint8_t nonull);
  void digit4_setdez8bit(uint8_t value, uint8_t pos);
  void digit4_sethex(uint32_t value, uint8_t nonull);
  void digit4_setall8(uint8_t c7, uint8_t c6, uint8_t c5, uint8_t c4,
                      uint8_t c3, uint8_t c2, uint8_t c1, uint8_t c0);
  void digit4_setall4(uint8_t c3, uint8_t c2, uint8_t c1, uint8_t c0);
  void digit4_setdp(uint8_t pos);
  void digit4_clrdp(uint8_t pos);
  void digit4_clr(void);
  void digit4_init(void);

/*
  // nur zur Fehlersuche "oeffentlich" machen
  static void tim3_init(void);
  void tim3_isr(void);
*/

#endif
