/*   -------------------------------------------------------
                          hd44780.h

     Header fuer grundlegende Funktionen eines HD44780
     kompatiblen LC-Textdisplays

     Hardware : Text-LCD

     MCU      :  AVR

     14.02.2020  R. Seelig

     Hinweis:
     Alle Anschlusspins des LCD muessen mit einem 1k
     Pop-Up Widerstand gegen +5V geschaltet sein, weil ein
     Hi-Pegel auf den Leitungen durch ein Konfigurieren
     der Controllerpins als Eingaenge erreicht wird.

     Der Vorteil dieser Methode besteht darin, dass ein
     Controller somit auch mit 3,3V betrieben werden kann,
     das Display aber dennoch funktioniert.
   ------------------------------------------------------- */

/*
      Default Displayanschluss an den Controller
      ----------------------------------------------------------
         o +5V
         |                            Display             STM32F030F4P6 (TSSOP20)
         _                        Funktion   PIN             PIN   Funktion
        | |
        |_|                          GND      1 ------------
         |                          +5V       2 ------------
         o----o Kontrast   ---    Kontrast    3 ------------
        _|_                           RS      4 ------------  11     PA5
        \ /                        rd /wr     5 ------------  GND
        -'-                    (Takt) E       6 ------------  12     PA6
         |                           D0       7                      n.c.
        ---                          D1       8                      n.c.
                                     D2       9                      n.c.
                                     D3      10                      n.c.
                                     D4      11 ------------  6      PA0
                                     D5      12 ------------  13     PA7
                                     D6      13 ------------  7      PA1
                                     D7      14 ------------  10     PA4

      Hinweis: Alle Anschluesse des Controllers muessen mit einem 10K Pull-Up
               Widerstand gegen +5V !!! versehen sein

*/

#ifndef in_txlcd
  #define in_txlcd

  #include <stdint.h>

  #include "sysf030_init.h"

  #define boardversion      1

  /* -------------------------------------------------------
        Pinbelegung
     ------------------------------------------------------- */

  #if (boardversion == 1)

    // command / data
    #define LCD_RS_PORT    PA
    #define LCD_RS_BIT     5

    // clock
    #define LCD_E_PORT     PA
    #define LCD_E_BIT      6

    #define LCD_D4_PORT    PA
    #define LCD_D4_BIT     0

    #define LCD_D5_PORT    PA
    #define LCD_D5_BIT     7

    #define LCD_D6_PORT    PA
    #define LCD_D6_BIT     1

    #define LCD_D7_PORT    PA
    #define LCD_D7_BIT     4

  #elif (boardversion == 2)

    // command / data
    #define LCD_RS_PORT    PA
    #define LCD_RS_BIT     5

    // clock
    #define LCD_E_PORT     PA
    #define LCD_E_BIT      6

    #define LCD_D4_PORT    PB
    #define LCD_D4_BIT     0

    #define LCD_D5_PORT    PA
    #define LCD_D5_BIT     7

    #define LCD_D6_PORT    PB
    #define LCD_D6_BIT     8

    #define LCD_D7_PORT    PB
    #define LCD_D7_BIT     6

  #else
    #error Es wurde keine gueltige Boardversion angegeben...
  #endif

  #define _delay_ms      delay

  #define lcd_wait       10                                 // Verzoegerungswert bei Takterzeugung des Displays
                                                            // 10 fuer 48 MHz Coretakt des STM32

  /* -------------------------------------------------------
       diverse Macros
     ------------------------------------------------------- */

  #define testbit(reg,pos) ((reg) & (1<<pos))               // testet an der Bitposition pos das Bit auf 1 oder 0


  /* -------------------------------------------------------
       Prototypen
     ------------------------------------------------------- */

    void txlcd_init(void);
    void txlcd_setuserchar(uint8_t nr, const uint8_t *userchar);
    void gotoxy(uint8_t x, uint8_t y);
    void txlcd_putchar(char ch);
    void txlcd_putramstring(uint8_t *c);

    #define clrscr()      txlcd_init()

    extern uint8_t wherex,wherey;


  // ----------------------------------------------------------------
  // Praeprozessormacros um 2 Stringtexte zur weiteren Verwendung
  // innerhalb des Praeprozessors  zu verknuepfen
  //
  // Bsp.:
  //        #define ionr      A
  //        #define ioport    conc2(PORT, ionr)
  //
  //        ioport wird nun als "PORTA" behandelt
  #define CONC2EXP(a,b)     a ## b
  #define conc2(a,b)        CONC2EXP(a, b)
  // ----------------------------------------------------------------

  // ----------------------------------------------------------------
  //   Makros zum Initialiseren der verwendeten Pins als Ausgaenge.
  //   Setzt aus oben angegebenen Portbits neue Makros zusammen.
  //   Bsp.:
  //             #define LCD_D4_PORT  PB
  //             #define LCD_D4_BIT   1
  //   generiert:
  //             #define d4_set()     PB1_input_init()
  //             #define d4_clr()     PB1_clr()
  //             #define d4_output()  PB1_output_init()
  // ----------------------------------------------------------------

  // ---------------- LCD_D4 ---------------
  #define d4_tmp            conc2(LCD_D4_BIT,_set())
  #define d4_set()          conc2(LCD_D4_PORT,d4_tmp)

  #define d4_tmp2           conc2(LCD_D4_BIT,_output_init())
  #define d4_output()       conc2(LCD_D4_PORT,d4_tmp2)

  #define d4_tmp3           conc2(LCD_D4_BIT,_clr())
  #define d4_clr()          conc2(LCD_D4_PORT,d4_tmp3)

  // ---------------- LCD_D5 ---------------
  #define d5_tmp            conc2(LCD_D5_BIT,_set())
  #define d5_set()          conc2(LCD_D5_PORT,d5_tmp)

  #define d5_tmp2           conc2(LCD_D5_BIT,_output_init())
  #define d5_output()       conc2(LCD_D5_PORT,d5_tmp2)

  #define d5_tmp3           conc2(LCD_D5_BIT,_clr())
  #define d5_clr()          conc2(LCD_D5_PORT,d5_tmp3)

  // ---------------- LCD_D6 ---------------
  #define d6_tmp            conc2(LCD_D6_BIT,_set())
  #define d6_set()          conc2(LCD_D6_PORT,d6_tmp)

  #define d6_tmp2           conc2(LCD_D6_BIT,_output_init())
  #define d6_output()       conc2(LCD_D6_PORT,d6_tmp2)

  #define d6_tmp3           conc2(LCD_D6_BIT,_clr())
  #define d6_clr()          conc2(LCD_D6_PORT,d6_tmp3)

  // ---------------- LCD_D7 ---------------
  #define d7_tmp            conc2(LCD_D7_BIT,_set())
  #define d7_set()          conc2(LCD_D7_PORT,d7_tmp)

  #define d7_tmp2           conc2(LCD_D7_BIT,_output_init())
  #define d7_output()       conc2(LCD_D7_PORT,d7_tmp2)

  #define d7_tmp3           conc2(LCD_D7_BIT,_clr())
  #define d7_clr()          conc2(LCD_D7_PORT,d7_tmp3)

  // --------------- LCD_E (clk) -----------
  #define e_tmp             conc2(LCD_E_BIT,_set())
  #define e_set()           conc2(LCD_E_PORT,e_tmp)

  #define e_tmp2            conc2(LCD_E_BIT,_output_init())
  #define e_output()        conc2(LCD_E_PORT,e_tmp2)

  #define e_tmp3            conc2(LCD_E_BIT,_clr())
  #define e_clr()           conc2(LCD_E_PORT,e_tmp3)

  // -----------------LCD_RS ---------------

  #define rs_tmp             conc2(LCD_RS_BIT,_set())
  #define rs_set()           conc2(LCD_RS_PORT,rs_tmp)

  #define rs_tmp2            conc2(LCD_RS_BIT,_output_init())
  #define rs_output()        conc2(LCD_RS_PORT,rs_tmp2)

  #define rs_tmp3            conc2(LCD_RS_BIT,_clr())
  #define rs_clr()           conc2(LCD_RS_PORT,rs_tmp3)

#endif
