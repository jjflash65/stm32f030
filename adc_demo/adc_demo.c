/* -----------------------------------------------------
                        adc_demo.c

    Demoprogramm fuer die ADC - Benutzung des
    STM32F030F4P6 Controllers

    Hardware  : STM32F030F4P6
                10 kOhm Widerstand als PopUp an PA5

    IDE       : make - Projekt
    Library   : libopencm3
    Toolchain : arm-none-eabi

    27.02.2020   R. seelig
  ------------------------------------------------------ */


#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#include <libopencm3.h>

#include "sysf030_init.h"
#include "uart.h"
#include "adc.h"
#include "my_printf.h"

#define printf   my_printf

#define TEMP110_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1ffff7c2))
#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1ffff7b8))


void printlogo(void)
{
  printf("\n\r   ____ _____ __  __   _________  ");
  printf("\n\r  / ___|_   _|  \\/  | |___ /___ \\");
  printf("\n\r  \\___ \\ | | | |\\/| |   |_ \\ __) |");
  printf("\n\r   ___) || | | |  | |  ___) / __/ ");
  printf("\n\r  |____/ |_| |_|  |_| |____/____|");
}

/* --------------------------------------------------------
   putchar

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
  int  i;
  int  tval, val1, val2, mval, cnt;
  int  temperature;
  int  r;

  sys_init();
  uart_init(19200);
  adc_init(GPIO4 | GPIO5);                     // GPIO4 und GPIO5 als Analogeingaenge

  printfkomma= 1;

  printlogo();
  printf("\n\r  STM32F030F4P6 / 48 MHz 115200bd 8N1");
  printf("\n\r -------------------------------------\n\r");
  printf("\n\r  ADC-Test");
  printf("\n\r  Oktober 2016 R. Seelig \n\r");
  printf("\n\r -------------------------------------\n\n\r");

  printf("\n\n\r Temp110_cal: %d\n\r", *TEMP110_CAL_ADDR);
  printf(" Temp30_cal: %d\n\n\n\r", *TEMP30_CAL_ADDR);

  while(uart_ischar()) { uart_getchar();}  // eventuell eingegangene Zeichen alle loeschen

  cnt= 0;
  while (1)
  {

    // Mittelwert aus 100 Messungen bilden
    mval= 0;
    for (i= 0; i< 100; i++)
    {
      mval += adc_getchannel(16);
      delay(1);
    }
    tval= mval / 100;

    temperature = tval;

    // Temperaturrechnung lt. Referencemanual
    // Chiptemperatur ist ca. 7-10 Grad waermer als Umgebungstemperatur
    temperature= (temperature - *TEMP30_CAL_ADDR) * (110 - 30);
    temperature= temperature / (*TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR);
    temperature += 30;

    mval= 0;
    for (i= 0; i< 100; i++)
    {
      mval += adc_getchannel(4);
      delay(1);
    }
    val1= mval / 100;

    mval= 0;
    for (i= 0; i< 100; i++)
    {
      mval += adc_getchannel(5);
      delay(1);
    }
    val2= mval / 100;

    r= (100*val2) / (4096-val2);               // Spannungsteiler mit 10 kOhm PopUp Widerstand

    printf("\r Cnt: %d  Chip.Temp: %d  Tval: %d  / ADC_Value1: %d  ADC_Value2: %d   R: %kkOhm      ",cnt, temperature, tval, val1, val2, r);
    delay(500);
    cnt++;
  }
}
