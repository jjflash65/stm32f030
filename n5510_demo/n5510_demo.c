/* ------------------------------------------------------------
                            n5510_demo.c

    Demoprogramm fuer die Verwendung von N5510 LC-Displays

    Hardware  : STM32F030F4P6
    IDE       : make - Projekt
    Library   : libopencm3
    Toolchain : arm-none-eabi

    27.02.2020   R. Seelig
   ------------------------------------------------------------ */

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <libopencm3.h>

#include "sysf030_init.h"
#include "my_printf.h"
#include "tftmono.h"

#include "female84.h"
#include "smiley_bmp.h"


#define printf   my_printf

/* --------------------------------------------------------
   putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  lcd_putchar(ch);
}


/* ------------------------------------------------------------------------
                                    MAIN
   ------------------------------------------------------------------------ */
int main(void)
{
  int cnt, i;

  sys_init();
  lcd_init();
  lcd_enable();

  fb_init(84, 6);               // Framebuffer: 84 Spalten / 6 Reihen zu je 8 Pixel = 8xx48 Pixel

  delay(150);

  gotoxy(0,0);
  printf("STM32F030F4P6");
  printf("\n\r--------------");
  gotoxy(0,2); printf("mit N5510 LCD");
  gotoxy(0,5); printf("2017 R. Seelig");

  delay(2000);

  while(1)
  {
    fb_clear();
    for (i= 0; i< 84; i = i + 4)
    {
      line(83, 47, i, 10, 1);
      delay(30);
      fb_show(0,0);
      gotoxy(0,0);
      printf("Grafikelemente");
    }
    fillrect(4,20,80,36, 0);
    fillellipse(42,28,18,16, 1);
    rectangle(4,20,80,36, 1);
    rectangle(5,21,79,35, 0);
    fb_show(0,0);
    gotoxy(0,0);
    printf("Grafikelemente");
    delay(4000);
    clrscr();

    showimage(0,0,&bmppic[0],2);

    cnt= 0;

    do
    {

      delay(1000);

      gotoxy(0,0); printf("%d",cnt);
      cnt++;

    } while (cnt< 5);

    clrscr();

    for (i= 0; i<36; i++)
    {
       showimage(i,0, &smiley[0], 2);           // Smiley zeichnen
       delay(40);
       showimage(i,0, &smiley[0], 0);           // Smiley loeschen
    }
    showimage(i,0, &smiley[0], 2);
    delay(1000);
  }
}
