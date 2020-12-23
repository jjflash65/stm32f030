/* ------------------------------------------------
                      tftmono_demo.c

   Demoprogramm zur Verwendung mit
     - SSD1306 Displays (I2C und SPI Interface)
     - Nokia 5110 Displays (84x48)

   Displaytyp ist in tftmono.h einstellbar

   MCU   :  STM32F030
   Takt  :

   27.02.2020 by R. Seelig
  -------------------------------------------------- */


#include <stdlib.h>                             // fuer abs()
#include "sysf030_init.h"
#include "tftmono.h"

#include "my_printf.h"

#include "girl2.h"


#define printf       my_printf

#define demo_speed    2000


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

  sys_init();
  lcd_init();
  lcd_enable();

  #if (pcd8544 == 1)

    // Demo fuer N5110 Display
    fb_init(84, 6);
    while(1)
    {
      clrscr();
      setfont(fnt8x8);
      gotoxy(1,1);
      printf("Mono-TFT\n\r");
      setfont(fnt5x7);
      delay(demo_speed);
      clrscr();

      fb_clear();
      fb_outtextxy(1,22, 0, "Hallo Welt");

      fb_outtextxy(1,31, 0,"STM32F030");

      fb_show((_xres/2)-42, 0);
      delay(demo_speed);

      fb_clear();
      fillcircle(42,20, 17, 1);
      line(0,0, 83,39, 2);
      fb_show((_xres/2)-42, 0);
      delay(demo_speed);

      clrscr();
      showimage((_xres/2)-42, 2, &girl2_image[0], 2);           // Invertierte Ausgabe
      delay(demo_speed);
    }

  #else
    // Demo fuer SD1306 OLED Display (128x64)
    uint8_t ascii, x, y;

    setfont(fnt8x8);
    fb_init(127,8);
    fb_clear();

    fb_show(0,0);

    while(1)
    {
      fb_clear();
      fb_show(0,0);

      fillcircle(28,32,20,1);
      circle(60,39,24,2);
      fillrect(64,32,100,64,2);
      line(0,0,127,63,2);
      fb_show(0,0);

      gotoxy(1,0);
      printf("Framebuffer GFX");

      delay(demo_speed);

      clrscr();
      fb_clear();

      for (x= 0; x<128; x+= 8)
      {
        line(0,63, x,0, 1);
        fb_show(0,0);
      }

      for (y= 4; y< 64; y+= 8)
      {
        line(0,63, 127,y, 1);
        fb_show(0,0);
      }

      delay(demo_speed);

      fb_clear();
      line(0,10,126,10,1);

      fb_show(0,0);
      gotoxy(1,0);
      printf("SSD1306 - OLED");

      showimage(22,18, &girl2_image[0], 1);
      delay(demo_speed);

      fb_clear();
      line(0,10,127,10,1);
      fb_show(0,0);
      setfont(fnt8x8);
      gotoxy(0,0);
      printf("ASCII - Zeichen\n\n\r");
      for (ascii= 32; ascii< 128; ascii++)
      {
        my_putchar(ascii);
      }
      delay(demo_speed);
      setfont(fnt8x8);

      textsize= 0;
      clrscr();
      gotoxy(0,0);
      printf("Normalgroesse\n\r");
      textsize= 2;
      printf("\n\rGROSS");
      textsize= 0;
      printf("\n\n\rklein");
      delay(demo_speed);

    }


  #endif

}
