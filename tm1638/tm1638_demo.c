/* ------------------------------------------------
                   tm1638b_demo.c


     01.02.2019
   ----------------------------------------------- */

#include <libopencm3.h>

#include "sysf030_init.h"
#include "tm1638.h"


#if (board_version == 1)

  const uint8_t lauflseq [20] =
  { 56, 48, 40, 32, 24, 16,  8,  0,
     5,  4,
     3, 11, 19, 27, 35, 43, 51, 59,
    58, 57 };

#endif


#if (board_version == 2)

  const uint8_t lauflseq [20] =
  { 0,  1,  2,  3,  4,  5,  6,  7,
   47, 39,
   31, 30, 29, 28, 27, 26, 25, 24,
   16,  8 };

#endif

/*  -----------------------------------------------------------------------------
                                          MAIN
    -----------------------------------------------------------------------------  */
int main(void)
{
  int8_t i;
  uint8_t leds;
  uint8_t keynr;

  sys_init();

  tm1638_init();
  tm1638_clear();
  tm1638_brightness = 3;

  // nur fuer Board 1, Board 2 hat keine Einzelleuchtdioden
  #if (board_version == 1)

    leds= 1;
    for (i= 0; i< 8; i++)
    {
      tm1638_setled(leds);
      delay(100);
      leds= (leds<< 1)+1;
    }
    leds= 1;
    for (i= 0; i< 8; i++)
    {
      tm1638_setled(~leds);
      delay(100);
      leds= (leds<< 1)+1;
    }
  #endif

  // Demo einzelne Segmente im Lauflicht auf- und abblenden
  fb1638_clr();
  for (i= 0; i< 20; i++)
  {
    leds= lauflseq[i];
    fb1638_putseg(leds,1);
    tm1638_showbuffer();
    delay(70);
  }
  for (i= 0; i< 20; i++)
  {
    leds= lauflseq[i];
    fb1638_putseg(leds,0);
    tm1638_showbuffer();
    delay(70);
  }

  delay(200);

  // "Hallo" Text einscrollen
  for (i= 0; i< 8; i++)
  {
    fb1638_clr();
    fb1638_puts("HALLO",i);
    tm1638_showbuffer();
    delay(200);
  }

  // und einen Dezimalpunkt nach "Hallo" setzen
  tm1638_setdp(3,1);
  delay(1300);
  tm1638_setdp(3,0);

  tm1638_setdez(12345678, 0, 1);
  delay(1000);

  while(1)
  {
    keynr= tm1638_readkeys();
    tm1638_setdez(keynr, 0, 1);

    #if (board_version == 1)

      if (keynr) tm1638_setled(1 << (7 - (keynr-1)));
            else tm1638_setled(0);
    #endif

    delay(80);
  }

}

