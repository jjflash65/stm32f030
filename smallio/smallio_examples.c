/* ----------------------------------------------------------
     smallio_examples.c

     Demoprogramm zu smallio.c

     Verfuegbarkeit von printf und der Ausgabe auf der
     seriellen Schnittstelle

    Hardware  : STM32F030
    IDE       : keine (Editor / make)
    Library   : libopencm3
    Toolchain : arm-none-eabi

    19.06.2019   R. seelig
   ---------------------------------------------------------- */


#include "smallio.h"            // einzig einzubindender Header, alle anderen
                                // benoetigten Header weden in smallio.h inkludiert


/* ---------------------------------------------------------------------------
                                    M A I N
   --------------------------------------------------------------------------- */
int main(void)
{
  int my_zahl;
  int16_t cnt;

  smallio_init();

  printf("\n\r Eingabe einer Integer Zahl: ");
  my_zahl= readint();
  printf("\n\r ... die Zahl war: %d\n\n\r", my_zahl);

  cnt= 0;
  while(1)
  {
    printf("\r Counter: %d      ",cnt);
    delay(1000);
    cnt++;
  }
}
