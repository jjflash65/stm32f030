/* -----------------------------------------------------------
                          image2c.c

     Erzeugt aus einer binaeren Grafikdateien regulaere
     C-Arrays.

     Unterstuetzte Grafikformate sind:

        PCX - 256 Farben
        BMP - 256 Farben
        BMP -  16 Farben
        BMP -   2 Farben

      Besonderes Format:
        ASCII

        Ascii ist ein mittels MTPAINT auf 4 Farben (CGA)
        reduziertes Bitmap, das mittels MTPAINT als
        Ascii-Grafik exportiert wurde. Somit belegt ein
        Pixel 2 Bits => ein einem Byte sind somit 4 Pixel
        gespeichert und reduziert den Speicherplatzbedarf
        enorm.

     Uebersetzen mit:

     gcc image2c.c -o image2c

     17.01.2019     R. Seelig
   --------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>


/* ----------------------------------------------------------
   fileexists

   stellt fest, ob die angegebene Datei datnam existiert.

   Rueckgabe:

           0 : Datei existiert nicht
           1 : Datei existiert
   ---------------------------------------------------------- */
int fileexists(const char *datnam)
{
  struct stat buffer;
  int exist = stat(datnam, &buffer);

  if (exist) return 0; else return 1;
}

/* ----------------------------------------------------------
   fgetint32

   liest einen 32-Bit Integer aus dem angegebenen Filehandle
   ---------------------------------------------------------- */
uint32_t fgetint32(FILE *binfile)
{

  uint32_t     datvalue;

  datvalue= fgetc(binfile);
  datvalue |= (fgetc(binfile)<< 8);
  datvalue |= (fgetc(binfile)<< 16);
  datvalue |= (fgetc(binfile)<< 24);
  return datvalue;
}

/* ----------------------------------------------------------
   fread32

   liest einen 32-Bit Integer aus dem angegebenen Filehandle
   an der angegebenen Position bytepos
   ---------------------------------------------------------- */
uint32_t fread32(FILE *binfile, uint32_t bytepos)
{
  fseek(binfile,bytepos,SEEK_SET);                       // Dateizeiger auf Byteposition in der Datei setzen
  return fgetint32(binfile);
}

/* ----------------------------------------------------------
   fread16

   liest einen 16-Bit Integer aus dem angegebenen Filehandle
   an der angegebenen Position bytepos
   ---------------------------------------------------------- */
uint16_t fread16(FILE *binfile, uint32_t bytepos)
{
  fseek(binfile,bytepos,SEEK_SET);                       // Dateizeiger auf Byteposition in der Datei setzen
  return fgetint32(binfile);
}


/* ----------------------------------------------------------
   getdatlen

   gibt fuer die im String "datnam" angegebene Datei die
   Dateigroesse in Bytes zurueck
   ---------------------------------------------------------- */
long int getdatlen(unsigned char* datnam)
{
  FILE *myfile;
  long int datlen;

  myfile = fopen(datnam, "r");
  fseek(myfile ,0,SEEK_END);                        // Position auf das Ende der Datei setzen
  datlen= ftell(myfile);                            // Position innerhalb der Datei holen;
  fseek(myfile,0, SEEK_SET);
  fclose(myfile);
  return datlen;
}

/* ----------------------------------------------------------
   rgbfromvalue

   Setzt einen 16-Bitfarbwert aus 3 einzelnen Farbwerten
   fuer (r)ot, (g)ruen und (b)lau zusammen.

     r,g,b  : 8-Bit Farbwerte fuer rot, gruen, blau. Aus
              diesen wird ein 16 Bit (RGB565) Farbwert
              generiert und dieser als Funktionsergebnis
              zurueck geliefert
   ---------------------------------------------------------- */
uint16_t rgbfromvalue(uint8_t r, uint8_t g, uint8_t b)
{
  uint16_t value;

  r= r >> 3;
  g= g >> 2;
  b= b >> 3;
  value= b;
  value |= (g << 5);
  value |= (r << 11);
  return value;
}


/* ----------------------------------------------------------
   pcx256_convert

   Konvertiert eine 256 Farben PCX-Datei in ein regulaeres
   C-Array.

   Uebergabe:

       *inputfile  : Zeiger auf Dateinamensstring der
                     PCX-Datei
       *outputfile : Die anzulegende Datei, in der das
                     C-Array gespeichert wird.
       convdata    : 1 = Bilddaten werden als Array
                     generiert
                     0 = Farbpalette wird als Array
                     generiert
       avrstyle    : 0 = Standard C-Array
                     1 = Array wird fuer einen AVR-Controller
                     angelegt (PROGMEM)
   ---------------------------------------------------------- */
int pcx256_convert(char *inputfile, char *outputfile, char convdata, char avrstyle)
{

  #define bufsize    256000
  #define byprow     16
  #define worprow    8

  long int       filesize;
  FILE           *binfile;
  FILE           *cfile;

  unsigned char  buffer[bufsize];

  int            anz, ganz;
  int            i, cnt;
  uint8_t        r, g, b, vh, vl;
  uint16_t       rgb565;
  char           ch;

  if (!(fileexists(inputfile)))
  {
    printf("\nError: file %s not found...\n\n", inputfile);
    return 1;
  }
  cfile= fopen(outputfile, "w");
  fprintf(cfile,"\n//Array generated with IMAGE2C by R. Seelig\n");

  filesize= getdatlen(inputfile);
  binfile= fopen(inputfile, "r+b");                // Datei zum Lesen oeffnen
  fseek(binfile,0,SEEK_SET);                       // Dateizeiger auf Anfang der Datei setzen
  cnt= 0; ganz= 0;
  anz= fread(buffer, 1, bufsize, binfile);
  fclose(binfile);
  if (convdata)                                    // Bilddaten konvertieren
  {
    fprintf(cfile,"\n#define pcximagebytes        %u\n\n",filesize-768);
    if (avrstyle)
      fprintf(cfile,"static const unsigned char pcximage[%u] PROGMEM = {\n",filesize-768);
    else
      fprintf(cfile,"static const unsigned char pcximage[%u] = {\n",filesize-768);
    for (i= 0; i< anz-1-768; i++)
    {
      if ( !(cnt % byprow ))
      {
        fprintf(cfile,"\n  ");
      }
      fprintf(cfile,"0x%.2x, ",buffer[i]);
      cnt++;
    }
    if ( !(cnt % byprow ))
    {
      fprintf(cfile,"\n  ");
    }
    fprintf(cfile,"0x%.2x }; \n\n\n",buffer[anz]);
  }
  else
  {
    if (avrstyle)
      fprintf(cfile,"static const uint16_t pcximagepal[%u] PROGMEM = {\n",256);
    else
      fprintf(cfile,"static const uint16_t pcximagepal[%u] = {\n",256);

    cnt= 0;
    for (i= anz-768; i< anz-3; i += 3)
    {
      if ( !(cnt % worprow ))
      {
        fprintf(cfile,"\n  ");
      }
      r= buffer[i+0]; g= buffer[i+1]; b= buffer[i+2];
      rgb565= rgbfromvalue(r, g, b);
  //    vh= rgb565 >> 8; vl = rgb565 & 0xff;
      fprintf(cfile,"0x%.4x, ", rgb565);
      cnt++;
    }
    r= buffer[anz-1]; g= buffer[anz-2]; b= buffer[anz-3];
    rgb565= rgbfromvalue(r, g, b);
  //  vh= rgb565 >> 8; vl = rgb565 & 0xff;
    fprintf(cfile,"0x%.4x };\n\n", rgb565);
  }
  fclose(cfile);
  return 0;
}

/* ----------------------------------------------------------
   bmp256pal_convert

   Extrahiert die Farbpalette einer 256 Farben BMP-Datei
   und konvertiert diese in ein regulaeres C-Array.

   Uebergabe:

       *inputfile  : Zeiger auf Dateinamensstring der
                     BMP-Datei
       *outputfile : Die anzulegende Datei, in der das
                     C-Array gespeichert wird.
       avrstyle    : 0 = Standard C-Array
                     1 = Array wird fuer einen AVR-Controller
                     angelegt (PROGMEM)
   ---------------------------------------------------------- */
int bmp256pal_convert(char *inputfile, char *outputfile, char avrstyle)
{
  #define palwerte   256

  uint16_t    bmpid;
  uint32_t    filesize;
  uint32_t    bmpbreite;
  uint32_t    bmphoehe;
  uint16_t    bppx;                                 // Bit per Pixel
  uint32_t    bdatptr;                              // Offset an dem die Bilddaten abgelegt sind

  uint32_t    palofs = 0x36;                        // ab Offset 36h faengt Farbpallette an, fuer
                                                    // 256 Farben wird fuer jeden Farbwert ein 4 Byte
                                                    // grosser Farbwert hinterlegt in der Bytereihenfolge
                                                    // blau, gruen, rot, reserviert. 4 Bytes * 256 Farben
                                                    // entspricht einer 1024 Byte grossen Farbpalette.
                                                    // diese Farbpalette wird automatischen in einen
                                                    // 16 Bit RGB565 Farbwert umgerechnet

  uint8_t     r,g,b,resv;
  uint16_t    rgb565;                               // 16 Bit RGB - Farbwert;
  FILE        *binfile;
  FILE        *cfile;

  int            i, cnt;

  if (!(fileexists(inputfile)))
  {
    printf("\nError: file %s not found...\n\n", inputfile);
    return 1;
  }

  cfile= fopen(outputfile, "w");

  binfile= fopen(inputfile, "r+b");                   // Datei zum Lesen oeffnen
  fseek(binfile,0,SEEK_SET);                       // Dateizeiger auf Anfang der Datei setzen

  bmpid= fread16(binfile,0);
  filesize= fread32(binfile,2);
  bmpbreite= fread32(binfile,18);
  bmphoehe= fread32(binfile,22);
  bppx= fread16(binfile,28);
  bdatptr= fread32(binfile,10);

/*

  // nur zu Testzwecken

  printf("\nBMP-ID       : 0x%x = %c%c",bmpid,bmpid & 0xff,bmpid>>8);             // ist die ID NICHT 0x4d42  ("BM") dann ist es keine BMP-Datei
  printf("\nDateigroesse : %d",filesize);
  printf("\nBitmapbreite : %d",bmpbreite);
  printf("\nBitmapbreite : %d",bmphoehe);
  printf("\nBit per Pixel: %d",bppx);
  printf("\nDatenzeiger  : 0x%.4X\n",bdatptr);
*/

  fprintf(cfile,"\n//Array generated with IMAGE2C by R. Seelig\n\n");

  if (avrstyle)
    fprintf(cfile,"static const uint16_t bmp256_pal[] PROGMEM = {\n\n  ");
  else
    fprintf(cfile,"static const uint16_t bmp256_pal[]= {\n\n  ");

  fseek(binfile,palofs,SEEK_SET);                                                 // Dateizeiger auf Anfang Farbpalette setzen

  i= 0;
  for (cnt= 0; cnt< palwerte; cnt++)
  {
    b= fgetc(binfile);
    g= fgetc(binfile);
    r= fgetc(binfile);
    resv= fgetc(binfile);

    rgb565= 0;
    r= r >> 3;
    g= g >> 2;
    b= b >> 3;
    rgb565= b | (g << 5) | (r << 11);

    fprintf(cfile,"0x%.4X",rgb565);
    if (cnt != (palwerte-1))
    {
      fprintf(cfile,", ");
    }
    i++;
    if (i==8)
    {
      i= 0;
      fprintf(cfile,"\n  ");
    }
  }
  fprintf(cfile,"};\n");
  fclose(binfile);
}


/* ----------------------------------------------------------
   bmp256_convert

   Konvertiert eine 256 Farben BMP-Datei in ein regulaeres
   C-Array.

   Hinweis:

   Eine BMP-Datei hat pro Zeile immer eine durch 4 teilbare
   Anzahl Bytes. Hat das Bild eine davon abweichende
   Pixelanzahl sind in der BMP-Datei stattdessen 0-Bytes
   eingefuegt.  Diese Bytes werden von dieser Funktion beim
   Speichern des C-Arrays entfernt.

   Uebergabe:

       *inputfile  : Zeiger auf Dateinamensstring der
                     BMP-Datei
       *outputfile : Die anzulegende Datei, in der das
                     C-Array gespeichert wird.
       avrstyle    : 0 = Standard C-Array
                     1 = Array wird fuer einen AVR-Controller
                     angelegt (PROGMEM)
   ---------------------------------------------------------- */
int bmp256_convert(char *inputfile, char *outputfile, char avrstyle)
{
  #define palwerte   256

  uint16_t    bmpid;
  uint32_t    filesize;
  uint16_t    bmpbreite;
  uint16_t    bmphoehe;
  uint16_t    bppx;                                 // Bit per Pixel
  uint32_t    bdatptr;                              // Offset an dem die Bilddaten abgelegt sind

  uint32_t    palofs = 0x36;                        // ab Offset 36h faengt Farbpallette an, fuer
                                                    // 256 Farben wird fuer jeden Farbwert ein 4 Byte
                                                    // grosser Farbwert hinterlegt in der Bytereihenfolge
                                                    // blau, gruen, rot, reserviert. 4 Bytes * 256 Farben
                                                    // entspricht einer 1024 Byte grossen Farbpalette.
                                                    // diese Farbpalette wird automatischen in einen
                                                    // 16 Bit RGB565 Farbwert umgerechnet

  uint8_t       b;
  uint16_t      rgb565;                            // 16 Bit RGB - Farbwert;
  FILE          *binfile;
  FILE          *cfile;
  unsigned int  i, cnt, xcnt;

  unsigned int  xadd, i2;

  if (!(fileexists(inputfile)))
  {
    printf("\nError: file %s not found...\n\n", inputfile);
    return 1;
  }

  cfile= fopen(outputfile, "w");
  fprintf(cfile,"\n//Array generated with IMAGE2C by R. Seelig\n");

  binfile= fopen(inputfile, "r+b");                // Datei zum Lesen oeffnen
  fseek(binfile,0,SEEK_SET);                       // Dateizeiger auf Anfang der Datei setzen

  bmpid= fread16(binfile,0);
  filesize= fread32(binfile,2);
  bmpbreite= fread32(binfile,18);
  bmphoehe= fread32(binfile,22);
  bppx= fread16(binfile,28);
  bdatptr= fread32(binfile,10);

  fprintf(cfile,"\n\n");

  if (avrstyle)
    fprintf(cfile,"static const unsigned char bmp256_image[] PROGMEM = {\n\n  ");
  else
    fprintf(cfile,"static const unsigned char bmp256_image[]= {\n\n  ");

  fprintf(cfile,"0x%.2X, 0x%.2X, 0x%.2X, 0x%.2X,\n  ",bmpbreite>>8,
                                                      bmpbreite & 0xff,
                                                      bmphoehe>>8,
                                                      bmphoehe & 0xff);

  fseek(binfile,bdatptr,SEEK_SET);                 // Dateizeiger auf Anfang Rasterbilddaten setzen

  i= 0; xcnt= 0;
  xadd = 4-(bmpbreite % 4);

  for (cnt= 0; cnt< (bmpbreite * bmphoehe); cnt++)
  {
    xcnt++;

    b= fgetc(binfile);

    fprintf(cfile,"0x%.2X",b);
    if (cnt != ((bmpbreite * bmphoehe)-1))
    {
      fprintf(cfile,", ");
    }
    i++;
    if (i== 16)
    {
      i= 0;
      fprintf(cfile,"\n  ");
    }

    if (xcnt== bmpbreite)
    {
      if ((xadd>0) && (xadd<4))
      {
        for (i2= 0; i2 != xadd; i2++)
        {
           b= fgetc(binfile);
        }
      }
      xcnt= 0;
    }

  }
  fprintf(cfile,"};\n");

  fclose(binfile);
  fclose(cfile);

  #undef palwerte

}


/* ----------------------------------------------------------
   bmp16_convert

   Konvertiert eine 16 Farben BMP-Datei zu 2 regulaeren
   C-Arrays (je eines fuer die Palette und eines fuer die
   Bilddaten).

   Uebergabe:

       *inputfile  : Zeiger auf Dateinamensstring der
                     BMP-Datei
       *outputfile : Die anzulegende Datei, in der das
                     C-Array gespeichert wird.
       avrstyle    : 0 = Standard C-Array
                     1 = Array wird fuer einen AVR-Controller
                     angelegt (PROGMEM)
   ---------------------------------------------------------- */
int bmp16_convert(char *inputfile, char *outputfile, char avrstyle)
{
  #define palwerte   16

  uint16_t    bmpid;
  uint32_t    filesize;
  uint16_t    bmpbreite;
  uint16_t    bmphoehe;
  uint16_t    bppx;                                 // Bit per Pixel
  uint32_t    bdatptr;                              // Offset an dem die Bilddaten abgelegt sind

  uint32_t    palofs = 0x36;                        // ab Offset 36h faengt Farbpallette an, fuer
                                                    // 16 Farben wird fuer jeden Farbwert ein 4 Byte
                                                    // grosser Farbwert hinterlegt in der Bytereihenfolge
                                                    // blau, gruen, rot, reserviert. 4 Bytes * 16 Farben
                                                    // entspricht einer 64 Byte grossen Farbpalette.
                                                    // diese Farbpalette wird automatischen in einen
                                                    // 16 Bit RGB565 Farbwert umgerechnet

  uint8_t     r,g,b,resv;
  uint16_t    rgb565;                               // 16 Bit RGB - Farbwert;
  FILE        *binfile;
  FILE        *cfile;

  int           i, cnt;
  unsigned int  xadd, i2, xcnt;

  if (!(fileexists(inputfile)))
  {
    printf("\nError: file %s not found...\n\n", inputfile);
    return 1;
  }

  binfile= fopen(inputfile, "r+b");                   // Datei zum Lesen oeffnen
  fseek(binfile,0,SEEK_SET);                       // Dateizeiger auf Anfang der Datei setzen

  bmpid= fread16(binfile,0);
  filesize= fread32(binfile,2);
  bmpbreite= fread32(binfile,18);
  bmphoehe= fread32(binfile,22);
  bppx= fread16(binfile,28);
  bdatptr= fread32(binfile,10);


/*
  // nur zu Testzwecken

  printf("\nBMP-ID       : 0x%x = %c%c",bmpid,bmpid & 0xff,bmpid>>8);             // ist die ID NICHT 0x4d42  ("BM") dann ist es keine BMP-Datei
  printf("\nDateigroesse : %d",filesize);
  printf("\nBitmapbreite : %d",bmpbreite);
  printf("\nBitmapbreite : %d",bmphoehe);
  printf("\nBit per Pixel: %d",bppx);
  printf("\nDatenzeiger  : 0x%.4X\n",bdatptr);
*/

  cfile= fopen(outputfile, "w");
  fprintf(cfile,"\n//Array generated with IMAGE2C by R. Seelig\n\n");

  if (avrstyle)
    fprintf(cfile,"static const uint16_t bmp16_pal[] PROGMEM = {\n\n  ");
  else
    fprintf(cfile,"static const uint16_t bmp16_pal[]= {\n\n  ");

  fseek(binfile,palofs,SEEK_SET);                                                 // Dateizeiger auf Anfang Farbpalette setzen

  i= 0;
  for (cnt= 0; cnt< palwerte; cnt++)
  {
    b= fgetc(binfile);
    g= fgetc(binfile);
    r= fgetc(binfile);
    resv= fgetc(binfile);

    rgb565= 0;
    r= r >> 3;
    g= g >> 2;
    b= b >> 3;
    rgb565= b | (g << 5) | (r << 11);

    fprintf(cfile,"0x%.4X",rgb565);
    if (cnt != (palwerte-1))
    {
      fprintf(cfile,", ");
    }
    i++;
    if (i==8)
    {
      i= 0;
      fprintf(cfile,"\n  ");
    }
  }
  fprintf(cfile,"};\n");
  fclose(binfile);

  binfile= fopen(inputfile, "r+b");                    // Datei zum Lesen oeffnen
  fseek(binfile,0,SEEK_SET);                        // Dateizeiger auf Anfang der Datei setzen

  bmpid= fread16(binfile,0);
  filesize= fread32(binfile,2);
  bmpbreite= fread32(binfile,18);
  bmphoehe= fread32(binfile,22);
  bppx= fread16(binfile,28);
  bdatptr= fread32(binfile,10);

  fprintf(cfile,"\n\n");

  if (avrstyle)
    fprintf(cfile,"static const unsigned char bmp16_image[] PROGMEM = {\n\n  ");
  else
    fprintf(cfile,"static const unsigned char bmp16_image[]= {\n\n  ");

  fprintf(cfile,"0x%.2X, 0x%.2X, 0x%.2X, 0x%.2X,\n  ",bmpbreite>>8,
                                                      bmpbreite & 0xff,
                                                      bmphoehe>>8,
                                                      bmphoehe & 0xff);
  fseek(binfile,bdatptr,SEEK_SET);                 // Dateizeiger auf Anfang Rasterbilddaten setzen

  i= 0; xcnt= 0;
  xadd = 4-(bmpbreite % 4);

  for (cnt= 0; cnt< ((bmpbreite * bmphoehe) / 2); cnt++)
  {
    xcnt++;

    b= fgetc(binfile);

    fprintf(cfile,"0x%.2X",b);
    if (cnt != ((bmpbreite * bmphoehe)-1))
    {
      fprintf(cfile,", ");
    }
    i++;
    if (i== 16)
    {
      i= 0;
      fprintf(cfile,"\n  ");
    }

    if (xcnt== bmpbreite)
    {
      if ((xadd>0) && (xadd<4))
      {
        for (i2= 0; i2 != xadd; i2++)
        {
           b= fgetc(binfile);
        }
      }
      xcnt= 0;
    }

  }
  fprintf(cfile,"};\n");
  fclose(binfile);
  fclose(cfile);

  #undef palwerte

}



/* ----------------------------------------------------------
   bmp2bit_convert

   Konvertiert eine 4 Farben Grafik, die im Ascii - Format
   vorliegt ( entspricht einer von MTPAINT exportierten
   Textdatei) in ein C-Array.

   Uebergabe:

       *inputfile  : Zeiger auf Dateinamensstring der
                     BMP-Datei
       *outputfile : Die anzulegende Datei, in der das
                     C-Array gespeichert wird.
       avrstyle    : 0 = Standard C-Array
                     1 = Array wird fuer einen AVR-Controller
                     angelegt (PROGMEM)
   ---------------------------------------------------------- */
int bmp2bit_convert(char *inputfile, char *outputfile, char avrstyle)
{
  typedef char string[512];

  FILE     *tdat;
  FILE     *cfile;
  string   tx[512];

  uint8_t  bmparray[32768];
  uint16_t yanz;
  uint16_t xanz;
  int      i, x, y, pind;
  uint8_t  pixpos, fb;

  if (!(fileexists(inputfile)))
  {
    printf("\nError: file %s not found...\n\n", inputfile);
    return 1;
  }

  yanz= 0;
  tdat= fopen(inputfile,"r");
  do
  {
    fgets(tx[yanz],255,tdat);
    yanz++;
    }while (!feof(tdat));
  fclose(tdat);
  yanz--;


  xanz= strlen(tx[0])-1;

  pind= -1;
  for (y= 0; y< yanz; y++)
  {
    for (x= 0; x< xanz; x++)
    {
      if (!(x % 4))
      {
        pind++;
        fb= 0;
      }
      pixpos=  (3-(x % 4))*2;

      if (tx[y][x]== '.') { fb |= 1 << pixpos; }
      if (tx[y][x]== ',') { fb |= 2 << pixpos; }
      if (tx[y][x]== ':') { fb |= 3 << pixpos; }

      if ((x % 4)== 3)
      {
        bmparray[pind]= fb;
      }
    }
  }

  cfile= fopen(outputfile, "w");
  fprintf(cfile,"\n//Array generated with IMAGE2C by R. Seelig\n");

  fprintf(cfile,"\n#define bmp2bit_imagebytes        %u\n\n",pind+5);

  if (avrstyle)
    fprintf(cfile,"static const unsigned char bmp2bit_image[%u] PROGMEM = {\n\n  ", pind+5);
  else
    fprintf(cfile,"static const unsigned char bmp2bit_image[%u]= {\n\n  ", pind+5);
  fprintf(cfile,"0x%.2X, 0x%.2X, 0x%.2X, 0x%.2X,\n  ",xanz >> 8,
                                                      xanz & 0xff,
                                                      yanz >> 8,
                                                      yanz & 0xff);
  for (i= 0; i< (pind+1); i++)
  {
    if (((i % 16)== 0) && (i > 0))
    {
      fprintf(cfile,"\n  ");
    }
    if (i != pind)
    {
      fprintf(cfile,"0x%.2x, ",bmparray[i]);
    }
    else
    {
      fprintf(cfile,"0x%.2x ",bmparray[i]);
    }
  }
  fprintf(cfile,"};\n");


  fclose(cfile);

}


/* ----------------------------------------------------------
   bmpsw_convert

   Konvertiert eine s/w BMP-Datei in ein regulaeres
   C-Array.

   Uebergabe:

       *inputfile  : Zeiger auf Dateinamensstring der
                     BMP-Datei
       *outputfile : Die anzulegende Datei, in der das
                     C-Array gespeichert wird.
       avrstyle    : 0 = Standard C-Array
                     1 = Array wird fuer einen AVR-Controller
                     angelegt (PROGMEM)
   ---------------------------------------------------------- */
int bmpsw_convert(char *inputfile, char *outputfile, char avrstyle)
{

  uint16_t       bmpid;
  uint32_t       filesize;
  uint32_t       bmpbreite;
  uint32_t       bmphoehe;
  uint16_t       bppx;                                 // Bit per Pixel
  uint32_t       bdatptr;                              // Offset an dem die Bilddaten abgelegt sind

  FILE           *binfile;
  FILE           *cfile;

  unsigned char  bmpdat[0x7fff];

  uint8_t        pixbytex, dbyte;
  int            x, xw, xanz, y, datanz;
  int            banz;
  int            i, cnt, icnt, b, ch;
  int            rcnt;

  if (!(fileexists(inputfile)))
  {
    printf("\nError: file %s not found...\n\n", inputfile);
    return 1;
  }

  binfile= fopen(inputfile, "r+b");                   // Datei zum Lesen oeffnen
  fseek(binfile,0,SEEK_SET);                       // Dateizeiger auf Anfang der Datei setzen

  bmpid= fread16(binfile,0);
  filesize= fread32(binfile,2);
  bmpbreite= fread32(binfile,18);
  bmphoehe= fread32(binfile,22);
  bppx= fread16(binfile,28);
  bdatptr= fread32(binfile,10);


  fseek(binfile,bdatptr,SEEK_SET);                                                 // Dateizeiger auf Anfang Farbpalette setzen
  xanz= bmpbreite / 32;
  pixbytex= bmpbreite / 8;
  if (bmpbreite % 32) { xanz++; }                                                  // wenn X Aufloesung kein Vielfaches von 32 ist
                                                                                   // (BMP-Datei hat eine Speicherblock von 4 Bytes)
  if (bmpbreite % 8) { pixbytex++; }                                               // wenn X Aufloesung kein Vielfaches von 8 ist
                                                                                   // fuer die zu erstellende Headerdatei werden
                                                                                   // volle Bytes per Linie erzeugt, der Rest eines
                                                                                   // Bytes ist mit Nullen aufgefuellt
  xanz= xanz*4;
  datanz= bmphoehe*xanz;
  i= fread(&bmpdat,datanz,1,binfile);

/* nur zu Testzwecken

  printf("\nBMP-ID       : 0x%x = %c%c",bmpid,bmpid & 0xff,bmpid>>8);              // ist die ID NICHT 0x4d42  ("BM") dann ist es keine BMP-Datei
  printf("\nDateigroesse : %d",filesize);
  printf("\nBitmapbreite : %d",bmpbreite);
  printf("\nBitmapbreite : %d",bmphoehe);
  printf("\nBit per Pixel: %d",bppx);
  printf("\nDatenzeiger  : 0x%.4X\n",bdatptr);
  printf("Anzahl Bytes per X-Reihe (BMP-Datei): %d\n",xanz);
  printf("Anzahl Bytes per X-Reihe (Header): %d\n",pixbytex);
  printf("Anzahl Bitmap Datenbytes: %d\n",i*datanz);
  printf("Byte an 0x3E: %.2X\n",bmpdat[0x3e]);
  ch= getchar(); */

  banz= (bmphoehe*pixbytex)+2;
  cfile= fopen(outputfile, "w");
  fprintf(cfile,"\n//Array generated with IMAGE2C by R. Seelig\n");

  fprintf(cfile,"\n#define bmpsw_imagebytes        %u\n\n",banz+4);


  if (avrstyle)
    fprintf(cfile,"static const unsigned char bmpsw_image[%u] PROGMEM = {\n\n",banz+4);
  else
    fprintf(cfile,"static const unsigned char bmpsw_image[%u]= {\n\n",banz+4);

  fprintf(cfile,"  0x%.2X, 0x%.2X, 0x%.2X, 0x%.2X,\n  ",bmpbreite>>8,
                                                        bmpbreite & 0xff,
                                                        bmphoehe>>8,
                                                        bmphoehe & 0xff);

  icnt= datanz;
  for (y= 0; y < bmphoehe; y++)
  {
    icnt -= xanz;
    xw= 0;
    for (x= 0; x < pixbytex; x++)
    {
      b= bmpdat[icnt+x];
      dbyte= 0;
      for (cnt = 7; cnt > -1; cnt--)
      {
        if ((xw < bmpbreite))
        {
          if (!(b & (1 << cnt)))
          {
            dbyte |= 1;
          }
        }
        if (cnt != 0) { dbyte = dbyte << 1;}
        xw++;
      }
      if ((y == bmphoehe-1) && (x == pixbytex-1))
      {
        fprintf(cfile,"0x%.2X };\n");
      }
      else
      {
        fprintf(cfile,"0x%.2X, ",dbyte);
      }
    }
    fprintf(cfile,"\n  ");
  }

  fclose(binfile);
  fclose(cfile);
}


/* ----------------------------------------------------------
     help_show

     Zeigt Hilfetext auf der Konsole an
   ---------------------------------------------------------- */
void help_show(void)
{
  printf("\n\nIMAGE2C 0.01   2019 by R. Seelig\n");
  printf(    "--------------------------------------\n\n");
  printf("Converts a graphic to a regular C-arrays\n\n");
  printf("Supported graphics formats are:\n\n");
  printf("- pcx256  PCX with 256 indexed colors \n");
  printf("- bmp256  uncompressed BMP with 256 indexed colors \n");
  printf("- bmp16   uncompressed BMP wit 16 indexed colors \n");
  printf("- bmpsw   BMP black and white\n");
  printf("- ascii   from mtpaint (originaly 4 colors, no indexed colors)\n\n");
  printf("Syntax:     image2c options\n\n");
  printf("Options:\n");
  printf("    -i inputfile\n");
  printf("    -o outputfile\n");
  printf("    -a : outfileformat is AVR-progmem array\n");
  printf("    -f inputfileformat (allowed formats are pcx256, bmp256, bmp16, bmpsw, ascii\n");
  printf("    -p : only generate the colorpalette. Available only with 256 color images\n");
  printf("         With 16 color images, the palette is generated with the data\n");
  printf("    -h : show this help\n\n");
  printf("Example:\n");
  printf("    image2c -i testpic.pcx -o testpicdata -a -f pcx256 -p\n\n");
}

/* ----------------------------------------------------------------------------------
                                       MAIN
   ---------------------------------------------------------------------------------- */
int main(int argc, char **argv)
{
  int aflag = 0;
  int pflag = 1;
  int hflag = 0;
  int parseerr = 0;

  char *ivalue = NULL;
  char *ovalue = NULL;
  char *fvalue = NULL;


  int index;
  int c;

  opterr = 0;

  while ((c = getopt (argc, argv, "aphf:i:o:")) != -1)
  {
    switch (c)
      {
      case 'a':
        aflag = 1;
        break;
      case 'p':
        pflag = 0;
        break;
      case 'h':
        hflag = 1;
        break;
      case 'i':
        ivalue = optarg;
        break;
      case 'o':
        ovalue = optarg;
        break;
      case 'f':
        fvalue = optarg;
        break;
      case '?':
        if ((optopt == 'i') || (optopt == 'o') || (optopt == 'f'))
        {
          printf (" Missing argument for Option -%c .\n", optopt);
          parseerr= 1;
        }
        else
        {
          printf (" unknown option -%c'.\n", optopt);
          parseerr= 1;
        }
        return 1;
      default:
        abort ();
      }
  }

  for (index = optind; index < argc; index++)
  {
    printf ("No argument %s\n", argv[index]);
  }

  if (hflag)
  {
    help_show();
    return 0;
  }

  if (ovalue == NULL)
  {
    printf("\nError: no outputfilename is given... \n\n");
    return 1;
  }

  if (ivalue == NULL)
  {
    printf("\nError: no inputfilename is given... \n\n");
    return 1;
  }

  if (fvalue == NULL)
  {
    printf("\nError: no input fileformat (-f) is given... \n\n");
    return 1;
  }

  if (strcmp(fvalue,"pcx256")== 0)
    pcx256_convert(ivalue, ovalue, pflag, aflag);

  if (strcmp(fvalue,"bmp256")== 0)
  {
    if (!pflag)
    {
      bmp256pal_convert(ivalue, ovalue, aflag);
    }
    else
    {
      bmp256_convert(ivalue, ovalue, aflag);
    }
  }

  if (strcmp(fvalue,"ascii")== 0)
  {
    if (!pflag)
    {
      printf("\nInfo: Ascii-Art contains no palette!\n");
      printf("Option -p ignored...\n");
    }
    bmp2bit_convert(ivalue, ovalue, aflag);
  }

  if (strcmp(fvalue,"bmpsw")== 0)
  {
    if (!pflag)
    {
      printf("\nInfo: Ascii-Art contains no palette!\n");
      printf("Option -p ignored...\n");
    }
    bmpsw_convert(ivalue, ovalue, aflag);
  }

  if (strcmp(fvalue,"bmp16")== 0)
  {
    if (!pflag)
    {
      printf("\nInfo: creating only a 16 color palette is not supported.\n");
      printf("Generate palette AND imagedata instead...\n\n");
    }
    bmp16_convert(ivalue, ovalue, aflag);
  }

}
