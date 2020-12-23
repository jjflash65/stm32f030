/* -------------------------------------------------
                   tft_spiro.c

     Spirograph auf TFT-Display auf parallelem
     Graphic-Display mit 320x240 Pixel


     MCU :  STM32F030C8T6    (64 kByte)
     Takt:

     02.03.2020  R. Seelig
   ------------------------------------------------- */

#include <libopencm3.h>

#include "sysf030_init.h"
#include "tftdisplay.h"
#include "my_printf.h"
#include "gfx_pictures.h"

#include "math_fixed.h"


#define printf        my_printf


/* --------------------------------------------------------
                      spiro_generate

     zeichnet ein "Spirograph"

     inner:  innerer Radius
     outer:  aeusserer Radius
     evol :  Anzahl der "Schwingungen" fuer eine Umdrehung
     resol:  Aufloesung (Schrittweite) beim Zeichnen
             des Graphen
     col  :  Farbe, mit der der Graph gezeichnet wird
   -------------------------------------------------------- */
void spiro_generate(int inner, int outer, int evol, int resol, uint16_t col)
{
  const int c_width  = 320;
  const int c_height = 240;
  float     inner_xpos, inner_ypos;
  float     outer_xpos, outer_ypos;
  float     j, k;
  int       i;

  inner_xpos = (c_width / 2.0f);
  inner_ypos = (c_height / 2.0f) + inner;

  outer_xpos= inner_xpos;
  outer_ypos= inner_ypos + outer;
  turtle_moveto(outer_xpos, outer_ypos);

  for (i= 0; i< resol + 1; i++)
  {
    j= ((float)i / resol) * (2.0f * MY_PI);
    inner_xpos = (c_width / 2.0f) + (inner * fk_sin(j));
    inner_ypos = (c_height / 2.0f) + (inner * fk_cos(j));

    k= j * ((float)evol / 10.0f);

    outer_xpos= inner_xpos + (outer * fk_sin(k));
    outer_ypos= inner_ypos + (outer * fk_cos(k));

    turtle_lineto(outer_xpos, outer_ypos, col);
    delay(3);
  }
}


/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  switch (fontnr)
  {
    case 0:  lcd_putchar(ch); break;
    case 1:  lcd_putchar12x16(ch); break;
    default: break;
  }
}

/* ------------------------------------------------------------------------------
                                     M-A-I-N
   ------------------------------------------------------------------------------ */
int main()
{
  int w, y, x;
  float s;

  sys_init();
  lcd_init();
  lcd_orientation(1);

  printfkomma= 3;
  setfont(1);
  textsize= 0;
  bkcolor= rgbfromvalue(0,20,0);
  textcolor= rgbfromega(14);
  clrscr();
  gotoxy(0,0); printf("STM32F030C8T6");

  textcolor= rgbfromvalue(0xff, 0xb6, 0x00);
  gotoxy(0,1); printf("Sinus-Cosinus Kurve");

  // zeichne Sinus- und Cosinus-Kurve
  line(0,10, 0, 230, rgbfromvalue(0,40,0));
  line(0,120, 319, 120, rgbfromvalue(0,40,0));
  for (w= 0; w< 360; w++)
  {
    s= 120.0f - (fk_sin(w * MY_PI / 180.0f)*95.0f);
    y= s;
    s= w * (320.0f / 360.0f);
    x= s;
    putpixel(x, y, rgbfromega(14));

    s= 120.0f - (fk_cos(w * MY_PI / 180.0f)*95.0f);
    y= s;
    putpixel(x, y, rgbfromega(12));
  }
  delay(3000);

  bkcolor= rgbfromega(0);
  textcolor= rgbfromega(14);
  clrscr();

  setfont(1);
  gotoxy(0,0); printf("STM32F030C8T6");

  // zeichne Spirographe
  while(1)
  {
    spiro_generate(56, 56, 140,512, rgbfromega(1));
    spiro_generate(56, 56, 140,512, rgbfromega(9));
    spiro_generate(56, 56, 140,512, rgbfromega(2));
    spiro_generate(56, 56, 140,512, rgbfromega(4));
    spiro_generate(56, 56, 140,512, rgbfromega(9));
  }
}
