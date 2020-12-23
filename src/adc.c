/* -----------------------------------------------------
                          adc.c

    Softwaremodul fuer die Benutzung des Analog-
    Digital-Converters (ADC)

    Hardware  : STM32F030F4P6

    IDE       : keine (Editor / make)
    Library   : libopencm3
    Toolchain : arm-none-eabi

    18.10.2016   R. Seelig
  ------------------------------------------------------ */

#include "adc.h"

/* -----------------------------------------------------
                   adc_setchannel
     selektiert den Eingangspin, auf dem der analoge
     Wert gemessen werden soll.

     Beispiel:
               adc_setchannel(4);

               waehlt selektiert GPIOA4 als analogen
               Eingang
   ----------------------------------------------------- */
void adc_setchannel(uint8_t channel)
{
  uint8_t channel_array[3];

  channel_array[0]= channel;
  channel_array[1]= channel;
  channel_array[2]= ADC_CHANNEL_TEMP;
  adc_set_regular_sequence(ADC1, 1, channel_array);
}

/* -----------------------------------------------------
                   adc_getchannel
     ermittelt den analogen Wert an einem gewaehlten
     Eingangspin

     Beispiel:
               value= adc_getchannel(4);

               value beinhaltet den analogen wert von
               GPIOA4
   ----------------------------------------------------- */
int adc_getchannel(uint8_t channel)
{
  adc_setchannel(channel);
  adc_start_conversion_regular(ADC1);
  while (!(adc_eoc(ADC1)));
  return adc_read_regular(ADC1);
}

/* -----------------------------------------------------
                   adc_init

     initialisiert den ADC als 12 Bit ADC und setzt
     GPIO Pins des Ports GPIOA als analogen Eingang

     Beispiel:
               adc_(GPIO3 | GPIO4 | GPIO5);

               setzt GPIO3, GPIO4 und GPIO5 als
               analoge Eingaenge
   ----------------------------------------------------- */
void adc_init(unsigned int gpiopins)
{
  rcc_periph_clock_enable(RCC_ADC);

  gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, gpiopins);

  adc_power_off(ADC1);
  adc_set_clk_source(ADC1, ADC_CLKSOURCE_ADC);
  adc_calibrate(ADC1);
  adc_set_operation_mode(ADC1, ADC_MODE_SCAN);
  adc_disable_external_trigger_regular(ADC1);
  adc_set_right_aligned(ADC1);
  adc_enable_temperature_sensor();
  adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_071DOT5);
  adc_set_resolution(ADC1, ADC_RESOLUTION_12BIT);
  adc_disable_analog_watchdog(ADC1);
  adc_power_on(ADC1);

  delay(3);             // warten bis ADC gestartet ist
}
