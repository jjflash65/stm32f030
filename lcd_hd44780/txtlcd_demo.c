/* -------------------------------------------------------
                      txtlcd_demo.c

     Testprogramm fuer ein Standardtextdisplay mit
     HD44780 kompatiblen LC-Controller, 4-Bit Daten-
     transfermodus

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
// ------------------------------------------------------------------------------------------------------------------

#include <libopencm3.h>

#include "sysf030_init.h"
#include "hd44780.h"
#include "my_printf.h"


#define printf    my_printf

static const unsigned char charbmp0[8]  =         // hochgestelltes "o" (fuer Gradangaben)
  { 0x0c, 0x12, 0x12, 0x0c, 0x00, 0x00, 0x00, 0x00 };

static const unsigned char charbmp1[8]  =         // Pfeil nach links
  { 0x08, 0x0c, 0x0e, 0x0f, 0x0e, 0x0c, 0x08, 0x00 };

static const unsigned char ohmbmp[8]    =           // Ohmzeichen
  { 0x0e, 0x11, 0x11, 0x11, 0x0a, 0x0a, 0x1b, 0x00};

/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  txlcd_putchar(ch);
}


/* -------------------------------------------------------
                          M-A-I-N
   ------------------------------------------------------- */
int main()
{
  uint16_t cx= 0;

  sys_init();
  txlcd_init();

  txlcd_setuserchar(0,&charbmp0[0]);
  txlcd_setuserchar(1,&charbmp1[0]);
  txlcd_setuserchar(2,&ohmbmp[0]);

  gotoxy(1,1); printf("UserChar");
  gotoxy(1,2); printf("%c %c %c", 0,1,2);
  delay(1000);
  clrscr();
  gotoxy(1,1); printf("Counter");
  gotoxy(1,2); printf("cx=");
  while(1)
  {
    gotoxy(5,2); printf("    ");
    gotoxy(5,2); printf("%d",cx);
    cx++;
    cx = cx % 1000;
    delay(500);
  }
}
