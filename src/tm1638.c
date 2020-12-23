/* ------------------------------------------------------------------
                              tm1638.c

    Softwaremodul zu Chinaboards mit TM1638 7-Segmentanzeige-
    baustein.

    Derzeit (Stand Maerz 2019) existieren 2 Boards, eines mit 8,
    ein anderes mit 16 Tasten und leider ist das Ansprechen der
    Boards verschieden, denn:

    - auf dem Board mit 8 Tasten wurden Anzeigen mit gemeinsamer
      Kathode und zusaetzlichen 8 Einzel-LEDs verbaut

    - auf dem Board mit 16 Tasten wurden Anzeigen mit gemeinsamer
      Anode verbaut

    Hieraus ergibt sich die Notwendigkeit unterscheiden zu muessen,
    fuer welches der Boards die vorliegende Software eingesetzt
    wird.

    Beim Board mit 8 Tasten gibt es zusaetzlich Funktionen zum Setzen
    der Einzel_LEDs sowie das Lesen einer Tastenmatrix (welche beim
    Board mit 16 Tasten aufgrund der anderen Verschaltung nicht
    moeglich ist).

    MCU  : STM32F030
    Takt :

    07.03.2019  R. Seelig
   ------------------------------------------------------------------ */
#include "tm1638.h"

// Globale Variable

uint8_t fb1638[8];                       // der Framebuffer fuer die Anzeige
uint8_t tm1638_brightness = 7;

// Bitmap des Ascii-Codes (am Ende dieser Datei)
extern const uint8_t bmps7asc [96];

/* -------------------------------------------------------------------
                     Funktionen Framebuffer (fb1638_)
   ------------------------------------------------------------------- */

/* ---------------------------------------------------------
                             fb1638_setbmp

       setzt ein Bitmap in den Framebuffer ein.

       Uebergabe:

              b      : 8 einzelne Bits, die als Bitmap
                       auf einer Anzeigepostion eingefuegt
                       wird
              pos    : Position der Ziffer ( links = 0)
   --------------------------------------------------------- */
void fb1638_setbmp(uint8_t b, uint8_t pos)
{
  #if (board_version == 1)
    fb1638[7-pos]= b;
  #endif

  #if (board_version == 2)
    uint8_t i;

    for (i= 0; i< 8; i++)
    {
      if (b & (1 << i))                              // ist Bit eines Bitmaps gesetzt
      {
        fb1638[i] |= (1 << pos);
      }
      else
      {
        fb1638[i] &= ~(1 << pos);
      }
    }
  #endif
}

void puls_len(void)
{
  volatile uint32_t cx;

  for (cx= 0; cx < (puls_us); cx++);
}

/* ---------------------------------------------------------
                             fb1638_putseg

       setzt / loescht eine einzelne Leuchtdiode, oben
       rechts entspricht Position 0

       Uebergabe:

              pos    : Position der Leuchtdiode
              setrst : 1 = Leuchtdiode wird gesetzt (an)
                       0 = wird geloescht (aus)
   --------------------------------------------------------- */
void fb1638_putseg(uint8_t pos, uint8_t setrst)
{
  if (setrst)
    fb1638[pos >> 3] |= (1 << (pos % 8));
  else
    fb1638[pos >> 3] &= ~(1 << (pos % 8));
}


/* ---------------------------------------------------------
                           fb1638_setchar

      setzt ein (Pseudo)Ascii-Zeichen an der Position pos
      im Framebuffer ein. Bitmaps des Index 0 entspricht
      Ascii-Zeichen 0x20 (Leerzeichen)

      Uebergabe:
            ch    : anzuzeigendes Zeichen
            pos   : Ausgabeposition (0 = rechts, 7 = links)
   --------------------------------------------------------- */
void fb1638_setchar(uint8_t ch, uint8_t pos)
{
  fb1638_setbmp(bmps7asc[ch - ' '],pos);
}


/* ---------------------------------------------------------
                           fb1638_puts

      Fuegt einen String in den Framebuffer ein

      Uebergabe:
            *s    : Zeiger auf Stringtext
            pos   : linke Ausgabeposition (0 = links aussen)
   --------------------------------------------------------- */
void fb1638_puts(const char *s, char pos)
{
  while(*s && pos > -1)
  {
    fb1638_setchar(*s, pos);
    s++;
    pos--;
  }
}

//                     Ende _fb (Framebuffer)
// -------------------------------------------------------------------

/* -------------------------------------------------------------------
                      Funktionen tm1638_
   ------------------------------------------------------------------- */

/* ---------------------------------------------------------
                           tm1638_init

       Initialisiert die zur Datenuebertragung benoetigten
       Pins als Ein- / Ausgang, setzt die Defaultpegel,
       loescht den Framepuffer und die Anzeige
   --------------------------------------------------------- */
void tm1638_init(void)
{
  sda_init(); scl_init(); stb_init();
  bb_stb_hi(); bb_scl_lo();
  fb1638_clr();
  tm1638_showbuffer();
}

/* ---------------------------------------------------------
                         tm1638_write

        sendet einen 8 Bit Wert seriell mit LSB first
   --------------------------------------------------------- */
void tm1638_write (uint8_t value)
{
  uint8_t i;

  for (i = 0; i <8; i++)
  {
    bb_scl_lo();

    //  serielle Bitbangingausgabe, LSB first
    if (value & 0x01) { bb_sda_hi(); }
                   else { bb_sda_lo(); }
    puls_len();
    value = value >> 1;
    bb_scl_hi();
    puls_len();
  }
  bb_scl_lo();
}

/* ---------------------------------------------------------
                       tm1638_selectledadr

       sendet Kommandos fuer Auswahl
          - Aktivierung LED-Register  (0x40)
          - Controllregister (0x80) mit
            Einstellung Hellikeit (0x08 | (7 Helligkeitbits)
          - Select LED-Adresse

       Sendet KEIN abschliesendes Strobe (erforderlich, damit
       ein nachfolgender Schreibzugrif auf die LED-Adresse
       erfolgen kann).

       Uebergabe:
             adr       : LED-Register
             ledbright : Hellikeit (0..7)
   --------------------------------------------------------- */
void tm1638_selectledadr(uint8_t adr, uint8_t ledbright)
{
  bb_stb_lo();
  tm1638_write(0x40);                         // Auswahl LED-Register
  bb_stb_hi();
  puls_len();

  bb_stb_lo();
  tm1638_write(0x88 | (ledbright & 0x07));    // Controllregister
  bb_stb_hi();
  puls_len();

  bb_stb_lo();
  tm1638_write(0xc0 | adr);                   // Adresse
}

/* ---------------------------------------------------------
                         tm1638_wradr

        schreibt einen 8-Bit Wert an die angegebene Adresse
        des TM1638

        Uebergabe:
            adr   : Speicherstelle im TM1638 (0x00 .. 0x0F)
            value : zu schreibender Wert
   --------------------------------------------------------- */
void tm1638_wradr(uint8_t adr, uint8_t value)
{
  tm1638_selectledadr(adr, tm1638_brightness);
  tm1638_write(value);
  bb_stb_hi();
}

/* ---------------------------------------------------------
                           tm1638_clear

      loescht die Anzeige (aber nicht den Framebuffer oder
      die Einzel-LEDS)
   --------------------------------------------------------- */
void tm1638_clear(void)
{
  uint8_t i;

  #if (board_version == 1)
    for (i= 0; i< 16; i+= 2)
    {
      tm1638_wradr(i, 0x00);
    }
  #endif

  #if (board_version == 2)
    tm1638_selectledadr(0, tm1638_brightness);
    for (i= 0; i< 16; i++)
    {
      tm1638_write(0x00);
    }
    bb_stb_hi();
  #endif
}

/* ---------------------------------------------------------
                           tm1638_showbuffer

       zeigt den 8 Byte grossen Pufferspeicher fb1638
       auf den 7-Segmentanzeigen an.

       Uebergabe:
            *buffer   : Zeiger auf einen 8 Byte grossen
                        Pufferspeicher
   --------------------------------------------------------- */
void tm1638_showbuffer(void)
{
  uint8_t i;

  for (i= 0; i< 8; i++) tm1638_wradr(i << 1, fb1638[i]);
}

/*  ---------------------------------------------------------
                        tm1638_setdp

      setzt oder loescht einen Dezimalpunkt

      Uebergabe:
         pos    : Position, an der ein Dezimalpunkt gesetzt
                  oder geloescht wird
         enable : 1 = dp wird gesetzt
                  0 = dp wird geloescht
    --------------------------------------------------------- */
void tm1638_setdp(uint8_t pos, uint8_t enable)
{

  #if (board_version == 1)
    pos= 7 - pos;
    if (enable)
      fb1638[pos] |= 0x80;
    else
      fb1638[pos] &= ~(0x80);
    tm1638_showbuffer();

  #endif

  #if (board_version == 2)
    if (enable)
      fb1638[7] |= (1 << pos);
    else
      fb1638[7] &= ~(1 << pos);
  #endif
  tm1638_showbuffer();
}

/*  ---------------------------------------------------------
                        tm1638_setdez

      zeigt einen 32-Bit Integerwert dezimal auf der
      8-stelligen Anzeige an (beschreiben des Framebuffers
      und diesen anzeigen).

      Uebergabe:
         value  : anzuzeigender Wert
         pos    : Position, ab der der Wert rechtsbuendig
                  angezeigt wird
         nozero : 1 = fuehrende Nullen werden nicht angezeigt
                  0 = werden angezeigt
    --------------------------------------------------------- */
void tm1638_setdez(int32_t value, uint8_t pos, uint8_t nozero)
{
  int32_t teiler = 10000000;
  uint8_t i, v, b;

  for (i= 0; i< pos; i++) teiler /= 10;
  fb1638_clr();
  for (i= 0; i< 8-pos; i++)
  {
    v= value / teiler;
    value= value - ( v * teiler);
    teiler /= 10;
    if ((i== (8-pos-1) && nozero)) nozero= 0;
    if (v || !nozero)
    {
      b= bmps7asc[v + 16];                                  // Bitmap der Ziffer aus Asciibitmap holen
                                                            // ( 16 ist Offset zu Ascii 0, da die ersten 32
                                                            // Asciizeichen nicht implementiert sind

      // und in Buffer einfuegen
      #if (board_version == 1)
        fb1638[i]= b;
      #endif
      #if (board_version == 2)
        fb1638_setbmp(b, 7-i);
      #endif

      nozero=  0;
    }
  }
  tm1638_showbuffer();
}

/*  ---------------------------------------------------------
                        tm1638_sethex

      zeigt einen 32-Bit Integerwert hexdezimal auf der
      8-stelligen Anzeige an (beschreiben des Framebuffers
      und diesen anzeigen).

      Uebergabe:
         value  : anzuzeigender Wert
         pos    : Position, ab der der Wert rechtsbuendig
                  angezeigt wird
         nozero : 1 = fuehrende Nullen werden nicht angezeigt
                  0 = werden angezeigt
    --------------------------------------------------------- */
void tm1638_sethex(int32_t value, uint8_t pos, uint8_t nozero)
{
  int32_t teiler = 0x10000000;
  uint8_t i, v, b;

  for (i= 0; i< pos; i++) teiler /= 0x10;
  fb1638_clr();
  for (i= 0; i< 8-pos; i++)
  {
    v= value / teiler;
    value= value - ( v * teiler);
    teiler /= 0x10;
    if ((i== (8-pos-1) && nozero)) nozero= 0;
    if (v || !nozero)
    {
      if (v < 10)                                     // Bitmap der Ziffer aus Asciibitmap holen
        b= bmps7asc[v + 16];
      else
        b= bmps7asc[v + 23];

      // und in Buffer einfuegen
      #if (board_version == 1)
        fb1638[i]= b;
      #endif
      #if (board_version == 2)
        fb1638_setbmp(b, 7-i);
      #endif

      nozero=  0;
    }
  }
  tm1638_showbuffer();
}

#if (board_version == 1)
  /* ---------------------------------------------------------
                           tm1638_setled

       beschreibt die 8 einzelnen LEDs des Boards mit einem
       8-Bit Wert
     --------------------------------------------------------- */
  void tm1638_setled(uint8_t value)
  {
    int8_t pos;
    uint8_t b;

    for (pos= 0; pos< 8; pos ++)
    {

      if (value & (0x80 >> pos))
        tm1638_wradr((pos << 1)+1, 0x01);
      else
        tm1638_wradr((pos << 1)+1, 0x00);

    }
  }
#endif

/*  ---------------------------------------------------------
                        tm1638_read

      Lesen der aktuell gewaehlten Adresse des Keyregisters.
      Es erfolgt weder vor noch nach dem Lesen ein Strobe-
      signal

    --------------------------------------------------------- */
uint8_t tm1638_read(void)
{
  uint8_t data= 0x00;
  uint8_t i;

  bb_sda_hi();

  for(i= 0; i< 8; i++)
  {
    bb_scl_lo();
    if(bb_is_sda()) data|= (1 << i);
    puls_len();
    bb_scl_hi();

    puls_len();

  }
  bb_scl_lo();

  return data;
}

#if (board_version == 1)
  /*  ---------------------------------------------------------
                          tm1638_readkeymatrix

       liefert beim 8-Tasten Board eine Bitmatrix der aktuell
       gedrueckten Tasten (keine Taste gedrueckt = 0, alle
       Tasten gedrueckt = 0xFF)
     ---------------------------------------------------------- */
  uint8_t tm1638_readkeymatrix(void)
  {
    uint32_t value;
    uint8_t i, keynr, pressedkey;

    value= 0;
    bb_stb_hi();
    bb_stb_lo();
    tm1638_write(0x42);                                   // cmd fuer Tastenscan lesen (Keyregister)
    puls_len();

    keynr= 0;
    value =  (uint32_t)tm1638_read();                     // 4 Bytes des Tastenscan zusammen setzen
    if (value & 0x01) keynr  = 0x01;
    if (value & 0x10) keynr |= 0x10;
    value = (uint32_t)tm1638_read();
    if (value & 0x01) keynr |= 0x02;
    if (value & 0x10) keynr |= 0x20;
    value= (uint32_t)tm1638_read();
    if (value & 0x01) keynr |= 0x04;
    if (value & 0x10) keynr |= 0x40;
    value= (uint32_t)tm1638_read();
    if (value & 0x01) keynr |= 0x08;
    if (value & 0x10) keynr |= 0x80;

    bb_stb_hi();
    return keynr;
  }

#endif

/*  ---------------------------------------------------------
                        tm1638_readkeys

      liefert den Wert einer eventuell gedrueckten Tasten
      nach folgendem Schema zurueck:

      Board 2: 16 Tasten

                       1   2   3   4
                       5   6   7   8
                       9  10  11  12
                      13  14  15  16

            Wert 0 wenn keine Taste gedrueckt ist.

      Leider unterstuetzt das 16-Tasten Board hardwaremaessig
      ein Erfassen aller gedrueckten Tasten in einer Matrix
      nicht.

      Board 1: 8 Tasten

      Der Wert einer gedrueckten Taste wird zurueck gegeben.
   ---------------------------------------------------------- */
uint8_t tm1638_readkeys(void)
{
  #if (board_version == 1)
    if (tm1638_readkeymatrix() & 0x01) return 1;
    if (tm1638_readkeymatrix() & 0x02) return 2;
    if (tm1638_readkeymatrix() & 0x04) return 3;
    if (tm1638_readkeymatrix() & 0x08) return 4;
    if (tm1638_readkeymatrix() & 0x10) return 5;
    if (tm1638_readkeymatrix() & 0x20) return 6;
    if (tm1638_readkeymatrix() & 0x40) return 7;
    if (tm1638_readkeymatrix() & 0x80) return 8;
    return 0;
  #endif

  #if (board_version == 2)
    uint32_t value;
    uint8_t i, keynr, pressedkey;

    value= 0;
    bb_stb_hi();
    bb_stb_lo();
    tm1638_write(0x42);                                   // cmd fuer Tastenscan lesen (Keyregister)
    puls_len();
    value =  (uint32_t)tm1638_read();                     // 4 Bytes des Tastenscan zusammen setzen
    value |= (uint32_t)tm1638_read() << 8;
    value |= (uint32_t)tm1638_read() <<  16;
    value |= (uint32_t)tm1638_read() <<  24;
    bb_stb_hi();
    value = value >> 1;
    /*
        Anordnung der Tastenmatrix auf Chinaboard

        2   4   6   8
       10  12  14  16
        1   3   5   7
        9  11  13  15
    */

    keynr= 1; pressedkey= 0;

    /*
       Zuordnen der Tastenmatrix zu den Tasten mit Nummern 1..16
       Es entsteht folgende Tastenmatrix:

        1   2   3   4
        5   6   7   8
        9  10  11  12
       13  14  15  16
    */

    for (i= 0; i< 8; i++)
    {
      if (value & 1) pressedkey= keynr;
      keynr++;
      value = value >> 1;
      if (value & 1) pressedkey= keynr;
      keynr++;
      value = value >> 3;
    }
    if (!(pressedkey & 1)) return (uint8_t) (pressedkey >> 1);
                      else return (uint8_t) ((pressedkey + 1) >> 1) + 8;
    return 0;

  #endif
}

#if ((readint_enable == 1) && (board_version == 2))
  /*
     Calculator Keymap:

     7    8    9    /
     4    5    6    *
     1    2    3    -
    ent   0   clr   +
  */

  const uint8_t calckeymap [16] =
  {  7,    8,    9, '/',
     4,    5,    6, '*',
     1,    2,    3, '-',
     0x0d, 0, 0x18, '+' };

/* ---------------------------------------------------------
                       tm1638_readint

     liest einen Integerwert ueber die Tasten ein. Die
     Tastaturmap ist in calckeymap abgelegt. Es besteht
     (logischerweise) keine Korrekturmoeglichkeit. Jede
     von einer numerischen Ziffer abweichende Taste
     schliesst die Eingabe ab.

     Bei Eintreten wird der in value angegebene Wert ange-
     zeigt und mit der ersten betaetigten Taste geloescht.

     Uebergabe
        *value   : Zeiger auf Wert, der bei Eintreten in
                   die Funktion angezeigt wird und in dem
                   der eingegebene Integer bei Rueckkehr
                   abgelegt ist.

     Rueckgabe
                 Die Funktionstaste, mit der die Eingabe
                 beendet wurde.
   --------------------------------------------------------- */
uint8_t tm1638_readint(uint32_t *value)
{
  uint8_t k, key, anz, first;

  k= 0; anz= 0; first= 1;
  tm1638_setdez(*value,0,1);
  *value= 0;

  do
  {
    key= tm1638_readkeys();
    if (key)
    {
      k= calckeymap[key-1];
      if (k < 10)
      {
        if ((!first || k) && (anz < 8))
        {
          first= 0;
          *value = *value * 10;
          *value += (uint32_t) k;
          anz++;
        }
        tm1638_setdez(*value,0,1);
        while(tm1638_readkeys());
        delay(50);
      }
    }
  } while (k < 10);
  return k;
}
#endif


/*
    Segmentbelegung der Anzeige:

        a
       ---
    f | g | b            Segment |  a  |  b  |  c  |  d  |  e  |  f  |  g  | Doppelpunkt (nur fuer POS1) |
       ---               ---------------------------------------------------------------------------------
    e |   | c            Bit-Nr. |  0  |  1  |  2  |  3  |  4  |  5  |  6  |              7              |
       ---
        d
*/

// Bitmapmuster ASCII - Code, beginnend mit 0x20
const uint8_t bmps7asc [96] =
{
// space   !     "     #     $     %     &     '
   0x00, 0x86, 0x22, 0x7E, 0x6D, 0xD2, 0x46, 0x20,
//   (     )     *     +     ,     -     .      /
   0x29, 0x0B, 0x21, 0x70, 0x10, 0x40, 0x80, 0x52,
//   0     1     2     3     4     5     6     7
   0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
//   8     9     :     ;     <     =     >     ?
   0x7F, 0x6F, 0x09, 0x0D, 0x61, 0x48, 0x43, 0xD3,
 //  @     A     B     C     D     E     F     G
   0x5F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D,
//    H     I     J     K     L     M     N     O
   0x76, 0x30, 0x1E, 0x75, 0x38, 0x15, 0x37, 0x3F,
//   P     Q     R     S     T     U     V     W
   0x73, 0x6B, 0x33, 0x6D, 0x78, 0x3E, 0x3E, 0x2A,
//   X     Y     Z     [     \     ]     ^     _
   0x76, 0x6E, 0x5B, 0x39, 0x64, 0x0F, 0x23, 0x08,
//    `    a     b     c     d     e     f     g
   0x02, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F,
//   h     i     j     k     l     m     n     o
   0x74, 0x10, 0x0C, 0x75, 0x30, 0x14, 0x54, 0x5C,
//  p     q     r     s     t     u     v     w
   0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x1C, 0x14,
//  x     y     z     {     |     }     ~    blk
   0x76, 0x6E, 0x5B, 0x46, 0x30, 0x70, 0x01, 0x00
};
