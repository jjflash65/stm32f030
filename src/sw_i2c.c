/* -----------------------------------------------------
                        sw_i2c.c

    Softwareimplementierung  I2C-Bus (Bitbanging)

    Hardware  : STM32F030F4P6
    IDE       : keine (Editor / make)
    Library   : libopencm3
    Toolchain : arm-none-eabi

    16.10.2016   R. seelig
  ------------------------------------------------------ */

#include "sw_i2c.h"

// --------------------------------------------------------------------
//                      Prototypen aus sw_i2c.h
// --------------------------------------------------------------------

/* -------------------------------------------------------

                   i2c_master_init

    setzt die Pins die fuer den I2C Bus verwendet werden
    als Ausgaenge


                   i2c_sendstart(void)

    erzeugt die Startcondition auf dem I2C Bus


                   i2c_start(uint8_t addr)

    erzeugt die Startcondition auf dem I2C Bus und
    schreibt eine Deviceadresse auf den Bus


                    i2c_stop(void)

    erzeugt die Stopcondition auf dem I2C Bus


               i2c_write_nack(uint8_t data)
   schreibt einen Wert auf dem I2C Bus OHNE ein Ack-
   nowledge einzulesen


                i2c_write(uint8_t data)
   schreibt einen Wert auf dem I2C Bus.

   Rueckgabe:
               > 0 wenn Slave ein Acknowledge gegeben hat
               == 0 wenn kein Acknowledge vom Slave
                    i2c_read(uint8_t ack)

   liest ein Byte vom I2c Bus.

   Uebergabe:
               1 : nach dem Lesen wird dem Slave ein
                   Acknowledge gesendet
               0 : es wird kein Acknowledge gesendet

   Rueckgabe:
               gelesenes Byte
   -------------------------------------------------------

void i2c_master_init(void);
void i2c_sendstart(void);
uint8_t i2c_start(uint8_t addr);
void i2c_stop();
void i2c_write_nack(uint8_t data);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_read(uint8_t ack);
#define i2c_read_ack()    i2c_read(1)
#define i2c_read_nack()   i2c_read(0)

*/

void i2c_delay(int cnt)
{
  volatile int i;
  for (i= 0; i< cnt; i++);
}

void i2c_master_init()
{
  i2c_sda_hi();
  i2c_scl_hi();
}

void i2c_sendstart(void)
{
  i2c_scl_hi();
  long_del();

  i2c_sda_lo();
  long_del();
}

uint8_t i2c_start(uint8_t addr)
{
  uint8_t ack;

  i2c_sendstart();
  ack= i2c_write(addr);
  return ack;
}

void i2c_stop(void)
{
   i2c_sda_lo();
   long_del();
   i2c_scl_hi();
   short_del();
   i2c_sda_hi();
   long_del();
}

void i2c_write_nack(uint8_t data)
{
  uint8_t i;

  for(i=0;i<8;i++)
  {
    i2c_scl_lo();
    short_del();

    if(data & 0x80) i2c_sda_hi();
               else i2c_sda_lo();

    short_del();

    i2c_scl_hi();
    short_del();

    while(i2c_is_scl() == 0);

    data=data<<1;
  }
  i2c_scl_lo();
  short_del();
}


uint8_t i2c_write(uint8_t data)
{
   uint8_t ack;

   i2c_write_nack(data);

  //  9. Taktimpuls (Ack)

  i2c_sda_hi();
  long_del();

  i2c_scl_hi();
  long_del();

  if (i2c_is_sda()) ack= 0; else ack= 1;

  i2c_scl_lo();
  long_del();

  return ack;
}

uint8_t i2c_write16(uint16_t data)
{
  uint8_t ack;
  ack= i2c_write(data >> 8);
  if (!(ack)) return 0;
  ack= i2c_write(data & 0xff);
  return ack;
}


  sek= rtc_read(0) & 0x7f;
