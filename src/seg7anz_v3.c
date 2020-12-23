/* -------------------------------------------------------
                        seg7anz_v3.h

     Softwaremodul fuer max. 8-stellige 7-Segmentanzeige
     mittels zwei 74HC595 Schieberegistern

     Anmerkung: die Anzeigesegmente des werden ge-
                multiplext werden, da nur
                2 Schieberegister verwendet werden
                SR sind kaskadiert, zuerst ist der
                Datenwert der Ziffer, danach die
                Multiplexstelle auszuschieben.

                Zum Multiplexen wird Timer3 als
                interruptbetriebene Taktquelle
                eingesetzt.

                Dieser Interrupt toggelt zusaetzlich die
                globalen Variablen halfsec und zaehlt
                tim3_zsek hoch

     Hardware : 7-Segment LED Anzeigen
                2 Stck. SN74HC595 Schiebberegister
                               oder
                Chinamodul "4-Bit LED Digital Tube Modul"


     MCU       : STM32f030
     Takt      :

     02.03.2020  R. Seelig

     Anschlussbelegung siehe seg7anz_v3.h
   ------------------------------------------------------ */


#include "seg7anz_v3.h"

// Pufferspeicher der anzuzeigenden Ziffern
uint8_t seg7_4digit[8] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

// Bitmapmuster der Ziffern
uint8_t    led7sbmp[16] =
                { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
                  0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71 };

volatile uint32_t my_ticker;
volatile uint32_t tim3_zsek;
volatile char     halfsek;

void tim3_isr(void)
{
  static uint8_t segmpx= 0;


  TIM_SR(TIM3) &= ~TIM_SR_UIF;
  my_ticker++;
  if (!(my_ticker % 100)) tim3_zsek++;
  if (!(my_ticker % 500))
  {
    if (halfsek) { halfsek= 0; } else {halfsek= 1;}
  }

  digit4_outbyte(seg7_4digit[segmpx]);     // zuerst Zifferninhalt
  #if (mirror== 1)
    digit4_outbyte(1 << (3-segmpx));       // ... dann Position ausschieben
  #endif
  #if (mirror== 0)
    digit4_outbyte(1 << segmpx);           // ... dann Position ausschieben
  #endif

  segmpx++;
  segmpx= segmpx % 4;
  digit4_stpuls();                        // Inhalt Schieberegister ins Latch (und damit anzeigen)

}

static void tim3_init(void)
{
  // Timer3 Initialisierung ruft jede Millisekunde < tim3_isr > auf
  rcc_periph_clock_enable(RCC_TIM3);

  timer_reset(TIM3);
  timer_set_prescaler(TIM3, 4799);
  timer_set_period(TIM3, 10);
  nvic_enable_irq(NVIC_TIM3_IRQ);
  timer_enable_update_event(TIM3);
  timer_enable_irq(TIM3, TIM_DIER_UIE);
  timer_enable_counter(TIM3);
}


/* ----------------------------------------------------------
   digit4_delay

   Verzoegerungsfunktion zur Erzeugung von Takt und Strobe-
   impuls
   ---------------------------------------------------------- */
void digit4_delay(void)
{
  __asm volatile
  (
    "nop\n\r"
    "nop\n\r"
    "nop\n\r"
    "nop\n\r"
   );
}

/* ----------------------------------------------------------
   digit4_ckpuls

   nach der Initialisierung besitzt die Taktleitung low-
   Signal. Hier wird ein Taktimpuls nach high und wieder
   low erzeugt
   ---------------------------------------------------------- */
void digit4_ckpuls(void)
// Schieberegister Taktimpuls
{
  digit4_delay();
  srclock_set();
  digit4_delay();
  srclock_clr();
}

/* ----------------------------------------------------------
   digit4_stpuls

   nach der Initialisierung besitzt die Strobeleitung low-
   Signal. Hier wird ein Taktimpuls nach high und wieder
   low erzeugt
   ---------------------------------------------------------- */
void digit4_stpuls(void)
// Strobe Taktimpuls
{
  digit4_delay();
  srstrobe_set();
  digit4_delay();
  srstrobe_clr();
}


/* ----------------------------------------------------------
                          digit4_outbyte

   uebertraegt das Byte in - value - in das Schieberegister.
   ---------------------------------------------------------- */
void digit4_outbyte(uint8_t value)
{
  uint8_t mask, b;

  mask= 0x80;

  for (b= 0; b< 8; b++)
  {
    // Byte ins Schieberegister schieben, MSB zuerst
    #if (seg7_common == 1)
      if (mask & value) srdata_clr();              // 1 oder 0 entsprechend Wert setzen
                   else srdata_set();
    #else
      if (mask & value) srdata_set();              // 1 oder 0 entsprechend Wert setzen
                   else srdata_clr();
    #endif

    digit4_ckpuls();                             // ... Puls erzeugen und so ins SR schieben
    mask= mask >> 1;                             // naechstes Bit
  }
}

/* ----------------------------------------------------------
                           digit4_setdez

       gibt einen 4-stelligen dezimalen Wert auf der
       Anzeige aus
    --------------------------------------------------------- */
void digit4_setdez(uint32_t value, uint8_t nonull)
{
  uint32_t v, d;
  uint8_t  i;
  uint8_t  first;

  d= 10000000; first= 1;
  for (i= 0; i< 8; i++)
  {
    v= value / d;
    seg7_4digit[7-i] &= 0x80;             // eventuellen DP belassen
    if (first && (v== 0) && nonull)
    {
      seg7_4digit[7-i] |= 0x7f;
    }
    else
    {
      first= 0;
      seg7_4digit[7-i] |= (~led7sbmp[v]) & 0x7f;
    }
    value= value -(v * d);
    d= d / 10;
  }
}

/* ----------------------------------------------------------
                         digit4_setdez8bit

       gibt einen 2-stelligen dezimalen Wert auf der
       Anzeige aus

       pos    : Position, ab der die 2-stellige Zahl ausge-
                geben wird. Zulaessige Werte sind 0..6.
                0 entspricht Ausgabe rechts, 6 entspricht
                Ausgabe links.
    --------------------------------------------------------- */
void digit4_setdez8bit(uint8_t value, uint8_t pos)
{
    seg7_4digit[1+pos] &= 0x80;             // eventuellen DP belassen
    seg7_4digit[0+pos] &= 0x80;             // eventuellen DP belassen
    seg7_4digit[1+pos] |= (~led7sbmp[value / 10]) & 0x7f;
    seg7_4digit[0+pos] |= (~led7sbmp[value % 10]) & 0x7f;
}

/* ----------------------------------------------------------
                         digit4_sethex

       gibt einen 4-stelligen hexadezimalen Wert auf der
       Anzeige aus
    --------------------------------------------------------- */
void digit4_sethex(uint32_t value, uint8_t nonull)
{
  uint32_t v, d;
  uint8_t  i;
  uint8_t  first;

  d= 0x10000000; first= 1;
  for (i= 0; i< 8; i++)
  {
    v= value / d;
    seg7_4digit[7-i] &= 0x80;             // eventuellen DP belassen
    if (first && (v== 0) && nonull)
    {
      seg7_4digit[7-i] |= 0x7f;
    }
    else
    {
      first= 0;
      seg7_4digit[7-i] |= (~led7sbmp[v]) & 0x7f;
    }
    value= value -(v * d);
    d= d / 0x10;
  }


}

/* ----------------------------------------------------------
                        digit4_setall8

       setzt insgesamt 8 einzelne Segmente mit dem angegebenen
       Bitmuster.

       c7  => MSB
       c0  => LSB

       Hinweis:

       Die Segmente des Software-Moduls arbeiten mit
       aktiv - Low.

         0xff schaltet alle Segmente aus
         0x00 schaltet alle Segmente an

          a
         ---
      f | g | b            Segment | dp |  g  |  f  |  e  |  d  |  c  |  b  |  a  |
         ---               --------------------------------------------------------
      e |   | c            Bit-Nr. |  7 |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
         ---
          d                gr. "C" |  0    0     1     1     1     0     0     1

      fuer ein grosses "C" muessen Segmente a, d, e und f
      aktiviert werden. Wuerden die 1er realisiert werden
      entspraeche dieses 0x39.

      Bei (hier vorliegendem) aktive-LOW muessen die Bits
      invertiert werden:

      /0x39 = 0xc6

    --------------------------------------------------------- */
void digit4_setall8(uint8_t c7, uint8_t c6, uint8_t c5, uint8_t c4,
                    uint8_t c3, uint8_t c2, uint8_t c1, uint8_t c0)
{
  seg7_4digit[0] = c0;
  seg7_4digit[1] = c1;
  seg7_4digit[2] = c2;
  seg7_4digit[3] = c3;
  seg7_4digit[4] = c4;
  seg7_4digit[5] = c5;
  seg7_4digit[6] = c6;
  seg7_4digit[7] = c7;
}

/* ----------------------------------------------------------
                        digit4_setall4

       setzt insgesamt 4 einzelne Segmente mit dem angegebenen
       Bitmuster.

       c3  => MSB
       c0  => LSB

       Bedingungen wie digit4_setall8

    --------------------------------------------------------- */
void digit4_setall4(uint8_t c3, uint8_t c2, uint8_t c1, uint8_t c0)
{
  seg7_4digit[0] = c0;
  seg7_4digit[1] = c1;
  seg7_4digit[2] = c2;
  seg7_4digit[3] = c3;
}

/* ----------------------------------------------------------
                         digit4_setdp

       zeigt Dezimalpunkt an angegebener Position an
    --------------------------------------------------------- */
void digit4_setdp(uint8_t pos)
{
  seg7_4digit[pos] &= 0x7f;
}

/* ----------------------------------------------------------
                         digit4_clrdp

       loescht Dezimalpunkt an angegebener Position an
    --------------------------------------------------------- */
void digit4_clrdp(uint8_t pos)
{
  seg7_4digit[pos] |= 0x80;
}

/* ----------------------------------------------------------
                           digit4_clr

       loescht die gesamte Anzeige (alles aus)
    --------------------------------------------------------- */
void digit4_clr(void)
{
  uint8_t i;

  for (i= 0; i< 8; i++)  seg7_4digit[i] = 0xff;
}

/* ----------------------------------------------------------
                          digit4_init

     initalisiert alle GPIO Pins die das Schieberegister
     benoetigt als Ausgaenge und setzt alle Ausgaenge des
     Schieberegisters auf 0
   ---------------------------------------------------------- */
void digit4_init(void)
// alle Pins an denen das Modul angeschlossen ist als
// Ausgang schalten
{
  srdata_init();
  srstrobe_init();
  srclock_init();

  srdata_clr();
  srclock_clr();
  srstrobe_clr();

  digit4_outbyte(0);
  tim3_init();
}

