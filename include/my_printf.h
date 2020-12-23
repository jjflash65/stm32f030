/* -------------------------------------------------------
                      my_printf.h

     Library-Header fuer eine eigene (sehr kleine) printf-
     Funktion.

     Compiler : arm-none-eabi-gcc

     fuer | myprintf | muss im Hauptprogramm irgendwo
     ein my_putchar vorhanden sein.

     Bsp.:

        void my_putchar(char ch)
        {
          txlcd_putchar(ch);
        }

     21.09.2016  R. Seelig
   ------------------------------------------------------ */

#ifndef in_myprintf
  #define in_myprintf

  #include <stdint.h>
  #include <stdarg.h>

//  #define uint8_t     unsigned char
//  #define uint16_t    unsigned int

  extern char printfkomma;

  extern void my_putchar(char ch);
  void putint(int i, char komma);
  void hexnibbleout(uint8_t b);
  void puthex(uint16_t h, char out16);
  void putstring(char *p);
  void my_printf(const char *s,...);

#endif
