/* -------------------------------------------------------
                         tetris_ega.c
                     YetAnotherTetrisGame

     Bricks - Breakout Game fuer STM32F030 mit 128x128 TFT
     SPI Display

     MCU   :  STM32F030
     Takt  :  48 MHz

     27.02.2020  R. Seelig

     Hinweis: Mit dem Startbildschirm in bricksscr5.h
     steigt der Flashspeicherbedarf knapp ueber 16 kB
     an. Zwar haben viele Chips des STM32F030F4P6 tat-
     saechlich 32k, darauf verlassen sollte kann man sich
     jedoch darauf nicht.

     Sollte also ein Chip mit 16k verwendet werden, so
     ist das speicherreduzierte CGA-Logo zu verwenden.
     Das Spiel selbst bleibt jedoch farbiger.
   ------------------------------------------------------ */

/* -----------------------------------------------------------------------------------
      Displays aus China werden haeufig mit unterschiedlichen
      Bezeichnungen der Pins ausgeliefert. Moegliche
      Pinzuordnungen sind:

      Controller STM32F030          Display
      --------------------------------------------------------------------------
         SPI-SCK  / PA5    ----    SCK / CLK    (clock)
         SPI-MOSI / PA7    ----    SDA / DIN    (data in display)
         SPI-SS   / PA4    ----    CS  / CE     (chip select display)
                    PA3    ----    A0  / D/C    (selector data or command write)
                    PB1    ----    Reset / RST  (reset)

   ------------------------------------------------------------------------------------ */

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <libopencm3.h>

#include "sysf030_init.h"
#include "tftdisplay.h"
#include "my_printf.h"
#include "gfx_pictures.h"

#define use_ega_logo            0        // 1: ein 16 Farben-Logo wird verwendet
                                         // 0: ein 4 Farben-Logo wird verwendet


#if (use_ega_logo == 1)
  #include "./include_bmps/tetrislogo_ega.h"
#else
  #include "./include_bmps/tetrislogo_cga.h"
#endif

#include "./include_bmps/tetleft.h"
#include "./include_bmps/tetright.h"

#define intbit(wert,nr)    ((wert) & (1<<nr))
#define setintbit(wert,nr) (wert |= (1<<nr))
#define clrintbit(wert,nr) (wert &= ~(1<<nr))

#define tast1_init()    PB1_input_init()                    // rechts
#define tast2_init()    PA6_input_init()                    // oben
#define tast3_init()    PA2_input_init()                    // links
#define tast4_init()    PA3_input_init()                    // unten

#define is_tast1()      is_PB1()                            // rechts
#define is_tast2()      is_PA6()                            // oben
#define is_tast3()      is_PA2()                            // links
#define is_tast4()      is_PA3()                            // unten


#define  keyleft      3
#define  keyright     1
#define  keyup        2
#define  keydown      4

volatile char keys;

#define printf       my_printf


#define klotzgr     5
#define dryofs      20                                      // Y-Offset an der die Kloetzchen beginnen
#define drxofs      40                                      // dto. X-Koordinate
#define bmpofs      13                                      // X-Offset, ab der Images ausgegeben werden

#define gamrows     15
#define gamcol      11

#define scorepos    5,0
#define intime      60
#define outerloop   2


#define figanz   7
const uint16_t tetfigures[figanz][4] =
{
                                          //   o
   { 0xe400, 0x4c40, 0x4e00, 0x4640 },    //  ooo     Figur 0

                                          //  o
   { 0xe200, 0x44c0, 0x8e00, 0x6440 },    //  ooo     Figur 1

                                          //    o
   { 0xe800, 0xc440, 0x2e00, 0x4460 },    //  ooo     Figur 2

                                          //   oo
   { 0x6c00, 0x8c40, 0x6c00, 0x8c40 },    //  oo      Figur 3

                                          //  oo
   { 0xc600, 0x4c80, 0xc600, 0x4c80 },    //   oo     Figur 4

                                          //  oo
   { 0xcc00, 0xcc00, 0xcc00, 0xcc00 },    //  oo      Figur 5

                                          //
   { 0xf000, 0x4444, 0xf000, 0x4444 }     //  oooo    Figur 6
};

// Farbwerte der Kloetzchen: oberes Nibble  = Umrandungsfarbe,
//                           unteres Nibble = Fuellfarbe
const uint8_t colpair[figanz] =
{
  0x4e, 0x2a, 0x3b, 0x87, 0x5d, 0x19, 0x4c
};


uint8_t    tetfeld[gamrows][gamcol+2];       // Tetris Spielfeld, nimmt gamrows -  Reihen zu je gamcol Positionen + 2
                                             // Feldbegrenzer

uint8_t    aktfig, aktrot;
char       figx, figy;
uint8_t    hb;                               // Hilfsbyte
uint8_t    loopcx;                           // Schleifenzaehler fuer Zeit und Tastenaktion
uint8_t    innerloop;
int        rndinit = 0;
char       action;                           // Auszufuehrende Aktion waehrend des Spiels
uint16_t   hint;                             // Hilfsinteger
uint16_t   scorecx;
uint8_t    gameover;


uint16_t get16zufall(uint16_t startwert, uint16_t xormask)
/*
   xormask:
      0xb400 => Zahlenanzahl 65535
      0x07c1 =>       "      2047
      0x0be0 =>       "      4095
*/

{
  static uint16_t start_state= 1;
  static uint16_t lfsr= 1;
  static char first = 0;

  uint16_t lsb;

  if (first== 0)
  {
    first= 1;
    start_state= startwert;
    lfsr= start_state;
  }

  lsb = lfsr & 1;                        // niederwertigstes Bit des Schieberegisters
  lfsr = lfsr >> 1;                      // das Schieberegister, eine Stelle nach rechts
  if (lsb) { lfsr ^= xormask; }          // wenn LSB gesetzt, XOR-Togglemaske auf SR anwenden

  return lfsr;
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


uint8_t getalltast(void)
{
  uint8_t b;

  b= 0;
  if (!is_tast1()) b |= 1;
  if (!is_tast2()) b |= 2;
  if (!is_tast3()) b |= 4;
  if (!is_tast4()) b |= 8;

  return b;
}

void tim3_isr(void)
{
  TIM_SR(TIM3) &= ~TIM_SR_UIF;
  keys= getalltast();
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


void showscore(void)
{
  gotoxy(scorepos);
  textcolor= rgbfromega(2);
  printf(" Score: ");
  textcolor= rgbfromega(10);
  printf("%d ",scorecx);
}

void drawklotz(char xpos, char ypos, uint8_t fa, char mode)
{
  int x,y;

  x= (xpos*(klotzgr+2))+drxofs;
  y= (ypos*(klotzgr+2))+dryofs;
  if (fa== 0x08)
  {
    line(x+(klotzgr / 2),y-1, x+(klotzgr/2), y+klotzgr, rgbfromega(8) );
  }
  else
  {
    if (mode)
    {
      fillrect(x+1,y+1, x+klotzgr-1, y+klotzgr-1, rgbfromega(fa & 0x0f));
//      rectangle(x,y, x+klotzgr, y+klotzgr, rgbfromega((fa >> 4) & 0x0f));
      rectangle(x,y, x+klotzgr, y+klotzgr, rgbfromega((fa) & 0x0f));
    }
    else
    {
      fillrect(x+1,y+1, x+klotzgr-1, y+klotzgr-1, rgbfromega(0));
      rectangle(x,y, x+klotzgr, y+klotzgr, rgbfromega(0));
    }
  }
}

void drawfig(char xpos, char ypos, uint16_t figbmp, uint8_t fa, char mode)
{
  signed char      x,y,i;

  for (i= 15; i>= 0; i--)
  {
    x= (15-i)%4;
    y= (15-i)/4;
    if (intbit(figbmp,i)) drawklotz(x+xpos, y+ypos, fa, mode);
  }
}

char readkey()
{
  if (keys & 0x01) return 1;           // Taste rechts  / keyright
  if (keys & 0x02) return 2;           // Taste oben    / keyup
  if (keys & 0x04) return 3;           // Taste links   / keyleft
  if (keys & 0x08) return 4;           // Taste runter  / keydown
  return 0;
}

void gamefeld_init(void)
{
  uint8_t x, y;

  for (y= 0; y< gamrows; y++)
  {
    for (x= 1; x< (gamcol+1); x++)
    {
      tetfeld[y][x]= 0;
    }
    tetfeld[y][0]= 0xff;
    tetfeld[y][gamcol+1]= 0xff;
  }
  for (x= 0; x< (gamcol+2); x++)
  {
    tetfeld[gamrows-1][x]= 0xff;
  }
}

void drawfeld(void)
{
  uint8_t x, y, b;

  for (y= 0; y< gamrows-1; y++)
  {
    for (x= 0; x< (gamcol+2); x++)
    {
      b= tetfeld[y][x];
      if (b== 0xff)
      {
        drawklotz(x,y,0x08,1);
      }
      if (b== 0x00)
      {
        drawklotz(x,y,0x07,0);
      }
      if ((b>0) && (b != 0xff))
      {
        drawklotz(x,y,b+8,1);
      }
    }
  }
}

/* -------------------------------------------------
                        canposxy
     testet, ob an der angegebenen Position die
     uebergebene Figur positionierbar ist !
   ------------------------------------------------- */
char canposxy(char fx, char fy, uint16_t fig)
{
  uint8_t x,y,ind;

  ind= 15;
  for (y= fy; y< (fy+4); y++)
  {
    for (x= fx; x< (fx+4); x++)
    {
      if ((tetfeld[y][x]> 0) & (intbit(fig,ind)> 0))
      {
        return 0;
      }
      ind--;
    }
  }
  return 1;
}

/* -------------------------------------------------
                        copyfig
    kopiert die aktuelle Spielefigur in der
    aktuellenk Rotationslage ins Spielegamefeld
    mit der angegeben Figurnummer
   ------------------------------------------------- */
void copyfig(char figx, char figy, uint16_t fig, uint8_t nr)
{
  uint8_t x,y,ind;

  ind= 15;
  for (y= figy; y < (figy + 4); y++)
  {
    for (x= figx; x < (figx + 4); x++)
    {
      if (intbit(fig,ind)>0) { tetfeld[y][x]= nr; }
      ind--;
    }
  }
}

void delrow(char nr)
{
  int x,y;

  for (y= nr; y>= 0; y--)
  {
    for (x= 1; x< gamcol+1; x++)
    {
      tetfeld[y][x]= tetfeld[y-1][x];
    }
  }
  for (x= 1; x< gamcol+1; x++)
  {
    tetfeld[0][x]= 0x00;
  }
  scorecx++;
  showscore();
}

char scangamefeld(void)
//   Scant das Spielfeld auf vollstaendig ausgefuellte Reihen und
//   entfernt eine solchige bei gleichzeitiger Erhoehung des
//   Spielstands <scorecx>

{
  signed char x,y;
  char kcnt;
  char retval;

  retval= 0;
  for (y= gamrows-2; y>= 0 ; y--)
  {
    kcnt= 0;
    for (x= 1; x< gamcol+1; x++)
    {
      if (tetfeld[y][x]> 0) { kcnt++; }
    }
    if (kcnt== gamcol)
    {
      delrow(y);
      retval= 1;
    }
  }
  return retval;
}

void checkdownrow(void)
{
  copyfig(figx,figy,tetfigures[aktfig][aktrot],aktfig+1);
  while(scangamefeld());
  drawfeld();

  aktfig= get16zufall(rndinit, 0x0be0) % 7;
  aktrot= 0; figx= 5; figy= 0;
  action= 0;
  if (canposxy(figx,figy,tetfigures[aktfig][aktrot]))
  {
    drawfig(figx,figy,tetfigures[aktfig][aktrot],aktfig+9,1);
  }
  else {gameover= 1;}
}


int main(void)
{

  int     ch;
  uint8_t x,y;
  char    endgame = 0;

  sys_init();

  lcd_init();

  tast1_init();
  tast2_init();
  tast3_init();
  tast4_init();

  tim3_init();

  delay(100);
  lcd_init();
  outmode= 2;

  bkcolor= 0;
  clrscr();
  textcolor= rgbfromega(9);
  #if (use_ega_logo == 1)
    bmp16_show(bmpofs,0,&tetrislogo[0], &logopal[0]);
  #else
    bmpcga_show(bmpofs,0,&tetrislogo[0], &cgapal[0]);
  #endif

  do
  {
    ch= readkey();
    rndinit++;
    rndinit= rndinit % 2048;
  }while(ch== 0);


  while(1)
  {
    gamefeld_init();
    delay(300);

    while(endgame== 0)
    {
      bkcolor= 0;
      clrscr();
      fillrect(bmpofs+32,18,bmpofs+112,118,rgbfromvalue(0x10,0x10,0x10));
      bmp16_show(bmpofs,8,&tetleft[0], &logopal[0]);
      bmp16_show(bmpofs+117,55,&tetright[0], &logopal[0]);

/*
      fillrect(drxofs+1,dryofs+1, drxofs-1+(klotzgr*(gamcol+6)),                        \
               dryofs-1+(klotzgr*(gamrows+5)), rgbfromvalue(0x08, 0x08, 0x00));
*/
      scorecx= 0;
      while(scangamefeld());
      drawfeld();
      showscore();


      aktfig= get16zufall(rndinit, 0x0be0) % 7;
      aktrot= 0; figx= 5; figy= 0;
      action= 0;
      loopcx= outerloop;
      drawfig(figx,figy,tetfigures[aktfig][aktrot],aktfig+9,1);

      gameover= 0;
      while((gameover==0) && (endgame== 0))
      {
        switch (action)
        {
  /*
          case 20:                                                      // Testzweck : Figur auf Raster uebernehmen
            clrscr();
            fflush(stdout);
            gotoxy(1,30);
            usleep(7000);
            copyfig(figx,figy,tetfigures[aktfig][aktrot],aktfig+1);
            drawfeld();

            fflush(stdout);
            gotoxy(1,30);
            usleep(7000);
            aktfig= 0; aktrot= 0;
            clrscr();
            drawfeld();
            figx= 5; figy= 2;
            drawfig(figx,figy,tetfigures[aktfig][aktrot],aktfig+9,1);
            break;
  */
          case 1:                                                       // rotieren
            hb= aktrot;
            aktrot++;
            aktrot %= 4;
            hint= tetfigures[aktfig][aktrot];
            if (aktfig==6) {hint= 0xffff;}                              // Figur 7 (langer Stab) kann nur
                                                                        // gedreht werden, wenn alle Plaetze
                                                                        // unter ihm unbelegt sind !!!
            if (canposxy(figx,figy,hint))
            {
              drawfig(figx,figy,tetfigures[aktfig][hb],aktfig+9,0);
              drawfig(figx,figy,tetfigures[aktfig][aktrot],aktfig+9,1);
            }
            else
            {
              aktrot= hb;                                               // rotieren war nicht moeglich;
            }
            break;

          case 2:                                                       // rechts schieben
            if (canposxy(figx+1,figy,tetfigures[aktfig][aktrot]))
            {
              drawfig(figx,figy,tetfigures[aktfig][aktrot],aktfig+9,0);
              figx++;
              drawfig(figx,figy,tetfigures[aktfig][aktrot],aktfig+9,1);

            }
            break;

          case 3:                                                       // links schieben
            if (canposxy(figx-1,figy,tetfigures[aktfig][aktrot]))
            {
              drawfig(figx,figy,tetfigures[aktfig][aktrot],aktfig+9,0);
              figx--;
              drawfig(figx,figy,tetfigures[aktfig][aktrot],aktfig+9,1);

            }
            break;
          case 4:
            while (canposxy(figx,figy+1,tetfigures[aktfig][aktrot])) {figy++;}
            showscore();
            checkdownrow();
            break;
        }

        if (innerloop> 0)
        {
          for (hb= innerloop; hb> 0; hb--)
          {
            delay(1);
          }
        }

        action= 0;
        while ((action==0) & (loopcx>0))
        {
          loopcx--;
          innerloop= intime;
          while ((action==0) & (innerloop > 0))
          {
            ch= readkey();
            switch (ch)
            {
              case keyup    :   action= 1; break;
              case keyright :   action= 2; break;
              case keyleft  :   action= 3; break;
              case keydown  :   action= 4; break;
              default       :   break;
            }
            if (ch>0) delay(80);
            delay(3);
            innerloop--;
          }
          if ((action<4) & (action>0)) { innerloop= intime; }
        }
        if (loopcx== 0)
        {
          loopcx= outerloop;
          if (canposxy(figx,figy+1,tetfigures[aktfig][aktrot]))
          {
            drawfig(figx,figy,tetfigures[aktfig][aktrot],aktfig+9,0);
            figy++;
            drawfig(figx,figy,tetfigures[aktfig][aktrot],aktfig+9,1);

          }
          else
          {
            showscore();
            checkdownrow();
          }
        }
      }
      if (!endgame)
      {
        for (y= 0; y < gamrows-1; y++)
        {
          for (x= 1; x < gamcol+1; x++)
          {
            drawklotz(x, y, 5, 1);
            delay(10);
          }
        }
        textcolor= rgbfromega(15);
        fillrect(60,56, 108,88, rgbfromega(0));
        outtextxy(70,63,0, "Game");
        outtextxy(70,73,0, "over");
        while( !(readkey()) );
        gamefeld_init();
        endgame= 0;
      }
    }
  }
}

