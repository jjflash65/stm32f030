/* -------------------------------------------------------
                         uart.c

     Softwaremodul fuer rudimentaere Funktionen der
     seriellen Schnittstelle

     MCU   :  STM32F030F4P6
     Takt  :  interner Takt

     28.09.2016  R. Seelig

     Anmerkung:

     PA9 : TxD
     PA10: RxD
   ------------------------------------------------------ */


#include "uart.h"

/* -------------------------------------------------------
                           uart_init

     initialisiert die serielle Schnittstelle. In uart.h
     wird bestimmt, ob hierfuer die Anschluesse PA2 / PA3
     oder PA9 / PA10 verwendet werden

     Uebergabe
       baud : die einzustellende Baudrate
   ------------------------------------------------------- */
void uart_init(int baud)
{
  rcc_periph_clock_enable(RCC_USART1);

#if (uart_pinset == 1)
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
  gpio_set_af(GPIOA, GPIO_AF1, GPIO2);
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO3);
  gpio_set_af(GPIOA, GPIO_AF1, GPIO3);
#else
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);
  gpio_set_af(GPIOA, GPIO_AF1, GPIO9);
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10);
  gpio_set_af(GPIOA, GPIO_AF1, GPIO10);
#endif

  usart_set_baudrate(USART1, baud);
  usart_set_databits(USART1, 8);
  usart_set_parity(USART1, USART_PARITY_NONE);
  usart_set_stopbits(USART1, USART_CR2_STOP_1_0BIT);
  usart_set_mode(USART1, USART_MODE_TX_RX);
  usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

  usart_enable(USART1);
}

/* -------------------------------------------------------
                      uart_putchar

     sendet ein Zeichen auf der seriellen Schnittstelle
   ------------------------------------------------------- */
void uart_putchar(uint8_t ch)
{
  usart_send_blocking(USART1, ch);
}

/* -------------------------------------------------------
                      uart_getchar

     wartet solange, bis ein Zeichen auf der seriellen
     Schnittstelle eintrifft, liest dieses ein und gibt
     das Zeichen als Return-Wert zurueck
   ------------------------------------------------------- */
uint8_t uart_getchar(void)
{
  return usart_recv_blocking(USART1);
}

/* -------------------------------------------------------
                      uart_ischar

     testet, ob ein Zeichen auf der seriellen Schnitt-
     stelle eingetroffen ist, liest aber ein eventuell
     vorhandenes Zeichen NICHT ein
   ------------------------------------------------------- */
uint8_t uart_ischar(void)
{
  return (USART_ISR(USART1) & USART_ISR_RXNE);
}
