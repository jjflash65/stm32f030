/* -----------------------------------------------------
                        spif030.h

    Header Softwaremodul zum Umgang mit Interface SPI
    STM32F030F4P6 Controller

    Hardware  : STM32F030F4P6
    IDE       : keine (Editor / make)
    Library   : libopencm3
    Toolchain : arm-none-eabi

    07.10.2016   R. seelig
  ------------------------------------------------------ */

#ifndef in_spif030
  #define in_spif030

  #include <stdint.h>
  #include <libopencm3.h>
  #include "sysf030_init.h"

  //  PA4 = NSS, PA5 = SCK, PA6 = MISO, PA7 = MOSI, Alternate Funktion fuer SPI = AF0

  #define spi_ss     GPIO4
  #define spi_sck    GPIO5
  #define spi_miso   GPIO6
  #define spi_mosi   GPIO7

// ------------------ Prototypen -----------------------

  void spi_init(void);
  void spi_out(uint8_t data);
  uint8_t spi_in(void);

#endif

