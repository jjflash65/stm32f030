/* -------------------------------------------------------
                        digit4_demo.c

     Testprogramm fuer 4 stelliges 7-Segmentmodul (China)
     mit 74HC595 Schieberegistern

     Hardware : Chinamodul "4-Bit LED Digital Tube Modul"

     MCU      : STM32f030F4P6
     Takt     : interner Takt 48 MHz

     02.03.2020  R. Seelig
   ------------------------------------------------------ */

/*

                                                  +-------------+
                                           boot0  |  1       20 | PA14 - SWCLK - USART1_TX
                         PF0 - osc_in - I2C1_SDA  |  2   S   19 | PA13 - SWDIO - IR_OUT
                        PF1 - osc_out - I2C1_SCL  |  3   T   18 | PA10 - USART1_RX - TIM1_CH3 - TIM17_BKIN - I2C1_SDA
                             NRST - (active low)  |  4   M   17 | PA9 - USART1_TX - TIM1_CH2 - I2C1_SCL
          Vdda - analog power supply (max. 3.6V)  |  5   3   16 | Vdd - power supply (max. 3.6V)
  PA0 - USART1_CTS - ADC_in0 - RTC_TAMP2 - WKUP1  |  6   2   15 | Vss - GND
                      PA1 - USART1_RTS - ADC_in1  |  7   F   14 | PB1 - TIM3_CH4 - TIM14_CH1 - TIM1_CH3N - ADC_in9
                       PA2 - USART1_TX - ADC_in2  |  8   0   13 | PA7 - SPI1_MOSI - TIM3_CH2 - TIM14_CH1 - TIM1_CH1N - EVENTOUT - ADC_in7
                       PA3 - USART1_RX - ADC_in3  |  9   3   12 | PA6 - SPI1_MISO - TIM3_CH1 - TIM1_BKIN - TIM16_CH1 - EVENTOUT - ADC_in6
PA4 - SPI1_NSS - USART1_CK - TIM14_CH1 - ADC_in4  | 10   0   11 | PA5 - SPI1_SCK - ADC_in5
                                                  +-------------+


   Anschluesse (in seg7anz_v3.h):
 ------------------------------------------------------
   Pinbelegung:

   4 Bit LED Digital Tube Module                 STM32F030F4P6
   -----------------------------------------------------------

       (shift-clock)   Sclk   -------------------- PA5
       (strobe-clock)  Rclk   -------------------- PA0
       (ser. data in)  Dio    -------------------- PA7
       (+Ub)           Vcc
                       Gnd


   Anzeigenposition 0 ist das rechte Segment des Moduls

            +-----------------------------+
            |  POS3   POS2   POS1   POS0  |
    Vcc  o--|   --     --     --     --   |
    Sclk o--|  |  |   |  |   |  |   |  |  |
    Rclk o--|  |  |   |  |   |  |   |  |  |
    Dio  o--|   -- o   -- o   -- o   -- o |
    GND  o--|                             |
            |   4-Bit LED Digital Tube    |
            +-----------------------------+

   Segmentbelegung der Anzeige:

       a
      ---
   f | g | b            Segment |  a  |  b  |  c  |  d  |  e  |  f  |  g  | dp |
      ---               ---------------------------------------------------------
   e |   | c            Bit-Nr. |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7 |
      ---
       d

*/

#include <stdint.h>

#include <libopencm3.h>
#include "sysf030_init.h"
#include "seg7anz_v3.h"

#define d4char_C    0xc6       // Bitmuster fuer grosses C
#define d4char_oo   0x9c       // Bitmuster fuer hochgestelltes o
#define d4char_h    0x8b       // Bitmuster fuer kleines h
#define d4char_d    0xa1       // Bitmuster fuer kleines d

uint8_t hallo_str[6] = { 0x76, 0x77, 0x38, 0x38, 0x3f, 0x00 };


void digit4_puts(uint8_t *ch, uint8_t pos)
{
  while(*ch)
  {
    seg7_4digit[pos]= ~(*ch);
    ch++;
    pos--;
  }
}


/* ---------------------------------------------------------------------------------
                                      M-A-I-N
   ---------------------------------------------------------------------------------*/

int main(void)
{
  uint16_t   oldzsek = 1;
  uint32_t   counter = 0;
  uint8_t    i;


  sys_init();

  digit4_init();                        // Modul initialisieren

  digit4_clr();

  while(1)
  {

    // "Lauflicht" auf den Dezimalpunkten
    for (i= 0; i< 8; i++)
    {
      digit4_setdp(i);
      delay(100);
      digit4_clrdp(i);
    }
    digit4_clr();

    // hexadezimale Ausgabe mit links angezeigtem
    // Buchstaben "h", Unterdrueckung fuehrender Nullen
    digit4_sethex(0xabcdef,1);
    delay(3);                          // das Ausschieben dauert "zu lange"
    seg7_4digit[7]= d4char_h;

    delay(2500);
    digit4_clr();

    // dezimale Ausgabe mit links angezeigtem
    // Buchstaben "d", Unterdrueckung fuehrender Nullen
    digit4_setdez(12345, 1);
    delay(3);
    seg7_4digit[7]= d4char_d;
    delay(2500);
    digit4_clr();


     // Bsp. fuer Anzeige von 15.0 Grad Celcius
    digit4_setdez8bit(15,4);
    digit4_setdp(4);
    delay(3);
    seg7_4digit[3]= ~led7sbmp[0];
    seg7_4digit[2]= d4char_oo;
    seg7_4digit[1]= d4char_C;
    delay(2500);
    digit4_clr();

    // Ausgabe blinkender "Hallo"-Text
    // bei einer nur 4-stelligen Anzeige ist das H vorne
    // abgeschnitten
    for (i= 0; i< 8; i++)
    {
      digit4_puts(&hallo_str[0],4);
      delay(500);
      digit4_clr();
      delay(500);
    }

    digit4_setdp(1);                      // Kommapunkt anzeigen

    counter= 100;
    // endlose Zaehlschleife
    while(counter)
    {
      if (oldzsek != tim3_zsek)
      {
        oldzsek= tim3_zsek;
        counter--;
        counter= counter % 10000000;
        digit4_setdez(counter,1);
      }
    }
  }
}
