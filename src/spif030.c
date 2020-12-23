/* -----------------------------------------------------
                        spif030.c

    Softwaremodul zum Umgang mit Interface SPI
    STM32F030F4P6 Controller

    Hardware  : STM32F030F4P6
    IDE       : keine (Editor / make)
    Library   : libopencm3
    Toolchain : arm-none-eabi

    07.10.2016   R. seelig
  ------------------------------------------------------ */

#include "spif030.h"

/* -------------------------------------------------------------
   SPI_INIT

      initialisiert SPI-Schnittstelle (vorzugsweise zur
      Benutzung mit SPI LC-Displays)
   ------------------------------------------------------------- */
void spi_init(void)
{
  rcc_periph_clock_enable(RCC_SPI1);
  delay(250);

  // Pins der SPI Schnittstelle konfigurieren
  // AF0 ist Funktionsnummer der GPIO-Pins fuer SPI-Funktionalitaet
  gpio_mode_setup(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE, spi_sck | spi_miso | spi_mosi);
  gpio_set_af(GPIOA,GPIO_AF0, spi_sck | spi_miso | spi_mosi);

  spi_set_master_mode(SPI1);
  spi_set_baudrate_prescaler(SPI1, SPI_CR1_BR_FPCLK_DIV_2);
  spi_set_clock_polarity_1(SPI1);
  spi_set_clock_phase_1(SPI1);
  spi_set_bidirectional_transmit_only_mode(SPI1);
  spi_set_data_size(SPI1, SPI_CR2_DS_8BIT);

  spi_send_msb_first(SPI1);
  spi_enable_ss_output(SPI1);

  spi_enable(SPI1);
}

/* -------------------------------------------------------------
   SPI_OUT

      Byte ueber SPI senden
      data ==> zu sendendes Datum
   ------------------------------------------------------------- */
void spi_out(uint8_t data)
{
  spi_send8(SPI1, data);
}

/* -------------------------------------------------------------
   SPI_IN

      Byte ueber SPI einlesen
   ------------------------------------------------------------- */
uint8_t spi_in(void)
{
  return (spi_read8(SPI1) );
}
