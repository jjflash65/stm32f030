/* -------------------------------------------------------
                         gfx_pictures.h

     Header fuer Softwaremodul zum Anzeigen eingebetteter
     Pixelgraphiken.

     Damit die Pixelgrafiken angezeigt werden koennen, muss
     einem Programm eine Funktion

       void putpixel(int x, int y,uint16_t color);

     hinzugelinkt werden, die die Ausgabe eines farbigen
     Punktes im RGB565-Format vornimmt (5 Bit fuer rot,
     6 Bit fuer gruen, 5 Bit fuer blau)

     Verfuegbare Formate sind:

       PCX 256 Farben
       BMP s/w, 4, 16, 256 Farben

     MCU   :  AVR / STM32 / STM8

     25.01.2019  R. Seelig
   ------------------------------------------------------ */

#ifndef in_gfxpictures
  #define in_gfxpictures

  // Fuer Verwendung mit einer Atmel AVR MCU ist hier eine 1 anzugeben,
  // ansonsten 0

  #define avr_mcu            0

  #include <stdint.h>
  #include <stdlib.h>
  #include <stdarg.h>
  #include <string.h>

  #if (avr_mcu == 1)
    #include <avr/pgmspace.h>
  #endif

  // hier auswaehlen welche der Funktionen uebersetzt werden sollen um sie spaeter einem
  // Programm hinzulinken zu koennen

  //     1 : Funktion vorhanden
  //     0 : Funktion nicht vohanden


  #define bmpsw_enable   0
  #define bmpcga_enable  1
  #define bmp16_enable   1
  #define bmp256_enable  0
  #define pcx256_enable  0


  // Putpixel ist hardwareabhaengig. Deshalb muss fuer das zu verwendende Display ein
  // putpixel innerhalb eines Programmes verfuegbar sein
  extern void putpixel(int x, int y,uint16_t color);

  #if (avr_mcu == 1)
    #define readarray(arr,ind)       (pgm_read_byte(&(arr[ind])))
    #define readwarray(arr,ind)      (pgm_read_word(&(arr[ind])))
  #else
    #define readarray(arr,ind)       (arr[ind])
    #define readwarray(arr,ind)      (arr[ind])
  #endif

/* -------------------------------------------------------
                         Prototypen
   ------------------------------------------------------- */

  #if (bmpsw_enable == 1)
    void bmpsw_show(uint16_t ox, uint16_t oy, const unsigned char* const image, uint16_t fwert);
  #endif

  #if (bmpcga_enable == 1)
    void bmpcga_show(int ox, int oy, const uint8_t* const image, const uint16_t* const pal);
  #endif

  #if (bmp16_enable == 1)
    void bmp16_show(int16_t ox, int16_t oy, const uint8_t* const image, const uint16_t* const palette);
  #endif

  #if (bmp256_enable == 1)
    void bmp256_show(uint8_t ox, uint8_t oy, const uint8_t* const image, const uint16_t* const palette);
  #endif

  #if (pcx256_enable == 1)
    void pcx256_show(int16_t x, int16_t y, const unsigned char* const image, const uint16_t *const pal);
  #endif

#endif
