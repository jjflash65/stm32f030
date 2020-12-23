/* -------------------------------------------------------
                         ana_uhr.c

     Darstellung einer analogen Uhr auf einem Grafphic-LCD
     mit Aufloesung von 128x128 oder 128x160 Pixel

     Demonstriert u.a. die Verwenung von Timer3 im
     Interruptbetrieb

     MCU   :  STM32F030F4P6
     Takt  :  interner Takt 48 MHz

     26.02.2026  R. Seelig
   ------------------------------------------------------ */

/* -----------------------------------------------------------------------------------
      Displays aus China werden haeufig mit unterschiedlichen
      Bezeichnungen der Pins ausgeliefert. Deshalb sind in der folgenden
      Tabelle mehrere "Anschlussnamen" fuer das Display vergeben. Aktuell
      eingestellte Pinzuordnungen (in tftdisplay.h als #define pindefs 3) ist:

      Controller STM32F030          Display
      --------------------------------------------------------------------------
         SPI-SCK  / PA5    ----    SCK / CLK    (clock)
         SPI-MOSI / PA7    ----    SDA / DIN    (data in display)
         SPI-SS   / PA4    ----    CS  / CE     (chip select display)
                    PA0    ----    A0  / D/C    (selector data or command write)
                    PA1    ----    Reset / RST  (reset)

   ------------------------------------------------------------------------------------ */

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <libopencm3.h>

#include "sysf030_init.h"
#include "tftdisplay.h"
#include "gfx_pictures.h"
#include "my_printf.h"

#include "math_fixed.h"

#define printf        my_printf

#define intbit(wert,nr)    ((wert) & (1<<nr))
#define setintbit(wert,nr) (wert |= (1<<nr))
#define clrintbit(wert,nr) (wert &= ~(1<<nr))

/* ----------------------------------------------------------
                     Tastenanschluesse
   ---------------------------------------------------------- */
#define butre_init()    PB1_input_init()                    // rechts
#define butup_init()    PA6_input_init()                    // oben
#define butli_init()    PA2_input_init()                    // links
#define butdw_init()    PA3_input_init()                    // unten

#define is_butre()      (!is_PB1())                         // rechts
#define is_butup()      (!is_PA6())                         // oben
#define is_butli()      (!is_PA2())                         // links
#define is_butdw()      (!is_PA3())                         // unten

/* ----------------------------------------------------------
                 Anzeigepositionen und Aussehen
   ---------------------------------------------------------- */

#define xuo             2                                   // X-Achse Anzeigeoffset fuer analoge Uhrendarstellung
#define yuo             0                                   // dto. Y-Achse
#define ruhr            53                                  // Radius der analogen Uhrendarstellung
#define stdzeig         24                                  // Laenge Stundenzeiger, je kleiner Zahl umso groesser Zeiger


/* ----------------------------------------------------------
                   Timing der Bedientasten
   ---------------------------------------------------------- */
#define tasthispeed     70
#define tastlospeed     400

/* ----------------------------------------------------------
                 globale Variable
   ---------------------------------------------------------- */

uint16_t ziffbk;                                            // Hintergrundfarbe des Ziffernblatts

char wtag[7][3] = {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};

volatile int std,min,sek;
volatile int year, month, day;
volatile int oldstd, oldmin, oldsek;

volatile uint32_t my_ticker;                                // Zaehler der im Timerinterrupt hochgezaehlt wird
volatile uint32_t intv_ticker = 0;                          // Intervallticker fuer Tastenabfrage
volatile char    halfsek;                                   // 0,5 Sekundentakt, generiert durch Interrupt

/* ----------------------------------------------------------
                            tim3_isr
        Timer3 Interrupt, generiert eine Softwareuhr inkl.
        Tag, Monat, Jahr
   ---------------------------------------------------------- */
void tim3_isr(void)
{
  TIM_SR(TIM3) &= ~TIM_SR_UIF;
  my_ticker++;
  intv_ticker++;
  intv_ticker = intv_ticker % 30000;
  if (!(my_ticker % 500))
  {
    if (halfsek) { halfsek= 0; } else {halfsek= 1;}
  }
  if (!(my_ticker % 1000))
  {
    sek++;
    sek= sek % 60;
    if (!(sek))
    {
      min++;
      min= min % 60;
      if (!(min))
      {
        std++;
        if (std==24)
        {
          std= 0;

          // ab hier Kalenderfunktionen
          if (day< 28)                                            // Monatsende nicht erreicht
          {
            day++;
            return;
          }

          if (month== 2)
          {
            if ((year % 4)== 0)                                   // Schaltjahr
            {
              if (day==28)
              {
                day++;
                return;
              }
              else
              {
                month++;
                day= 1;
                return;
              }
            }
            else
            {
              month++;
              day= 1;
              return;
            }
          }

          if ((month==4) || (month==6) || (month== 9) || (month== 11))     // April, Juni, September, November
          {
            if (day< 30)
            {
              day++;
              return;
            }
            else
            {
              day= 1;
              month++;
              return;
            }
          }

          // ab hier alle Monate mit 31 Tagen

          if ((month== 12) && (day== 31))                                // Feuerwerk, es ist Sylvester
          {
            day= 1;
            month= 1;
            year++;
            return;
          }

          if (day< 31)                                                  // Monatsende nicht erreicht
          {
            day++;
            return;
          }

          day= 1;                                                       // monthsende der Monate mit 31 Tagen
          month++;
          return;

        }
      }
    }
  }
}

/* --------------------------------------------------------
                        tim3_init

     initialisiert den Timer3 so, dass dieser jede Milli-
     sekunde einen Interrupt ausloest
   -------------------------------------------------------- */
static void tim3_init(void)
{
  rcc_periph_clock_enable(RCC_TIM3);

  timer_reset(TIM3);
  timer_set_prescaler(TIM3, 4362);
  timer_set_period(TIM3, 10);
  nvic_enable_irq(NVIC_TIM3_IRQ);
  timer_enable_update_event(TIM3);
  timer_enable_irq(TIM3, TIM_DIER_UIE);
  timer_enable_counter(TIM3);
}

/* --------------------------------------------------------
                       zeigerpos

  Berechnung der Endkoordinaten eines graphischen Zeigers
  vom Mittelpuntk aus gesehen:

     x,y     : Koordinaten Mittelpunkt
     r       : Radius des Zeigers
     w       : Winkel des Zeigers
     x2,y2   : Koordinaten des Endpunktes desf Zeigers
   -------------------------------------------------------- */
void zeigerpos(int x, int y, int r, int w, int *x2, int *y2)
{
  int w2;
  float a;

  w2= 90 - w;
  a = r * (fk_cos(w2 * (MY_PI / 180.0f)));
  *x2= (int)x+a;
  a = r * (fk_sin(w2 * (MY_PI / 180.0f)));
  *y2= (int)y-a;
}

/* --------------------------------------------------------
                      drawstdskala

       Zeichnet die Stundeneinteilung einer Uhr
   -------------------------------------------------------- */
void drawstdskala(int x, int y, int r1, int r2, uint16_t col)
{
  int i;
  int zx1,zy1, zx2,zy2;

  for (i= 0; i < 12; i++)
  {
    zeigerpos(x,y,r1,i*30,&zx1,&zy1);
    zeigerpos(x,y,r2,i*30,&zx2,&zy2);
    line(zx1,zy1,zx2,zy2,col);
  }
}

/* --------------------------------------------------------
                      drawminskala

        Zeichnet die Minuteneinteilung einer Uhr
   -------------------------------------------------------- */
void drawminskala(int x, int y, int r1, int r2, uint16_t col)
{
  int i;
  int zx1,zy1, zx2,zy2;

  for (i= 0; i < 60; i++)
  {
    zeigerpos(x,y,r1,i*6,&zx1,&zy1);
    zeigerpos(x,y,r2,i*6,&zx2,&zy2);
    line(zx1,zy1,zx2,zy2,col);
  }
}

/* --------------------------------------------------------
                         drawzweiger

    Zeichnet einen "dicken" Zeiger, vom Mittelpunkt x,y
    mit dem  Radius r und dem Winkel w in der Farbe col
   --------------------------------------------------------*/
void drawzeiger(int x, int y, int r, int w, uint16_t col)
{
  int x2,y2;

  zeigerpos(x,y,r,w, &x2 ,&y2);
  line(x,y,x2,y2,col);

  zeigerpos(x+1,y,r,w, &x2 ,&y2);
  line(x+1,y,x2,y2,col);
  zeigerpos(x-1,y,r,w, &x2 ,&y2);
  line(x-1,y,x2,y2,col);

  zeigerpos(x,y+1,r,w, &x2 ,&y2);
  line(x,y+1,x2,y2,col);
  zeigerpos(x,y-1,r,w, &x2 ,&y2);
  line(x,y-1,x2,y2,col);
}


/* --------------------------------------------------------
                         drawsmallzweiger

    Zeichnet einen "duennen" Zeiger, vom Mittelpunkt x,y
    mit dem  Radius r und dem Winkel w in der Farbe col
   --------------------------------------------------------*/
void drawsmallzeiger(int x, int y, int r, int w, uint16_t col)
{
  int x2,y2;

  zeigerpos(x,y,r,w, &x2 ,&y2);
  line(x,y,x2,y2,col);
}

/* --------------------------------------------------------
                          showzeiger

      zeichnet die Zeiger einer Uhr.

      ziffbk   : Farbwert des Ziffernblatts der Uhr
      clear    : 0 = Zeiger werden gezeichnet
                 1 = Zeiger werden mit Hintergrundfarbe
                     gezeichnet (und somit geloescht)
   -------------------------------------------------------- */
void showzeiger(uint8_t astd, uint8_t amin, uint8_t asek, uint16_t ziffbk, uint8_t clear)
{
  if (astd > 12) astd -= 12;
  if (clear)
  {
    drawzeiger(xuo+ruhr+9, yuo+ruhr+9, ruhr-(ruhr/5)-20, amin*6, ziffbk);
    drawzeiger(xuo+ruhr+9, yuo+ruhr+9, ruhr-(ruhr/5)-stdzeig, (astd*30) + (amin / 2), ziffbk);
    drawsmallzeiger(xuo+ruhr+9, yuo+ruhr+9, ruhr-(ruhr/5)-20, asek*6, ziffbk);
  }
  else
  {
    drawzeiger(xuo+ruhr+9, yuo+ruhr+9, ruhr-(ruhr/5)-20, amin*6, rgbfromega(red));
    drawzeiger(xuo+ruhr+9, yuo+ruhr+9, ruhr-(ruhr/5)-stdzeig, (astd*30) + (amin / 2), rgbfromega(lightred));
    drawsmallzeiger(xuo+ruhr+9, yuo+ruhr+9, ruhr-(ruhr/5)-20, asek*6, rgbfromega(black));
  }
}


/* --------------------------------------------------------
                      spiro_generate

     zeichnet ein "Spirograph"

     inner:  innerer Radius
     outer:  aeusserer Radius
     evol :  Anzahl der "Schwingungen" fuer eine Umdrehung
     resol:  Aufloesung (Schrittweite) beim Zeichnen
             des Graphen
     col  :  Farbe, mit der der Graph gezeichnet wird
   -------------------------------------------------------- */
void spiro_generate(int inner, int outer, int evol, int resol, uint16_t col)
{
  const int c_width  = 128;
  const int c_height = 128;
  float     inner_xpos, inner_ypos;
  float     outer_xpos, outer_ypos;
  float     j, k;
  int       i;

  inner_xpos = (c_width / 2.0f);
  inner_ypos = (c_height / 2.0f) + inner;

  outer_xpos= inner_xpos;
  outer_ypos= inner_ypos + outer;
  turtle_moveto(outer_xpos, outer_ypos);

  for (i= 0; i< resol + 1; i++)
  {
    j= ((float)i / resol) * (2.0f * MY_PI);
    inner_xpos = (c_width / 2.0f) + (inner * fk_sin(j));
    inner_ypos = (c_height / 2.0f) + (inner * fk_cos(j));

    k= j * ((float)evol / 10.0f);

    outer_xpos= inner_xpos + (outer * fk_sin(k));
    outer_ypos= inner_ypos + (outer * fk_cos(k));

    turtle_lineto(outer_xpos, outer_ypos, col);
//    delay(15);
  }
}

/* -------------------------------------------------------
                      stellen_screen

     Bildschirmmaske zum Uhrzeiteinstellen
   ------------------------------------------------------ */
void stellen_screen(void)
{
  bkcolor= rgbfromega(7);
  clrscr();
  fillrect(0,0,127,24,rgbfromega(8));
  bkcolor= rgbfromega(8);
  textcolor= rgbfromega(14);
  gotoxy(1,1);
  printf("Uhr stellen");
  textcolor= rgbfromega(1);
  bkcolor= rgbfromega(7);
  gotoxy(0,4);
  printf("\n\r Stunde :");
  printf("\n\r Minute :");
  printf("\n\r Jahr   :");
  printf("\n\r Monat  :");
  printf("\n\r Tag    :");
}

/* -------------------------------------------------------
                           getwtag

   Wochentagsberechnung nach Carl-Friedrich-Gauss

   liefert den Wochentag zu einem Datum zurueck:
   0 = Sonntag  .. 6 = Samstag
   ------------------------------------------------------- */
char getwtag(int tag, int monat, int jahr)
{
  int w_tag;

  if (monat < 3)
  {
     monat = monat + 12;
     jahr--;
  }
  w_tag = (tag+2*monat + (3*monat+3)/5 + jahr + jahr/4 - jahr/100 + jahr/400 + 1) % 7 ;
  return w_tag;
}

/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  lcd_putchar(ch);
}


/* -------------------------------------------------------
                          putdez2
     zeigt die 2 stellige, vorzeichenbehaftete dezimale
     Zahl in val an.

     mode greift bei Zahlen kleiner 10:

     mode        0  : es wird eine fuehrende 0 ausgegeben
                 1  : es wird anstelle einer 0 ein Leer-
                      zeichen ausgegeben
                 2  : eine fuehrende 0 wird unterdrueckt
   ------------------------------------------------------- */
void putdez2(signed char val, uint8_t mode)
{
  char b;
  if (val < 0)
  {
    my_putchar('-');
    val= -val;
  }
  b= val / 10;
  if (b == 0)
  {
    switch(mode)
    {
      case 0 : my_putchar('0'); break;
      case 1 : my_putchar(' '); break;
      default : break;
    }
  }
  else
    my_putchar(b+'0');
  b= val % 10;
  my_putchar(b+'0');
}

/* -------------------------------------------------------
                      butre_counter

      Zaehlt Variable cnt hoch, wenn Taster rechts
      gedrueckt ist. Bleibt Taster rechts laengere Zeit
      gedrueckt, wird die Zaehlfrequenz erhoeht.
      Der Zaehlwert ist das Rueckgabeergebnis

      outx,outy    : Koordinaten auf dem Display an
                     der das Zaehlen angezeigt wird

      maxcnt       : Maximalwert-1, den die Variable cnt
                     erreichen kann.
      addtocnt     : Offsetwert, der zum Zaehlergebnis
                     hinzuaddiert wird
   ------------------------------------------------------ */
uint8_t butre_counter(uint8_t outx, uint8_t outy, uint8_t maxcnt, uint8_t cnt, uint8_t addtocnt)
{
  uint16_t cntspeed;

  delay(50);
  intv_ticker= 0;
  cntspeed= tastlospeed;
  while(is_butre())
  {
    if (intv_ticker> 2000) cntspeed= tasthispeed;
    cnt++;
    cnt = cnt % maxcnt;
    gotoxy(outx,outy);
    putdez2(cnt+addtocnt,2);
    my_putchar(' ');
    delay(cntspeed);
  }
  delay(50);
  return (cnt+addtocnt);
}

/* -------------------------------------------------------
                         digitalscreen

     zeigt die Uhrzeit und das Datum digital an
   ------------------------------------------------------- */
void digitalscreen(void)
{
  uint8_t temp;

  temp= getwtag(day, month, year);

  bkcolor= 0;
  textcolor= rgbfromvalue(0x20, 0x20, 0xff);
  textcolor= rgbfromvalue(0x60, 0x60, 0x60);
  gotoxy(4,14);
  putdez2(std,1); my_putchar(':');
  putdez2(min,0); my_putchar('.');
  putdez2(sek,0);

  gotoxy(2,15);
  printf("%c%c  ",wtag[temp][0],wtag[temp][1]);
  putdez2(day,0); my_putchar('.');
  putdez2(month,0); my_putchar('.');
  putdez2(year,0);

}

/* -------------------------------------------------------
                         uhrscreen

     zeichnet das Ziffernblatt fuer eine analoge Uhren-
     anzeige
   ------------------------------------------------------- */
void uhrscreen(void)
{
  textcolor= rgbfromega(7);
  bkcolor= rgbfromega(0);
  ziffbk= rgbfromvalue(0xaa, 0xaa, 0xff);
  clrscr();

  // Hintergrund zeichnen
  spiro_generate(32, 32, 140,220, rgbfromega(1));
  // Ziffernblatt der analogen Uhr zeichnen
  fillcircle(xuo+ruhr+9,yuo+ruhr+9,ruhr-8,rgbfromvalue(0x50, 0x50, 0xff));
  circle(xuo+ruhr+9,yuo+ruhr+9,ruhr-8,rgbfromvalue(0x00, 0x00, 0x80));
  fillcircle(xuo+ruhr+9,yuo+ruhr+9,ruhr-12, ziffbk);

  drawminskala(xuo+ruhr+9, yuo+ruhr+9, ruhr-(ruhr/10)-16, ruhr-16, rgbfromega(9));
  drawstdskala(xuo+ruhr+9, yuo+ruhr+9, ruhr-(ruhr/5)-16, ruhr-16, rgbfromega(0));

  digitalscreen();
}


/* ------------------------------------------------------------------------
                                   main
   ------------------------------------------------------------------------ */
int main(void)
{
  uint8_t my_std, my_min, my_year, my_month, my_day;
  uint8_t z_year, e_year;

//  sys_init_extclk();                          // externer 8MHz Quarz hohe Ganggenauigkeit
  sys_init();                                   // interner Taktgeber, eigentlich fuer eine Uhr ungeeignet
  lcd_init();
  lcd_enable();
  outmode= 2;

  tim3_init();

  std= 15; min= 38; sek= 11;
  day= 27; month= 2; year= 20;

  uhrscreen();

  oldsek= sek;
  oldmin= min;
  oldstd= std;

  while(1)
  {
    if (sek != oldsek)
    {
      // alte Uhrstellung loeschen
      showzeiger(oldstd, oldmin, oldsek, ziffbk, 1);
      oldsek= sek;
      oldmin= min;
      oldstd= std;
      // neue Uhrstellung anzeigen
      showzeiger(std, min, sek, ziffbk, 0);
      digitalscreen();
    }
    while (oldsek == sek)
    {
      if (is_butli())
      {
        stellen_screen();
        my_std= std; my_min= min; my_year= year; my_month= month; my_day= day;
        gotoxy(10,5); putdez2(my_std,2); my_putchar(' ');

        delay(50);
        while(is_butli());
        delay(50);

        while(!is_butli())
        {
          if (is_butre())
          {
            my_std= butre_counter(10,5,24, my_std, 0);
          }
        }
        delay(50);
        while(is_butli());
        delay(50);

        gotoxy(10,6); putdez2(my_min,2); my_putchar(' ');
        while(!is_butli())
        {
          if (is_butre())
          {
            my_min= butre_counter(10,6,60, my_min, 0);
          }
        }
        delay(50);
        while(is_butli());
        delay(50);

        z_year= (my_year % 100) / 10;
        gotoxy(10,7); printf("20");
        putdez2(z_year,2);
        while(!is_butli())
        {
          if (is_butre())
          {
            z_year= butre_counter(12,7,10, z_year, 0);
          }
        }
        delay(50);
        while(is_butli());
        delay(50);

        e_year= my_year % 10;
        gotoxy(10,7); printf("20");
        putdez2(z_year,2);
        putdez2(e_year,2);

        while(!is_butli())
        {
          if (is_butre())
          {
            e_year= butre_counter(13,7,10, e_year, 0);
          }
        }
        my_year= (z_year*10)+e_year;
        delay(50);
        while(is_butli());
        delay(50);

        gotoxy(10,8); putdez2(my_month,2); my_putchar(' ');
        while(!is_butli())
        {
          if (is_butre())
          {
            my_month= butre_counter(10,8,12, my_month-1, 1);
          }
        }
        delay(50);
        while(is_butli());
        delay(50);

        gotoxy(10,9); putdez2(my_day,2); my_putchar(' ');
        while(!is_butli())
        {
          if (is_butre())
          {
            if ((my_month == 2) && ((my_year % 4) == 0))           // Februar im Schaltjahr
              my_day= butre_counter(10,9,29, my_day-1, 1);
            else
            if ((my_month == 2) && ((my_year % 4) > 0))            // Februar im Nichtschaltjahr
              my_day= butre_counter(10,9,28, my_day-1, 1);
            else
            if ((my_month==4) || (my_month==6) || (my_month==9) || (my_month== 11))
              my_day= butre_counter(10,9,30, my_day-1, 1);
            else
              my_day= butre_counter(10,9,31, my_day-1, 1);
          }
        }
        delay(50);
        while(is_butli());
        delay(50);


        std= my_std; min= my_min; sek= 0; oldsek= 61;
        year= (z_year * 10) + (e_year); month= my_month; day= my_day;
        uhrscreen();
      }
    }
  }

  while(1);
}

