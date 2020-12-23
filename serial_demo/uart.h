/* -------------------------------------------------------
                         uart.h

     Header  fuer rudimentaere Funktionen zur seriellen
     Schnittstelle

     MCU   :  STM32F030F4P6
     Takt  :  interner Takt

     28.09.2016  R. Seelig

     Anmerkung:

     PA9 / PA2  : TxD
     PA10 / PA3 : RxD
   ------------------------------------------------------ */

#ifndef in_uart
  #define in_uart

  #include <stdint.h>
  #include <libopencm3.h>

  /* -------------------------------------------------------
                        UART_INIT

    initialisiert serielle Schnittstelle mit anzugebender
    Baudrate. Protokoll 1 Startbit, 8 Databit, 1 Stopbit
    keine Paritaet (8N1)

    PA9:  TxD
    PA10: RxD

        oder

    PA2:  TxD
    PA3:  RxD
   ------------------------------------------------------- */
  #define uart_pinset         0                    // 0 = Anschluesse PA9 / PA10
                                                   // 1 = Anschluesse PA2 / PA3


  void uart_init(int baud);
  void uart_putchar(uint8_t ch);
  uint8_t uart_getchar(void);
  uint8_t uart_ischar(void);

#endif
