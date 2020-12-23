/* -----------------------------------------------------------------------------------
                            tftdisplay.h

     Header Softwaremodul fuer farbige TFT-Displays

     unterstuetzte Displaycontroller:
     SPI
     -----------------------------------
           ili9163
           ili9340
           st7735r
           s6d02a1
           ili9225

     8-Bit parallel
     -----------------------------------
           ili9341
           ili9481
           ili9486

     MCU   :  STM32F030
     Takt  :  interner Takt 48 MHz

     27.05.2019  R. Seelig
   ----------------------------------------------------------------------------------- */

#ifndef in_tftdisplay_module
  #define in_tftdisplay_module

  #include <stdint.h>
  #include <stdlib.h>
  #include <libopencm3.h>

  #include "sysf030_init.h"


  /* ------------------------------------------------------------------
                            Displayauswahl

       es kann (und muss) nur ein einziges Display ausgewaehlt sein.
     ------------------------------------------------------------------ */

  // ----------------------- SPI- Displays ----------------------------

  #define  ili9163                  0
  #define  ili9340                  0
  #define  st7735r                  0
  #define  s6d02a1                  1
  #define  ili9225                  0
  #define  st7789                   0

  // -----------------  ST7735R, 2. Generation Controller ------------
  //  die 128er Display der 2. Generation haben in Verebindung mit
  //  ST7735 eine andere StartColum und RowColum

  #define  st7735r_g2               0


  // -------------- Displays mit 8-Bit Parallelinterface -------------

  #define  ili9341                  0                // 320 x 240 Pixel
  #define  ili9481                  0                // 480 x 320 Pixel
  #define  ili9486                  0                // 480 x 320 Pixel

  /* ------------------------------------------------------------------
        verfuegbare Textfonts auswaehlen (auch Kombinationen erlaubt)
     ------------------------------------------------------------------ */

  #define fnt5x7_enable             1                // 1 : Font verfuegbar
                                                     // 0 : nicht verfuegbar
  #define fnt8x8_enable             1                // 1 : Font verfuegbar
                                                     // 0 : nicht verfuegbar
  #define fnt12x16_enable           0                // 1 : Font verfuegbar
                                                     // 0 : nicht verfuegbar

  #define lastascii 126                              // letztes verfuegbares Asciizeichen

  /*  ------------------------------------------------------------
                         Displayaufloesung
      ------------------------------------------------------------ */

  #define _xres                   128
  #define _yres                   160

  #define mirror                  0                 // 0 : normale Ausgabe
                                                    // 1 : Spiegelbildausgabe


  #if ((ili9163 == 1) || (ili9340 == 1) || (st7735r == 1) || (s6d02a1 == 1 ) || (ili9225 == 1) || (st7789 == 1))
    #define USE_SPI_TFT             1
    #define USE_8BIT_TFT            0
  #endif

  #if ((ili9341 == 1) || (ili9481 == 1) || (ili9486 == 1))
    #define USE_8BIT_TFT            1
    #define USE_SPI_TFT             0
  #endif

  #if (USE_SPI_TFT == 1)

    /*  ------------------------------------------------------------
                         Setupflags fuer SPI-Displays
        ------------------------------------------------------------ */

    // fuer Berechnung Bildadressen. ACHTUNG: manche Chinadisplays behandeln 128x128 Displays
    // so, als haette es 160 Pixel in Y-Aufloesung.

    // In diesem Fall ist fuer _lcyofs  -32 anzugeben
    // (hat nur Effekt, wenn _yres   128 , im Hauptprogramm dann outmode= 3; damit das Bild
    // nicht af dem Kopf steht)

    #define tft128                  2                 // 1: Display ohne Offset (aelter)
                                                      // 2: Display mit Offset (neuer)

    #define rgbseq                  1                 // Reihenfolge der erwarteten Farbuebergabe bei ST7735 LC-Controllern
                                                      // 0: blau-gruen-rot
                                                      // 1: rot-gruen-blau

    #define pindefs                 3                 // unterschiedliche Anschluesse an den Controller
                                                      // Deklarationen der Anschlusspins in tft_pindefs.h
                                                      // 1 : Anschluss Lochrasterboard
                                                      // 2 : Anschluss R3 - Evalboard (gedruckte Schaltung)
                                                      // 3 : Steckbrett (F030F4P6)
                                                      // 4 : TFT-Button Shield Board 2
                                                      // 5 : TFT-Button Shield

    #define tft_wait                0                 // 0 = keine Wartefunktion nach spi_out
                                                      // 1 = es wird nach spi_out ein nop eingefuegt


    #define flickerreduce           0                 // 0 : normal
                                                      // 1 : Reduzierung fuer neuere KMR-1.8 SPI Displays

    #define negativout              0                 // 0 : normal
                                                      // 1 : Farben werden invertiert wiedergegeben (fuer ST7789 notwendig)

    /*  ------------------------------------------------------------
          Sonderfall TFT 128x128 / ST7735 Controller 2. Generation
        ------------------------------------------------------------ */
    #if ((st7735r == 1) && (st7735r_g2 == 1) && (_xres == 128) && (_yres == 128))
      #define colofs                2
      #define rowofs                3
    #else
      #define colofs                0
      #define rowofs                0
    #endif

    /*  ------------------------------------------------------------
          Display-Offset neue/alte 128x128 Pixel TFTs
        ------------------------------------------------------------ */
    #if (tft128 == 2)
      #define _lcyofs               -32               // manche Display sprechen das Display an
                                                      // als haette es 160 Pixel Y-Aufloesung
    #else
      #define _lcyofs               0
    #endif


  #endif // USE_SPI_TFT

  #if (USE_8BIT_TFT == 1)

    /*  ------------------------------------------------------------
                Setupflags fuer 8-Bit TFT mit Parallelinterface
        ------------------------------------------------------------ */

      #define colofs                0
      #define rowofs                0

    /* ------------------------------------------------------------
                     Pinbelegung Display zu Controller
       ------------------------------------------------------------ */
      #define boardversion     1                           // 0: Nucleo R3
                                                           // 1: eigenes STM32F103 Board R3

      //  Defines LCD Darstellung

      #define MEM_Y    7
      #define MEM_X    6
      #define MEM_V    5
      #define MEM_L    4
      #define MEM_BGR  3
      #define MEM_H    2

  #endif    // USE_8BIT_TFT

  /*  ------------------------------------------------------------
                Anschlusspins Display zu Controller
       ----------------------------------------------------------- */
  #include "tft_pindefs.h"

  /*  ------------------------------------------------------------
       soll innerhalb der fillrect - Funktion ein Fastfill mittels
       der Funktionen des Displays vorgenommen werden (hier
       funktioniert dann ein "Drehen" mittels outmode NICHT)
     ------------------------------------------------------------- */

  #define  fastfillmode             0

  /*  ------------------------------------------------------------
                         P R O T O T Y P E N
      ------------------------------------------------------------ */

  // ----------------- LCD - Benutzerfunktionen ---------------

  void lcd_init(void);                                                        // initialisiert Display
  void lcd_orientation (uint8_t ori);                                         // kompletten Displayinhalt bei der Ausgabe drehen
  void putpixel(int x, int y,uint16_t color);                                 // schreibt einen einzelnen Punkt auf das Display
  void clrscr();                                                              // loescht Display-Inhalt
  void fastxline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t color);      // zeichnet eine Linie in X-Achse
  void fillrect(int x1, int y1, int x2, int y2, uint16_t color);              // fuellt einen rechteckigen Bereich mit Farbe aus
  uint16_t rgbfromvalue(uint8_t r, uint8_t g, uint8_t b);                     // konvertiert einen 24 Bit RGB-Farbwert in einen 16 Bit Farbwert
  uint16_t rgbfromega(uint8_t entry);                                         // konvertiert einen Farbwert aus der EGA-Palette in einen 16 Bit Farbwert
  void gotoxy(unsigned char x, unsigned char y);                              // setzt den Textcursor fuer Textausgaben
  void setfont(uint8_t nr);                                                   // setzt Schriftstil: 0= 8x8 Pixel, 2= 5x7 Pixel
  void lcd_putchar(char ch);                                                  // setzt ein Zeichen auf das Display
  void lcd_putchar5x7(unsigned char ch);
  void lcd_putchar8x8(unsigned char ch);
  void lcd_putchar12x16(unsigned char ch);
  void putcharxy(int oldx, int oldy, unsigned char ch);                       // setzt ein Zeichen an der angegebenen  Grafikkoordinate
  void outtextxy(int x, int y, uint8_t dir, char *dataPtr);                   // gibt einen String an Pixelkoordinaten auf dem LCD aus
  void line(int x0, int y0, int x1, int y1, uint16_t color);                  // zeichnet eine Linie
  void rectangle(int x1, int y1, int x2, int y2, uint16_t color);             // zeichnet ein Rechteck
  void ellipse(int xm, int ym, int a, int b, uint16_t color );                // zeichnet eine Ellipse
  void fillellipse(int xm, int ym, int a, int b, uint16_t color );            // zeichnet eine ausgefuellte Ellipse
  void circle(int x, int y, int r, uint16_t color );                          // zeichnet einen Kreis
  void fillcircle(int x, int y, int r, uint16_t color );                      // zeichnet einen ausgefuellten Kreis
  void showimage(uint16_t ox, uint16_t oy, const unsigned char* const image, uint16_t fwert);     // zeichnet ein monochromes Bitmap
  void putstring(char *c);                                                    // schreibt einen Textstring auf das LCD
  void turtle_moveto(int x, int y);
  void turtle_lineto(int x, int y, uint16_t col);

  // ---------------- Low-level Displayfunktionen -------------

  void set_ram_address (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);  // setzt den zu beschreibenden Speicherbereich
  void setcol(int startcol);                    // setzt zu beschreibende Spalte
  void setpage(int startpage);                  // setzt zu beschreibende Reihe
  void setxypos(int x, int y);                  // setzt die zu beschreibende Koordinate im Display-Ram

  // --------------------- SPI-Funktionen ---------------------

  void spi_init(void);
  void spi_out(uint8_t data);
  void wrcmd(uint8_t cmd);                      // schreibt einzelnes Kommandodatum (Registerzugriff)
  void wrdata(uint8_t data);                    // schreibt einzelnen Registerwert oder Ramwert
  void wrdata16(int data);                      // schreibt einen Integerwert

  /*  ------------------------------------------------------------
                      EGA - Farbzuweisungen
      ------------------------------------------------------------ */

  #define black                   0
  #define blue                    1
  #define green                   2
  #define cyan                    3
  #define red                     4
  #define magenta                 5
  #define brown                   6
  #define grey                    7
  #define darkgrey                8
  #define lightblue               9
  #define lightgreen              10
  #define lightcyan               11
  #define lightred                12
  #define lightmagenta            13
  #define yellow                  14
  #define white                   15

  //-------------------------------------------------------------
  // Registerzuordnung der Adressierungsregister der
  // verschiedenen Displaycontroller
  //-------------------------------------------------------------


  #if  (ili9225 == 1)
    #define coladdr      0x20
    #define rowaddr      0x21
    #define writereg     0x22
  #else
    #define coladdr      0x2a
    #define rowaddr      0x2b
    #define writereg     0x2c
  #endif

  //-------------------------------------------------------------
  //  Variable Farben
  //-------------------------------------------------------------

  extern uint16_t textcolor;        // Beinhaltet die Farbwahl fuer die Vordergrundfarbe
  extern uint16_t bkcolor;          // dto. fuer die Hintergrundfarbe
  extern uint16_t egapalette [];    // Farbwerte der DOS EGA/VGA Farben


  //-------------------------------------------------------------
  //  Variable und Defines Schriftzeichen
  //-------------------------------------------------------------

  extern int aktxp;                 // Beinhaltet die aktuelle Position des Textcursors in X-Achse
  extern int aktyp;                 // dto. fuer die Y-Achse
  extern uint8_t outmode;           // Richtungssinn der Displayausgabe
  extern uint8_t textsize;          // Skalierung der Ausgabeschriftgroesse
  extern uint8_t fntfilled;         // gibt an, ob eine Zeichenausgabe ueber einen Hintergrund gelegt
                                    // wird, oder ob es mit der Hintergrundfarbe aufgefuellt wird
                                    // 1 = Hintergrundfarbe wird gesetzt, 0 = es wird nur das Fontbitmap
                                    // gesetzt, der Hintergrund wird belassen

  extern uint8_t fontnr;            // 0= 8x8 Pixel, 1, 12x16, 2= 5x7 Pixel
  extern uint8_t fontsizex;
  extern uint8_t fontsizey;

  #define PSTR(txt)   ((char*)txt)  // uebergibt einen Zeiger auf einen Text (fuer outtextxy)

  //-------------------------------------------------------------
  //  Variable Turtle-Grafik
  //-------------------------------------------------------------
  extern int t_lastx, t_lasty;       // x,y - Positionen der letzten Zeichenaktion von moveto

#endif
