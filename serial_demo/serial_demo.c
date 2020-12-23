/* -----------------------------------------------------
                        serial_demo.c

    Demoprogramm fuer die Verwendung von USART1 des
    STM32F030F4P6 Controllers

    Hardware  : STM32F030F4P6
    IDE       : make - Projekt
    Library   : libopencm3
    Toolchain : arm-none-eabi

    27.02.2020   R. Seelig
  ------------------------------------------------------ */


#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#include <libopencm3.h>

#include "sysf030_init.h"
#include "uart.h"
#include "my_printf.h"


#define led_init()     ( PA5_output_init() )
#define led_on()       ( PA5_clr() )            // ... gegen +Vcc geschaltet
#define led_off()      ( PA5_set() )

#define tast_init()    ( PA6_input_init() )
#define is_tast()      ( is_PA6() )

#define printf   my_printf

#define M_PI     3.14159265359f

/* -----------------------------------------------------
                         f1_equal

     testet, ob 2 Gleitkommazahlen gleich gross sind.
   ----------------------------------------------------- */
bool fl_equal(float a, float b)
{
  const float epsilon = 1e-5;

  return abs(a - b) < epsilon;
}

/* -----------------------------------------------------
                         tiny_sqrt

     eigene, kuerzere, Implementierung einer Quadrat-
     wurzelfunktion (allerdings ungenauer als die
     Definition in math.h)
   ----------------------------------------------------- */
float tiny_sqrt(float value)
{
  int counter  = 0;
  float difval = value;
  float grenze = difval / 2;
  float quaval = difval * difval;
  uint8_t domul = 0;
  uint8_t dozmul = 0;

  if (value< 10)
  {
    value = value * 10000;
    dozmul = 1;
  }

  if ( (value< 100) && !(dozmul) )
  {
    value = value * 100;
    domul = 1;
  }

  while (!fl_equal(quaval,value))
  {
    if (quaval < value)
    {
      difval = difval + grenze;
    }
    else
    {
      difval = difval - grenze;
      grenze /= 2;
    }
    counter++;
    quaval = difval * difval;
  }
  if (domul)
  {
    difval= difval / 10;
    return difval;
  }
  if (dozmul)
  {
    difval= difval / 100;
  }
  return difval;
}


/* -----------------------------------------------------
                         tiny_pow

     eigene, kurze, Implementierung einer Exponential-
     funktion. Dafuer darf ist value^n n nur ein
     Integerwert erlaubt.
   ----------------------------------------------------- */
float tiny_pow(int n, float value)
{
  float tmp;

  tmp= value;
  for (int i= 0; i < n-1; i++)
  {
    tmp= tmp*value;
  }
  return tmp;
}

/* -----------------------------------------------------
                         tiny_sin

     eigene, kurze, Implementierung einer Sinusfunktion.

     Kuerzer, aber ungenauer als die in math.h
   ----------------------------------------------------- */
float tiny_sin(float value)
{

  float degree;
  float p3;
  float p5;
  float p7;
  float sinx;

  int   mflag= 0;

  while (value > 360.0f) value -= 360.0f;
  if (value > 180.0f)
  {
    mflag= - 1;
    value -= 180.0f;
  }

  if (value > 90.0f) value = 180.0f - value;

  degree= (value * M_PI) / 180.0f;

  p3 = tiny_pow(3, degree);
  p5  = tiny_pow(5, degree);
  p7  = tiny_pow(7, degree);

  sinx= (degree - (p3/6.0f) + (p5/120.0f) - (p7/5040.0f));

  if (mflag) sinx = sinx * (-1);
  return sinx;

}

/* -----------------------------------------------------
                         draw_sincurve

     zeichnet eine Sinuskurve aus Textzeichen
   ----------------------------------------------------- */
void draw_sincurve(void)
{
  for (int i= 0; i< 361; i += 10)
  {
    int x2;

    x2 = (tiny_sin(i) * 24) + 25;
    for (int x= 0; x < x2; x++) uart_putchar(' ');
    printf("o\n\r");
    delay(60);
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
  uart_putchar(ch);
}


/* --------------------------------------------------------
                              main
   -------------------------------------------------------- */
int main(void)
{
  uint16_t counter = 0;
  uint16_t ch;
  float    wuwert;
  int      wuwerti;

  sys_init();

  led_init();
  tast_init();

  uart_init(19200);

  printfkomma= 3;

  printf("\n\r-------------------------------------\n\r");
  printf("\n\r STM32F030F4P6 / %d MHz 115200bd 8N1",  rcc_ahb_frequency/1000000);
  printf("\n\r Januar 2017 R. Seelig \n\r");
  printf("\n\r APB = %d MHz",  rcc_apb1_frequency/1000000);
  printf("\n\r AHB = %d MHz",  rcc_ahb_frequency/1000000);
  printf("\n\r-------------------------------------\n\r");
  printf("\n\rEin Integer       : %d",82137988);
  printf("\n\rEine Festkommazahl: %k",82137988);
  printf("\n\rEin 32-Bit Hexwert: %x",0xca4b3a4c);
  printf("\n\rTaste fuer Counterstop... \n\n\r");

//  draw_sincurve();
  while(uart_ischar()) {ch= uart_getchar(); my_putchar(ch); }  // eventuell eingegangene Zeichen alle loeschen

  while (1)
  {
    led_off();
    delay(500);
    led_on();

    wuwert= tiny_sin(counter);
    wuwerti= wuwert*1000;

    printf("  Counter: %xh sin(%d)= %k Taster= ", counter, counter, wuwerti);

    if (is_tast()) printf("1     \r");
            else printf("0     \r");

    delay(500);
    counter++;
    counter = counter % 3600;

    if (uart_ischar())
    {
      ch= uart_getchar();
      printf("\n\n\rGedrueckte Taste war: %c\n\r", ch );
      printf("Beliebige Taste fuer Counterstart...\n\n\r");
      ch= uart_getchar();
      draw_sincurve();

    }

  }
}
