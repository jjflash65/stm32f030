/* ----------------------------------------------------------
                         reset_f103_b.c

   "Reset-Controller" fuer einen STM32F103

   Generiert die UART-Bootloaderbedingungen fuer ein
   STM32F103 Controller

   Programm empfaengt Impulse (vorzugsweise vom HOST auf
   RTS gesendet). 3 Impulse (Hi-Lo Flanke) schalten
   Bootloadermodus ein, 6 Impulse schalten diesen aus

   Hier:

   MCU:   ATtiny13
   Takt:  intern 4.8 MHz

   Fuses:
     lo: 79h
     hi: FFh

   03.03.2017 R. Seelig
   ---------------------------------------------------------- */

/*
                               ATtiny13 Pins

                               +-----------+
          /reset - PB5 - ADC0  | 1       8 |  Vcc
                   PB3 - CLKI  | 2       7 |  PB2 - SCK - ADC1
                   PB4 - ADC2  | 3       6 |  PB1 - MISO
                          GND  | 4       5 |  PB0 - MOSI
                               +-----------+
*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// PORT, DDR und PIN benoetigen keinen Bezeichner, da ein ATtiny nur einen PortB
// besitzt und somit nur PORTB, DDRB und PINB Gueltigkeit haben

#define bootsel          PB4         // Leitung zum aktivieren / deaktivieren des Bootloaders
#define rmabutton        PB3         // fuehrt ueber den Tiny13 einen Reset am STM32F103 durch

#define bootout          PB2         // Ausgang, der mit dem Boot0-Pin des STM32F103 verbunden ist
#define rmaout           PB1         // Ausgang, der mit dem Reset des STM32F103 verbunden ist
#define statled          PB0         // LED gegen GND geschaltet, zeigt aktiven Bootloaderzustand an



#define init_pins()      { DDRB |= (1 << bootout) | (1 << rmaout) | (1 << statled);    \
                           PORTB |= (1 << bootsel) | (1 << rmabutton); }

#define statled_set()    (PORTB |= (1 << statled))
#define statled_clr()    (PORTB &= ~(1 << statled))

#define boot_set()       (PORTB |= (1 << bootout))
#define boot_clr()       (PORTB &= ~(1 << bootout))

#define rma_clr()        (PORTB &= ~(1 << rmaout))
#define rma_set()        (PORTB |= (1 << rmaout))

#define is_rmabutton()   ( !( PINB & (1 << rmabutton) ))


#define debtime          20           // Entprellzeit der Taster

#define btime            15           // Zeit, die Boot0 vor einem Reset auf 1
                                      // gezogen wird UND nach einem Reset auf 1 bleibt
#define rtime            15           // Zeit, die ein Resetimpuls andauert


#define  mftime          25           // "Monoflop-Time" : die Anzahl der im Timervektor herunter-
                                      // gezaehlten Zyklen. Timer wird alle 3,413 mS aufgerufen.
                                      // D.h. innerhalb von 35 * 3,413mS koennen die Impulse zum aktivieren
                                      // und deaktiveren des Bootloaders eintreffen

uint8_t              bootactive = 0;
volatile uint16_t    timcx;
volatile uint8_t     rts_cnt = 0;
volatile uint8_t     rts_recv = 0;

/* ------------------------------------------------------------
                           stm32_reset
     fuehrt per Bitbanging einen Reset am STM32 aus
   ------------------------------------------------------------ */
void stm32_reset(void)
{
  rma_set();
  _delay_ms(rtime);
  rma_clr();                      // Resetimpuls
  _delay_ms(rtime);
  rma_set();
}

/* ------------------------------------------------------------
                         stm32_runstate
     deaktiviert den Bootloadermodus und fuehrt Reset durch
   ------------------------------------------------------------ */
void stm32_runstate(void)
{
  boot_clr();
  _delay_ms(btime);
  stm32_reset();
  statled_clr();
  bootactive= 0;
}

/* ------------------------------------------------------------
                   Pinchange - Interruptvektor
     wird durch eingehenden Impuls auf PB4 ausgeloest und
     zaehlt innerhalb des Vectors die eingegangenen Impulse
   ------------------------------------------------------------ */
ISR (PCINT0_vect)
{
  rts_recv = 1;
  rts_cnt++;
}


/* ------------------------------------------------------------
               Timer0 Overflow - Interruptvektor
      wird alle 3,141 mS aufgerufen und zaehlt den Counter
      timcx herunter. Bestimmt somit die Torzeit fuer ein-
      gehende Impulse auf PB4
   ------------------------------------------------------------ */
ISR (TIM0_OVF_vect)
{
  if (timcx) --timcx;
}

/* ------------------------------------------------------------
                          init_timer
      initialisiert Timer0 als Overflowtimer. Timer laueft
      alle 3,414 mS "ueber"
   ------------------------------------------------------------ */
void init_timer(void)
{
  // Interrupttiming, Bsp.: Prescaler Fcpu / 64
  // (64 / 4800000Hz) * 256 = 3.413 mS
  TCCR0B = (1 << CS01) | (1 << CS00);            // prescale Fcpu / 64

  TIMSK0 = 1 << TOIE0;                           // Timer overflow interrupt
  timcx= mftime;
}

/* ------------------------------------------------------------
                       init_pinchange_int
     festlegen, dass PB4 als Pinchange Interupt ausloest
   ------------------------------------------------------------ */
void init_pinchange_int(void)
{
  PCMSK |= (1 << PCINT4);
  GIMSK |= (1 << PCIE);
}

/* ---------------------------------------------------------------------------------------------
                                            M-A-I-N
   --------------------------------------------------------------------------------------------- */
int main(void)
{
  _delay_ms(900);                                // damit evtl. Impulse eines USB2RS232 Chips
                                                 // beim Anlegen der Vcc keine Auswirkungen
                                                 // haben
  init_pins();
  init_timer();
  init_pinchange_int();

  statled_clr();
  boot_clr();
  stm32_reset();

  sei();                                        // Interrupts zulassen
  _delay_ms(500);
  rts_recv= 0;                                  // keine Eingegangenen Impulse

  statled_set();                                // Kontrollblinken, zeigt an, dass Tiny13 aktiv ist
  _delay_ms(100);
  statled_clr();

  while(1)
  {
    if (rts_recv)                               // eingegangener Impuls vom Host ( RTS )
    {
      timcx= mftime;
      while(timcx);                             // warten bis interruptgesteuerte Zeit abgelaufen ist
      cli();

      switch (rts_cnt)
      {
        case 6 :                                // 3 gesendete Impulse erzeugen 6 Pin changes (H-L ; L-H)
          {                                     // Bootloader aktiv setzen
            boot_set();                         // Boot0 Pin = 1
            _delay_ms(btime);
            stm32_reset();
            statled_set();
            bootactive= 1;
            break;
          }
        case 12 :                               // 6 gesendete Impulse erzeuge 12 Pin changes
          {                                     // Bootloader deaktivieren
            bootactive= 0;
            stm32_runstate();
            break;
          }
        default : break;
      }

      sei();
      rts_recv= 0;                              // Eingangsimpulse abgearbeitet
      rts_cnt= 0;                               // Impulszaehler zu null
    }
    if (is_rmabutton())                         // Reset Taster fuer F103
    {
      if(!(bootactive))
      {
        rma_clr();
        _delay_ms(debtime);
        while(is_rmabutton());
        _delay_ms(debtime);
        rma_set();
      }
      else
      {
        rts_recv= 0;
        bootactive= 0;
        stm32_runstate();
      }
    }
  }
}
