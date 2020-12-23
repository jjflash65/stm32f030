/* -----------------------------------------------------
                        mathe_funcs.c

    Versuche und Loesungen fuer Mathefunktionen OHNE
    Bibliothek < math.h >

    Hardware  : STM32F030F4P6
                N5510 Display
    IDE       : keine (Editor / make)
    Library   : libopencm3
    Toolchain : arm-none-eabi

    28.09.2016   R. seelig
  ------------------------------------------------------ */


#include <stdint.h>
#include <stdarg.h>

#include <libopencm3.h>


#include "sysf030_init.h"
#include "uart.h"
#include "my_printf.h"

#define M_PI     3.14159265359f


#define printf   my_printf
#define itanz    5


const float lgin [] =
  { 2.0f, 1.1f, 1.01f, 1.001f, 1.0001f, 1.00001f };

const float lgout [] =
  { 0.301029995664f,    0.041392685158f,     0.00432137378264f,
    0.000434077479319f, 0.0000434272768627f, 0.00000434292310445f };


/* -----------------------------------------------------
                         fl_abs

     gibt den Absolutwert eines Floats zurueck (bspw.
     wird aus -12.8523 wird 12.853)
   ----------------------------------------------------- */
float fl_abs(float value)
{
  if (value < 0) return (-(value));
            else return value;
}


/* -----------------------------------------------------
                         fl_equal

     vergleicht 2 Floats auf gleiche Werteinhalte
   ----------------------------------------------------- */
bool fl_equal(float a, float b)
{
  const float epsilon = 1e-5;

  return fl_abs(a - b) < epsilon;
}

/* -----------------------------------------------------
                         tiny_sqrt

     Berechnung einer Quadratwurzel
   ----------------------------------------------------- */
float tiny_sqrt(float value)
{
  int cnt  = 0;
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
    cnt++;
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

     Berechnet value^n

     Fuer n sind nur Integerangaben erlaubt
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

     Berechnet einen Sinuswert
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
  if (sinx >= 0.9998f) { sinx= 1.0f; }

  if (mflag) sinx = sinx * (-1);
  return sinx;

}

/* -----------------------------------------------------
                      tiny_log10

     Berechnet den 10er-Logarithmus
   ----------------------------------------------------- */
float tiny_log10(float value)
{
  int b[] = { 0, 0, 0, 0, 0, 0 };
  float e = 0.0f;
  float c = 1.0f;
  float f;
  int   a;
  float addw = 0.0f;

  if (fl_abs(value) < 1e-13) { return 1e14; }
  if (fl_abs(value) > 1e+13) { return 1e14; }
  if (value== 1.0f) { return 1.0f; }
  if (value >= 10.0f)
  {
    while (value > 10.0f)
    {
      addw += 1.0f;
      value = value / 10.0f;
      if (value == 1.0f) { return addw; }
    }
  }
  else
  {
    if (value <= 1.0f)
    {
      while (value < 1.0f)
      {
        addw -= 1.0f;
        value = value * 10.0f;
        if (value == 1.0f) { return addw; }
      }
    }
  }


  for (a= 0; a<= itanz; a++)
  {
    while (c < value || ( fl_abs(c-value) < 1e-13) )
    {
      b[a]++;
      f= c;
      c= lgin[a] * c;
    }
    b[a]--;
    c= f;
    e= e + ( ((float) b[a]) * lgout[a] );
  }

  return ( e + addw );
}



/* -----------------------------------------------------
                         my_putchar

     wird von my_printf zur Zeichenausgabe benoetigt
   ----------------------------------------------------- */
void my_putchar(char c)
{
  uart_putchar(c);
}


/* -----------------------------------------------------
                           main
   ----------------------------------------------------- */
int main(void)
{
  uint16_t cnt;
  float    wuwert, f;
  int      wuwerti, i;

  sys_init();
  uart_init(19200);

  printfkomma= 4;

  printf("\n\rSinusberechnung:\n\n\r");
  for (cnt= 0; cnt < 360; cnt +=15)
  {
    wuwert= tiny_sin(cnt);
    wuwerti= wuwert*10000;

    printf("  sin(%d)= %k \n\r",cnt, wuwerti);

  }

  printfkomma= 3;

  printf("\n\rLogarithmusberechnung:\n\n\r");
  f= 0.017f;
  for (cnt= 0; cnt< 15; cnt++)
  {
    wuwert= tiny_log10(f);
    wuwerti= wuwert*1000;
    i= f*1000;
    printf("\n\r  log10(%k)= %k",i, wuwerti);
    f= f * 3.0f;
  }
  printf("\n\n\rEnde...");

  while(1);
}
