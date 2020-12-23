/* -------------------------------------------------------
                         tm1637_demo.c

     Demoprogramm 4-stellige 7-Segmentanzeige mit
     TM1637 Chip

     Hardware  : 4-stellige Anzeige mit Doppelpunkt
                 (Uhrenanzeige)

     IDE       : make - Projekt
     Library   : libopencm3
     Toolchain : arm-none-eabi

     MCU       :  STM32f030F4P6
     Takt      :

     02.03.2020  R. Seelig
   ------------------------------------------------------ */

/*
   Anzeigenposition 0 ist das linke Segment des Moduls

        +---------------------------+
        |  POS0  POS1   POS2  POS3  |
        |  +--+  +--+   +--+  +--+  | --o  GND
        |  |  |  |  | o |  |  |  |  | --o  +5V
        |  |  |  |  | o |  |  |  |  | --o  DIO
        |  +--+  +--+   +--+  +--+  | --o  CLK
        |      4-Digit Display      |
        +---------------------------+

   Segmentbelegung der Anzeige:

       a
      ---
   f | g | b            Segment |  a  |  b  |  c  |  d  |  e  |  f  |  g  | Doppelpunkt (nur fuer POS1) |
      ---               ---------------------------------------------------------------------------------
   e |   | c            Bit-Nr. |  0  |  1  |  2  |  3  |  4  |  5  |  6  |              7              |
      ---
       d


   Bit 7 der 7-Segmentanzeige POS1 ist der Doppelpunkt
*/

#include <stdint.h>
#include <libopencm3.h>

#include "sysf030_init.h"
#include "tm1637.h"

//   Anschlussbelegung siehe tm1637.h

const uint8_t laufmust1[] =
{
  0x11, 0x21, 0x31, 0x41,   0x42,
  0x47, 0x37, 0x27, 0x17,   0x15,
  0x14, 0x24, 0x34, 0x44,   0x43,
  0x47, 0x37, 0x27, 0x17,   0x16,  0x00
};

/* -------------------------------------------------------
                           playmuster

     spielt ein Lauflichtmuster auf der 7-Segmentanzeige
     ab
   ------------------------------------------------------- */
void playmuster(const uint8_t *muster, int speed)
{
  uint8_t c, anzp, segp;

  while(1)
  {
    c= *muster;
    if (!(c)) return;
    muster++;
    tm1637_clear();
    anzp= ((c >> 4) & 0x0f) - 1;
    segp= (c & 0x0f) - 1;
    tm1637_setseg(anzp, segp);
    delay(speed);
  };
}


/* -------------------------------------------------------
                              main
   ------------------------------------------------------- */
int main(void)
{
  uint16_t b;

  sys_init();
  tm1637_init();

  while(1)
  {
    playmuster(&laufmust1[0], 50);

    b= 0;
    tm1637_sethex(b);
    delay(500);

    do                                         // hexadezimaler Aufwaertszaehler
    {
      tm1637_sethex(b);
      delay(500);
      b++;
    } while(b< 0x10);
    tm1637_sethex(b);
    delay(1000);

    tm1637_clear();
    delay(300);

    b= 10;
    tm1637_setdez(b);
    delay(500);

    do                                        // dezimaler Countdown
    {
      tm1637_dp= 1;                           // "Uhrzeit-Doppelpunkt" = an
      tm1637_setdez(b);
      delay(500);
      tm1637_dp= 0;                           // "Uhrzeit-Doppelpunkt" = aus
      tm1637_setdez(b);
      delay(500);
      b--;
    } while(b);
    tm1637_setdez(b);
    delay(1000);
  }
}

