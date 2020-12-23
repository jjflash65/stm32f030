/* -----------------------------------------------------
                        tftmono.c

    Softwaremodul fuer monochrome Displays. Momentan
    unterstuetzte Controller:

        - PCD8544
        - SSD1306  (SPI und I2C)

    Hardware  : STM32F030
    IDE       : make - Projekt
    Library   : libopencm3
    Toolchain : arm-none-eabi

    27.02.2020   R. Seelig
  ------------------------------------------------------ */


#include "tftmono.h"

extern const uint8_t font5x7[][5];
extern const uint8_t font8x8[][8];


#if (fb_enable == 1)
  uint8_t vram[fb_size];
#endif

// --------------------------------------------
//              globale Variable
// --------------------------------------------

uint8_t textcolor = 1;
uint8_t bkcolor   = 0;
uint8_t invchar   = 0;

uint8_t aktxp     = 0;                                // Beinhaltet die aktuelle Position des Textcursors in X-Achse
uint8_t aktyp     = 0;                                // dto. fuer die Y-Achse

uint8_t fontnr    = 0;                                //  0 : 5x7  Font
                                                      //  1 : 8x8  Font

uint8_t fontsizex = 6;
uint8_t textsize  = 0;                                // Skalierung der Ausgabeschriftgroesse

/* -------------------------------------------------------
      Initialisierungssequenzen fuer SPI-Displays
   ------------------------------------------------------- */
  #define delay_flag 0x80                               // Markierungsflag: bestimmt, ob nach einem Kommando

#if ( ssd1306 == 1 )
  const uint8_t lcdinit_seq[] =                       // Initialisierungssequenzen
  {
    5,                                                // Anzahl Gesamtkommandos

  /*
    Byte 0 | Byte 1       | Byte 2 u. folgende | evtl. Delaytime-Byte
    CMD    | Anzahl Datas | Datas | evtl. Delaytime
  */

    0x8d, 0,
    0x14, 0,
    0xaf, 0 | delay_flag, 150,
    0xa1, 0,
    0xc0
  };
#endif

#if ( pcd8544 == 1 )
  const uint8_t lcdinit_seq[] =                       // Initialisierungssequenzen
  {
    7,                                                // Anzahl Gesamtkommandos

  /*
    Byte 0 | Byte 1       | Byte 2 u. folgende | evtl. Delaytime-Byte
    CMD    | Anzahl Datas | Datas | evtl. Delaytime
  */

    0x21, 0 | delay_flag, 2,
    0x09, 0 | delay_flag, 2,
    0xc8, 0 | delay_flag, 2,
    0x10, 0 | delay_flag, 2,
    0x04, 0 | delay_flag, 2,
    0x20, 0 | delay_flag, 2,
    0x0c, 0 | delay_flag, 20

  };

#endif


/*  ---------------------------------------------------------
                          reverse_byte

              tauscht die Bits eines Bytes:

      D7 <> D0, D6 <> D1, D5 <> D2, D4 <> D3
    --------------------------------------------------------- */
uint8_t reverse_byte(uint8_t b)
{
  b= (b & 0xf0) >> 4 | (b & 0x0f) << 4;
  b= (b & 0xcc) >> 2 | (b & 0x33) << 2;
  b= (b & 0xaa) >> 1 | (b & 0x55) << 1;
  return b;
}

/*  ---------------------------------------------------------
                           setfont

      legt Schriftstil fuer die Ausgabe fest.

      fnr== 0  => Font 5x7
      fnr== 1  => Font 8x8
    --------------------------------------------------------- */
void setfont(uint8_t fnr)
{
  if (fnr > 1) { fontnr= 0; return; };
  fontnr= fnr;
  if (fnr == 0) { fontsizex= 6; return; };
  if (fnr == 1) { fontsizex= 8; return; };
  return ;
}

/*  ---------------------------------------------------------
                         doublebits

      dupliziert Bits eines Nibbles, so dass diese "doppelt"
      vorhanden sind (von Zeichenausgabe mit textsize > 0
      benoetigt).

      Uebergabe:
            b      : zu duplizierendes Byte
            nibble : Nibble, dessen Bytes dupliziert werden
                     soll

      Bsp.:
         b= doublebits(0xc5, 0);

         Unteres Nibble ist relevant,
         Value hier = 5 entspricht 0101b

         b hat den Wert 00110011b
    --------------------------------------------------------- */
uint8_t doublebits(uint8_t b, uint8_t nibble)
{
  uint8_t b2;

  if (nibble) b = b >> 4;
  b= b & 0x0f;
  b2= 0;
  b2 = ((b & 1) << 0) | ((b & 1) << 1) | ((b & 2) << 1) | ((b & 2) << 2) |           //
       ((b & 4) << 2) | ((b & 4) << 3) | ((b & 8) << 3) | ((b & 8) << 4);

  return b2;
}


#if (use_i2c == 0)
  /* -------------------------------------------------------
                  SPI - Kommunikation
     ------------------------------------------------------- */
  /* -----------------------------------------------------
                          out_byte
       sendet ein Datum ueber Interface
     ----------------------------------------------------- */
  void out_byte(uint8_t value)
  {
    spi_send8(SPI1, value);
    #if (pcd8544 == 1)
      // warum auch immer, scheinbar ist HW-SPI zu schnell fuer die
      // Displays, deshalb ein NOP nach abgeschicktem Byte
      __asm volatile
      (
        "nop\n\r"
        "nop\n\r"
        "nop\n\r"
        "nop\n\r"
        "nop\n\r"
        "nop\n\r"
      );
    #endif
  }
#endif                  // SPI - Kommunikation

#if (use_i2c == 1)
  /* -------------------------------------------------------
                  I2C - Kommunikation
     ------------------------------------------------------- */
  /*  ---------------------------------------------------------
                              i2c_init

        initialisiert den I2C Bus je nach Angabe in den Defines
        als I2C1 (bspw. 20 pol. IC Gehaeuse) oder I2C2 (bspw.
        fuer 48 pol. IC Gehaeuse)

        Pinbelegungen:

        I2C1  : SCL = PA9,  SDA = PA10
        I2C2  : SCL = PB10, SDA = PB11
      --------------------------------------------------------- */
  void i2c_init(void)
  {

    #if (i2c_nr == I2C2)
      rcc_periph_clock_enable(RCC_I2C2);
      rcc_periph_clock_enable(RCC_GPIOB);
      rcc_set_i2c_clock_hsi(i2c_nr);

      gpio_set_af(GPIOB, GPIO_AF1, GPIO10 | GPIO11);
      gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10 | GPIO11);
    #endif

    #if (i2c_nr == I2C1)
      rcc_periph_clock_enable(RCC_I2C1);
      rcc_periph_clock_enable(RCC_GPIOA);
      rcc_set_i2c_clock_hsi(i2c_nr);

      gpio_set_af(GPIOA, GPIO_AF4, GPIO10 | GPIO9);
      gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10 | GPIO9);
    #endif

    i2c_reset(i2c_nr);
    i2c_peripheral_disable(i2c_nr);

    // Hier wurde ABSICHTLICH eine "falsche" HSI-Frequenz angegeben. Bei einem Takt von
    // 48MHz und der Angabe "16" laeuft der I2C-Clock sehr viel schneller (gemessen 1,2 MHz)

    i2c_set_speed(i2c_nr, i2c_speed_fm_400k, 16);

    // Adressmodus
    i2c_set_7bit_addr_mode(i2c_nr);

    i2c_peripheral_enable(i2c_nr);
  }

  /*  ---------------------------------------------------------
                                i2c_start
                                               -
        sendet Startcondition auf I2C_2 Bus. Device-Adresse
        wird als 8-Bit Adresse uebergeben (und in der Funktion
        auf 7 Bit Adresse "reduziert"
      --------------------------------------------------------- */
  void i2c_start(uint8_t addr, uint8_t byte_to_write)
  {
    i2c_set_7bit_address(i2c_nr, addr >> 1);
    i2c_set_write_transfer_dir(i2c_nr);
    i2c_set_bytes_to_transfer(i2c_nr, byte_to_write);
    i2c_enable_autoend(i2c_nr);

    i2c_send_start(i2c_nr);
  }


  /*  ---------------------------------------------------------
                                i2c_write

        schreibt einen 8 Bit Wert auf dem I2C_2 Bus
      --------------------------------------------------------- */
  void i2c_write(uint8_t value)
  {
    bool wait = true;

    while (wait)
    {
      if (i2c_transmit_int_status(i2c_nr))
      {
        wait = false;
      }
     while (i2c_nack(i2c_nr)); /* FIXME Some error */
    }
    i2c_send_data(i2c_nr, value);                                   // Wert senden
  }

  /*  ---------------------------------------------------------
                             ssd1306_init

        initialisiert das OLED-Display fuer die Benutzung
      --------------------------------------------------------- */
  void ssd1306_init(void)
  {
    i2c_init();

    delay(10);

    //Init LCD

    i2c_start(ssd1306_addr, 6);
    i2c_write(0x00);

    i2c_write(0x8d);      // Ladungspumpe an
    i2c_write(0x14);
    i2c_write(0xaf);      // Display an
    delay(150);
    i2c_write(0xa1);      // Segment Map
    i2c_write(0xc0);      // Direction Map
  }

#endif                    // I2C - Kommunikation

#if (use_i2c == 0)
  /*  ---------------------------------------------------------
                   Funktionen fuer SPI Display
      --------------------------------------------------------- */
  /* -----------------------------------------------------
                        lcd_setxypos

          addressiert das Displayram in Abhaengigkeit
          der X-Y Koordinate
     ----------------------------------------------------- */
  void lcd_setxypos(uint8_t x, uint8_t y)
  {
    #if ( ssd1306 == 1 )
      lcd_cmdmode();
      y= 7-y;

      out_byte(0xb0 | (y & 0x0f));
      out_byte(0x10 | (x >> 4 & 0x0f));
      out_byte(x & 0x0f);

    #endif

    #if ( pcd8544 == 1 )
      lcd_cmdmode();
      out_byte(0x80+x);
      out_byte(0x40+y);
    #endif
  }


  /*  ---------------------------------------------------------
                         lcd_setxybyte

        setzt ein Byte an Koordinate x,y

        Anmerkung:
              da bei monochromen Displays gleichzeitig 8 Pixel
              in Y Richtung geschrieben werden, ist bei bspw.
              einer 64 Pixel Aufloesung in Y-Richtung der
              Wertebereich fuer y = 0..7 !

              Bsp. Koordinatenuebergabe y== 6 beschreibt
              y-Pixelkoordinaten 48 bis 55 (inclusive)
      --------------------------------------------------------- */
  void lcd_setxybyte(uint8_t x, uint8_t y, uint8_t value)
  {
    #if ( ssd1306 == 1 )
      lcd_cmdmode();
      y= 7-y;

      out_byte(0xb0 | (y & 0x0f));
      out_byte(0x10 | (x >> 4 & 0x0f));
      out_byte(x & 0x0f);

      lcd_datamode();
      if ((!textcolor)) value= ~value;
      out_byte(value);

    #endif

    #if ( pcd8544 == 1 )
      lcd_cmdmode();
      out_byte(0x80+x);
      out_byte(0x40+y);
      lcd_datamode();
      value= reverse_byte(value);
      if ((!textcolor)) value= ~value;
      out_byte(value);
      lcd_cmdmode();
      out_byte(0);
    #endif

  }

  /* -----------------------------------------------------
                          gotoxy

     positioniert die Textausgabeposition auf X/Y
     ----------------------------------------------------- */
  void gotoxy(uint8_t x, uint8_t y)
  {
    uint8_t xp;
    uint8_t sizexfak;

    aktxp= x; aktyp= y;

    switch (textsize)
    {
      case 0  : sizexfak= 1; break;
      case 1  : sizexfak= 2; break;
      case 2  : sizexfak= 2; break;
      default : break;
    }

    switch (fontnr)
    {
      case 0  : xp = x * 6 * sizexfak; break;
      case 1  : xp = x * 8 * sizexfak; break;
      default : break;
    }

    #if ( ssd1306 == 1 )

      lcd_setxypos(xp,y);
      lcd_datamode();

    #endif

    #if ( pcd8544 == 1 )

      lcd_cmdmode();
      out_byte(0x80 + xp);
      out_byte(0x40 + y);
      lcd_datamode();

    #endif
  }

  /*  ---------------------------------------------------------
                             clrscr

        loescht den Displayinhalt mit der in bkcolor ange-
        gebenen "Farbe" (0 = schwarz, 1 = hell)
      --------------------------------------------------------- */
  void clrscr(void)
  {
    #if ( ssd1306 == 1 )

      uint8_t x,y;

      for (y= 0; y < (_yres >> 3); y++)         // ein Byte in Y-Achse = 8 Pixel...
                                    // 8*8Pixel = 64 Y-Reihen
      {
        lcd_setxypos(0, y);
        lcd_datamode();

        for (x= 0; x < _xres; x++)
        {
          if (bkcolor) out_byte(0xff); else out_byte(0x00);
        }
      }
    #endif

    #if ( pcd8544 == 1 )
      uint16_t i;

      lcd_cmdmode();
      out_byte(0x80);
      out_byte(0x40);
      lcd_datamode();

      for (i= 0; i < (_xres * (_yres >> 3)); i++)
        if (bkcolor) out_byte(0xff); else out_byte(0x00);

    #endif
  }

  /*  ---------------------------------------------------------
                               lcd_init

        initialisiert die zusaetzlich zum Kommunikations-
        interface benoetigten Anschlusspins und initialisiert
        das Display hierueber.

        Einw bereits initialisierte Kommunikationsschnittstelle
        (je nach verwendetem Display) wird vorrausgesetzt.
      --------------------------------------------------------- */
  void lcd_init(void)
  {
    const uint8_t *tabseq;
    volatile uint8_t  cmd_anz;
    volatile uint8_t  arg_anz;
    volatile uint16_t ms;
    uint16_t i;

    spi_init();

    lcd_ce_init();
    lcd_rst_init();
    lcd_dc_init();

    lcd_enable();
    delay(10);

    lcd_rst_clr();                // Display reset
    delay(10);
    lcd_rst_set();

    tabseq= &lcdinit_seq[0];

    // ein einzelnes Kommando besteht aus mehreren Datenbytes. Zuerst wird ein Kommandobyte
    // auf dem SPI geschickt, anschliessend die zu diesem Kommandobytes dazugehoerigen Datenbytes
    // abschliessend wird evtl. ein Timingwait eingefuegt. Dieses wird fuer alle vorhandenen
    // Kommandos durchgefuehrt

    cmd_anz = *tabseq++;               // Anzahl Gesamtkommandos

    while(cmd_anz--)                                 // alle Kommandos auf SPI schicken
    {
      lcd_cmdmode();
      out_byte(*tabseq++);                            // Kommando lesen
      arg_anz= *tabseq++;                            // Anzahl zugehoeriger Datenbytes lesen
      ms= arg_anz & delay_flag;                      // bei gesetztem Flag folgt ein Timingbyte
      arg_anz &= ~delay_flag;                        // delay_flag aus Anzahl Argumenten loeschen

      while(arg_anz--)                               // jedes Datenbyte des Kommandos
      {
        lcd_datamode();
        out_byte(*tabseq++);                          // senden
      }
      if(ms)                                         // wenn eine Timingangabe vorhanden ist
      {
        ms= *tabseq++;                               // Timingzeit lesen
        if(ms == 255) ms = 500;
        for (i= 0; i< ms; i++) delay(1);             // und entsprechend "nichts" tun
      }
    }

    lcd_datamode();
    clrscr();
    gotoxy(0,0);
  }

  /* -----------------------------------------------------
                       lcd_putchar

       gibt ein Zeichen direkt auf dem Display (nicht
       ueber den Framebuffer) entsprechend dem gewaehlten
       Schriftstil aus
     ----------------------------------------------------- */
  void lcd_putchar(uint8_t ch)
  {
    uint8_t b, rb;
    uint8_t ax, ay;
    uint8_t fak;

    ax= aktxp; ay= aktyp;
    if (ch== 13)
    {
      gotoxy(0,aktyp);
      return;
    }
    if (ch== 10)
    {
      aktyp++;
      gotoxy(aktxp,aktyp);
      return;
    }

    if (ch== 8)
    {
      if ((aktxp> 0))
      {
        aktxp--;
        gotoxy(aktxp,aktyp);
        lcd_datamode();
        for (b= 0; b < fontsizex; b++)
        {
            if (invchar) { out_byte(0xff); }
                    else { out_byte(0); }
        }
        gotoxy(aktxp,aktyp);
      }
      return;
    }
    // Kopiere Daten eines Zeichens aus dem Zeichenarray in den LCD-Screenspeicher
    lcd_datamode();

    for (b= 0; b< (fontsizex-1); b++)
    {
      if (fontnr) rb= font8x8[ch-32][b];
             else rb= font5x7[ch-32][b];

      #if ( ssd1306 == 1 )
        rb= reverse_byte(rb);
        if (invchar) {rb= ~rb;}
        if (textsize == 2) rb= doublebits(rb,1);
      #endif

      #if ( pcd8544 == 1 )
        if (invchar) {rb= ~rb;}
        if (textsize == 2) rb= doublebits(rb,0);
      #endif

      out_byte(rb);
      if (textsize) out_byte(rb);
    }

    if (invchar) { out_byte(0xff); }
            else { out_byte(0); }

    if (textsize== 2)
    {
      gotoxy(aktxp,aktyp+1);
      lcd_datamode();

      for (b= 0; b< (fontsizex-1); b++)
      {
        if (fontnr) rb= font8x8[ch-32][b];
               else rb= font5x7[ch-32][b];

        #if ( ssd1306 == 1 )
          rb= reverse_byte(rb);
          if (invchar) {rb= ~rb;}
          if (textsize == 2) rb= doublebits(rb,0);
        #endif

        #if ( pcd8544 == 1 )
          if (invchar) {rb= ~rb;}
          if (textsize == 2) rb= doublebits(rb,1);
        #endif

        out_byte(rb);
        if (textsize) out_byte(rb);

      }

      if (invchar) { out_byte(0xff); }
                else { out_byte(0); }

      aktxp= ax; aktyp= ay;
    }

    aktxp++;

    if (textsize) fak= 2; else fak= 1;

    if (aktxp> (_xres / (6 * fak))-1)
    {
      aktxp= 0;
      aktyp++;
    }

    gotoxy(aktxp,aktyp);
  }

  #if (fb_enable == 1)
    /* ----------------------------------------------------------
       fb_show

       zeigt den Framebufferspeicher ab der Koordinate x,y
       (links oben) auf dem Display an
       ---------------------------------------------------------- */
    void fb_show(uint8_t x, uint8_t y)
    {
      uint8_t   xp, yp;
      uint16_t  fb_ind;
      uint8_t   value;

      fb_ind= 2;
      for (yp= y; yp< vram[1]+y; yp++)
      {

        #if ( ssd1306 == 1 )
          lcd_setxypos(x, yp);

          lcd_datamode();

          for (xp= x; xp< vram[0]+x; xp++)
          {
            value= vram[fb_ind];

            if ((!textcolor)) value= ~value;
            out_byte(value);

            fb_ind++;
          }
        #endif

        #if ( pcd8544 == 1 )
          lcd_cmdmode();
          out_byte(0x80+x);
          out_byte(0x40+yp);

          lcd_datamode();

          for (xp= x; xp< vram[0]+x; xp++)
          {
            value= vram[fb_ind];
            value= reverse_byte(value);

            if ((!textcolor)) value= ~value;
            out_byte(value);

            fb_ind++;
          }

          lcd_cmdmode();
          out_byte(0);

        #endif
      }
    }
  #endif                // fb_enable

  #if (showimage_enable == 1)
    /* ---------------------------------------------------------
                             showimage

       zeigt ein Bitmap an den Koordinaten x,y an

       Parameter

       mode: Zeichenmodus
                0 = Bitmap wird mit der in bkcolor angegebenen
                    Farbe geloescht
                1 = Bitmap wird gezeichnet
                2 = Bitmap wird invertiert gezeichnet

       zeigt ein im FLASH abgelegtes Bitmap an, wie es von
       IMAGE2C erzeugt wurde.

       Speicherorganisation des Bitmaps:

       Y       X-Koordinate
       |        0  1  2  3  4  5  6  7    8  9 10 11 12 13 14 15
       K               Byte 0                    Byte 1
       o  0     D7 D6 D5 D4 D3 D2 D1 D0   D7 D6 D5 D4 D3 D2 D1 D0
       o
       r         Byte (Y*XBytebreite)     Byte (Y*XBytebreite)+1
       d  1     D7 D6 D5 D4 D3 D2 D1 D0   D7 D6 D5 D4 D3 D2 D1 D0
       i
       n
       a
       t
       e

       --------------------------------------------------------- */
    void showimage(uint8_t ox, uint8_t oy, const uint8_t* const image, char mode)
    {
      uint8_t x,y,xp;
      uint8_t hb,b;
      char i;
      uint8_t resX, resY;


      resX= image[1];
      resY= image[3];

      if ((resX % 8) == 0) { resX= resX / 8; }
                     else  { resX= (resX / 8)+1; }

      for (y=0; y < (resY / 8); y++)
      {
        xp= 0;
        lcd_setxypos(ox, y + (oy / 8));
        lcd_datamode();

        for (x= 0; x < (resX * 8); x++)
        {

          if ((mode==1) || (mode==2))
          {

            b= 0xff;
            for (i= 0; i < 8; i++)
            {
              hb = image[(((y*8)+i) * resX) +(x / 8)+4];
              hb &= 1<<(7-xp);
              if (hb != 0)
              {
                b&= ~(1 << i);
              }
            }

            xp++;
            xp = xp % 8;

            #if ( ssd1306 == 1 )
              b = reverse_byte(b);
            #endif

            if (mode==2) b= ~b;
            out_byte(b);
          }
          else
          {
            if (bkcolor) out_byte(0xff); else  out_byte(0x00);
          }
        }
      }
    }

  #endif                  // showimage

#endif                  // SPI - Display

#if (use_i2c == 1)

  /*  ---------------------------------------------------------
                   Funktionen fuer I2C Display
      --------------------------------------------------------- */
  /*  ---------------------------------------------------------
                             lcd_init

        initialisiert das OLED-Display fuer die Benutzung
      --------------------------------------------------------- */
  void lcd_init(void)
  {
    i2c_init();

    delay(10);

    //Init LCD

    i2c_start(ssd1306_addr, 6);
    i2c_write(0x00);

    i2c_write(0x8d);      // Ladungspumpe an
    i2c_write(0x14);
    i2c_write(0xaf);      // Display an
    delay(150);
    i2c_write(0xa1);      // Segment Map
    i2c_write(0xc0);      // Direction Map
  }

  /*  ---------------------------------------------------------
                                gotoxy

       legt die naechste Textausgabeposition auf dem
       Display fest. Koordinaten 0,0 bezeichnet linke obere
       Position
      --------------------------------------------------------- */
  void gotoxy(uint8_t x, uint8_t y)
  {
    uint8_t xp, sizexfak;

    switch (textsize)
    {
      case 0  : sizexfak= 1; break;
      case 1  : sizexfak= 2; break;
      case 2  : sizexfak= 2; break;
      default : break;
    }

    switch (fontnr)
    {
      case 0  : xp = x * 6 * sizexfak; break;
      case 1  : xp = x * 8 * sizexfak; break;
      default : break;
    }

    aktxp= x;
    aktyp= y;
    y= 7-y;

    i2c_start(ssd1306_addr, 4);
    i2c_write(0x00);

    i2c_write(0xb0 | (y & 0x0f));
    i2c_write(0x10 | (xp >> 4 & 0x0f));
    i2c_write(xp & 0x0f);

  }


  /*  ---------------------------------------------------------
                             clrscr

        loescht den Displayinhalt mit der in bkcolor ange-
        gebenen "Farbe" (0 = schwarz, 1 = hell)
      --------------------------------------------------------- */
  void clrscr(void)
  {
    uint8_t x,y;

    i2c_start(ssd1306_addr, 6);
    i2c_write(0x00);

    i2c_write(0x8d);               // Ladungspumpe an
    i2c_write(0x14);

    i2c_write(0xaf);                  // Display on

    i2c_write(0xa1);                  // Segment Map
    i2c_write(0xc0);                  // Direction Map


    for (y= 0; y< 8; y++)                  // ein Byte in Y-Achse = 8 Pixel...
                                           // 8*8Pixel = 64 Y-Reihen
    {
      i2c_start(ssd1306_addr, 4);
      i2c_write(0x00);

      i2c_write((0xb0 | y));            // Pageadresse schreiben
      i2c_write(0x00);                // MSB X-Adresse
      i2c_write(0x00);                // LSB X-Adresse (+Offset)


      i2c_start(ssd1306_addr, 129);
      i2c_write(0x40);
      for (x= 0; x< 128; x++)
      {

        if (bkcolor) i2c_write(0xff); else i2c_write(0x00);

      }

    }
    gotoxy(0,0);
  }

  /*  ---------------------------------------------------------
                           lcd_putchar

       gibt ein Zeichen auf dem Display aus. Steuerzeichen
       (fuer bspw. printf) sind implementiert:

               13 = carriage return
               10 = line feed
                8 = delete last char
      --------------------------------------------------------- */
  void lcd_putchar(uint8_t ch)
  {
    uint8_t  b;
    uint8_t  ax, ay, fak;
    uint8_t  rb;
    uint8_t  anz;

    if (ch== 0) return;

    ax= aktxp; ay= aktyp;
    if (ch== 13)
    {
      gotoxy(0,aktyp);
      return;
    }
    if (ch== 10)
    {
      aktyp++;
      gotoxy(aktxp,aktyp);
      return;
    }

    if (ch== 8)
    {
      if ((aktxp> 0))
      {

        aktxp--;
        gotoxy(aktxp, aktyp);

        i2c_start(ssd1306_addr, fontsizex+1);
        i2c_write(0x40);
        for (b= 0; b< fontsizex; b++)
        {
         if (invchar) i2c_write(0xff); else i2c_write(0x00);
        }
        gotoxy(aktxp, aktyp);
      }
      return;

    }

    if (textsize == 2)
    {
      ax= aktxp;
      anz= (fontsizex-1)*2;

      i2c_start(ssd1306_addr, anz+1 );
      i2c_write(0x40);

      for (b= 0; b< fontsizex-1; b++)
      {
        if (fontnr) rb= font8x8[ch-32][b];
               else rb= font5x7[ch-32][b];

        rb= reverse_byte(rb);
        if (invchar) {rb= ~rb;}
        rb= doublebits(rb,1);

        i2c_write(rb);
        if (textsize) i2c_write(rb);
      }

      ax= aktxp; ay= aktyp;
      gotoxy(aktxp,aktyp+1);

      i2c_start(ssd1306_addr, anz+1 );
      i2c_write(0x40);

      for (b= 0; b< fontsizex-1; b++)
      {
        if (fontnr) rb= font8x8[ch-32][b];
               else rb= font5x7[ch-32][b];

        rb= reverse_byte(rb);
        if (invchar) {rb= ~rb;}
        rb= doublebits(rb,0);

        i2c_write(rb);
        if (textsize) i2c_write(rb);
      }

      ax++;
      if (textsize) fak= 2; else fak= 1;

      if (aktxp> (_xres / (6 * fak))-1)
      {
        ax= 0;
        ay++;
      }

      gotoxy(ax,ay);

    }
    else
    {
      anz= fontsizex-1;
      if (textsize) anz= anz*2;
      i2c_start(ssd1306_addr, anz+1 );
      i2c_write(0x40);

      for (b= 0; b< fontsizex-1; b++)
      {
        if (fontnr) rb= font8x8[ch-32][b];
               else rb= font5x7[ch-32][b];

        rb= reverse_byte(rb);
        if (invchar) {rb= ~rb;}

        i2c_write(rb);
        if (textsize) i2c_write(rb);
      }

      ax++;
      if (textsize) fak= 2; else fak= 1;

      if (aktxp> (_xres / (6 * fak))-1)
      {
        ax= 0;
        ay++;
      }
      gotoxy(ax,ay);
    }
  }

  /*  ---------------------------------------------------------
                            setxybyte

        setzt ein Byte an Koordinate x,y

        Anmerkung:
              da monochromes Display werden immer 8 Pixel
              gleichzeitig in Y Richtung geschrieben. Daher
              ist Wertebereich fuer y = 0..7 !

              Bsp. Koordinate y== 6 beschreibt tatsaechliche
              y-Koordinaten 48-55 (inclusive)
      --------------------------------------------------------- */
  void setxybyte(uint8_t x, uint8_t y, uint8_t value)
  {
      y= 7-y;

      i2c_start(ssd1306_addr, 4);
      i2c_write(0x00);
      i2c_write(0xb0 | (y & 0x0f));
      i2c_write(0x10 | (x >> 4 & 0x0f));
      i2c_write(x & 0x0f);

      if ((!textcolor)) value= ~value;
      i2c_start(ssd1306_addr, 2);
      i2c_write(0x40);
      i2c_write(value);
  }



  /*  ---------------------------------------------------------
                             setpageadr

        adressiert den Speicher des Displays (und gibt somit
        die Speicherstelle an, die als naechstes beschrieben
        wird)
      --------------------------------------------------------- */
  void setpageadr(uint8_t x, uint8_t y)
  {
    y= 7-y;

    i2c_start(ssd1306_addr, 4);
    i2c_write(0x00);

    i2c_write(0xb0 | (y & 0x0f));
    i2c_write(0x10 | (x >> 4 & 0x0f));
    i2c_write(x & 0x0f);
  }

  #if (fb_enable == 1)

    /* ----------------------------------------------------------
                                 fb_show

       zeigt den Framebufferspeicher ab der Koordinate x,y
       (links oben) auf dem Display an.

       Da die Speicherorganisation in Y-Achse jeweils 8 Pixel
       per Byte umfassen, ist die moeglichste unterste Y-
       Koordinate 7 (entspricht Pixelkoordinate 7*8= 56)
       ---------------------------------------------------------- */
    void fb_show(uint8_t x, uint8_t y)
    {
      volatile static uint8_t   xp, yp;
      volatile static uint16_t  fb_ind;
      volatile static uint8_t   value;

      fb_ind= 2;

      for (yp= y; yp< vram[1]+y; yp++)
      {
        setpageadr(x, yp);

        i2c_start(ssd1306_addr, vram[0]+1);
        i2c_write(0x40);

        for (xp= x; xp< vram[0]+x; xp++)
        {
          value= vram[fb_ind];

          if ((!textcolor)) value= ~value;

          i2c_write(value);

          fb_ind++;
        }
      }
    }
  #endif                // fb_enable

  #if (showimage_enable == 1)
    void showimage(uint8_t ox, uint8_t oy, const uint8_t* const image, char mode)
    {
      uint8_t x,y,xp;
      uint8_t hb,b;
      char i;
      uint8_t resX, resY;


      resX= image[1];
      resY= image[3];

      if ((resX % 8) == 0) { resX= resX / 8; }
                     else  { resX= (resX / 8)+1; }

      for (y=0; y < (resY / 8); y++)
      {
        xp= 0;
        setpageadr(ox, y + (oy / 8));

        i2c_start(ssd1306_addr, (resX*8)+1);
        i2c_write(0x40);

        for (x= 0; x < (resX * 8); x++)
        {

          if ((mode==1) || (mode==2))
          {

            b= 0xff;
            for (i= 0; i < 8; i++)
            {
              hb = image[(((y*8)+i) * resX) +(x / 8) + 4];          // Bilddaten beginnen ab Offset 4, Byte 0..3 enthaelt Breite/Hoehe des Images
              hb &= 1<<(7-xp);
              if (hb != 0)
              {
                b&= ~(1 << i);
              }
            }

            xp++;
            xp = xp % 8;
            b = reverse_byte(b);
            if (mode==2) b= ~b;
            i2c_write(b);
          }
          else
          {
            if (bkcolor) i2c_write(0xff); else  i2c_write(0x00);
          }
        }
      }
    }

  #endif                // showimage_enable

#endif                  // I2C - Funktionen


/* ----------------------------------------------------------
                Framebuffer Funktionen
   ---------------------------------------------------------- */
#if (fb_enable == 1)

  uint8_t txoutmode = 0;

  /* ----------------------------------------------------------
     fb_putixel

     setzt einen Pixel im Framebufferspeicher an Position
     x,y.

     col:      0 = loeschen
               1 = setzen
               2 = Pixelpositon im XOR-Modus verknuepfen
     ---------------------------------------------------------- */

  void fb_putpixel(uint8_t x, uint8_t y, uint8_t col)
  {
    uint16_t fbi;
    uint8_t  xr;
//    uint8_t  yr;
    uint8_t  pixpos;

    xr= vram[0];
//    yr= vram[1];
    fbi= ((y >> 3) * xr) + 2 + x;
    pixpos= 7- (y & 0x07);

    switch (col)
    {
      case 0  : vram[fbi] &= ~(1 << pixpos); break;
      case 1  : vram[fbi] |= 1 << pixpos; break;
      case 2  : vram[fbi] ^= 1 << pixpos; break;

      default : break;
    }
  }

  /* ----------------------------------------------------------
     line

     Zeichnet eine Linie von den Koordinaten x0,y0 zu x1,y1
     im Screenspeicher.

     Linienalgorithmus nach Bresenham (www.wikipedia.org)

     col       0 = loeschen
               1 = setzen
               2 = Pixelpositon im XOR-Modus verknuepfen
     ---------------------------------------------------------- */
  void line(int x0, int y0, int x1, int y1, uint8_t col)
  {

    //    Linienalgorithmus nach Bresenham (www.wikipedia.org)

    int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2;

    for(;;)
    {
      fb_putpixel(x0,y0, col);
      if (x0==x1 && y0==y1) break;
      e2 = 2*err;
      if (e2 > dy) { err += dy; x0 += sx; }
      if (e2 < dx) { err += dx; y0 += sy; }
    }
  }

  /* ----------------------------------------------------------
     RECTANGLE

     Zeichnet ein Rechteck von den Koordinaten x0,y0 zu x1,y1
     im Screenspeicher.

     Linienalgorithmus nach Bresenham (www.wikipedia.org)

     col       0 = loeschen
               1 = setzen
               2 = Pixelpositon im XOR-Modus verknuepfen
     ---------------------------------------------------------- */
  void rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t col)
  {
    line(x1,y1,x2,y1, col);
    line(x2,y1,x2,y2, col);
    line(x1,y2,x2,y2, col);
    line(x1,y1,x1,y2, col);
  }

  /* ----------------------------------------------------------
     ELLIPSE

     Zeichnet eine Ellipse mit Mittelpunt an der Koordinate xm,ym
     mit den Hoehen- Breitenverhaeltnis a:b
     im Screenspeicher.

     Ellipsenalgorithmus nach Bresenham (www.wikipedia.org)

     col       0 = loeschen
               1 = setzen
               2 = Pixelpositon im XOR-Modus verknuepfen
     ---------------------------------------------------------- */
  void ellipse(int xm, int ym, int a, int b, uint8_t col )
  {
    // Algorithmus nach Bresenham (www.wikipedia.org)

    int dx = 0, dy = b;                       // im I. Quadranten von links oben nach rechts unten
    long a2 = a*a, b2 = b*b;
    long err = b2-(2*b-1)*a2, e2;             // Fehler im 1. Schritt */

    do
    {
      fb_putpixel(xm+dx, ym+dy,col);            // I.   Quadrant
      fb_putpixel(xm-dx, ym+dy,col);            // II.  Quadrant
      fb_putpixel(xm-dx, ym-dy,col);            // III. Quadrant
      fb_putpixel(xm+dx, ym-dy,col);            // IV.  Quadrant

      e2 = 2*err;
      if (e2 <  (2*dx+1)*b2) { dx++; err += (2*dx+1)*b2; }
      if (e2 > -(2*dy-1)*a2) { dy--; err -= (2*dy-1)*a2; }
    } while (dy >= 0);

    while (dx++ < a)                         // fehlerhafter Abbruch bei flachen Ellipsen (b=1)
    {
      fb_putpixel(xm+dx, ym,col);              // -> Spitze der Ellipse vollenden
      fb_putpixel(xm-dx, ym,col);
    }
  }

  /* ----------------------------------------------------------
     CIRCLE

     Zeichnet einen Kreis mit Mittelpunt an der Koordinate xm,ym
     und dem Radius r im Screenspeicher.

     col       0 = loeschen
               1 = setzen
               2 = Pixelpositon im XOR-Modus verknuepfen
     ---------------------------------------------------------- */
  void circle(int x, int y, int r, uint8_t col )
  {
    ellipse(x,y,r,r,col);
  }

  /* ----------------------------------------------------------
     fastxline

     zeichnet eine Linie in X-Achse mit den X Punkten
     x1 und x2 auf der Y-Achse y1

     col       0 = loeschen
               1 = setzen
               2 = Pixelpositon im XOR-Modus verknuepfen

     ---------------------------------------------------------- */

  void fastxline(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t col)
  {
    uint8_t x;

    if (x2< x1) { x= x1; x1= x2; x= x2= x; }

    for (x= x1; x< (x2+1); x++)
    {
      fb_putpixel(x,y1, col);
    }
  }

  #if (fillrect_enable == 1)
    /* ----------------------------------------------------------
       fillrect

       zeichnet ein ausgefuelltes Rechteck mit den
       Koordinatenpaaren x1/y1 (linke obere Ecke) und
       x2/y2 (rechte untere Ecke);

       col       0 = loeschen
                 1 = setzen
                 2 = Pixelpositon im XOR-Modus verknuepfen

       ---------------------------------------------------------- */

    void fillrect(int x1, int y1, int x2, int y2, uint8_t col)
    {
      int y;

      if (y1> y2)
      {
        y= y1;
        y1= y2;
        y2= y;
      }

      for (y= y1; y< y2+1; y++)
      {
        fastxline(x1,y,x2, col);
      }
    }
  #endif


  #if (fillellipse_enable == 1)
    /* -------------------------------------------------------------
       fillellipse

       Zeichnet eine ausgefuellte Ellipse mit Mittelpunt an der
       Koordinate xm,ym mit den Hoehen- Breitenverhaeltnis a:b
       mit der angegebenen Farbe

       Parameter:
          xm,ym = Koordinate des Mittelpunktes der Ellipse
          a,b   = Hoehen- Breitenverhaeltnis

          col       0 = loeschen
                    1 = setzen
                    2 = Pixelpositon im XOR-Modus verknuepfen


       Ellipsenalgorithmus nach Bresenham (www.wikipedia.org)
       ------------------------------------------------------------- */
    void fillellipse(int xm, int ym, int a, int b, uint8_t col )
    {
      // Algorithmus nach Bresenham (www.wikipedia.org)

      int dx = 0, dy = b;                       // im I. Quadranten von links oben nach rechts unten
      long a2 = a*a, b2 = b*b;
      long err = b2-(2*b-1)*a2, e2;             // Fehler im 1. Schritt */

      do
      {
        fastxline(xm+dx, ym+dy,xm-dx, col);            // I. und II.   Quadrant
        fastxline(xm-dx, ym-dy,xm+dx, col);            // III. und IV. Quadrant

        e2 = 2*err;
        if (e2 <  (2*dx+1)*b2) { dx++; err += (2*dx+1)*b2; }
        if (e2 > -(2*dy-1)*a2) { dy--; err -= (2*dy-1)*a2; }
      } while (dy >= 0);

      while (dx++ < a)                        // fehlerhafter Abbruch bei flachen Ellipsen (b=1)
      {
        fb_putpixel(xm+dx, ym,col);             // -> Spitze der Ellipse vollenden
        fb_putpixel(xm-dx, ym,col);
      }
    }

    /* -------------------------------------------------------------
       fillcircle

       Zeichnet einen ausgefuellten Kreis mit Mittelpunt an der
       Koordinate xm,ym und dem Radius r mit der angegebenen Farbe

       Parameter:
          xm,ym = Koordinate des Mittelpunktes der Ellipse
          r     = Radius des Kreises

          col       0 = loeschen
                    1 = setzen
                    2 = Pixelpositon im XOR-Modus verknuepfen
       ------------------------------------------------------------- */
    void fillcircle(int x, int y, int r, uint8_t col )
    {
      fillellipse(x,y,r,r,col);
    }
  #endif

  /* --------------------------------------------------------
     fb_init

     initalisiert einen Framebufferspeicher.

     x = Framebuffergroesse in x Richtung
     y = Framebuffergroesse in y Richtung
         Aufloesung der y-Pixel muss durch 8 geteilt werden

     Bsp. fuer einen Framebuffer der Pixelgroesse 84x48

                  fb_init(84, 6);

     benoetigt als Framebufferspeicher (84*6)+2 = 506 Bytes
     -------------------------------------------------------- */
  void fb_init(uint8_t x, uint8_t y)
  {
    vram[0]= x;
    vram[1]= y;
  }

  /* --------------------------------------------------------
     fb_clear

     loescht den Framebufferspeicher
     -------------------------------------------------------- */
  void fb_clear(void)
  {
    uint16_t i;

    for (i= 2; i< fb_size; i++) vram[i]= 0x0;
  }


  #if (putcharxy_enable == 1)
    /* --------------------------------------------------------
                             fb_putcharxy

       gibt ein Zeichen auf dem Framebuffer aus
       -------------------------------------------------------- */
    void fb_putcharxy(uint8_t x, uint8_t y, uint8_t ch)
    {
      uint8_t xo, yo;
      uint8_t rb, rt;

      if (textsize < 2)
      {
        for (xo= 0; xo < fontsizex; xo++)
        {
          if (fontnr) rb= font8x8[ch-32][xo];
                 else rb= font5x7[ch-32][xo];
          if ((xo== 5) && (fontsizex== 6)) rb= 0;

          if (invchar) {rb= ~rb;}

          for (yo= 0; yo < 8; yo++)
          {
            if (rb & 0x01) fb_putpixel(x+(xo*(textsize+1)), y+yo, 1);
                      else fb_putpixel(x+(xo*(textsize+1)), y+yo, 0);
            if (textsize)
            {
              if (rb & 0x01) fb_putpixel(x+(xo*(textsize+1)+1), y+yo, 1);
                        else fb_putpixel(x+(xo*(textsize+1)+1), y+yo, 0);
            }
            rb= rb >> 1;
          }
        }
      }

      if (textsize == 2)
      {
        for (xo= 0; xo < fontsizex; xo++)
        {
          if (fontnr) rb= font8x8[ch-32][xo];
                 else rb= font5x7[ch-32][xo];

          if (invchar) {rb= ~rb;}
          rt= rb;
          rb= doublebits(rb, 0);

          for (yo= 0; yo < 8; yo++)
          {
            if (rb & 0x01)
            {
              fb_putpixel(x+(xo*2), y+yo, 1);
              fb_putpixel(x+(xo*2)+1, y+yo, 1);
            }
            else
            {
              fb_putpixel(x+(xo*2), y+yo, 0);
              fb_putpixel(x+(xo*2)+1, y+yo, 0);
            }
            rb= rb >> 1;
          }

          rb= doublebits(rt, 1);

          for (yo= 0; yo < 8; yo++)
          {
            if (rb & 0x01)
            {
              fb_putpixel(x+(xo*2), y+yo+8, 1);
              fb_putpixel(x+(xo*2)+1, y+yo+8, 1);
            }
            else
            {
              fb_putpixel(x+(xo*2), y+yo+8, 0);
              fb_putpixel(x+(xo*2)+1, y+yo+8, 0);
            }
            rb= rb >> 1;
          }
        }
      }
    }

    void fb_outtextxy(uint8_t x, uint8_t y, uint8_t dir, char *p)
    {
      while (*p)
      {
        fb_putcharxy(x,y, *p );
        x += fontsizex;
        p++;
      }
    }

  #endif


#endif                  // Framebuffer Funktionen


/* ------------------------------------------------
     Scriftstil Bitmaps fuer 5x7 und 8x8 Pixel
     grosse Zeichensaetze
  -------------------------------------------------- */

#if (font5x7_enable == 1)

  /* Bitmaps des Ascii-Zeichensatzes
     ein Smily wuerde so aussehen:
        { 0x36, 0x46, 0x40, 0x46, 0x36 }  // Smiley

     ein grosses R ist folgendermassen definiert:

     { 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R

     . x x x x x x x
     . . . . x . . x
     . . . x x . . x
     . . x . x . . x
     . x . . . x x .

  */

  const uint8_t font5x7[][5] =
  {
    { 0x00, 0x00, 0x00, 0x00, 0x00 },   // space
    { 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
    { 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
    { 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
    { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
    { 0xc4, 0xc8, 0x10, 0x26, 0x46 },   // %
    { 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
    { 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
    { 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
    { 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
    { 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
    { 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
    { 0x00, 0x00, 0x50, 0x30, 0x00 },   // ,
    { 0x10, 0x10, 0x10, 0x10, 0x10 },   // -
    { 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
    { 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
    { 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
    { 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
    { 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
    { 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
    { 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
    { 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
    { 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
    { 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
    { 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
    { 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
    { 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
    { 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
    { 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
    { 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
    { 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
    { 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
    { 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
    { 0x7E, 0x11, 0x11, 0x11, 0x7E },   // A
    { 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
    { 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
    { 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
    { 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
    { 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
    { 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
    { 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
    { 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
    { 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
    { 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
    { 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
    { 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
    { 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
    { 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
    { 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
    { 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
    { 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
    { 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
    { 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
    { 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
    { 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
    { 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
    { 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
    { 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
    { 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
    { 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
    { 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // "Yen"
    { 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
    { 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
    { 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
    { 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
    { 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
    { 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
    { 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
    { 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
    { 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
    { 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
    { 0x0C, 0x52, 0x52, 0x52, 0x3E },   // g
    { 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
    { 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
    { 0x20, 0x40, 0x44, 0x3D, 0x00 },   // j
    { 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
    { 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
    { 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
    { 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
    { 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
    { 0x7C, 0x14, 0x14, 0x14, 0x08 },   // p
    { 0x08, 0x14, 0x14, 0x18, 0x7C },   // q
    { 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
    { 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
    { 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
    { 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
    { 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
    { 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
    { 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
    { 0x0C, 0x50, 0x50, 0x50, 0x3C },   // y
    { 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z
    // Zeichen vom Ascii-Satz abweichend
    { 0x3E, 0x7F, 0x7F, 0x3E, 0x00 },   // Zeichen 123 : ausgefuelltes Oval
    { 0x06, 0x09, 0x09, 0x06, 0x00 },   // Zeichen 124 : hochgestelltes kleines o (fuer Gradzeichen);
    { 0x01, 0x01, 0x01, 0x01, 0x01 },   // Zeichen 125 : Strich in der obersten Reihe
    { 0x00, 0x1D, 0x15, 0x17, 0x00 }    // Zeichen 126 : "hoch 2"
  };
#else
  // Dummy-Array

  const uint8_t font5x7[][5] = {{ 0 }};

#endif


#if (font8x8_enable == 1)

  /* Bitmaps des Ascii-Zeichensatzes

     ein grosses P ist folgendermassen definiert:

     { 0x7f, 0x7f, 0x09, 0x09, 0x09, 0x0f, 0x06, 0x00 },    // Ascii 80 = 'P'

       x x x x x x x
       x x x x x x x
             x     x
             x     x
             x     x
             x x x x
               x x
  */

  const uint8_t font8x8[][8] =
  {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },    // Ascii 32 = ' '
    { 0x00, 0x00, 0x00, 0x5f, 0x5f, 0x00, 0x00, 0x00 },    // Ascii 33 = '!'
    { 0x00, 0x07, 0x07, 0x00, 0x07, 0x07, 0x00, 0x00 },    // Ascii 34 = '"'
    { 0x14, 0x7f, 0x7f, 0x14, 0x7f, 0x7f, 0x14, 0x00 },    // Ascii 35 = '#'
    { 0x00, 0x24, 0x2a, 0x7f, 0x7f, 0x2a, 0x12, 0x00 },    // Ascii 36 = '$'
    { 0x00, 0x46, 0x66, 0x30, 0x18, 0x0c, 0x66, 0x62 },    // Ascii 37 = '%'
    { 0x00, 0x30, 0x7a, 0x4f, 0x5d, 0x37, 0x7a, 0x48 },    // Ascii 38 = '&'
    { 0x00, 0x00, 0x04, 0x07, 0x03, 0x00, 0x00, 0x00 },    // Ascii 39 = '''
    { 0x00, 0x00, 0x1c, 0x3e, 0x63, 0x41, 0x00, 0x00 },    // Ascii 40 = '('
    { 0x00, 0x00, 0x41, 0x63, 0x3e, 0x1c, 0x00, 0x00 },    // Ascii 41 = ')'
    { 0x08, 0x2a, 0x3e, 0x1c, 0x1c, 0x3e, 0x2a, 0x08 },    // Ascii 42 = '*'
    { 0x00, 0x08, 0x08, 0x3e, 0x3e, 0x08, 0x08, 0x00 },    // Ascii 43 = '+'
    { 0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00 },    // Ascii 44 = ','
    { 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00 },    // Ascii 45 = '-'
    { 0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00 },    // Ascii 46 = '.'
    { 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x01, 0x00 },    // Ascii 47 = '/'
    { 0x3e, 0x7f, 0x51, 0x49, 0x45, 0x7f, 0x3e, 0x00 },    // Ascii 48 = '0'
    { 0x00, 0x00, 0x02, 0x7f, 0x7f, 0x00, 0x00, 0x00 },    // Ascii 49 = '1'
    { 0x00, 0x42, 0x63, 0x71, 0x59, 0x4f, 0x46, 0x00 },    // Ascii 50 = '2'
    { 0x00, 0x22, 0x63, 0x49, 0x49, 0x7f, 0x36, 0x00 },    // Ascii 51 = '3'
    { 0x18, 0x1c, 0x16, 0x13, 0x7f, 0x7f, 0x10, 0x00 },    // Ascii 52 = '4'
    { 0x00, 0x27, 0x67, 0x45, 0x45, 0x7d, 0x39, 0x00 },    // Ascii 53 = '5'
    { 0x00, 0x3e, 0x7f, 0x49, 0x49, 0x7b, 0x32, 0x00 },    // Ascii 54 = '6'
    { 0x00, 0x01, 0x01, 0x71, 0x79, 0x0f, 0x07, 0x00 },    // Ascii 55 = '7'
    { 0x00, 0x36, 0x7f, 0x49, 0x49, 0x7f, 0x36, 0x00 },    // Ascii 56 = '8'
    { 0x00, 0x06, 0x4f, 0x69, 0x39, 0x1f, 0x0e, 0x00 },    // Ascii 57 = '9'
    { 0x00, 0x00, 0x00, 0x6c, 0x6c, 0x00, 0x00, 0x00 },    // Ascii 58 = ':'
    { 0x00, 0x00, 0x01, 0x6d, 0x6c, 0x00, 0x00, 0x00 },    // Ascii 59 = ';'
    { 0x00, 0x08, 0x1c, 0x36, 0x63, 0x41, 0x00, 0x00 },    // Ascii 60 = '<'
    { 0x00, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x00 },    // Ascii 61 = '='
    { 0x00, 0x41, 0x63, 0x36, 0x1c, 0x08, 0x00, 0x00 },    // Ascii 62 = '>'
    { 0x00, 0x02, 0x03, 0x51, 0x59, 0x0f, 0x06, 0x00 },    // Ascii 63 = '?'
    { 0x3e, 0x7f, 0x41, 0x5d, 0x5d, 0x5f, 0x1e, 0x00 },    // Ascii 64 = '@'
    { 0x7c, 0x7e, 0x13, 0x11, 0x13, 0x7e, 0x7c, 0x00 },    // Ascii 65 = 'A'
    { 0x7f, 0x7f, 0x49, 0x49, 0x49, 0x7f, 0x36, 0x00 },    // Ascii 66 = 'B'
    { 0x1c, 0x3e, 0x63, 0x41, 0x41, 0x63, 0x22, 0x00 },    // Ascii 67 = 'C'
    { 0x7f, 0x7f, 0x41, 0x41, 0x63, 0x3e, 0x1c, 0x00 },    // Ascii 68 = 'D'
    { 0x7f, 0x7f, 0x49, 0x49, 0x49, 0x41, 0x41, 0x00 },    // Ascii 69 = 'E'
    { 0x7f, 0x7f, 0x09, 0x09, 0x09, 0x01, 0x01, 0x00 },    // Ascii 70 = 'F'
    { 0x1c, 0x3e, 0x63, 0x41, 0x51, 0x73, 0x72, 0x00 },    // Ascii 71 = 'G'
    { 0x7f, 0x7f, 0x08, 0x08, 0x08, 0x7f, 0x7f, 0x00 },    // Ascii 72 = 'H'
    { 0x00, 0x41, 0x7f, 0x7f, 0x41, 0x00, 0x00, 0x00 },    // Ascii 73 = 'I'
    { 0x30, 0x70, 0x40, 0x40, 0x40, 0x7f, 0x3f, 0x00 },    // Ascii 74 = 'J'
    { 0x7f, 0x7f, 0x09, 0x1d, 0x36, 0x63, 0x41, 0x00 },    // Ascii 75 = 'K'
    { 0x7f, 0x7f, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00 },    // Ascii 76 = 'L'
    { 0x7f, 0x7f, 0x06, 0x0c, 0x06, 0x7f, 0x7f, 0x00 },    // Ascii 77 = 'M'
    { 0x7f, 0x7f, 0x06, 0x0c, 0x18, 0x7f, 0x7f, 0x00 },    // Ascii 78 = 'N'
    { 0x1c, 0x3e, 0x63, 0x41, 0x63, 0x3e, 0x1c, 0x00 },    // Ascii 79 = 'O'
    { 0x7f, 0x7f, 0x09, 0x09, 0x09, 0x0f, 0x06, 0x00 },    // Ascii 80 = 'P'
    { 0x1c, 0x3e, 0x63, 0x51, 0x33, 0x6e, 0x5c, 0x00 },    // Ascii 81 = 'Q'
    { 0x7f, 0x7f, 0x09, 0x09, 0x19, 0x7f, 0x66, 0x00 },    // Ascii 82 = 'R'
    { 0x26, 0x6f, 0x49, 0x49, 0x49, 0x7b, 0x32, 0x00 },    // Ascii 83 = 'S'
    { 0x01, 0x01, 0x7f, 0x7f, 0x01, 0x01, 0x00, 0x00 },    // Ascii 84 = 'T'
    { 0x3f, 0x7f, 0x40, 0x40, 0x40, 0x7f, 0x3f, 0x00 },    // Ascii 85 = 'U'
    { 0x1f, 0x3f, 0x60, 0x40, 0x60, 0x3f, 0x1f, 0x00 },    // Ascii 86 = 'V'
    { 0x7f, 0x7f, 0x30, 0x18, 0x30, 0x7f, 0x7f, 0x00 },    // Ascii 87 = 'W'
    { 0x63, 0x77, 0x1c, 0x08, 0x1c, 0x77, 0x63, 0x00 },    // Ascii 88 = 'X'
    { 0x00, 0x07, 0x0f, 0x78, 0x78, 0x0f, 0x07, 0x00 },    // Ascii 89 = 'Y'
    { 0x41, 0x61, 0x71, 0x59, 0x4d, 0x47, 0x43, 0x00 },    // Ascii 90 = 'Z'
    { 0x00, 0x00, 0x7f, 0x7f, 0x41, 0x41, 0x00, 0x00 },    // Ascii 91 = '['
    { 0x01, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x00 },    // Ascii 92 = '\'
    { 0x00, 0x00, 0x41, 0x41, 0x7f, 0x7f, 0x00, 0x00 },    // Ascii 93 = ']'
    { 0x00, 0x08, 0x0c, 0x06, 0x03, 0x06, 0x0c, 0x08 },    // Ascii 94 = '^'
    { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 },    // Ascii 95 = '_'
    { 0x00, 0x00, 0x01, 0x03, 0x06, 0x04, 0x00, 0x00 },    // Ascii 96 = '`'
    { 0x20, 0x74, 0x54, 0x54, 0x54, 0x7c, 0x78, 0x00 },    // Ascii 97 = 'a'
    { 0x7f, 0x7f, 0x44, 0x44, 0x44, 0x7c, 0x38, 0x00 },    // Ascii 98 = 'b'
    { 0x38, 0x7c, 0x44, 0x44, 0x44, 0x6c, 0x28, 0x00 },    // Ascii 99 = 'c'
    { 0x38, 0x7c, 0x44, 0x44, 0x44, 0x7f, 0x7f, 0x00 },    // Ascii 100 = 'd'
    { 0x38, 0x7c, 0x54, 0x54, 0x54, 0x5c, 0x18, 0x00 },    // Ascii 101 = 'e'
    { 0x08, 0x7e, 0x7f, 0x09, 0x09, 0x03, 0x02, 0x00 },    // Ascii 102 = 'f'
    { 0x98, 0xbc, 0xa4, 0xa4, 0xa4, 0xfc, 0x7c, 0x00 },    // Ascii 103 = 'g'
    { 0x7f, 0x7f, 0x04, 0x04, 0x04, 0x7c, 0x78, 0x00 },    // Ascii 104 = 'h'
    { 0x00, 0x00, 0x00, 0x7d, 0x7d, 0x00, 0x00, 0x00 },    // Ascii 105 = 'i'
    { 0x40, 0xc0, 0x80, 0x80, 0xfd, 0x7d, 0x00, 0x00 },    // Ascii 106 = 'j'
    { 0x7f, 0x7f, 0x10, 0x10, 0x38, 0x6c, 0x44, 0x00 },    // Ascii 107 = 'k'
    { 0x00, 0x00, 0x00, 0x7f, 0x7f, 0x00, 0x00, 0x00 },    // Ascii 108 = 'l'
    { 0x78, 0x7c, 0x0c, 0x18, 0x0c, 0x7c, 0x78, 0x00 },    // Ascii 109 = 'm'
    { 0x7c, 0x7c, 0x04, 0x04, 0x04, 0x7c, 0x78, 0x00 },    // Ascii 110 = 'n'
    { 0x38, 0x7c, 0x44, 0x44, 0x44, 0x7c, 0x38, 0x00 },    // Ascii 111 = 'o'
    { 0xfc, 0xfc, 0x24, 0x24, 0x24, 0x3c, 0x18, 0x00 },    // Ascii 112 = 'p'
    { 0x18, 0x3c, 0x24, 0x24, 0x24, 0xfc, 0xfc, 0x00 },    // Ascii 113 = 'q'
    { 0x7c, 0x7c, 0x04, 0x04, 0x04, 0x0c, 0x08, 0x00 },    // Ascii 114 = 'r'
    { 0x48, 0x5c, 0x54, 0x54, 0x54, 0x74, 0x20, 0x00 },    // Ascii 115 = 's'
    { 0x04, 0x3f, 0x7f, 0x44, 0x44, 0x64, 0x20, 0x00 },    // Ascii 116 = 't'
    { 0x3c, 0x7c, 0x40, 0x40, 0x40, 0x7c, 0x7c, 0x00 },    // Ascii 117 = 'u'
    { 0x1c, 0x3c, 0x60, 0x40, 0x60, 0x3c, 0x1c, 0x00 },    // Ascii 118 = 'v'
    { 0x3c, 0x7c, 0x60, 0x30, 0x60, 0x7c, 0x3c, 0x00 },    // Ascii 119 = 'w'
    { 0x44, 0x6c, 0x38, 0x10, 0x38, 0x6c, 0x44, 0x00 },    // Ascii 120 = 'x'
    { 0x9c, 0xbc, 0xa0, 0xa0, 0xa0, 0xfc, 0x7c, 0x00 },    // Ascii 121 = 'y'
    { 0x00, 0x44, 0x64, 0x74, 0x5c, 0x4c, 0x44, 0x00 },    // Ascii 122 = 'z'
    { 0x00, 0x08, 0x08, 0x3e, 0x77, 0x41, 0x41, 0x00 },    // Ascii 123 = '{'
    { 0x00, 0x00, 0x00, 0x77, 0x77, 0x00, 0x00, 0x00 },    // Ascii 124 = '|'
    { 0x00, 0x41, 0x41, 0x77, 0x3e, 0x08, 0x08, 0x00 },    // Ascii 125 = '}'
    { 0x00, 0x02, 0x01, 0x01, 0x02, 0x02, 0x01, 0x00 },    // Ascii 126 = '~'
    { 0x70, 0x78, 0x4c, 0x46, 0x46, 0x4c, 0x78, 0x70 },    // Ascii 127 = ''

    { 0x3c, 0x42, 0x81, 0x81, 0x81, 0x42, 0x3c, 0x00 },    // Ascii 128 abweichend: rundes o
    { 0x3c, 0x42, 0xbd, 0xbd, 0xbd, 0x42, 0x3c, 0x00 },    // Ascii 129 abweichend: ausgefuelltes o
    { 0x00, 0x00, 0x0e, 0x11, 0x11, 0x11, 0x0e, 0x00 }     // Ascii 130 abweichend: hochgestelltes o

  };
#else
  // Dummy-Array

  const uint8_t font8x8[][8] = {{ 0 }};

#endif


