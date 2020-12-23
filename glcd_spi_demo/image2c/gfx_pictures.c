/* -------------------------------------------------------
                         gfx_pictures.c

     Softwaremodul zum Anzeigen eingebetteter
     Pixelgraphiken.

     Damit die Pixelgrafiken angezeigt werden koennen, muss
     einem Programm eine Funktion

       void putpixel(int x, int y,uint16_t color);

     hinzugelinkt werden, die die Ausgabe eines farbigen
     Punktes im RGB565-Format vornimmt (5 Bit fuer rot,
     6 Bit fuer gruen, 5 Bit fuer blau)

     Verfuegbare Formate sind:

       PCX 256 Farben
       BMP s/w, 4, 16, 256 Farben

     MCU   :  AVR / STM32 / STM8

     25.01.2019  R. Seelig
   ------------------------------------------------------ */

#include "gfx_pictures.h"

/* ----------------------------------------------------------
   bmpsw_show

   Kopiert ein im Flash abgelegtes Bitmap in den Screens-
   peicher. Bitmap muss byteweise in Zeilen gespeichert
   vorliegen Hierbei entspricht 1 Byte 8 Pixel.
   Bsp.: eine Reihe mit 6 Bytes entsprechen 48 Pixel
         in X-Achse

   ox,oy        => linke obere Ecke an der das Bitmap
                   angezeigt wird
   image        => das anzuzeigende Bitmap
   fwert        => Farbwert mit der das Pixel gezeichnet wird

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
   ---------------------------------------------------------- */
void bmpsw_show(uint16_t ox, uint16_t oy, const unsigned char* const image, uint16_t fwert)
{
  int      x,y;
  uint8_t  b,bp;
  uint16_t resX, resY;

  resX= (readarray(image,0) << 8) + readarray(image,1);
  resY= (readarray(image,2) << 8) + readarray(image,3);

  if ((resX % 8) == 0) { resX= resX / 8; }
                 else  { resX= (resX / 8)+1; }

  for (y=0;y< resY;y++)
  {
    for (x= 0;x<resX;x++)
    {
      b= readarray(image, y *resX + x + 4);
      for (bp=8; bp>0; bp--)
      {
        if (b & (1 << (bp-1))) {putpixel(ox+(x*8)+8-bp, oy+y-4,fwert);}
      }
    }
  }
}

/* --------------------------------------------------------
                      bmpcga_show

     zeigt ein in einem Array abgelegtes Bitmap an. Die
     verwendete Farbpalette muss im RGB565 - Format
     codiert sein.

        *image     : Zeiger auf das Bytearray, dass die
                     Vierfarbgrafik enthaellt
        ox,oy      : linke obere Ecke, ab der die
                     Grafik angezeigt werden soll
        *pal       : Zeiger auf die zur Grafik ge-
                     hoerende Farbpalette

   -------------------------------------------------------- */
void bmpcga_show(int ox, int oy, const uint8_t* const image, const uint16_t* const pal)
{
  int16_t  x, y;
  uint16_t  width, height;
  uint16_t  ptr;
  uint8_t   pixpos, fb, cvalue;
  uint16_t  cgacolor;


  width= (readarray(image,0) << 8) + readarray(image,1);
  height= (readarray(image, 2) << 8) + readarray(image,3);

  ptr= 4;
  y= height;

  for (y= 0; y < height; y++)
  {
    for (x= 0; x < width; x++)
    {
      if ((x % 4)== 0)
      {
        fb= readarray(image,ptr);
        ptr++;
      }
      pixpos=  (3-(x % 4))*2;

      cvalue= (fb >> pixpos) & 0x03;
      switch (cvalue)
      {
        case 00 : cgacolor= *pal;  break;
        case 01 : cgacolor= *(pal+1);  break;
        case 02 : cgacolor= *(pal+2);  break;
        case 03 : cgacolor= *(pal+3);  break;
        default : break;
      }
      putpixel(x+ox, y+oy-1, cgacolor);
    }
  }
}

/* --------------------------------------------------------
     bmp16_show

     zeigt ein in einem Array liegende 16-Farben BMP-Datei
     ab den Koordinaten  x / y (linke obere Ecke)
     auf dem Bildschirm an. BMP-Grafik muss zwingend
     eine 16 Farben Grafik beinhalten.

        *image     : Zeiger auf das Bytearray, dass die
                     PCX-Grafik enthaellt
        ox,oy      : linke obere Ecke, ab der die
                     BMP-Grafik angezeigt werden soll
        *palette   : Zeiger auf die zur Grafik ge-
                     hoerende Farbpalette (rgb565)

   -------------------------------------------------------- */
void bmp16_show(int16_t ox, int16_t oy, const uint8_t* const image, const uint16_t* const palette)
{

  int16_t x,y,f;
  int16_t width, height;
  uint16_t ptr;


  width= (readarray(image,0) << 8) + readarray(image,1);
  height= (readarray(image,2) << 8) + readarray(image, 3);

  ptr= 4;
  y= height;

  while(y)
  {
    for (x= 0; x< width; x++)
    {
      if (x & 1)                          // bei 2 Farbpixel / Byte nur bei jedem zweiten Byte
                                          // den Zeiger auf naechstes Bilddatenbyte erhoehen
      {
        f= readarray(image, ptr) & 0x0f;
        ptr++;
      }
      else
      {
        f= (readarray(image,ptr) >> 4) & 0x0f;
      }
      putpixel(ox+x,oy+y-1, readwarray(palette,f) );
    }
    y--;
  }
}

/* --------------------------------------------------------
     bmp256_show

     zeigt ein in einem Array liegende BMP-Grafik
     ab den Koordinaten  x / y (linke obere Ecke)
     auf dem Bildschirm an. PCX-Grafik muss zwingend
     eine 256 Farben Grafik beinhalten.

        *image     : Zeiger auf das Bytearray, dass die
                     PCX-Grafik enthaellt
        ox,oy      : linke obere Ecke, ab der die
                     PCX-Grafik angezeigt werden soll
        *palette   : Zeiger auf die zur Grafik ge-
                     hoerende Farbpalette
   -------------------------------------------------------- */
void bmp256_show(uint8_t ox, uint8_t oy, const uint8_t* const image, const uint16_t* const palette)
{

  uint16_t x,y,f;
  uint16_t width, height;
  uint16_t ptr;

  width= (readarray(image,0) << 8) + readarray(image,1);
  height= (readarray(image,2) << 8) + readarray(image, 3);

  ptr= 4;
  y= height;
  while(y)
  {
    for (x= 0; x< width; x++)
    {
      f= readarray(image, ptr);
      putpixel(ox+x,oy+y, readwarray(palette,f));
      ptr++;
    }
    y--;
  }
}


/* --------------------------------------------------------
     pcx256_show

     zeigt ein in einem Array liegende PCX-Grafik
     ab den Koordinaten  x / y (linke obere Ecke)
     auf dem Bildschirm an. PCX-Grafik muss zwingend
     eine 256 Farben Grafik beinhalten.

        *image     : Zeiger auf das Bytearray, dass die
                     PCX-Grafik enthaellt
        x,y        : linke obere Ecke, ab der die
                     PCX-Grafik angezeigt werden soll
        *pal       : Zeiger auf die zur Grafik ge-
                     hoerende Farbpalette
   -------------------------------------------------------- */
void pcx256_show(int16_t x, int16_t y, const unsigned char* const image, const uint16_t *const pal)
{
  #define  pcx(nr)         ( readarray(image,nr) )
  #define  pcxw(nr)        ( readwarray(image,nr) )

  int32_t  pos, c, w, h, e, pack;
  int32_t  c2;
  uint16_t f;

  if ((pcx(0) != 10) | (pcx(3) != 8))                  // Identity Bytes abfragen
  {
                                                       // "Datei ist ein nicht darstellbares Format !!"
    return;                                            // Function mit Fehlercode beenden
  }

                                                       // Bildformat berechnen,
                                                       // w = Pixel in X-Achse / h = Pixel in Y-Achse
  w= ((pcx(9) - pcx(5))*256 + pcx(8) - pcx(4))+1;
  h= ((pcx(11) - pcx(7))*256 + pcx(10) - pcx(6))+1;

  pack= 0; c= 0; e= y+h;

  pos= 128;
  while (y <e)
  {
    if (pack != 0)
    {
      for (c2= 0; c2< (pack); c2++)
      {
          f= readwarray(pal, pcx(pos));                 // Farbe aus Palettenarray holen

          putpixel(x+c,y,f);

        if (c== w)
        {
          c= 0;
          y++;
        }
        else
        {
          c++;
        }
      }
      pack= 0;
    }
    else
    {
      if ((pcx(pos) & 0xc0)== 0xc0)
      {
        pack= pcx(pos) & 0x3f;
      }
      else
      {

        f= readwarray(pal, pcx(pos));                 // Farbe aus Palettenarray holen

        putpixel(x+c,y,f);

        c++;
      }
    }
    pos++;
    if (c== w)
    {
      c= 0;
      y++;
    }
  }
}

