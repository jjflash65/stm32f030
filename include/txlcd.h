/* -------------------------------------------------------
                         txlcd.h

     Library fuer HD44780 kompatible Displays

     MCU   :  STM32F030F4P6
     Takt  :  interner Takt

     21.09.2016  R. Seelig
   ------------------------------------------------------ */

/*
      Anschluss am Bsp. Pollin-Display C0802-04

      ---------------------------------------------------
         o +5V
         |                            Display            STM32f030F4P6  Controller
         _                        Funktion   PIN               PIN       Funktion
        | |
        |_| 1,8k                     GND      1 ------------
         |                          +5V       2 ------------
         o----o Kontrast   ---    Kontrast    3 ------------
         |                            RS      4 ------------    6          PA0
         _                           GND      5 ------------
        | |                    (Takt) E       6 ------------    7          PA1
        |_| 150                      D4       7 ------------    8          PA2
         |                           D5       8 ------------    9          PA3
        ---  GND                     D6       9 ------------   10          PA4
                                     D7      10 ------------   11          PA5


         Portpins des Controllers MUESSEN Pop-Up Widerstaende 10 kOhm an
         +5V angeschlossen haben !!!
*/

#ifndef in_txlcd
  #define in_txlcd

  #include <stdint.h>
  #include <stdlib.h>

  #include <libopencm3.h>


  /* ---------------------------------------
          I/O Macros fuer das Display
     --------------------------------------- */

  #define txlcd_rs_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0) )
  #define txlcd_e_init()    ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1) )
  #define txlcd_d4_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2) )
  #define txlcd_d5_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3) )
  #define txlcd_d6_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4) )
  #define txlcd_d7_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5) )

  #define txlcd_rs_set()    ( gpio_set(GPIOA, GPIO0 ) )
  #define txlcd_e_set()     ( gpio_set(GPIOA, GPIO1 ) )
  #define txlcd_d4_set()    ( gpio_set(GPIOA, GPIO2 ) )
  #define txlcd_d5_set()    ( gpio_set(GPIOA, GPIO3 ) )
  #define txlcd_d6_set()    ( gpio_set(GPIOA, GPIO4 ) )
  #define txlcd_d7_set()    ( gpio_set(GPIOA, GPIO5 ) )

  #define txlcd_rs_clr()    ( gpio_clear(GPIOA, GPIO0 ) )
  #define txlcd_e_clr()     ( gpio_clear(GPIOA, GPIO1 ) )
  #define txlcd_d4_clr()    ( gpio_clear(GPIOA, GPIO2 ) )
  #define txlcd_d5_clr()    ( gpio_clear(GPIOA, GPIO3 ) )
  #define txlcd_d6_clr()    ( gpio_clear(GPIOA, GPIO4 ) )
  #define txlcd_d7_clr()    ( gpio_clear(GPIOA, GPIO5 ) )

  #define testbit(reg,pos) ((reg) & (1<<pos))               // testet an der Bitposition pos das Bit auf 1 oder 0


  void nibbleout(unsigned char wert, unsigned char hilo);
  void txlcd_takt(void);
  void txlcd_io(char wert);
  void txlcd_pininit(void);
  void txlcd_init(void);
  void gotoxy(char x, char y);
  void txlcd_setuserchar(char nr, const char *userchar);
  void txlcd_putchar(char ch);

#endif
