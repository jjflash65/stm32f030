/*   -------------------------------------------------------
                          hd44780.c

     Softwaremodul fuer grundlegende Funktionen eines HD44780
     kompatiblen LC-Textdisplays

     Hardware : Text-LCD

     MCU      :
     Takt     :

     14.02.2020  R. Seelig

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

#include "hd44780.h"

uint8_t wherex, wherey;


/* -------------------------------------------------------
     nibbleout

     sendet ein Halbbyte an das LC-Display

     Uebergabe:
         value  : gesamtes Byte
         nibble : 1 => HiByte wird gesendet
                  0 => LoByte wird gesendet
         HILO= 1 => oberen 4 Bits werden gesendet
         HILO= 0 => untere 4 Bits werden gesendet
   ------------------------------------------------------- */
void nibbleout(uint8_t value, uint8_t nibble)
{
  if (nibble)
  {
    if (testbit(value, 7 )) { d7_set(); } else { d7_clr(); d7_output(); }
    if (testbit(value, 6 )) { d6_set(); } else { d6_clr(); d6_output(); }
    if (testbit(value, 5 )) { d5_set(); } else { d5_clr(); d5_output(); }
    if (testbit(value, 4 )) { d4_set(); } else { d4_clr(); d4_output(); }
  }
  else
  {
    if (testbit(value, 3 )) { d7_set(); } else { d7_clr(); d7_output(); }
    if (testbit(value, 2 )) { d6_set(); } else { d6_clr(); d6_output(); }
    if (testbit(value, 1 )) { d5_set(); } else { d5_clr(); d5_output(); }
    if (testbit(value, 0 )) { d4_set(); } else { d4_clr(); d4_output(); }
  }
}

void clock_delay(uint16_t tv)
{
  volatile uint32_t cx;

  tv= tv*150;
  for (cx= 0; cx < tv; cx++)
  {
  }
}

/* -------------------------------------------------------
      txlcd_clock

      gibt einen Clockimpuls an das Display
   ------------------------------------------------------- */
void txlcd_clock(void)
{
  e_set();
  clock_delay(60);
  e_clr(); e_output();
  clock_delay(60);
}

/* -------------------------------------------------------
      txlcd_io

      sendet ein Byte an das Display

      Uebergabe:
         value = zu sendender Wert
   ------------------------------------------------------- */
void txlcd_io(uint8_t value)
{
  nibbleout(value, 1);
  txlcd_clock();
  nibbleout(value, 0);
  txlcd_clock();
}

/* -------------------------------------------------------
     txlcd_init

     initialisiert das Display im 4-Bitmodus
   ------------------------------------------------------- */
void txlcd_init(void)
{
  char i;

  d4_output(); d5_output(); d6_output(); d7_output();
  rs_output(); e_output();
  delay(100);

  rs_clr(); rs_output();
  for (i= 0; i< 3; i++)
  {
    txlcd_io(0x20);
    _delay_ms(6);
  }
  txlcd_io(0x28);
  _delay_ms(6);
  txlcd_io(0x0c);
  _delay_ms(6);
  txlcd_io(0x01);
  _delay_ms(6);
  wherex= 0; wherey= 0;
}

/* -------------------------------------------------------
     gotoxy

     setzt den Textcursor an eine Stelle im Display. Die
     obere linke Ecke hat die Koordinate (1,1)
   ------------------------------------------------------- */
void gotoxy(uint8_t x, uint8_t y)
{
  uint8_t txlcd_adr;

  txlcd_adr= (0x80+((y-1)*0x40))+x-1;
  rs_clr(); rs_output();
  txlcd_io(txlcd_adr);
  wherex= x;
  wherey= y;
}

/* -------------------------------------------------------
     txlcd_setuserchar

     kopiert die Bitmap eines benutzerdefiniertes Zeichen
     in den Charactergenerator des Displaycontrollers

               nr : Position im Ram des Displays, an
                    der die Bitmap hinterlegt werden
                    soll.
        *userchar : Zeiger auf die Bitmap des Zeichens

   Bsp.:  txlcd_setuserchar(3,&meinezeichen[0]);
          txlcd_putchar(3);

   ------------------------------------------------------- */
void txlcd_setuserchar(uint8_t nr, const uint8_t *userchar)
{
  uint8_t b;

  rs_clr(); rs_output();
  txlcd_io(0x40+(nr << 3));                         // CG-Ram Adresse fuer eigenes Zeichen
  rs_set();
  for (b= 0; b< 8; b++) txlcd_io(*userchar++);
  rs_clr(); rs_output();
}


/* -------------------------------------------------------
     txlcd_putchar

     gibt ein Zeichen auf dem Display aus

     Uebergabe:
         ch = auszugebendes Zeichen
   ------------------------------------------------------- */

void txlcd_putchar(char ch)
{
  rs_set();
  txlcd_io(ch);
  wherex++;
}

/* -------------------------------------------------------
      txlcd_putramstring

      gibt einen AsciiZ Text der im RAM gespeichert ist
      auf dem Display aus.

      Bsp.:

      char strbuf[] = "H. Welt";

      putramstring(strbuf);
   ------------------------------------------------------- */

void txlcd_putramstring(uint8_t *c)                              // Uebergabe eines Zeigers (auf einen String)
{
  while (*c)
  {
    txlcd_putchar(*c++);
  }
}

