/* ----------------------------------------------------------
     smallio.c

     Sehr schlankes I/O Interface ueber die serielle
     Schnittstelle mit abgespecktem printf. Hierdurch ist es
     moeglich, die Beispiele der Hilfedate fuer viele
     Mikrocontroller und auf PC nachzuvollziehen.

     Fuer die unterstuetzten Microcontroller (AVR, STM32,
     MCS-51, STM8) ist jeweils ein eigenes smallio
     verfuegbar, sodass JEDES Programm lediglich im Header
     ein

                  "include smallio.h"

     eingefuegt werden muss. Im Makefile ist einzustellen,
     dass smallio.o fuer AVR,ARM und PC-Konsolenprogramme
     oder smallio.rel fuer  MCS-51 und STM8) hinzugelinkt
     werden muss. Eintrag im Makefile:

     SRCS      += ../src/smallio.o

     Damit Programmierversuche / Tests mit jedem Controller
     funktionieren ist die Baudrate absichtlich sehr lang-
     sam auf 4800 Bd eingestellt.

     Protokoll ist 8N1

     Der Controller wird mit 48 Mhz und interner Takt-
     quelle initialisiert.

     ------------------------------------------------------

     Hardware  : STM32F030
     IDE       : keine (Editor / make)
     Library   : libopencm3
     Toolchain : arm-none-eabi

     18.06.2019   R. seelig
  ------------------------------------------------------ */

#include "smallio.h"

volatile int tick_ms = 0;
char printfkomma = 2;

/* -------------------------------------------------------
                    sys_tick_handler

   Der "Systemticker", wird jede Millisekunde durch
   Interrupt aufgerufen und kann bei Bedarf erweitert
   werden
 ------------------------------------------------------- */
void sys_tick_handler(void)
{
  tick_ms++;
}

/* -------------------------------------------------------
                          delay

   Verzoegerungsschleife, die die verzoegerten Milli-
   sekunden aus der im Systemticker hochzaehlenden
   Variable "tick_ms" berechnet
 ------------------------------------------------------- */
void delay(int c)
{
  volatile int end_time = tick_ms + c;

  while (tick_ms < end_time)
  {
    __asm volatile("wfi");
  }
}

/* -------------------------------------------------------
                        systick_setup

   initialisiert den Systemticker fuer Aufruf jede
   Millisekunde bei 48 MHz Systemtakt
 ------------------------------------------------------- */
void systick_setup(void)
{
  systick_clear();
  systick_set_clocksource(STK_CSR_CLKSOURCE_EXT);
  systick_set_reload(6000);                         // 48 MHz / 8 = 6 MHz  => (1 / 6 MHz) * 6000 = 1 ms
  systick_interrupt_enable();
  systick_counter_enable();
}

/* -------------------------------------------------------
                        gpio_clkon

           Takt fuer GPIO-Pins einschalten
 ------------------------------------------------------- */
void gpio_clkon(void)
{
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOC);
}

/* -------------------------------------------------------
                        sys_init

   initialisiert den Controller fuer einen Systemtakt
   48 MHz mit interner Taktquelle
 ------------------------------------------------------- */
void sys_init(void)
{
  rcc_clock_setup_in_hsi_out_48mhz();
  systick_setup();
  gpio_clkon();
}


/* -------------------------------------------------------
                        smallio_init

   initialisiert den Controller fuer einen Systemtakt
   48 MHz mit interner Taktquelle und die serielle
   Schnittstelle
 ------------------------------------------------------- */
void smallio_init(void)
{
  sys_init();
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
                      putchar

     sendet ein Zeichen auf der seriellen Schnittstelle
   ------------------------------------------------------- */
int putchar(int ch)
{
  usart_send_blocking(USART1, ch);
  return ch;
}

/* -------------------------------------------------------
                      getchar

     wartet solange, bis ein Zeichen auf der seriellen
     Schnittstelle eintrifft, liest dieses ein und gibt
     das Zeichen als Return-Wert zurueck
   ------------------------------------------------------- */
uint8_t getchar(void)
{
  return usart_recv_blocking(USART1);
}

/* -------------------------------------------------------
                      keypressed

     testet, ob ein Zeichen auf der seriellen Schnitt-
     stelle eingetroffen ist, liest aber ein eventuell
     vorhandenes Zeichen NICHT ein
   ------------------------------------------------------- */
uint8_t keypressed(void)
{
  return (USART_ISR(USART1) & USART_ISR_RXNE);
}

/* ------------------------------------------------------------
                            readint

     liest einen signed Integer auf der seriellen
     Schnittstelle ein

     Korrektur ist mit der Loeschtaste nach links moeglich.
   ------------------------------------------------------------ */
int readint(void)
{
  uint8_t   signflag= 0;
  int       sum = 0;
  uint8_t   zif;
  uint8_t   ch;

  do
  {
    ch= getchar();
    if (ch== 0x0a) ch= 0x0d;

    // Ziffern auswerten
    if ((ch>= '0') && (ch<= '9'))
    {
      zif= ch-'0';

      if ((sum== 0) && (zif))        // erste Ziffer
      {
        sum= zif;
        putchar(ch);
      }
      else
      if (sum < 99999999)
      {
        if  (!( ((sum * 10) > 99999999) && (zif > 7) ))
        {
          sum= (sum*10) + zif;
          putchar(ch);
        }
      }
    }

    // letzte Eingabe loeschen
    if ((ch== 127) || (ch== 8))    // letzte Eingabe loeschen
    {
      if (sum)
      {
        sum /= 10;
        putchar(8);
        putchar(' ');
        putchar(8);
      }
      else
      if (signflag)
      {
        putchar(8);
        putchar(' ');
        putchar(8);
        signflag= 0;
      }
    }

    // Eingabe Minuszeichen
    if ((ch== '-') && (sum == 0))
    {
      signflag= 1;
        putchar('-');
    }

  } while (ch != 0x0d);              // wiederholen bis Returnzeichen eintrifft
  if (signflag) return sum *(-1); else return sum;
}


/* ------------------------------------------------------------
                            putint
     gibt einen Integer dezimal aus. Ist Uebergabe
     "komma" != 0 wird ein "Kommapunkt" mit ausgegeben.
     Groesste darstellare Zahl ist 99.999.999

     Bsp.: Bei komma==2 wird 12345 als 123.45 ausgegeben.
     (ermoeglicht Pseudofloatausgaben im Bereich)
   ------------------------------------------------------------ */
void putint(int i, char komma)
{
  typedef enum boolean { FALSE, TRUE }bool_t;

  static int zz[]  = { 10000000, 1000000, 100000, 10000, 1000, 100, 10 };
  bool_t not_first = FALSE;

  uint8_t zi;

  komma= 8-komma;

  if (!i)
  {
    putchar('0');
  }
  else
  {
    if(i < 0)
    {
      putchar('-');
      i = -i;
    }

    int z, b;

    for(zi = 0; zi < 7; zi++)
    {
      z = 0;
      b = 0;

      while(z + zz[zi] <= i)
      {
        b++;
        z += zz[zi];
      }

      if(b || not_first)
      {
        putchar('0' + b);
        not_first = TRUE;
      }

      if (zi+1 == komma)
      {
        if (!not_first) putchar('0');
        putchar('.');
        not_first= TRUE;
      }

      i -= z;
    }
    putchar('0' + i);
  }
}

/* ------------------------------------------------------------
                          hexnibbleout
     gibt die unteren 4 Bits eines chars als Hexaziffer aus.
     Eine Pruefung ob die oberen vier Bits geloescht sind
     erfolgt NICHT !
  -------------------------------------------------------------  */
void hexnibbleout(uint8_t b)
{
  if (b< 10) b+= '0'; else b+= 55;
  putchar(b);
}

/* ------------------------------------------------------------
                            puthex
     gibt einen Integer hexadezimal aus. Ist die auszugebende
     Zahl >= 0xff erfolgt die Ausgabe 2-stellig, ist sie
     groesser erfolgt die Ausgabe 4-stellig.

     Ist out16 gesetzt, erfolgt ausgabe immer 4 stellig
   ------------------------------------------------------------ */
void puthex(uint16_t h, char out16)
{
  uint8_t b;

  if ((h> 0xff) || out16)                    // 16 Bit-Wert
  {
    b= (h >> 12);
    hexnibbleout(b);
    b= (h >> 8) & 0x0f;
    hexnibbleout(b);
  }
  b= h;
  b= (h >> 4) & 0x0f;
  hexnibbleout(b);
  b= h & 0x0f;
  hexnibbleout(b);
}


void putstring(char *p)
{
  do
  {
    putchar( *p );
  } while( *p++);
}


/* ------------------------------------------------------------
                            my_printf
     alternativer Ersatz fuer printf.

     Aufruf:

         my_printf("Ergebnis= %d",zahl);

     Platzhalterfunktionen:

        %s     : Ausgabe Textstring
        %d     : dezimale Ausgabe
        %x     : hexadezimale Ausgabe
                 ist Wert > 0xff erfolgt 4-stellige
                 Ausgabe
                 is Wert <= 0xff erfolgt 2-stellige
                 Ausgabe
        %k     : Integerausgabe als Pseudokommazahl
                 12345 wird als 123.45 ausgegeben
        %f     : Gleitkommaausgabe (wenn diese in smallio.h
                 eingeschaltet ist)
        %c     : Ausgabe als Asciizeichen

   ------------------------------------------------------------ */
void my_printf(const char *s,...)
{
  int       arg1;
  double    arg1f;
  int       i, iex;
  uint32_t  xarg1;
  char      *arg2;
  char      ch;
  char      delf;
  va_list   ap;

  va_start(ap,s);
  do
  {
    delf= 0;
    ch= *s;
    if(ch== 0) return;

    if(ch=='%')            // Platzhalterzeichen
    {
      s++;
      uint8_t token= *s;
      switch(token)
      {
        case 'd':          // dezimale Ausgabe
        {
          arg1= va_arg(ap,int);
          putint(arg1,0);
          break;
        }
        case 'x':          // hexadezimale Ausgabe
        {
          xarg1= va_arg(ap,uint32_t);
          if (xarg1 <= 0xFFFF)
          {
            puthex(xarg1, 0);
          }
          else
          {
            puthex(xarg1 >> 16,0);
            puthex(xarg1 & 0xffff,1);
          }

          break;
        }
        case 'k':
        {
          arg1= va_arg(ap,int);
          putint(arg1,printfkomma);     // Integerausgabe mit Komma: 12896 zeigt 12.896 an
          break;
        }
        #if (printf_float_enable == 1)

          case '.':
          {
            s++;
            printfkomma= *s-'0';
            delf++;
          }
          case 'f':
          {
            arg1f= va_arg(ap,double);
            iex= 1;
            for (i= 0; i< printfkomma; i++) iex= iex* 10;
            putint((int)(arg1f*iex),printfkomma);     // Integerausgabe mit Komma: 12896 zeigt 12.896 an
            if (delf) {s++; delf--;}
            printfkomma= 2;
            break;
          }

        #endif
        case 'c':          // Zeichenausgabe
        {
          arg1= va_arg(ap,int);
          putchar(arg1);
          break;
        }
        case '%':
        {
          putchar(token);
          break;
        }
        case 's':
        {
          arg2= va_arg(ap,char *);
          putstring(arg2);
          break;
        }
      }
    }
    else
    {
      putchar(ch);
    }
    s++;
  }while (ch != '\0');
}

