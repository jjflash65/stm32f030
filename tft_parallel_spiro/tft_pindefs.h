/* -----------------------------------------------------------------------------------
                                     tft_pindefs.h

                     Pinbelegungen TFT-Display zu Controller


     28.01.2019  R. Seelig
   ----------------------------------------------------------------------------------- */

#ifndef in_tft_pindefs
  #define in_tft_pindefs

  #include "tftdisplay.h"

  #if (USE_SPI_TFT == 1)

    /*  ------------------------------------------------------------
                        Pinbelegung fuer SPI-Displays
        ------------------------------------------------------------ */

    //  PA4 = NSS, PA5 = SCK, PA6 = MISO, PA7 = MOSI, alternate function fuer SPI = AF0
    #define spi_ss     GPIO4
    #define spi_sck    GPIO5
    #define spi_miso   GPIO6
    #define spi_mosi   GPIO7

    #if (pindefs == 1)

      #define lcd_clk    spi_sck        // PA5
      #define lcd_din    spi_mosi       // PA7
      #define lcd_ce     spi_ss         // PA4
      #define lcd_dc     GPIO3          // PA3
      #define lcd_rst    GPIO2          // PA2

      #define lcd_pin_mask      ( lcd_dc | lcd_rst | lcd_ce)
      #define lcd_pin_init()    ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, lcd_pin_mask) )

      #define dc_set()          ( gpio_set(GPIOA, lcd_dc) )
      #define dc_clr()          ( gpio_clear(GPIOA, lcd_dc) )
      #define ce_set()          ( gpio_set(GPIOA, lcd_ce) )
      #define ce_clr()          ( gpio_clear(GPIOA, lcd_ce) )
      #define rst_set()         ( gpio_set(GPIOA, lcd_rst) )
      #define rst_clr()         ( gpio_clear(GPIOA, lcd_rst) )
      #define lcd_enable()      ( ce_clr() )
      #define lcd_disable()     ( ce_set() )

    #endif

    #if (pindefs == 2)

      #define lcd_clk    spi_sck        // PA5
      #define lcd_din    spi_mosi       // PA7
      #define lcd_ce     spi_ss         // PA4
      #define lcd_dc     GPIO3          // PA3
      #define lcd_rst    GPIO1          // PB1

      #define lcd_pin_mask      ( lcd_dc | lcd_ce)
      #define lcd_pin_init()    { gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, lcd_pin_mask);  \
                                  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, lcd_rst);   }


      #define dc_set()          ( gpio_set(GPIOA, lcd_dc) )
      #define dc_clr()          ( gpio_clear(GPIOA, lcd_dc) )
      #define ce_set()          ( gpio_set(GPIOA, lcd_ce) )
      #define ce_clr()          ( gpio_clear(GPIOA, lcd_ce) )
      #define rst_set()         ( gpio_set(GPIOB, lcd_rst) )
      #define rst_clr()         ( gpio_clear(GPIOB, lcd_rst) )
      #define lcd_enable()      ( ce_clr() )
      #define lcd_disable()     ( ce_set() )

    #endif                                      // pindefs

    #if (pindefs == 3)

      #define lcd_clk    spi_sck        // PA5
      #define lcd_din    spi_mosi       // PA7
      #define lcd_ce     spi_ss         // PA4
      #define lcd_dc     GPIO0          // PA0
      #define lcd_rst    GPIO1          // PA1

      #define lcd_pin_mask      ( lcd_dc | lcd_rst | lcd_ce)
      #define lcd_pin_init()    ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, lcd_pin_mask) )

      #define dc_set()          ( gpio_set(GPIOA, lcd_dc) )
      #define dc_clr()          ( gpio_clear(GPIOA, lcd_dc) )
      #define ce_set()          ( gpio_set(GPIOA, lcd_ce) )
      #define ce_clr()          ( gpio_clear(GPIOA, lcd_ce) )
      #define rst_set()         ( gpio_set(GPIOA, lcd_rst) )
      #define rst_clr()         ( gpio_clear(GPIOA, lcd_rst) )
      #define lcd_enable()      ( ce_clr() )
      #define lcd_disable()     ( ce_set() )

    #endif

    #if (pindefs == 4)

      #define lcd_clk    spi_sck        // PA5
      #define lcd_din    spi_mosi       // PA7
      #define lcd_ce     GPIO13         // PA13
      #define lcd_dc     GPIO1          // PB1
      #define lcd_rst    GPIO14         // PA14

      #define lcd_pin_mask      ( lcd_rst | lcd_ce)
      #define lcd_pin_init()    { gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, lcd_pin_mask);  \
                                  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, lcd_dc);   }


      #define dc_set()          ( gpio_set(GPIOB, lcd_dc) )
      #define dc_clr()          ( gpio_clear(GPIOB, lcd_dc) )
      #define ce_set()          ( gpio_set(GPIOA, lcd_ce) )
      #define ce_clr()          ( gpio_clear(GPIOA, lcd_ce) )
      #define rst_set()         ( gpio_set(GPIOA, lcd_rst) )
      #define rst_clr()         ( gpio_clear(GPIOA, lcd_rst) )
      #define lcd_enable()      ( ce_clr() )
      #define lcd_disable()     ( ce_set() )

    #endif

    #if (pindefs == 5)

      #define lcd_clk    spi_sck        // PA5
      #define lcd_din    spi_mosi       // PA7
      #define lcd_ce     GPIO6          // PB6
      #define lcd_dc     GPIO15         // PA15
      #define lcd_rst    GPIO8          // PA8

      #define lcd_pin_init()    { gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, lcd_ce );  \
                                  gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, lcd_rst | lcd_dc);  }

      #define dc_set()          ( gpio_set(GPIOA, lcd_dc) )
      #define dc_clr()          ( gpio_clear(GPIOA, lcd_dc) )

      #define ce_set()          ( gpio_set(GPIOB, lcd_ce) )
      #define ce_clr()          ( gpio_clear(GPIOB, lcd_ce) )

      #define rst_set()         ( gpio_set(GPIOA, lcd_rst) )
      #define rst_clr()         ( gpio_clear(GPIOA, lcd_rst) )

      #define lcd_enable()      ( ce_clr() )
      #define lcd_disable()     ( ce_set() )

    #endif                      // pindefs

    #if (pindefs == 6)

      #define lcd_clk    spi_sck        // PA5
      #define lcd_din    spi_mosi       // PA7
      #define lcd_ce     GPIO6          // PB6
      #define lcd_dc     GPIO7          // PB7
      #define lcd_rst    GPIO13         // PB13

      #define lcd_pin_init()    { gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, lcd_ce | lcd_dc | lcd_rst);  }

      #define dc_set()          ( gpio_set(GPIOB, lcd_dc) )
      #define dc_clr()          ( gpio_clear(GPIOB, lcd_dc) )

      #define ce_set()          ( gpio_set(GPIOB, lcd_ce) )
      #define ce_clr()          ( gpio_clear(GPIOB, lcd_ce) )

      #define rst_set()         ( gpio_set(GPIOB, lcd_rst) )
      #define rst_clr()         ( gpio_clear(GPIOB, lcd_rst) )

      #define lcd_enable()      ( ce_clr() )
      #define lcd_disable()     ( ce_set() )

    #endif                      // pindefs

  #endif               // USE_SPI_TFT

  #if (USE_8BIT_TFT == 1)
    /*  ------------------------------------------------------------
          Pinbelegung fuer TFT-Displays mit 8-Bit Parallelinterface
        ------------------------------------------------------------ */

    #define intbit(wert,nr)    ((wert) & (1<<nr))
    #define setintbit(wert,nr) (wert |= (1<<nr))
    #define clrintbit(wert,nr) (wert &= ~(1<<nr))

    //  -------------------------------------------------------------
    //    Nucleo Board
    //  -------------------------------------------------------------
    #if (boardversion == 0)                                       //   originales Nucleo Board

      #define lcd_d0_init()   PB12_output_init()
      #define lcd_d0_set()    PB12_set()
      #define lcd_d0_clr()    PB12_clr()

      #define lcd_d1_init()   PB13_output_init()
      #define lcd_d1_set()    PB13_set()
      #define lcd_d1_clr()    PB13_clr()

      #define lcd_d2_init()   PB6_output_init()
      #define lcd_d2_set()    PB6_set()
      #define lcd_d2_clr()    PB6_clr()

      #define lcd_d3_init()   PB7_output_init()
      #define lcd_d3_set()    PB7_set()
      #define lcd_d3_clr()    PB7_clr()

      #define lcd_d4_init()   PB8_output_init()
      #define lcd_d4_set()    PB8_set()
      #define lcd_d4_clr()    PB8_clr()

      #define lcd_d5_init()   PB9_output_init()
      #define lcd_d5_set()    PB9_set()
      #define lcd_d5_clr()    PB9_clr()

      #define lcd_d6_init()   PB10_output_init()
      #define lcd_d6_set()    PB10_set()
      #define lcd_d6_clr()    PB10_clr()

      #define lcd_d7_init()   PB11_output_init()
      #define lcd_d7_set()    PB11_set()
      #define lcd_d7_clr()    PB11_clr()


      #define lcd_rd_init()   PA0_output_init()
      #define lcd_rd_set()    PA0_set()
      #define lcd_rd_clr()    PA0_clr()

      #define lcd_wr_init()   PA1_output_init()
      #define lcd_wr_set()    PA1_set()
      #define lcd_wr_clr()    PA1_clr()

      #define lcd_rs_init()   PA2_output_init()
      #define lcd_rs_set()    PA2_set()
      #define lcd_rs_clr()    PA2_clr()

      #define lcd_cs_init()   PA3_output_init()
      #define lcd_cs_set()    PA3_set()
      #define lcd_cs_clr()    PA3_clr()

      #define lcd_rst_init()   PB0_output_init()
      #define lcd_rst_set()    PB0_set()
      #define lcd_rst_clr()    PB0_clr()

    #endif

    //  -------------------------------------------------------------
    //    Selfmade STM32 Board r3
    //  -------------------------------------------------------------
    #if (boardversion == 1)

      #define lcd_d0_init()   PB13_output_init()
      #define lcd_d0_set()    PB13_set()
      #define lcd_d0_clr()    PB13_clr()

      #define lcd_d1_init()   PB7_output_init()
      #define lcd_d1_set()    PB7_set()
      #define lcd_d1_clr()    PB7_clr()

      #define lcd_d2_init()   PB1_output_init()
      #define lcd_d2_set()    PB1_set()
      #define lcd_d2_clr()    PB1_clr()

      #define lcd_d3_init()   PB3_output_init()
      #define lcd_d3_set()    PB3_set()
      #define lcd_d3_clr()    PB3_clr()

      #define lcd_d4_init()   PB5_output_init()
      #define lcd_d4_set()    PB5_set()
      #define lcd_d4_clr()    PB5_clr()

      #define lcd_d5_init()   PB4_output_init()
      #define lcd_d5_set()    PB4_set()
      #define lcd_d5_clr()    PB4_clr()

      #define lcd_d6_init()   PA15_output_init()
      #define lcd_d6_set()    PA15_set()
      #define lcd_d6_clr()    PA15_clr()

      #define lcd_d7_init()   PA8_output_init()
      #define lcd_d7_set()    PA8_set()
      #define lcd_d7_clr()    PA8_clr()


      #define lcd_rd_init()   PA0_output_init()
      #define lcd_rd_set()    PA0_set()
      #define lcd_rd_clr()    PA0_clr()

      #define lcd_wr_init()   PA1_output_init()
      #define lcd_wr_set()    PA1_set()
      #define lcd_wr_clr()    PA1_clr()

      #define lcd_rs_init()   PA4_output_init()
      #define lcd_rs_set()    PA4_set()
      #define lcd_rs_clr()    PA4_clr()

      #define lcd_cs_init()   PB0_output_init()
      #define lcd_cs_set()    PB0_set()
      #define lcd_cs_clr()    PB0_clr()

      #define lcd_rst_init()   PB8_output_init()
      #define lcd_rst_set()    PB8_set()
      #define lcd_rst_clr()    PB8_clr()

    #endif

  #endif        // USE_8BIT_TFT

#endif
