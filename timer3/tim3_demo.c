/* ------------------------------------------------------------
                          tim3_demo.c

    Demonstriert die Verwendung von Timer3

    Hardware  : STM32F030F4P6
                LCD 128x128 / 160x128 Pixel
    IDE       : keine (Editor / make)
    Library   : libopencm3
    Toolchain : arm-none-eabi

    02.03.2030   R. Seelig
   ------------------------------------------------------------ */


#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libopencm3.h>

#include "sysf030_init.h"
#include "my_printf.h"
#include "tftdisplay.h"

#define led_init() ( PA3_output_init() )
#define led_port   GPIOA
#define led        GPIO3

#define printf     my_printf


volatile uint32_t my_ticker;
volatile char    halfsek;

/* ------------------------------------------------------------
                            tim_isr

     Interrupt-Service fuer Timer3
   ------------------------------------------------------------ */
void tim3_isr(void)
{
  TIM_SR(TIM3) &= ~TIM_SR_UIF;
  my_ticker++;
  if (!(my_ticker % 500))
  {
    if (halfsek) { halfsek= 0; } else {halfsek= 1;}
  }
}

/* ------------------------------------------------------------
                          tim3_init

     Initialisiert Timer3 fuer einen Interruptaufruf jede
     Millisekunde (gilt fuer Taktfrequenz = 48 MHz)
   ------------------------------------------------------------ */
static void tim3_init(void)
{
  rcc_periph_clock_enable(RCC_TIM3);

  timer_reset(TIM3);
  timer_set_prescaler(TIM3, 4800);
  timer_set_period(TIM3, 10);
  nvic_enable_irq(NVIC_TIM3_IRQ);
  timer_enable_update_event(TIM3);
  timer_enable_irq(TIM3, TIM_DIER_UIE);
  timer_enable_counter(TIM3);
}

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


int main(void)
{
  int cnt = 0;

  sys_init();
  lcd_init();
  lcd_enable();

  tim3_init();
  led_init();

  bkcolor= rgbfromega(1);
  textcolor= rgbfromega(15);

  clrscr();
  gotoxy(1,1); printf("Timer3 - Demo\n\r --------------");

  textcolor= rgbfromega(14);
  while(1)
  {

    while(halfsek);

    gotoxy(3,4);
    printf("cnt= %d ",cnt);
    gpio_toggle(led_port, led);
    cnt++;

    while(!(halfsek));
  }

}
