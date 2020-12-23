/* -------------------------------------------------------
                         breakout_0x.c

                     YetAnotherBreakOutGame

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
  #include "bricksscr5.h"
#else
  #include "bricksscr5_cga.h"
#endif


#define brickset                1             // 0: Steiner werden duch fillrect-Kloetzchen dargestellt
                                        // 1: Bitmap-Muster 1 wird verwendet
                                        // 2: Bitmap-Muster 2 wird verwendet
#include "stones.h"


// ----------------- Spieltasten ---------------

#define boardvers     2

#if (boardvers == 1)
  // TFT-Button-Shield

  #define butl_init()   PB1_input_init()
  #define is_butl()     (!(is_PB1()))
  #define butr_init()   PB4_input_init()
  #define is_butr()     (!(is_PB4()))
  #define buts_init()   PB5_input_init()
  #define is_buts()     (!(is_PB5()))


#elif (boardvers == 2)
  //  Steckbrettaufbau

  //  PA2  Taste links
  //  PB1        rechts
  //  PA6        oben
  //  PA3        unten

  #define butl_init()   PA2_input_init()
  #define is_butl()     (!(is_PA2()))
  #define butr_init()   PB1_input_init()
  #define is_butr()     (!(is_PB1()))
  #define buts_init()   PA6_input_init()
  #define is_buts()     (!(is_PA6()))

#elif (boardvers == 3)
  // erstes F030 Board mit TFT-Anschluss und Tasten

  #define butl_init()   PA13_input_init()
  #define is_butl()     (!(is_PA13()))
  #define butr_init()   PA1_input_init()
  #define is_butr()     (!(is_PA1()))
  #define buts_init()   PA2_input_init()
  #define is_buts()     (!(is_PA2()))
  #define butd_init()   PA0_input_init()
  #define is_butd()     (!(is_PA0()))

#else
  #error "Keine Boardversion fuer Tasten angegeben"
#endif

#define button_init()  { butl_init(); butr_init(); buts_init(); }

#define scrofsx       0
#define scrofsy       0

// --- Groesse und Offsetposition der Steine ---

#define brickofsx      2 + scrofsx
#define brickofsy     14 + scrofsy
#define bricksizex    14
#define bricksizey    10

// --- Groesse und Offsetposition des Paddles ---

#define paddleposy   118
#define paddlesizex   22
#define paddlesizey    2

// ----------------- Steinefeld -----------------

#define brickanzx      9
#define brickanzy      6
#define brickanz     (brickanzx * brickanzy)

// ------ Pixelaufloesung Display X-Achse -------

#define scrsizex     128

// ---- Position und Bewegungsrichtung Ball -----

struct balldefs
{
  uint16_t x, y;                   // aktuelle Position des Balls
  uint8_t xdir, ydir;              // Bewegungsrichtung des Balls: 0 dekrementieren
                                   //                              1 inkrementieren
  uint8_t angle;                   // Bewegungswinkel, 0    = senkrecht nach oben
                                   //                  3..5 = je groesser die Zahl desto "schraeger"
  uint8_t xsteps;                  // Zaehler fuer bereits gemachte Bewegungen
                                   // ergibt in Kombination mit "angle" den Bewegungswinkel
  uint16_t col;                    // Farbe
};

struct balldefs ball;

#define startlifes   4                    // Anzahl der "Leben" beim Spielbeginn

uint16_t score, level, lifes;

// ------------- Bitimage des Balls ------------
const uint8_t ball_img[] =
{
  1,0, 2,0, 1,3, 2,3,
  0,1, 1,1, 2,1, 3,1,
  0,2, 1,2, 2,2, 2,3
};

#define regulargame      1                          // zum Testen: 1 => normales Spiel
                                                    //             0 => vereinfachte Level
#define levelanz         6

// ----------- Bildmuster der Steine -----------
//   Byte0: Aussehen des Steins
//   Byte1: benoetigte Anzahl der Treffer

#if ( regulargame == 1)
  const uint8_t level01[levelanz][108] =
  {
    {
      2,1, 2,1, 2,1,  3,1, 3,1, 3,1,  1,1, 1,1, 1,1,
      1,1, 1,1, 1,1,  4,2, 4,2, 4,2,  3,1, 3,1, 3,1,
      1,1, 1,1, 1,1,  4,2, 0,0, 4,2,  3,1, 3,1, 3,1,
      1,1, 1,1, 1,1,  4,2, 4,2, 4,2,  3,1, 3,1, 3,1,
      2,1, 2,1, 2,1,  3,1, 3,1, 3,1,  1,1, 1,1, 1,1,
      2,1, 2,1, 2,1,  3,1, 3,1, 3,1,  1,1, 1,1, 1,1
    },
    {
      2,1, 2,1, 2,1,  2,1, 2,1, 2,1,  2,1, 2,1, 2,1,
      0,0, 3,1, 0,0,  0,0, 4,2, 0,0,  0,0, 3,1, 0,0,
      0,0, 3,1, 0,0,  4,2, 1,1, 4,2,  0,0, 3,1, 0,0,
      0,0, 3,1, 0,0,  4,2, 1,1, 4,2,  0,0, 3,1, 0,0,
      0,0, 3,1, 0,0,  0,0, 4,2, 0,0,  0,0, 3,1, 0,0,
      2,1, 2,1, 2,1,  2,1, 2,1, 2,1,  2,1, 2,1, 2,1
    },
    {
      1,1, 0,0, 3,1,  0,0, 2,1, 0,0,  1,1, 0,0, 3,1,
      0,0, 4,2, 0,0,  4,4, 0,0, 4,4,  0,0, 4,2, 0,0,
      3,1, 0,0, 2,1,  0,0, 3,1, 0,0,  2,1, 0,0, 1,1,
      0,0, 4,2, 0,0,  4,4, 0,0, 4,4,  0,0, 4,2, 0,0,
      2,1, 0,0, 1,1,  0,0, 1,1, 0,0,  3,1, 0,0, 2,1,
      0,0, 4,2, 0,0,  4,2, 0,0, 4,2,  0,0, 4,2, 0,0
    },
    {
      0,0, 1,1, 0,0,  4,2, 4,2, 4,2,  0,0, 3,1, 0,0,
      1,1, 0,0, 4,2,  0,0, 0,0, 0,0,  4,2, 0,0, 3,1,
      1,1, 0,0, 4,2,  0,0, 2,2, 0,0,  4,2, 0,0, 3,1,
      1,1, 0,0, 4,2,  0,0, 0,0, 0,0,  4,2, 0,0, 3,1,
      1,1, 0,0, 0,0,  4,2, 4,2, 4,2,  0,0, 0,0, 3,1,
      0,0, 1,1, 0,0,  0,0, 0,0, 0,0,  0,0, 3,1, 0,0
    },
    {
      1,1, 1,1, 1,1,  1,1, 1,1, 1,1,  1,1, 1,1, 1,1,
      2,1, 3,1, 2,1,  0,0, 4,2, 0,0,  3,1, 2,1, 3,1,
      3,1, 2,1, 3,1,  4,2, 4,2, 4,2,  2,1, 3,1, 2,1,
      2,1, 3,1, 2,1,  0,0, 4,2, 0,0,  3,1, 2,1, 3,1,
      0,0, 3,1, 0,0,  3,1, 0,0, 3,1,  0,0, 3,1, 0,0,
      1,1, 4,3, 1,1,  4,3, 1,1, 4,3,  1,1, 4,3, 1,1
    },
    {
      0,0, 4,2, 0,0,  0,0, 4,2, 0,0,  0,0, 4,2, 0,0,
      4,2, 2,1, 4,2,  4,2, 1,1, 4,2,  4,2, 3,1, 4,2,
      4,2, 0,0, 4,2,  4,2, 0,0, 4,2,  4,2, 0,0, 4,2,
      4,2, 0,0, 4,2,  4,2, 0,0, 4,2,  4,2, 0,0, 4,2,
      4,2, 2,1, 4,2,  4,2, 1,1, 4,2,  4,2, 3,1, 4,2,
      0,0, 4,2, 0,0,  0,0, 4,2, 0,0,  0,0, 4,2, 0,0
    }
  };
#else
  const uint8_t level01[levelanz][108] =
  {
    {
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 2,1, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0
    },
    {
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  2,1, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 2,1, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0
    },
    {
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 3,1,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  1,1, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 2,1, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0
    },
    {
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 3,1,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  1,1, 0,0, 1,1,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 2,1, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0
    },
    {
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 3,1,  0,0, 0,0, 0,0,  3,1, 0,0, 0,0,
      0,0, 0,0, 0,0,  1,1, 0,0, 1,1,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 2,1, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0
    },
    {
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 1,1, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 3,1,  0,0, 0,0, 0,0,  3,1, 0,0, 0,0,
      0,0, 0,0, 0,0,  1,1, 0,0, 1,1,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 2,1, 0,0,  0,0, 0,0, 0,0,
      0,0, 0,0, 0,0,  0,0, 0,0, 0,0,  0,0, 0,0, 0,0
    }
  };
#endif

uint8_t brickwall[108];              // Array der Mauer

/* --------------------------------------------------------
                        but_waitstart

     wartet auf das Druecken der Start-Taste
   -------------------------------------------------------- */
void but_waitstart(void)
{
  while (!(is_buts()));
  delay(50);
  while (is_buts());
  delay(50);
}

/* --------------------------------------------------------
                      bmpcga2_show

     zeigt ein in einem Array abgelegtes Bitmap an. Die
     verwendete Farbpalette muss im RGB565 - Format
     codiert sein.

     Farbe 0 ist transparent und wird NICHT gezeichnet

        *image     : Zeiger auf das Bytearray, dass die
                     Vierfarbgrafik enthaellt
        ox,oy      : linke obere Ecke, ab der die
                     Grafik angezeigt werden soll
        *pal       : Zeiger auf die zur Grafik ge-
                     hoerende Farbpalette

   -------------------------------------------------------- */
void bmpcga2_show(int ox, int oy, const uint8_t *const image, const uint16_t *const pal)
{
  int16_t   x, y;
  uint16_t  width, height;
  uint16_t  ptr;
  uint8_t   pixpos, fb, cvalue;
  uint16_t  cgacolor;


  width= (image[0] << 8) + image[1];
  height= (image[2] << 8) + image[3];

  ptr= 4;
  y= height;

  for (y= 0; y < height; y++)
  {
    for (x= 0; x < width; x++)
    {
      if ((x % 4)== 0)
      {
        fb= image[ptr];
        ptr++;
      }
      pixpos=  (3-(x % 4))*2;

      cvalue= (fb >> pixpos) & 0x03;
      switch (cvalue)
      {

        case 00 : cgacolor= pal[0];  break;
        case 01 : cgacolor= pal[1];  break;
        case 02 : cgacolor= pal[2];  break;
        case 03 : cgacolor= pal[3];  break;
      }
      if (cvalue) putpixel(x+ox, y+oy-1, cgacolor);
    }
  }
}

/* --------------------------------------------------------
                          level2wall

        kopiert einen Level aus dem Flash-Rom ins Ram
   -------------------------------------------------------- */
void level2wall(uint8_t *wall, const uint8_t *const level)
{
  uint8_t i;

  for (i= 0; i< 108; i++)
  {
    *wall= level[i];
    wall++;
  }
}

/* --------------------------------------------------------
                          brick_draw

     zeichnet einen einzelnen Stein mit den gegebenen
     Mauerkoordinaten (Grafikkoordinaten werden hieraus
     errechnet).

     Uebergabe:
       ind  : Index der Mauer, Stein links oben hat
              Index 0, rechts unten hat Index 53
       nr   : Farbe / Nummer des zu zeichnenden Steins
   -------------------------------------------------------- */
void brick_draw(uint8_t ind, uint16_t nr)
{
  uint16_t x1,y1, x2,y2;
  uint16_t x,y;

  x= ind % brickanzx;
  y= ind / brickanzx;

  x1= brickofsx+(x * bricksizex);
  y1= brickofsy+(y * bricksizey);

  x2= x1+bricksizex-4;
  y2= y1+bricksizey-4;

  #if (brickset == 0)

    y2 -= 2;

    switch (nr)
    {
      case 0 : fillrect(x1,y1, x2, y2, rgbfromega(0)); break;
      case 1 : fillrect(x1,y1, x2, y2, rgbfromega(1)); break;
      case 2 : fillrect(x1,y1, x2, y2, rgbfromega(2)); break;
      case 3 : fillrect(x1,y1, x2, y2, rgbfromega(14)); break;
      case 4 : fillrect(x1,y1, x2, y2, rgbfromega(4)); break;

      default : break;
    }

  #else

    if (nr)
    {
      switch (nr)
      {
        case 1 : bmpcga2_show(x1,y1, &brickcga_img[0], &brick01_pal[0]); break;
        case 2 : bmpcga2_show(x1,y1, &brickcga_img[0], &brick02_pal[0]); break;
        case 3 : bmpcga2_show(x1,y1, &brickcga_img[0], &brick03_pal[0]); break;
        case 4 : bmpcga2_show(x1,y1, &brickcga_img[0], &brick04_pal[0]); break;
        default : break;
      }
    }
    else
    {
      x2++; y1--;
      fillrect(x1,y1, x2, y2, 0);
    }
  #endif
}

/* --------------------------------------------------------
                           brick_walldraw

                         zeichnet die Mauer

     Uebergabe:
       wall  : Zeiger auf Array, dass die Steineanordnung
               beinhaltet.
   -------------------------------------------------------- */
void brick_walldraw(uint8_t *wall)
{
  uint8_t i;

  for (i= 0; i< brickanz; i++)
  {
    brick_draw(i, wall[i * 2]);
  }
}

/* --------------------------------------------------------
                           brick_getxy

     ermittelt aus einem Index die dazugehoerenden
     Koordinaten eines Steins

     Uebergabe:
       ind         : Index eines Steins im Array
       x1,y1,x2,y2 : Zeiger, die die Koordinaten des
                     Steins aufnehmen
   -------------------------------------------------------- */
void brick_getxy(uint8_t ind, uint8_t *x1, uint8_t *y1, uint8_t *x2, uint8_t *y2)
{
  uint8_t x,y;

  x= ind % brickanzx;           // x-Position eines Steins im Array
  y= ind / brickanzx;           // dto. y-Position

  *x1= brickofsx+(x * bricksizex);
  *x2= (*x1)+bricksizex-4;

  *y1= brickofsy+(y * bricksizey);
  *y2= (*y1)+bricksizey-4;
}

/* --------------------------------------------------------
                          paddle_draw

                zeichnet den Spieleschlaeger

     Uebergabe:
       show :      1 => Schlaeger wird komplett gezeichnet
                   0 => vorderste und hinterste Pixel-
                        reihe werden geloescht (damit ein
                        nachfolgender Zeichenvorgang
                        keinen "Nachzieheffekt" hat
   -------------------------------------------------------- */
void paddle_draw(uint16_t x, uint8_t show)
{
  int b;
  if (show)
  {
    for (b= x+1; b< x+paddlesizex+1; b++)
      putpixel(b, paddleposy+scrofsy, rgbfromvalue(0x80, 0x40, 0x10));
    for (b= x; b< x+paddlesizex+2; b++)
      putpixel(b, paddleposy+1+scrofsy, rgbfromvalue(0xc0, 0x80, 0x04));
    for (b= x+1; b< x+paddlesizex+1; b++)
      putpixel(b, paddleposy+2+scrofsy, rgbfromvalue(0x80, 0x40, 0x10));
  }
  else
  {
    putpixel(x + paddlesizex+1, paddleposy+scrofsy, 0);
    putpixel(x + paddlesizex+1, paddleposy+1+scrofsy, 0);
    putpixel(x + paddlesizex+1, paddleposy+2+scrofsy, 0);

    putpixel(x, paddleposy+scrofsy, 0);
    putpixel(x, paddleposy+1+scrofsy, 0);
    putpixel(x, paddleposy+2+scrofsy, 0);
  }
}

/* --------------------------------------------------------
                          ball_draw

     zeichnet den Ball an der in der Struktur enthaltenen
     Position.

     Uebergabe:
       *ball       : Zeiger auf Strukturvariable die die
                     aktuelle Bewegung und Position des
                     Spielballs enthaellt.
       draw        : 1 => Ball wird gezeichnet
                     0 => Ball wird geloescht
   -------------------------------------------------------- */
void ball_draw(struct balldefs *ball, uint8_t draw)
{
  uint16_t x,y, col;
  uint8_t i;

  x= ball-> x;
  y= ball-> y;

  if (!draw) col= 0; else col= ball->col;

  for (i= 0; i< sizeof(ball_img); i+= 2)
    putpixel(x+ball_img[i], y+ball_img[i+1], col);
}

/* --------------------------------------------------------
                          ball_move

     Bewegt den Spielball mit den in der Ballstruktur
     enthaltenen Daten.

     Uebergabe:
       *ball       : Zeiger auf Strukturvariable die die
                     aktuelle Bewegung und Position des
                     Spielballs enthaellt.
   -------------------------------------------------------- */
void ball_move(struct balldefs *ball)
{
  uint8_t  angle;
  uint16_t x,y;

  x= ball->x;
  y= ball->y;

  if (ball->angle)                             // wenn nicht seknkrecht nach oben
  {
    angle= 6-ball->angle;
    if (angle > 1)
    {
      ball->xsteps++;

        ball->xsteps %= (angle);               // Wenn Winkelschritte ueberlaufen

        // x je nach Bewegungsrichtung
        if (ball->xdir)
        {
          if (!ball->xsteps) x+= 2;            // inkrementieren
        }
        else
        {
          if (!ball->xsteps) x-= 2;            // dekrementieren
        }

    }
    else
    {
      // jeden Tick x aendern
      if (ball->xdir)
        x+=2;                                  // inkrementieren
      else
        x-=2;                                  // dekrementieren
    }
  }

  // y je nach Bewegungsrichtung
  if (ball->ydir)
    y++;                                       // inkrementieren
  else
    y--;                                       // dekrementieren

  ball_draw(ball,0);
  ball->x = x;
  ball->y = y;
  ball_draw(ball,1);
}

/* --------------------------------------------------------
                         ball_hit_bot
                      (Ball hit bottom)

     Ermittelt, ob die aktuelle Ballposition einen Stein
     von unten getroffen hat.

     Uebergabe:
       *ball       : Zeiger auf Strukturvariable die die
                     aktuelle Bewegung und Position des
                     Spielballs enthaellt.

       *wall       : Array der noch auf dem Screen vor
                     handenen Steine

     Rueckgabe:
                -1 : Ball ist ausserhalb des Mauerfelds
                -2 : Ball bewegt sich nach unten
   -------------------------------------------------------- */
int8_t ball_hit_bot(struct balldefs *ball, uint8_t *wall)
{
  uint8_t x1,y1, x2,y2;
  uint8_t ind, hit;

  if (ball->ydir > 0) return -2;
  brick_getxy(brickanz-1, &x1, &y1, &x2, &y2);        // y2 enthaellt die unterste y-Position
                                                      // die ein Stein haben kann

  if (ball->y > y2) return -1;                        // Ball befindet sich unterhalb der Mauer


  // ab hier waere ein Treffer moeglich
  wall++;
  ind= 0; hit= 0;
  do
  {
    if ((*wall))                                      // wenn ein Stein an der Index-Position vorhanden ist
    {
      brick_getxy(ind, &x1, &y1, &x2, &y2);
      if (ball->y == y2)                              // in dieser Reihe waere ein Treffer moeglich
      {
        if ( !(((ball->x + 4) < x1) || (ball->x > x2)) )
        {
          hit= ind+1;                                 // TREFFER
        }
      }

    }
    wall+= 2;                                         // Zeiger auf den naechsten Stein setzen
                                                      // (ein Stein hat 2 Eintraege)
    ind++;
  } while ((ind < brickanz) && (!hit));
  return hit;
}

/* --------------------------------------------------------
                         ball_hit_top
                        (Ball hit top)

     Ermittelt, ob die aktuelle Ballposition einen Stein
     von oben getroffen hat.

     Uebergabe:
       *ball       : Zeiger auf Strukturvariable die die
                     aktuelle Bewegung und Position des
                     Spielballs enthaellt.

       *wall       : Array der noch auf dem Screen vor
                     handenen Steine

     Rueckgabe:
                -1 : Ball ist ausserhalb des Mauerfelds
                -2 : Ball bewegt sich nach unten
   -------------------------------------------------------- */
int8_t ball_hit_top(struct balldefs *ball, uint8_t *wall)
{
  uint8_t x1,y1, x2,y2;
  uint8_t ind, hit;

  if (!(ball->ydir)) return -2;
  brick_getxy(brickanz-1, &x1, &y1, &x2, &y2);        // y2 enthaellt die unterste y-Position
                                                      // die ein Stein haben kann

  if (ball->y > y2) return -1;                        // Ball befindet sich unterhalb der Mauer


  // ab hier waere ein Treffer moeglich
  wall++;
  ind= 0; hit= 0;
  do
  {
    if ((*wall))                                      // wenn ein Stein an der Index-Position vorhanden ist
    {
      brick_getxy(ind, &x1, &y1, &x2, &y2);
      if (ball->y + 4 == y1)                          // in dieser Reihe waere ein Treffer moeglich
      {
        if ( !(((ball->x + 4) < x1) || (ball->x > x2)) )
        {
          hit= ind+1;                                 // TREFFER
        }
      }

    }
    wall+= 2;                                         // Zeiger auf den naechsten Stein setzen
                                                      // (ein Stein hat 2 Eintraege)
    ind++;
  } while ((ind < brickanz) && (!hit));
  return hit;
}

/* --------------------------------------------------------
                         ball_hit_left

     Ermittelt, ob die aktuelle Ballposition einen Stein
     von der linken Seite getroffen hat

     Uebergabe:
       *ball       : Zeiger auf Strukturvariable die die
                     aktuelle Bewegung und Position des
                     Spielballs enthaellt.

       *wall       : Array der noch auf dem Screen vor
                     handenen Steine

     Rueckgabe:
                -1 : Ball ist ausserhalb des Mauerfelds
                -2 : Ball bewegt sich nach links
   -------------------------------------------------------- */
int8_t ball_hit_left(struct balldefs *ball, uint8_t *wall)
{
  uint8_t x1,y1, x2,y2;
  uint8_t ind, hit;

  if (!(ball->xdir)) return -2;
  brick_getxy(brickanz-1, &x1, &y1, &x2, &y2);        // y2 enthaellt die unterste y-Position
                                                      // die ein Stein haben kann

  if (ball->y > y2) return -1;                        // Ball befindet sich unterhalb der Mauer

  // ab hier waere ein Treffer moeglich
  wall++;
  ind= 0; hit= 0;
  do
  {
    if ((*wall))                                      // wenn ein Stein an der Index-Position vorhanden ist
    {
      brick_getxy(ind, &x1, &y1, &x2, &y2);

      if ( ((ball->x + 4) >= x1) && ((ball->x) + 4 <= x2) )
      {
        if ( !(((ball->y + 4) < y1) || (ball->y > y2)) )
        {
          hit= ind+1;                                 // TREFFER
        }
      }
    }
    wall+= 2;                                         // Zeiger auf den naechsten Stein setzen
                                                      // (ein Stein hat 2 Eintraege)
    ind++;
  } while ((ind < brickanz) && (!hit));
  return hit;
}

/* --------------------------------------------------------
                         ball_hit_right

     Ermittelt, ob die aktuelle Ballposition einen Stein
     von der rechten Seite getroffen hat

     Uebergabe:
       *ball       : Zeiger auf Strukturvariable die die
                     aktuelle Bewegung und Position des
                     Spielballs enthaellt.

       *wall       : Array der noch auf dem Screen vor
                     handenen Steine

     Rueckgabe:
                -1 : Ball ist ausserhalb des Mauerfelds
                -2 : Ball bewegt sich nach rechts
   -------------------------------------------------------- */
int8_t ball_hit_right(struct balldefs *ball, uint8_t *wall)
{
  uint8_t x1,y1, x2,y2;
  uint8_t ind, hit;

  if (ball->xdir) return -2;
  brick_getxy(brickanz-1, &x1, &y1, &x2, &y2);        // y2 enthaellt die unterste y-Position
                                                      // die ein Stein haben kann

  if (ball->y > y2) return -1;                        // Ball befindet sich unterhalb der Mauer

  // ab hier waere ein Treffer moeglich
  wall++;
  ind= 0; hit= 0;
  do
  {
    if ((*wall))                                      // wenn ein Stein an der Index-Position vorhanden ist
    {
      brick_getxy(ind, &x1, &y1, &x2, &y2);

      if ( (ball->x <= x2) && (ball->x > x1) )
      {
        if (!(((ball->y + 4) < y1) || (ball->y > y2)))
        {
          hit= ind+1;                                 // TREFFER
        }
      }
    }
    wall+= 2;                                         // Zeiger auf den naechsten Stein setzen
                                                      // (ein Stein hat 2 Eintraege)
    ind++;
  } while ((ind < brickanz) && (!hit));
  return hit;
}

/* --------------------------------------------------------
                         get16zufall

      generiert eine 16-Bit grosse Pseudozufallszahl die
      mittels mitgekoppeltem Schieberegister erreicht wird

    Uebergabe:
      startwert  : Ausgangspunkt im Zahlenbereich
      xormask    : Maske beim Schieben, hierdurch kann
                   ein Zahlenbereich eingeschraenkt werden

          xormask   Anzahl unterschiedlicher Zahlen
          -----------------------------------------
          0xb400                65535
          0x07c1                 2047
          0x0be0                 4095
  -------------------------------------------------------- */
uint16_t get16zufall(uint16_t startwert, uint16_t xormask)
{
  // Variable MUESSEN static sein, damit das Schieberegister
  // mit jedem Tick von der vorherigen Position aus weiter
  // schiebt
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
                         hit_check

     testet, ob der Spielball einen Mauerstein getroffen
     hat, aktualisiert ggf. dessen noch zu erreichende
     Trefferanzahl. Stein, der nicht mehr getroffen werden
     muss wird vom Display geloescht.

     Bei getroffenem Stein wird Score aktualisiert und
     angezeigt.

     Rueckgabe:
           1 : wenn Level geloest ist
           0 : wenn weitere Spielsteine vorhanden sind
   -------------------------------------------------------- */
uint8_t hit_check(struct balldefs *ball, uint16_t *score)
{
  int8_t   hit;
  uint8_t  scrflag;
  uint16_t oldscore;
  char     zstr[7];    // nimmt Scorestring auf
  uint8_t  i;

  scrflag= 0;
  oldscore= *score;
  hit= (ball_hit_bot(ball, &brickwall[0]));              // Treffer von unten ?
  if (hit> 0)
  {
    *score += brickwall[((hit-1)*2)];
    ball->ydir= 1;
    ball_move(ball);

    brickwall[((hit-1)*2)+1]--;                          // Anzahl noetiger Treffer veringern
    if (brickwall[((hit-1)*2)+1]> 0)
      brick_draw(hit-1, brickwall[((hit-1)*2)]);
    else
      brick_draw(hit-1, 0);
    scrflag= 1;
  }

  hit= (ball_hit_top(ball, &brickwall[0]));     // Treffer von oben ?
  if (hit> 0)
  {
    *score += brickwall[((hit-1)*2)];
    ball->ydir= 0;
    ball_move(ball);

    brickwall[((hit-1)*2)+1]--;
    if (brickwall[((hit-1)*2)+1]> 0)
      brick_draw(hit-1, brickwall[((hit-1)*2)]);
    else
      brick_draw(hit-1, 0);
    scrflag= 1;
  }

  hit= (ball_hit_left(ball, &brickwall[0]));     // Treffer an der linken Seite ?
  if (hit> 0)
  {
    *score += brickwall[((hit-1)*2)];
    ball->xdir= 0;
    ball_move(ball);

    brickwall[((hit-1)*2)+1]--;
    if (brickwall[((hit-1)*2)+1]> 0)
      brick_draw(hit-1, brickwall[((hit-1)*2)]);
    else
      brick_draw(hit-1, 0);
    scrflag= 1;
  }

  hit= (ball_hit_right(ball, &brickwall[0]));    // Treffer an der rechten Seite ?
  if (hit> 0)
  {
    *score += brickwall[((hit-1)*2)];
    ball->xdir= 1;
    ball_move(ball);

    brickwall[((hit-1)*2)+1]--;
    if (brickwall[((hit-1)*2)+1]> 0)
      brick_draw(hit-1, brickwall[((hit-1)*2)]);
    else
      brick_draw(hit-1, 0);
    scrflag= 1;
  }

  if (scrflag)
  {
    textcolor= 0;
    itoa(oldscore, zstr, 10);
    outtextxy(35+scrofsx,3+scrofsy, 0, zstr);
    textcolor= rgbfromvalue(0xff, 0xbb, 0x80);
    itoa(*score, zstr, 10);
    outtextxy(35+scrofsx,3+scrofsy, 0, zstr);
  }
  i= 0;
  while (i< 108)
  {
    if (brickwall[i+1]) return 0;
    i+= 2;
  }
  return 1;
}

/* --------------------------------------------------------
                         lifes_show

     zeigt die noch verfuegbaren "Leben" an
   -------------------------------------------------------- */
void lifes_show(uint8_t lifes)
{
  uint8_t i;

  fillrect(105+scrofsx,3+scrofsy, 126+scrofsx,9+scrofsy, 0);
  if (lifes> 1)
  {
    for (i= 0; i< lifes-1; i++)
    {
      fillrect(105+scrofsx+(i*5),3+scrofsy, 107+(i*5)+scrofsx,9+scrofsy, rgbfromvalue(0xff, 0x78, 0x20));
    }
  }
}

/* --------------------------------------------------------
                          game_start

     setzt die Anfangswerte des Balls und des Schlaegers
     beim Spielstart und wartet auf die Starttaste
   -------------------------------------------------------- */
void game_start(struct balldefs *ball, uint16_t *paddlex)
{
  ball->x= 50; ball->y= paddleposy-5;    // Ausgangsposition
  ball->xdir= 1; ball->ydir= 0;          // zu Beginn nach rechts oben bewegen
  ball->angle= 3;
  ball->xsteps= 0;
  ball->col= rgbfromega(15);

//  ball->x=55; ball->y= 41; ball->ydir= 1; ball->xdir= 1;

  *paddlex= 40;
  paddle_draw((*paddlex),1);
  ball_move(ball);
  but_waitstart();
}

/* --------------------------------------------------------
                         gameover_show

                     zeigt GameOver an
   -------------------------------------------------------- */
void gameover_show(void)
{
  fillrect(17+scrofsx,35+scrofsy, 111+scrofsx,65+scrofsy, 0);
  fillrect(20+scrofsx,38+scrofsy, 108+scrofsx,62+scrofsy, rgbfromvalue(0x80, 0x00, 0x40));
  line(20+scrofsx,38+scrofsy, 108+scrofsx,38+scrofsy, rgbfromega(7));
  line(20+scrofsx,39+scrofsy, 108+scrofsx,39+scrofsy, rgbfromega(7));
  line(20+scrofsx,38+scrofsy, 20+scrofsx,62+scrofsy, rgbfromega(7));
  line(21+scrofsx,38+scrofsy, 21+scrofsx,62+scrofsy, rgbfromega(7));

  line(20+scrofsx,61+scrofsy, 108+scrofsx,61+scrofsy, rgbfromega(8));
  line(20+scrofsx,62+scrofsy, 108+scrofsx,62+scrofsy, rgbfromega(8));
  line(108+scrofsx,38+scrofsy, 108+scrofsx,62+scrofsy, rgbfromega(8));
  line(107+scrofsx,38+scrofsy, 107+scrofsx,62+scrofsy, rgbfromega(8));

  textcolor= rgbfromvalue(0x30,0x30,0x30);
  outtextxy(26+scrofsx,46+scrofsy, 0, "GaMe OvEr");
  textcolor= rgbfromvalue(0xff,0x7a,0x28);
  outtextxy(28+scrofsx,48+scrofsy, 0, "GaMe OvEr");

  // Meldung fuer Neustart blinken lassen
  do
  {
    textcolor= rgbfromega(1);
    outtextxy(18+scrofsx,80+scrofsy, 0, "Press button");
    outtextxy(18+scrofsx,90+scrofsy, 0, "  to start");
    outtextxy(18+scrofsx,100+scrofsy, 0, "  new game");
    delay(400);
    textcolor= rgbfromega(2);
    outtextxy(18+scrofsx,80+scrofsy, 0, "Press button");
    outtextxy(18+scrofsx,90+scrofsy, 0, "  to start");
    outtextxy(18+scrofsx,100+scrofsy, 0, "  new game");
    delay(400);
  } while(!(is_buts()));
  while(is_buts());
  delay(500);
  // komplettes Spielfeld loeschen
  fillrect(1+scrofsx, 1+scrofsy, 126+scrofsx, 126+scrofsy, 0);
}

/* --------------------------------------------------------
                       level_showlogo

                 zeigt das naechste Level an
   -------------------------------------------------------- */
void level_showlogo(uint8_t nr)
{
  char zstr[7];
  uint8_t l;
  uint8_t x;

  nr++;
  itoa(nr, zstr, 10);
  l= strlen(zstr);
  x= (l * 16) / 2;

  fillrect(1+scrofsx,brickofsy+scrofsy, 126+scrofsx, 126+scrofsy, 0);
  fillellipse(64+scrofsx,64+scrofsy, 50,20, rgbfromvalue(0xff, 0x7a, 0x2b));
  ellipse(64+scrofsx,64+scrofsy, 50,20, rgbfromega(8));
  ellipse(64+scrofsx,64+scrofsy, 48,18, rgbfromega(15));
  textsize= 1;
  textcolor= rgbfromvalue(0xff, 0x7a, 0x2b);
  outtextxy(29+scrofsx,19+scrofsy, 0, "Level");
  textcolor= rgbfromvalue(0x10, 0x10, 0x02);
  outtextxy(64-x+scrofsx,58+scrofsy, 0, zstr);
  textsize= 0;
  delay(2000);
  fillrect(1+scrofsx,brickofsy+scrofsy, 126+scrofsx, 126+scrofsy, 0);
}

/* ----------------------------------------------------------------------------------
                                       MAIN
   ---------------------------------------------------------------------------------- */
int main(void)
{
//  int x,y;
//  uint8_t  x1,y1,x2,y2;
  uint8_t  lostball;
  uint8_t  clearlevel;
//  uint8_t  b;
  uint8_t  ticks;
  uint16_t paddlex;

  sys_init();

  // Display initialisieren und Grundeinstellungen vornehmen
  lcd_init();
  clrscr();
  clrscr();
  textsize= 0;
  setfont(0);
  lcd_orientation(2);
  lcd_enable();
  bkcolor= rgbfromega(0);
  textcolor= rgbfromega(7);

  // Tasten initialisieren
  button_init()

  // Startbildschirm anzeigen
  clrscr();
  #if (use_ega_logo == 1)
    bmp16_show(scrofsx,scrofsy, &logoimg[0], &logopal[0]);
  #else
    bmpcga_show(scrofsx,scrofsy, &logoimg[0], &logopal[0]);
  #endif

  but_waitstart();                      // Anfangslogo mit Taste quittieren

  clrscr();
  score= 0; level= 0; lifes= startlifes;

  rectangle(scrofsx,scrofsy,scrofsx + 127,127+scrofsy, rgbfromega(7));

  textcolor= rgbfromvalue(0xff, 0x00, 0xff);
  outtextxy(5 + scrofsx,3+scrofsy, 0, "Sc:");
  textcolor= rgbfromvalue(0xff, 0xbb, 0x80);
  outtextxy(35 + scrofsx,3+scrofsy, 0, "0");

  lifes_show(lifes);
  level_showlogo(level);

  level2wall(&brickwall[0], &level01[level % levelanz][0]);
  brick_walldraw(&brickwall[0]);              // die Mauer aufbauen

  game_start(&ball, &paddlex);

  lostball= 0;

  while(1)
  {
    ball_move(&ball);                         // Ball bewegen
    clearlevel= hit_check(&ball, &score);     // und testen ob getroffen wurde und wenn ja
                                              // Stein ggf. entfernen und Score aktualisieren

    if (ball.x > scrofsx+scrsizex-8) ball.xdir= 0;
    if (ball.x < scrofsx+4) ball.xdir= 1;
    if (ball.y < 14+scrofsy) ball.ydir= 1;

    if (ball.y > (paddleposy-5+scrofsy))              // wenn Ball in Schlaegerhoehe ist
    {                                                 // ueberpruefen ob Schlaeger Ball trifft
      if ((paddlex-4 <= (int)ball.x) && ((paddlex+paddlesizex)>= ball.x))
      {
        ball.ydir= 0;                         // Ball wurde getroffen
        if (is_butr())                        // Ball wird nach rechts angeschnitten
        {
          if (ball.xdir)                      // Winkel wird spitzer
          {
            if (ball.angle< 5) ball.angle++;
          }
          else
          {
            if (ball.angle > 1) ball.angle--; // weniger spitz
          }
        }
        if (is_butl())                        // Ball wird nach links angeschnitten
        {
          if (!(ball.xdir))                   // Winkel wird spitzer
          {
            if (ball.angle< 4) ball.angle++;
          }
          else
          {
            if (ball.angle > 2) ball.angle--;  // weniger spitz
          }
        }
      }
      else
      {
        lostball= 1;
      }
    }

    // Schlaegerbewegung mit Zeitausgleich bei Nichtbewegen
    for (ticks = 0; ticks< 2; ticks++)
    {
      if (is_butr() || is_butl() )
      {
        if (is_butl())
        {
          if (paddlex > brickofsx)
          {
            paddle_draw(paddlex,0);
            paddlex--;
            paddle_draw(paddlex,1);
          }
          else
          {
            delay(2);
          }
        }
        if (is_butr())
        {
          if ((paddlex + paddlesizex) < scrofsx+scrsizex-3)
          {

            paddle_draw(paddlex,0);
            paddlex++;
            paddle_draw(paddlex,1);
          }
          else
          {
            delay(2);
          }
        }
        delay(6);
      }
      else
      {
        delay(7);
      }
    }

    if (clearlevel)
    {
      lostball= 0;
      level++;
      level_showlogo(level);
      level2wall(&brickwall[0], &level01[level % levelanz][0]);
      brick_walldraw(&brickwall[0]);              // die Mauer aufbauen
      if ((level % levelanz)== 0)
      {
        // wenn nicht bereits die maximale Anzahl von Leben vorhanden ist
        // ein Leben als Bonus fuer jeweils 6 gespielte Levels geben
        if (lifes < startlifes) lifes++;
      }
      lifes_show(lifes);
      game_start(&ball, &paddlex);
    }

    if (lostball)
    {
      delay(1000);
      ball_draw(&ball, 0);
      fillrect(1+scrofsx, paddleposy+scrofsy, 126+scrofsx, paddleposy+4+scrofsy, 0);    // komplette Schlaegerlinie loeschen
      lifes--;
      if (!(lifes))
      {
        gameover_show();
        // neues Spiel aufbauen
        score= 0; level= 0; lifes= startlifes;
        level_showlogo(level);

        level2wall(&brickwall[0], &level01[0][0]);      // erstes Level setzen
        brick_walldraw(&brickwall[0]);                  // die Mauer aufbauen
        textcolor= rgbfromvalue(0xff, 0x00, 0xff);
        outtextxy(5+scrofsx,3+scrofsy, 0, "Sc:");
        textcolor= rgbfromvalue(0xff, 0xbb, 0x80);
        outtextxy(35+scrofsx,3+scrofsy, 0, "0");
      }
      lifes_show(lifes);
      lostball= 0;
      game_start(&ball, &paddlex);
    }
  }
}
