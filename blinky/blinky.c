/* -----------------------------------------------------
                        blinky.c

    Blinky fuer STM32F030F4P6 Controller

    Hardware  : STM32F030F4P6
    IDE       : keine (Editor / make)
    Library   : libopencm3
    Toolchain : arm-none-eabi

    27.09.2016   R. seelig
  ------------------------------------------------------ */


#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#include <libopencm3.h>
#include "sysf030_init.h"


#define led_init()   PA4_output_init()
#define led_on()     PA4_set()                // gegen +Ub geschaltet
#define led_off()    PA4_clr()


#define bl_speed    1000

int main(void)
{
  sys_init();
  led_init();

  while (1)
  {
    led_on();
    delay(bl_speed);

    led_off();
    delay(bl_speed);
  }
}
