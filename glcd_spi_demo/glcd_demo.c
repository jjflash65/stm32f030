/* -------------------------------------------------------
                        glcd_demo.c

     Demoprogramm fuer grafische TFT-Displays mit Auf-
     loessungen von 128x128 oder 160x128 Pixeln.

     Einstellungen des verwendeten Displays in tftdisplay.h
     und tft_pindefs.h

     MCU   :  STM32F030
     Takt  :

     12.02.2020  R. Seelig
   ------------------------------------------------------ */

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <libopencm3.h>

#include "sysf030_init.h"
#include "my_printf.h"
#include "tftdisplay.h"
#include "gfx_pictures.h"

// Bilddateien

// sollte ein STM32F030F4P4 - mit nur 16 kByte Flash - verwendet werden, so kann das
// 256 Farben-Bitmap nicht eingebunden werden ( Flashspeicher nicht gross genug), stattdessen
// wird ein einfaches PCX-Bild eingebunden

#define with_bmp256    0

#include "./include_bmp/iconpal.h"
#include "./include_bmp/icon04.h"
#include "./include_bmp/swgirl01.h"

#if (with_bmp256 == 1)
  #include "./include_bmp/manga.h"
  #include "./include_bmp/mangapal.h"
#else
  #include "./include_bmp/marilyn.h"
#endif


#define printf       my_printf


// liefert einen 16-Bit RGB565 Farbwert aus einer Farbpalette,
// hier die Palette aus icon04
#define rgbfrompal(nr)   ( (icon04pal[nr]) )

/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  lcd_putchar(ch);
}

// Hier folgen Funktionen zum Zeichnen von Linien und Kreisen
// die es zwar auch schon in tftdisplay.c gibt. Dort wird je-
// doch aus Geschwindigkeitsgruenden darauf verzichtet, ob sich
// ein zu setzender Pixel im Bereich der darstellbaren
// Koordinaten des Displays befinden. Hier jedoch findet eine
// Ueberpruefung statt

/* -------------------------------------------------------------
                         putpixel2
   ------------------------------------------------------------- */
void putpixel2(int x, int y, uint16_t color)
{
  if ((x > -1) && (x < _xres) && (y > -1) && (y < _yres))
    putpixel(x,y,color);
}

/* -------------------------------------------------------------
     line2

     Linienalgorithmus nach Bresenham (www.wikipedia.org)

   ------------------------------------------------------------- */
void line2(int x0, int y0, int x1, int y1, uint16_t color)
{

  //    Linienalgorithmus nach Bresenham (www.wikipedia.org)

  int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
  int err = dx+dy, e2;                                     /* error value e_xy */

  for(;;)
  {
    putpixel2(x0,y0,color);
    if (x0==x1 && y0==y1) break;
    e2 = 2*err;
    if (e2 > dy) { err += dy; x0 += sx; }                  /* e_xy+e_x > 0 */
    if (e2 < dx) { err += dx; y0 += sy; }                  /* e_xy+e_y < 0 */
  }
}

/* -------------------------------------------------------------
     ellipse2

     Algorithmus nach Bresenham (www.wikipedia.org)
   ------------------------------------------------------------- */
void ellipse2(int xm, int ym, int a, int b, uint16_t color )
{
  // Algorithmus nach Bresenham (www.wikipedia.org)

  int dx = 0, dy = b;                       // im I. Quadranten von links oben nach rechts unten

  long a2 = a*a, b2 = b*b;
  long err = b2-(2*b-1)*a2, e2;             // Fehler im 1. Schritt */

  do
  {
    putpixel2(xm+dx, ym+dy,color);            // I.   Quadrant
    putpixel2(xm-dx, ym+dy,color);            // II.  Quadrant
    putpixel2(xm-dx, ym-dy,color);            // III. Quadrant
    putpixel2(xm+dx, ym-dy,color);            // IV.  Quadrant

    e2 = 2*err;
    if (e2 <  (2*dx+1)*b2) { dx++; err += (2*dx+1)*b2; }
    if (e2 > -(2*dy-1)*a2) { dy--; err -= (2*dy-1)*a2; }
  } while (dy >= 0);

  while (dx++ < a)                        // fehlerhafter Abbruch bei flachen Ellipsen (b=1)
  {
    putpixel2(xm+dx, ym,color);             // -> Spitze der Ellipse vollenden
    putpixel2(xm-dx, ym,color);
  }
}

/* -------------------------------------------------------------
     circle2
   ------------------------------------------------------------- */
void circle2(int x, int y, int r, uint16_t color )
{
  ellipse2(x,y,r,r,color);
}

/* --------------------------------------------------------
                            main
   -------------------------------------------------------- */
int main(void)
{
  uint8_t  y,x;
  int      x1,x2,y1,y2,rcol,r;
  uint16_t i;
  uint8_t  yofs;

  sys_init();

  lcd_init();

  printfkomma= 2;
  lcd_orientation(0);

  while(1)
  {
    // 200 Linien zeichnen
    textcolor= rgbfromega(15);
    bkcolor= 0;
    clrscr();
    for (i= 1; i< 200; i++)
    {
      x1= rand() % _xres; x2= rand() % _xres;
      y1= rand() % _yres; y2= rand() % _yres;
      rcol = rand() % 0x10000;
      line2(x1, y1, x2, y2, rcol);
    }
    gotoxy(0,0); printf(" 200 Linien ");
    delay(2000);

    // 200 Kreise zeichnen
    clrscr();
    for (i= 1; i< 200; i++)
    {
      x1= rand() % _xres;
      y1= rand() % _yres;
      r= (rand() % _xres)+1;
      rcol = rand() % 0x10000;
      circle2(x1, y1, r, rcol);
    }
    gotoxy(0,0); printf(" 200 Kreise ");
    delay(2000);


    // ASCII Zeichen ausgeben
    textcolor= rgbfromega(14);
    bkcolor= rgbfromvalue(0x00,0x00,0x66);
    clrscr();

    gotoxy(2,1); printf("  STM32F030\n\r");
    gotoxy(2,2); printf("    48MHz\n\r");
    gotoxy(2,3); printf("-------------\n\r");
    gotoxy(2,4); printf("Ascii-Zeichen\n\r");

    for (x= 32; x< 128; x++)
    {
      if ((x % 16)== 0) printf("\n\r");
      my_putchar(x);
    }
    delay(5000);

    bkcolor= rgbfromvalue(0xa8, 0x79, 0x1c);
    clrscr();

    // sw-Pixelgrafik anzeigen mit Code aus tftdisplay.c
    showimage(0,0,&girl01bmp[0],rgbfromega(0));
    textcolor= rgbfromega(15);
    gotoxy(1,14); printf(" Pixelgrafik ");
    delay(3000);

    // Farbpalette anzeigen
    bkcolor= rgbfromega(blue);
    textcolor= rgbfromega(yellow);
    clrscr();
    fillrect(0,0,127,23,rgbfromega(1));
    gotoxy(0,1); printf("  Palettentest");

    y= 2;
    gotoxy(0,y);
    for (i= 0; i< (256-48); i++)
    {
      if ((i % 16)== 0)
      {
        y++;
        gotoxy(0,y);
      }
      bkcolor= rgbfrompal(i);
      my_putchar(' ');
    }
    delay(2000);

    bkcolor= 0xffff;;
    clrscr();

    #if (with_bmp256 == 1)
      // Bitmap anzeigen mit Code aus gfx_pictures.c
      bmp256_show(0,0,&mangabmp[0], &mangapal[0]);
      textcolor= rgbfromega(4);
      bkcolor= rgbfromega(15);
      gotoxy(3,1); printf("Bitmap aus");
      gotoxy(5,2); printf("image2c");
      delay(4000);
    #else
      pcx256_show(22,20, &marilynpcx[0], &marilynpal[0]);
      textcolor= rgbfromega(4);
      bkcolor= rgbfromega(15);
      gotoxy(3,1); printf("PCX aus");
      gotoxy(5,2); printf("image2c");
      delay(4000);

    #endif


    // Animation eines Bitmaps anzeigen
    bkcolor= rgbfrompal(255);
    clrscr();

    textcolor= rgbfromega(1);
    gotoxy(2,13); printf("  Animation");
    gotoxy(2,14); printf("mit STM32F030");
    yofs= 25;
    for (x= 0; x < (127-33); x+= 1)
    {
      bmp256_show(x,yofs,&bmpimage04[0], &icon04pal[0]);
      delay(20);
    }
    for (x = (127-33); x > 0; x-= 1)
    {
      bmp256_show(x,yofs,&bmpimage04[0], &icon04pal[0]);
      delay(20);
    }
  }
}
