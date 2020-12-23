/* -------------------------------------------------------------
                         sysf030_init.h

   initialisiert MCU STM32F030F4P6 fuer einen "allgemeinen"
   Gebrauch.

    Hardware  : STM32F030F4P6
    IDE       : keine (Editor / make)
    Library   : libopencm3
    Toolchain : arm-none-eabi

    28.09.2016   R. seelig

   ------------------------------------------------------------- */

#ifndef in_sys_init
  #define in_sys_init

  #include <stdint.h>
  #include <stdlib.h>
  #include <stdarg.h>

  #include <libopencm3.h>

  // GPIO-Aliase fuer einfachere Initialisierungen
/* -------------------------------------------------------------
                               GPIOA
   ------------------------------------------------------------- */

  #define PA0_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0) )
  #define PA0_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO0) )
  #define PA0_set()          ( gpio_set(GPIOA, GPIO0) )
  #define PA0_clr()          ( gpio_clear(GPIOA, GPIO0) )
  #define is_PA0()           ( (gpio_get(GPIOA, GPIO0)) )

  #define PA1_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1) )
  #define PA1_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO1) )
  #define PA1_set()          ( gpio_set(GPIOA, GPIO1) )
  #define PA1_clr()          ( gpio_clear(GPIOA, GPIO1) )
  #define is_PA1()           ( (gpio_get(GPIOA, GPIO1)) )

  #define PA2_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2) )
  #define PA2_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO2) )
  #define PA2_set()          ( gpio_set(GPIOA, GPIO2) )
  #define PA2_clr()          ( gpio_clear(GPIOA, GPIO2) )
  #define is_PA2()           ( (gpio_get(GPIOA, GPIO2)) )

  #define PA3_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3) )
  #define PA3_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO3) )
  #define PA3_set()          ( gpio_set(GPIOA, GPIO3) )
  #define PA3_clr()          ( gpio_clear(GPIOA, GPIO3) )
  #define is_PA3()           ( (gpio_get(GPIOA, GPIO3)) )

  #define PA4_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4) )
  #define PA4_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO4) )
  #define PA4_set()          ( gpio_set(GPIOA, GPIO4) )
  #define PA4_clr()          ( gpio_clear(GPIOA, GPIO4) )
  #define is_PA4()           ( (gpio_get(GPIOA, GPIO4)) )

  #define PA5_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5) )
  #define PA5_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO5) )
  #define PA5_set()          ( gpio_set(GPIOA, GPIO5) )
  #define PA5_clr()          ( gpio_clear(GPIOA, GPIO5) )
  #define is_PA5()           ( (gpio_get(GPIOA, GPIO5)) )

  #define PA6_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6) )
  #define PA6_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO6) )
  #define PA6_set()          ( gpio_set(GPIOA, GPIO6) )
  #define PA6_clr()          ( gpio_clear(GPIOA, GPIO6) )
  #define is_PA6()           ( (gpio_get(GPIOA, GPIO6)) )

  #define PA7_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7) )
  #define PA7_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO7) )
  #define PA7_set()          ( gpio_set(GPIOA, GPIO7) )
  #define PA7_clr()          ( gpio_clear(GPIOA, GPIO7) )
  #define is_PA7()           ( (gpio_get(GPIOA, GPIO7)) )

  #define PA8_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8) )
  #define PA8_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO8) )
  #define PA8_set()          ( gpio_set(GPIOA, GPIO8) )
  #define PA8_clr()          ( gpio_clear(GPIOA, GPIO8) )
  #define is_PA8()           ( (gpio_get(GPIOA, GPIO8)) )

  #define PA9_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9) )
  #define PA9_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO9) )
  #define PA9_set()          ( gpio_set(GPIOA, GPIO9) )
  #define PA9_clr()          ( gpio_clear(GPIOA, GPIO9) )
  #define is_PA9()           ( (gpio_get(GPIOA, GPIO9)) )

  #define PA10_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10) )
  #define PA10_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO10) )
  #define PA10_set()          ( gpio_set(GPIOA, GPIO10) )
  #define PA10_clr()          ( gpio_clear(GPIOA, GPIO10) )
  #define is_PA10()           ( (gpio_get(GPIOA, GPIO10)) )

  #define PA11_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO11) )
  #define PA11_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO11) )
  #define PA11_set()          ( gpio_set(GPIOA, GPIO11) )
  #define PA11_clr()          ( gpio_clear(GPIOA, GPIO11) )
  #define is_PA11()           ( (gpio_get(GPIOA, GPIO11)) )

  #define PA12_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12) )
  #define PA12_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO12) )
  #define PA12_set()          ( gpio_set(GPIOA, GPIO12) )
  #define PA12_clr()          ( gpio_clear(GPIOA, GPIO12) )
  #define is_PA12()           ( (gpio_get(GPIOA, GPIO12)) )

  #define PA13_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13) )
  #define PA13_input_init()    ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO13) )
  #define PA13_set()           ( gpio_set(GPIOA, GPIO13) )
  #define PA13_clr()           ( gpio_clear(GPIOA, GPIO13) )
  #define is_PA13()            ( (gpio_get(GPIOA, GPIO13)) )

  #define PA14_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14) )
  #define PA14_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO14) )
  #define PA14_set()          ( gpio_set(GPIOA, GPIO14) )
  #define PA14_clr()          ( gpio_clear(GPIOA, GPIO14) )
  #define is_PA14()           ( (gpio_get(GPIOA, GPIO14)) )

  #define PA15_output_init()  ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15) )
  #define PA15_input_init()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO15) )
  #define PA15_set()          ( gpio_set(GPIOA, GPIO15) )
  #define PA15_clr()          ( gpio_clear(GPIOA, GPIO15) )
  #define is_PA15()           ( (gpio_get(GPIOA, GPIO15)) )

/* -------------------------------------------------------------
                               GPIOB
   ------------------------------------------------------------- */

  #define PB0_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0) )
  #define PB0_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO0) )
  #define PB0_set()          ( gpio_set(GPIOB, GPIO0) )
  #define PB0_clr()          ( gpio_clear(GPIOB, GPIO0) )
  #define is_PB0()           ( (gpio_get(GPIOB, GPIO0)) )

  #define PB1_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1) )
  #define PB1_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO1) )
  #define PB1_set()          ( gpio_set(GPIOB, GPIO1) )
  #define PB1_clr()          ( gpio_clear(GPIOB, GPIO1) )
  #define is_PB1()           ( (gpio_get(GPIOB, GPIO1)) )

  #define PB2_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2) )
  #define PB2_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO2) )
  #define PB2_set()          ( gpio_set(GPIOB, GPIO2) )
  #define PB2_clr()          ( gpio_clear(GPIOB, GPIO2) )
  #define is_PB2()           ( (gpio_get(GPIOB, GPIO2)) )

  #define PB3_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3) )
  #define PB3_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO3) )
  #define PB3_set()          ( gpio_set(GPIOB, GPIO3) )
  #define PB3_clr()          ( gpio_clear(GPIOB, GPIO3) )
  #define is_PB3()           ( (gpio_get(GPIOB, GPIO3)) )

  #define PB4_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4) )
  #define PB4_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO4) )
  #define PB4_set()          ( gpio_set(GPIOB, GPIO4) )
  #define PB4_clr()          ( gpio_clear(GPIOB, GPIO4) )
  #define is_PB4()           ( (gpio_get(GPIOB, GPIO4)) )

  #define PB5_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5) )
  #define PB5_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO5) )
  #define PB5_set()          ( gpio_set(GPIOB, GPIO5) )
  #define PB5_clr()          ( gpio_clear(GPIOB, GPIO5) )
  #define is_PB5()           ( (gpio_get(GPIOB, GPIO5)) )

  #define PB6_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6) )
  #define PB6_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO6) )
  #define PB6_set()          ( gpio_set(GPIOB, GPIO6) )
  #define PB6_clr()          ( gpio_clear(GPIOB, GPIO6) )
  #define is_PB6()           ( (gpio_get(GPIOB, GPIO6)) )

  #define PB7_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7) )
  #define PB7_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO7) )
  #define PB7_set()          ( gpio_set(GPIOB, GPIO7) )
  #define PB7_clr()          ( gpio_clear(GPIOB, GPIO7) )
  #define is_PB7()           ( (gpio_get(GPIOB, GPIO7)) )

  #define PB8_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8) )
  #define PB8_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO8) )
  #define PB8_set()          ( gpio_set(GPIOB, GPIO8) )
  #define PB8_clr()          ( gpio_clear(GPIOB, GPIO8) )
  #define is_PB8()           ( (gpio_get(GPIOB, GPIO8)) )

  #define PB9_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9) )
  #define PB9_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO9) )
  #define PB9_set()          ( gpio_set(GPIOB, GPIO9) )
  #define PB9_clr()          ( gpio_clear(GPIOB, GPIO9) )
  #define is_PB9()           ( (gpio_get(GPIOB, GPIO9)) )

  #define PB10_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10) )
  #define PB10_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO10) )
  #define PB10_set()          ( gpio_set(GPIOB, GPIO10) )
  #define PB10_clr()          ( gpio_clear(GPIOB, GPIO10) )
  #define is_PB10()           ( (gpio_get(GPIOB, GPIO10)) )

  #define PB11_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO11) )
  #define PB11_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO11) )
  #define PB11_set()          ( gpio_set(GPIOB, GPIO11) )
  #define PB11_clr()          ( gpio_clear(GPIOB, GPIO11) )
  #define is_PB11()           ( (gpio_get(GPIOB, GPIO11)) )

  #define PB12_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12) )
  #define PB12_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO12) )
  #define PB12_set()          ( gpio_set(GPIOB, GPIO12) )
  #define PB12_clr()          ( gpio_clear(GPIOB, GPIO12) )
  #define is_PB12()           ( (gpio_get(GPIOB, GPIO12)) )

  #define PB13_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13) )
  #define PB13_input_init()    ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO13) )
  #define PB13_set()           ( gpio_set(GPIOB, GPIO13) )
  #define PB13_clr()           ( gpio_clear(GPIOB, GPIO13) )
  #define is_PB13()            ( (gpio_get(GPIOB, GPIO13)) )

  #define PB14_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14) )
  #define PB14_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO14) )
  #define PB14_set()          ( gpio_set(GPIOB, GPIO14) )
  #define PB14_clr()          ( gpio_clear(GPIOB, GPIO14) )
  #define is_PB14()           ( (gpio_get(GPIOB, GPIO14)) )

  #define PB15_output_init()  ( gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15) )
  #define PB15_input_init()   ( gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO15) )
  #define PB15_set()          ( gpio_set(GPIOB, GPIO15) )
  #define PB15_clr()          ( gpio_clear(GPIOB, GPIO15) )
  #define is_PB15()           ( (gpio_get(GPIOB, GPIO15)) )

/* -------------------------------------------------------------
                               GPIOC
   ------------------------------------------------------------- */

  #define PC0_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0) )
  #define PC0_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO0) )
  #define PC0_set()          ( gpio_set(GPIOC, GPIO0) )
  #define PC0_clr()          ( gpio_clear(GPIOC, GPIO0) )
  #define is_PC0()           ( (gpio_get(GPIOC, GPIO0)) )

  #define PC1_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1) )
  #define PC1_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO1) )
  #define PC1_set()          ( gpio_set(GPIOC, GPIO1) )
  #define PC1_clr()          ( gpio_clear(GPIOC, GPIO1) )
  #define is_PC1()           ( (gpio_get(GPIOC, GPIO1)) )

  #define PC2_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2) )
  #define PC2_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO2) )
  #define PC2_set()          ( gpio_set(GPIOC, GPIO2) )
  #define PC2_clr()          ( gpio_clear(GPIOC, GPIO2) )
  #define is_PC2()           ( (gpio_get(GPIOC, GPIO2)) )

  #define PC3_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3) )
  #define PC3_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO3) )
  #define PC3_set()          ( gpio_set(GPIOC, GPIO3) )
  #define PC3_clr()          ( gpio_clear(GPIOC, GPIO3) )
  #define is_PC3()           ( (gpio_get(GPIOC, GPIO3)) )

  #define PC4_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4) )
  #define PC4_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO4) )
  #define PC4_set()          ( gpio_set(GPIOC, GPIO4) )
  #define PC4_clr()          ( gpio_clear(GPIOC, GPIO4) )
  #define is_PC4()           ( (gpio_get(GPIOC, GPIO4)) )

  #define PC5_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5) )
  #define PC5_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO5) )
  #define PC5_set()          ( gpio_set(GPIOC, GPIO5) )
  #define PC5_clr()          ( gpio_clear(GPIOC, GPIO5) )
  #define is_PC5()           ( (gpio_get(GPIOC, GPIO5)) )

  #define PC6_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6) )
  #define PC6_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO6) )
  #define PC6_set()          ( gpio_set(GPIOC, GPIO6) )
  #define PC6_clr()          ( gpio_clear(GPIOC, GPIO6) )
  #define is_PC6()           ( (gpio_get(GPIOC, GPIO6)) )

  #define PC7_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7) )
  #define PC7_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO7) )
  #define PC7_set()          ( gpio_set(GPIOC, GPIO7) )
  #define PC7_clr()          ( gpio_clear(GPIOC, GPIO7) )
  #define is_PC7()           ( (gpio_get(GPIOC, GPIO7)) )

  #define PC8_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8) )
  #define PC8_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO8) )
  #define PC8_set()          ( gpio_set(GPIOC, GPIO8) )
  #define PC8_clr()          ( gpio_clear(GPIOC, GPIO8) )
  #define is_PC8()           ( (gpio_get(GPIOC, GPIO8)) )

  #define PC9_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9) )
  #define PC9_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO9) )
  #define PC9_set()          ( gpio_set(GPIOC, GPIO9) )
  #define PC9_clr()          ( gpio_clear(GPIOC, GPIO9) )
  #define is_PC9()           ( (gpio_get(GPIOC, GPIO9)) )

  #define PC10_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10) )
  #define PC10_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO10) )
  #define PC10_set()          ( gpio_set(GPIOC, GPIO10) )
  #define PC10_clr()          ( gpio_clear(GPIOC, GPIO10) )
  #define is_PC10()           ( (gpio_get(GPIOC, GPIO10)) )

  #define PC11_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO11) )
  #define PC11_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO11) )
  #define PC11_set()          ( gpio_set(GPIOC, GPIO11) )
  #define PC11_clr()          ( gpio_clear(GPIOC, GPIO11) )
  #define is_PC11()           ( (gpio_get(GPIOC, GPIO11)) )

  #define PC12_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12) )
  #define PC12_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO12) )
  #define PC12_set()          ( gpio_set(GPIOC, GPIO12) )
  #define PC12_clr()          ( gpio_clear(GPIOC, GPIO12) )
  #define is_PC12()           ( (gpio_get(GPIOC, GPIO12)) )

  #define PC13_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13) )
  #define PC13_input_init()    ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO13) )
  #define PC13_set()           ( gpio_set(GPIOC, GPIO13) )
  #define PC13_clr()           ( gpio_clear(GPIOC, GPIO13) )
  #define is_PC13()            ( (gpio_get(GPIOC, GPIO13)) )

  #define PC14_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14) )
  #define PC14_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO14) )
  #define PC14_set()          ( gpio_set(GPIOC, GPIO14) )
  #define PC14_clr()          ( gpio_clear(GPIOC, GPIO14) )
  #define is_PC14()           ( (gpio_get(GPIOC, GPIO14)) )

  #define PC15_output_init()  ( gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15) )
  #define PC15_input_init()   ( gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO15) )
  #define PC15_set()          ( gpio_set(GPIOC, GPIO15) )
  #define PC15_clr()          ( gpio_clear(GPIOC, GPIO15) )
  #define is_PC15()           ( (gpio_get(GPIOC, GPIO15)) )


  // globale Variable

  extern volatile int tick_ms;            // wird durch den System-Ticker hochgezaehlt

  // Prototypen

  void sys_tick_handler(void);
  void delay(int c);
  void systick_setup(void);
  void sys_init_extclk(void);
  void gpio_clkon(void);
  void sys_init(void);

#endif
