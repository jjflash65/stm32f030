/* -----------------------------------------------------
                        flashrom.c

    Demoprogramm fuer das Schreiben des Flashroms 
    ( als Ersatz fuer fehlendes EEPROM )

    => nur ein ganzes Word zu schreiben macht Sinn !

    Die unteren 16 Bit liegen zuerst im Speicher !

    Hardware  : STM32F030F4P6
    IDE       : keine (Editor / make)
    Library   : libopencm3
    Toolchain : arm-none-eabi

    28.09.2016   R. seelig
  ------------------------------------------------------ */


#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#include <libopencm3.h>
#include <flash.h>

#include "sysf030_init.h"
#include "serial.h"
#include "my_printf.h"


#define printf   my_printf


/* --------------------------------------------------
                      UART_GETHEX
     liest einen 8-Bit Hex-Wert von der seriellen
     Schnittstelle ein

     echo => 1 : gelesener Wert wird zurueckgesendet
             0 : nicht zuruecksenden
   -------------------------------------------------- */
uint8_t uart_gethex(char echo)
{
  uint8_t ch;
  uint8_t value;

  ch= uart_getchar();
  if (echo) uart_putchar(ch);
  if (ch > 'F')
  {
    ch= (ch-'a')+10;
  }
  else
  {
    if (ch> '9') ch= (ch-'A')+10; else ch -= '0';
  }
  value= (ch<< 4);
  ch= uart_getchar();
  if (echo) uart_putchar(ch);

  if (ch > 'F')
  {
    ch= (ch-'a')+10;
  }
  else
  {
    if (ch> '9') ch= (ch-'A')+10; else ch -= '0';
  }
  value |= ch;
  return value;
}

/* --------------------------------------------------
                      UART_GETHEX16
     liest einen 16 Bit Hex-Wert von der seriellen
     Schnittstelle ein

     echo => 1 : gelesener Wert wird zurueckgesendet
             0 : nicht zuruecksenden
   -------------------------------------------------- */
uint16_t uart_gethex16(char echo)
{
  uint16_t i, i2;

  i= uart_gethex(1);
  i= i & 0x00ff;
  i2= uart_gethex(1);
  i2= i2 & 0x00ff;
  i2= i2 | (i << 8);
  if (echo) printf("%x",i2);
  return i2;
}


void my_putchar(char c)
{
  uart_putchar(c);
}

/* --------------------------------------------------
   flash_init

   "unlocked" den Flash-Speicher und loescht eine
   ganze Page ab adr

   VORSICHT: eine ganze Page betraegt 1kB
   -------------------------------------------------- */
void flash_init(uint32_t adr)
{
  flash_unlock();
  flash_clear_pgerr_flag();
  flash_clear_eop_flag();
  flash_clear_wrprterr_flag();
  flash_clear_pgerr_flag();

  flash_erase_page(adr);
}


int main(void)
{
  uint16_t   val16a, val16b;
  uint32_t   val32;
  uint32_t   memadr= 0x8003800;

  uint32_t  *flashmem = (uint32_t*)0x8003800;       // einen Zeiger auf die Flashspeicheradresse setzen

  int       index;
  uint8_t   b,ch;


  uint8_t   tarray[] = { 65, 66, 67, 68, 69, 70, 71, 72, 73 };

  sys_init();

  uart_init(115200);

  printf("\n\rSerielle Schnittstelle: OK\n\r");

  printf("Bytes lesen: \n\r");


  for (index= 0; index< 2; index++)
  {
     b= (*flashmem >> 24) & 0xff;
     uart_putchar(b);
     b= (*flashmem >> 16) & 0xff;
     uart_putchar(b);
     b= (*flashmem >> 8) & 0xff;
     uart_putchar(b);
     b= *flashmem & 0xff ;
     uart_putchar(b);
     flashmem++;
  }

  printf("\n\rBytes schreiben: (j/n)\n\r");
  ch= uart_getchar();

  if (ch== 'j')
  {

    flash_init(memadr);

    for (index= 0; index< 5; index+= 4)
    {
      val32= 0;
      b= tarray[index];
      val32 |= b << 24;

      b= tarray[index+1];
      val32 |= b << 16;

      b= tarray[index+2];
      val32 |= b << 8;

      b= tarray[index+3];
      val32 |= b;

      flash_program_word(memadr + index, val32);
    }

    flash_lock();

  }

  printf("\n\rReady...\n\r");
  while(1);


  val16a= *flashmem >> 16;
  val16b= *flashmem & 0xffff;
  printf("\n\raktuelles Datum1: %x.%x", val16a,val16b);
  flashmem++;
  val16a= *flashmem >> 16;
  val16b= *flashmem & 0xffff;
  printf("\n\raktuelles Datum3: %x.%x", val16a,val16b);


  printf("\n\rNeuen 16 !!!! Bit Hi-Wert eingeben: ");
  val16a= uart_gethex16(0);
  printf("\n\rNeuen 16 !!!! Bit Lo-Wert eingeben: ");
  val16b= uart_gethex16(0);
  val32= (val16a << 16) | val16b;

  flash_init(memadr);
  flash_program_word(memadr, val32);


  memadr+= 4;
  printf("\n\rNeuen 16 !!!! Bit Hi-Wert eingeben: ");
  val16a= uart_gethex16(0);
  printf("\n\rNeuen 16 !!!! Bit Lo-Wert eingeben: ");

  val16b= uart_gethex16(0);
  val32= (val16a << 16) | val16b;
  flash_program_word(memadr, val32);

  flash_lock();

  printf("\n\rReady...\n\r");


  while(1);

}
