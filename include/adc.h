/* -----------------------------------------------------
                         adc.h

    Header fuer ADC-Softwaremodul

    Hardware  : STM32F030F4P6

    IDE       : make - Projekt
    Library   : libopencm3
    Toolchain : arm-none-eabi

    27.02.2020   R. seelig
  ------------------------------------------------------ */

#ifndef in_adc_modul
  #define in_adc_modul

  #include <stdint.h>
  #include <libopencm3.h>
  #include "sysf030_init.h"

  void adc_setchannel(uint8_t channel);
  int adc_getchannel(uint8_t channel);
  void adc_init(unsigned int gpiopins);

#endif
