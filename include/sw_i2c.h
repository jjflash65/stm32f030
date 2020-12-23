/* -----------------------------------------------------
                        sw_i2c.c

    Softwareimplementierung  I2C-Bus (Bitbanging)

    Hardware  : STM32F030F4P6
    IDE       : keine (Editor / make)
    Library   : libopencm3
    Toolchain : arm-none-eabi

    16.10.2016   R. seelig
  ------------------------------------------------------ */

#ifndef in_sw_i2c
  #define in_sw_i2c

  #include <stdint.h>

  #include <libopencm3.h>

  #define sda_pin        GPIO9        // Anschlusspin fuer SDA Leitung
  #define scl_pin        GPIO10       // dto. fuer SCL Leitung

  #define short_puls     0            // Einheiten fuer einen langen Taktimpuls
  #define long_puls      0            // Einheiten fuer einen kurzen Taktimpuls

  #define i2c_sda_hi()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, sda_pin))
  #define i2c_sda_lo()   ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, sda_pin) )
  #define i2c_is_sda()   ( gpio_get(GPIOA, sda_pin))

  #define i2c_scl_hi()   ( gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, scl_pin))
  #define i2c_scl_lo()   ( gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, scl_pin) )
  #define i2c_is_scl()   ( gpio_get(GPIOA, scl_pin))

  #define short_del()   ( i2c_delay(short_puls) )
  #define long_del()    ( i2c_delay(long_puls) )

  // --------------------------------------------------------------------
  //                      Prototypen aus sw_i2c.h
  // --------------------------------------------------------------------

  /* -------------------------------------------------------

      ############### i2c_master_init ##############

      setzt die Pins die fuer den I2C Bus verwendet werden
      als Ausgaenge


      ############## i2c_sendstart(void) ###############

      erzeugt die Startcondition auf dem I2C Bus


      ############## i2c_start(uint8_t addr) ##############

      erzeugt die Startcondition auf dem I2C Bus und
      schreibt eine Deviceadresse auf den Bus


      ############## i2c_stop(void) ##############

      erzeugt die Stopcondition auf dem I2C Bus


      ############## i2c_write_nack(uint8_t data) ##############

      schreibt einen Wert auf dem I2C Bus OHNE ein Ack-
      nowledge einzulesen


      ############## i2c_write(uint8_t data) ##############

      schreibt einen Wert auf dem I2C Bus.

      Rueckgabe:
                 > 0 wenn Slave ein Acknowledge gegeben hat
                 == 0 wenn kein Acknowledge vom Slave


      ############## i2c_write16(uint16_t data) ##############

      schreibt einen 16 Bit Wert (2Bytes) auf dem I2C Bus.

      Rueckgabe:
                 > 0 wenn Slave ein Acknowledge gegeben hat
                 == 0 wenn kein Acknowledge vom Slave


      ############## i2c_read(uint8_t ack) ##############

      liest ein Byte vom I2c Bus.

      Uebergabe:
                 1 : nach dem Lesen wird dem Slave ein
                     Acknowledge gesendet
                 0 : es wird kein Acknowledge gesendet

      Rueckgabe:
                  gelesenes Byte
     ------------------------------------------------------- */

  void i2c_master_init(void);
  void i2c_sendstart(void);
  uint8_t i2c_start(uint8_t addr);
  void i2c_stop();
  void i2c_write_nack(uint8_t data);
  uint8_t i2c_write(uint8_t data);
  uint8_t i2c_write16(uint16_t data);
  uint8_t i2c_read(uint8_t ack);

  #define i2c_read_ack()    i2c_read(1)
  #define i2c_read_nack()   i2c_read(0)

#endif
