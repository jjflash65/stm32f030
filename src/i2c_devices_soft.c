/* -----------------------------------------------------
                        i2c_devices_soft.c

    Softwaremodul fuer I2C Buses mittels Software (Bit-
    banging) und einigen I2C-Devices.

    Implementierte I2C-Devices:

         - DS1307  (real time clock)
         - RDA5807 (UKW receiver)
         - LM75    (Temperatursensor)
         - 24LCxx  (EEProm)
         - SSD1306 (OLED-Display)

    Hardware  : STM32F030F4P6
    IDE       : keine (Editor / make)
    Library   : libopencm3
    Toolchain : arm-none-eabi

    05.04.2017   R. seelig
  ------------------------------------------------------ */

#include "i2c_devices_soft.h"
#include "font8x8h.h"

/* #################################################################
     Funktionen fuer einen I2C - Bus (Softwareimplementierung)
   ################################################################# */

/* -------------------------------------------------------
                   i2c_master_init

    setzt die Pins die fuer den I2C Bus verwendet werden
    als Ausgaenge
   ------------------------------------------------------- */
void i2c_master_init()
{
  i2c_sda_hi();
  i2c_scl_hi();
}

/* -------------------------------------------------------
                     i2c_sendstart(void)
    erzeugt die Startcondition auf dem I2C Bus
   ------------------------------------------------------- */
void i2c_sendstart(void)
{
  i2c_scl_hi();
  i2c_sda_lo();
}

/* -------------------------------------------------------
                     i2c_start
    erzeugt die Startcondition und anschliessend
    die Deviceadresse
   ------------------------------------------------------- */
uint8_t i2c_start(uint8_t addr)
{
  uint8_t ack;

  i2c_sendstart();
  ack= i2c_write(addr);
  return ack;
}

/* -------------------------------------------------------
                   i2c_startaddr

   startet den I2C-Bus und sendet Bauteileadresse
   rwflag bestimmt, ob das Device beschrieben oder
   gelesen werden soll
  -------------------------------------------------- */
void i2c_startaddr(uint8_t addr, uint8_t rwflag)
{
  addr = (addr << 1) | rwflag;

  i2c_sendstart();
  i2c_write(addr);
}

/* -------------------------------------------------------
                     i2c_stop
    erzeugt die Stopcondition auf dem I2C Bus
   ------------------------------------------------------- */
void i2c_stop(void)
{
   i2c_sda_lo();
   i2c_scl_hi();
   i2c_sda_hi();
}

/* -------------------------------------------------------
                   i2c_write_nack(uint8_t data)

   schreibt einen Wert auf dem I2C Bus OHNE ein Ack-
   nowledge einzulesen
  ------------------------------------------------------- */
void i2c_write_nack(uint8_t data)
{
  uint8_t i;

  for(i=0;i<8;i++)
  {
    i2c_scl_lo();

    if(data & 0x80) i2c_sda_hi();
               else i2c_sda_lo();
    i2c_scl_hi();

    while(i2c_is_scl() == 0);

    data=data<<1;
  }
  i2c_scl_lo();
}

/* -------------------------------------------------------
                   i2c_write(uint8_t data)

   schreibt einen Wert auf dem I2C Bus.

   Rueckgabe:
               > 0 wenn Slave ein Acknowledge gegeben hat
               == 0 wenn kein Acknowledge vom Slave
   ------------------------------------------------------- */
uint8_t i2c_write(uint8_t data)
{
   uint8_t ack;

   i2c_write_nack(data);

  //  9. Taktimpuls (Ack)

  i2c_sda_hi();
  i2c_scl_hi();

  if (i2c_is_sda()) ack= 0; else ack= 1;

  i2c_scl_lo();

  return ack;
}

/* -------------------------------------------------------
                   i2c_write16(uint8_t data)

   schreibt einen 16-Bit Wert auf dem I2C Bus.

   Rueckgabe:
               > 0 wenn Slave ein Acknowledge gegeben hat
               == 0 wenn kein Acknowledge vom Slave
   ------------------------------------------------------- */
uint8_t i2c_write16(uint16_t data)
{
  uint8_t ack;
  ack= i2c_write(data >> 8);
  if (!(ack)) return 0;
  ack= i2c_write(data & 0xff);
  return ack;
}


/* -------------------------------------------------------
                    i2c_read(uint8_t ack)

   liest ein Byte vom I2c Bus.

   Uebergabe:
               1 : nach dem Lesen wird dem Slave ein
                   Acknowledge gesendet
               0 : es wird kein Acknowledge gesendet

   Rueckgabe:
               gelesenes Byte
   ------------------------------------------------------- */
uint8_t i2c_read(uint8_t ack)
{
  uint8_t data= 0x00;
  uint8_t i;

  i2c_sda_hi();

  for(i=0;i<8;i++)
  {
    i2c_scl_lo();
    i2c_scl_hi();

    while( i2c_is_scl() == 0);

    if(i2c_is_sda()) data|=(0x80>>i);
  }

  i2c_scl_lo();
  if (ack)
  {
    i2c_sda_lo();
    i2c_scl_hi();
    i2c_scl_lo();
  }
  i2c_sda_hi();

  return data;
}
/* -----------------------------------------------------------------
     Ende I2C - Busfunktionen
   ----------------------------------------------------------------- */

/* #################################################################
     Funktionen des DS1307 (RTC - Echtzeituhr)
   ################################################################# */

/* --------------------------------------------------
     rtc_read

     liest einen einzelnen Wert aus dem RTC-Chip

     Uebergabe:
         addr : Registeradresse des DS1307 der
                gelesen werden soll
   -------------------------------------------------- */
uint8_t rtc_read(uint8_t addr)
{
  uint8_t value;

  i2c_sendstart();
  i2c_write(rtc_addr);
  i2c_write(addr);
  i2c_stop();
  i2c_sendstart();
  i2c_write(rtc_addr | 1);
  value= i2c_read_nack();
  i2c_stop();

  return value;
}

/* --------------------------------------------------
     rtc_write

     schreibt einen einzelnen Wert aus dem RTC-Chip

     Uebergabe:
         addr : Registeradresse des DS1307 der
                geschrieben werden soll
   -------------------------------------------------- */
void rtc_write(uint8_t addr, uint8_t value)
{
  i2c_sendstart();
  i2c_write(rtc_addr);
  i2c_write(addr);
  i2c_write(value);
  i2c_stop();
}

/* --------------------------------------------------
      rtc_bcd2dez

      wandelt eine BCD Zahl (NICHT hex)  in einen
      dezimalen Wert um

      Bsp: value = 0x34
      Rueckgabe    34
   -------------------------------------------------- */
uint8_t rtc_bcd2dez(uint8_t value)
{
  uint8_t hiz,c;

  hiz= value / 16;
  c= (hiz*10)+(value & 0x0f);
  return c;
}

/* --------------------------------------------------
      rtc_getwtag

      Berechnet zu einem bestimmten Datum den
      Wochentag (nach Carl Friedrich Gauss). Der
      Function werden die Daten mittels der
      Struktur my_datum so uebergeben, wie sie
      von rtc_readdate gelesen werden. Ein Wochen-
      tag beginnt mit 0 (0 entspricht Sonntag)

      Uebergabe:
           *date  : Zeiger auf eine Struktur
                    my_datum

      Rueckgabe:
           Tag der Woche

      Bsp.:      11.04.2017   ( das ist ein Dienstag )
      Rueckgabe: 2
   -------------------------------------------------- */
uint8_t rtc_getwtag(struct my_datum *date)
{
  int tag, monat, jahr;
  int w_tag;

  tag=  rtc_bcd2dez(date->tag);
  monat= rtc_bcd2dez(date->monat);
  jahr= rtc_bcd2dez(date->jahr)+2000;

  if (monat < 3)
  {
     monat = monat + 12;
     jahr--;
  }
  w_tag = (tag+2*monat + (3*monat+3)/5 + jahr + jahr/4 - jahr/100 + jahr/400 + 1) % 7 ;
  return w_tag;
}

/* --------------------------------------------------
      rtc_readdate

      liest den DS1307 Baustein in eine Struktur
      my_datum ein.

      Rueckgabe:
          Werte der gelesenen RTC in der Struktur
          my_datum
   -------------------------------------------------- */
struct my_datum rtc_readdate(void)
{
  struct my_datum date;

  date.sek= rtc_read(0) & 0x7f;
  date.min= rtc_read(1) & 0x7f;
  date.std= rtc_read(2) & 0x3f;
  date.tag= rtc_read(4) & 0x3f;
  date.monat= rtc_read(5) & 0x1f;
  date.jahr= rtc_read(6);
  date.dow= rtc_getwtag(&date);

  return date;
}

/* --------------------------------------------------
     rtc_writedate

     schreibt die in der Struktur enthaltenen Daten
     in den RTC-Chip
   -------------------------------------------------- */
void rtc_writedate(struct my_datum *date)
{
  rtc_write(0, date->sek);
  rtc_write(1, date->min);
  rtc_write(2, date->std);
  rtc_write(4, date->tag);
  rtc_write(5, date->monat);
  rtc_write(6, date->jahr);
}

/* -----------------------------------------------------------------
     Ende DS1307
   ----------------------------------------------------------------- */

/* #################################################################
     Funktionen des LM75 Temperatursensors
   ################################################################# */

/* --------------------------------------------------
     lm75_read

     liest den Temperatursensor aus.

     Rueckgabewert:
        gelesene Temperatur x 10.

        Bsp.: Rueckgabewert von 245 entspricht 24.5 Grad Celsius
   -------------------------------------------------- */
int lm75_read(void)
{
  char       ack;
  char       t1;
  uint8_t    t2;
  int        lm75temp;

  ack= i2c_start(lm75_addr);                // LM75 Basisadresse
  if (ack)
  {

    i2c_write(0x00);                        // LM75 Registerselect: Temp. auslesen
    i2c_write(0x00);

    i2c_stop();

    ack= i2c_start(lm75_addr | 1);          // LM75 zum Lesen anwaehlen
    t1= 0;
    t1= i2c_read_ack();                     // hoeherwertigen 8 Bit
    t2= i2c_read_nack();                    // niederwertiges Bit (repraesentiert 0.5 Grad)
    i2c_stop();

  }
  else
  {
    i2c_stop();
    return -127;                            // Abbruch, Chip nicht gefunden
  }

  lm75temp= t1;
  lm75temp = lm75temp*10;
  if (t2 & 0x80) lm75temp += 5;             // wenn niederwertiges Bit gesetzt, sind das 0.5 Grad
  return lm75temp;
}

/* -----------------------------------------------------------------
     Ende LM75
   ----------------------------------------------------------------- */


/* #################################################################
     Funktionen des RDA5807 FM Receiver
   ################################################################# */

uint16_t aktfreq =   1018;      // Startfrequenz ( 101.8 MHz )
uint8_t  aktvol  =   5;         // Startlautstaerke

const uint16_t festfreq[6] = { 1018, 1048, 888, 970, 978, 999 };

uint8_t  rda5807_adrs = 0x10;   // I2C-addr. fuer sequientielllen Zugriff
uint8_t  rda5807_adrr = 0x11;   // I2C-addr. fuer wahlfreien Zugriff
uint8_t  rda5807_adrt = 0x60;   // I2C-addr. fuer TEA5767 kompatiblen Modus

uint16_t rda5807_regdef[10] ={
            0x0758,             // 00 default ID
            0x0000,             // 01 reserved
            0xF009,             // 02 DHIZ, DMUTE, BASS, POWERUPENABLE, RDS
            0x0000,             // 03 reserved
            0x1400,             // 04 SOFTMUTE
            0x84DF,             // 05 INT_MODE, SEEKTH=0110, unbekannt, Volume=15
            0x4000,             // 06 OPENMODE=01
            0x0000,             // 07 reserved
            0x0000,             // 08 reserved
            0x0000 };           // 09 reserved

uint16_t rda5807_reg[16];

/* --------------------------------------------------
     rda5807_writereg

     einzelnes Register des RDA5807 schreiben
   -------------------------------------------------- */
void rda5807_writereg(uint8_t reg)
{
  uint16_t  w;

  i2c_startaddr(rda5807_adrr,0);
  i2c_write(reg);                        // Registernummer schreiben
  w= rda5807_reg[reg];
  i2c_write16(w);         // 16 Bit Registerinhalt schreiben
  i2c_stop();
}

/* --------------------------------------------------
    rda5807_write

     alle Register es RDA5807 schreiben
   -------------------------------------------------- */
void rda5807_write(void)
{
  uint8_t   i ;

  i2c_startaddr(rda5807_adrs,0);
  for (i= 2; i< 7; i++)
  {
    i2c_write16(rda5807_reg[i]);
  }
  i2c_stop();
}

/* --------------------------------------------------
     rda5807_reset
   -------------------------------------------------- */
void rda5807_reset(void)
{
  uint8_t i;
  for (i= 0; i< 7; i++)
  {
    rda5807_reg[i]= rda5807_regdef[i];
  }
  rda5807_reg[2]= rda5807_reg[2] | 0x0002;    // Enable SoftReset
  rda5807_write();
  rda5807_reg[2]= rda5807_reg[2] & 0xFFFB;    // Disable SoftReset
}

/* --------------------------------------------------
      rda5807_poweron
   -------------------------------------------------- */
void rda5807_poweron(void)
{
  rda5807_reg[3]= rda5807_reg[3] | 0x010;   // Enable Tuning
  rda5807_reg[2]= rda5807_reg[2] | 0x001;   // Enable PowerOn

  rda5807_write();

  rda5807_reg[3]= rda5807_reg[3] & 0xFFEF;  // Disable Tuning
}

/* --------------------------------------------------
      rda5807_setfreq

      setzt angegebene Frequenz * 0.1 MHz

      Bsp.:
         rda5807_setfreq(1018);    // setzt 101.8 MHz
                                   // die neue Welle
   -------------------------------------------------- */
int rda5807_setfreq(uint16_t channel)
{

  channel -= fbandmin;
  channel&= 0x03FF;
  rda5807_reg[3]= channel * 64 + 0x10;  // Channel + TUNE-Bit + Band=00(87-108) + Space=00(100kHz)

  i2c_startaddr(rda5807_adrs,0);
  i2c_write16(0xD009);
  i2c_write16(rda5807_reg[3]);
  i2c_stop();

  delay(100);
  return 0;
}

/* --------------------------------------------------
      rda5807_setvol

      setzt Lautstaerke, zulaessige Werte fuer
      setvol 0 .. 15
   -------------------------------------------------- */
void rda5807_setvol(int setvol)
{
  rda5807_reg[5]=(rda5807_reg[5] & 0xFFF0) | setvol;
  rda5807_writereg(5);
}

/* --------------------------------------------------
      rda5807_setmono
   -------------------------------------------------- */
void rda5807_setmono(void)
{
  rda5807_reg[2]=(rda5807_reg[2] | 0x2000);
  rda5807_writereg(2);
}

/* --------------------------------------------------
      rda5807_setstero
   -------------------------------------------------- */
void rda5807_setstereo(void)
{
  rda5807_reg[2]=(rda5807_reg[2] & 0xdfff);
  rda5807_writereg(2);
}

/* -----------------------------------------------------------------
     Ende RDA5807
   ----------------------------------------------------------------- */


/* #################################################################
     Funktionen 24LCxx EEProm
   ################################################################# */

/* --------------------------------------------------
     eep_write

     schreibt einen 8-Bit Wert value an die
     Adresse adr
   -------------------------------------------------- */
void eep_write(uint16_t adr, uint8_t value)
{
  i2c_start(eep_addr);
  i2c_write16(adr);
  i2c_write(value);
  i2c_stop();
  delay(4);
}

/* --------------------------------------------------
     eep_erase

     loescht den gesamten Inhalt des EEPROMS
   -------------------------------------------------- */
void eep_erase(void)
{
  uint16_t cnt;
  uint16_t adr, len;

  adr= 0; len= 0x7fff;

  i2c_start(eep_addr);
  i2c_write16(adr);

  cnt= 0;
  do
  {
    i2c_write(0xff);
    cnt++;
    adr++;

    if ((adr % eep_pagesize == 0))                 // Pagegrenze des EEProms
    {
      i2c_stop();
      delay(4);                                    // Wartezeit bis Page geschrieben ist
      i2c_start(eep_addr);                             // neue Page oeffnen
      i2c_write16(adr);
    }
  } while (cnt< len);
  i2c_stop();
  delay(4);
}

/* --------------------------------------------------
     eep_writebuf

     schreibt mehrere Datenbytes in das EEProm

     Uebergabe:
         adr    : Adresse, ab der die Bytes im
                  EEProm gespeichert werden
         *buf   : Zeiger auf die Datenbytes, die
                  gespeichert werden sollen
         len    : Anzahl zu speichernder Bytes
   -------------------------------------------------- */
void eep_writebuf(uint16_t adr, uint8_t *buf, uint16_t len)
{
  uint16_t cnt;

  i2c_start(eep_addr);
  i2c_write16(adr);

  cnt= 0;
  do
  {
    i2c_write(*buf);
    buf++;
    cnt++;
    adr++;

    if ((adr % eep_pagesize == 0))                 // Pagegrenze des EEProms
    {
      i2c_stop();
      delay(4);                                    // Wartezeit bis Page geschrieben ist
      i2c_start(eep_addr);                             // neue Page oeffnen
      i2c_write16(adr);
    }
  } while (cnt< len);
  i2c_stop();
  delay(4);
}

/* --------------------------------------------------
     eep_read

     liest ein einzelnes Byte aus dem EEProm an
     der Adresse adr aus
   -------------------------------------------------- */
uint8_t eep_read(uint16_t adr)
{
  uint8_t value;

  i2c_start(eep_addr);                                 // sicherheitshalber
  i2c_stop();                                      // I2C Bus garantiert frei

  i2c_start(eep_addr);
  i2c_write16(adr);
  delay(5);

  i2c_start(0xa1);

  value= i2c_read_nack();
  i2c_stop();
  return value;
}

/* --------------------------------------------------
     eep_readbuf

     liest mehrere Bytes aus dem EEProm in einen
     Pufferspeicher ein.

     Uebergabe:
         adr     : Adresse, ab der im EEProm
                   gelesen wird.
         *buf    : Zeiger auf einen Speicherbereich
                   in den die Daten aus dem EEPROM
                   kopiert werden.
         len     : Anzahl der zu lesenden Bytes
   -------------------------------------------------- */
void eep_readbuf(uint16_t adr, uint8_t *buf, uint16_t len)
{
  uint16_t cnt;

  i2c_start(eep_addr);                                 // sicherheitshalber
  i2c_stop();                                      // I2C Bus garantiert frei

  i2c_start(eep_addr);
  i2c_write16(adr);
  i2c_start(0xa1);

  cnt= 0;
  do
  {
    if (len== 1)
    {
      *buf= i2c_read_nack();
      i2c_stop();
      return;
    }
    *buf= i2c_read_ack();
    buf++;
    cnt++;
    adr++;
  } while (cnt < len-1);
  i2c_read_nack();                                  // noch einmal lesen um kein ack zu schicken
  i2c_stop();                                       // und somit Adresszaehler EEPROM freigeben
}

/* --------------------------------------------------
     eep_getmemsize

     stellt die Speicherkapazitaet des EEProms fest

     Uebergabe:
         0       : Fehler aufgetreten oder kein
                   EEPROM am Bus

         > 0     : Speicherkapazitaet in Bytes
   -------------------------------------------------- */
uint16_t eep_getmemsize(void)
{
  #define testvalue    0xa5

  uint16_t adr;

  for(adr = 0x100; adr <= 0x8000; adr = adr << 1)
  {
    eep_write(adr, testvalue);
    if((eep_read(0x00) == testvalue) && (eep_read(adr) == testvalue))
    {
      eep_write(0x00,0xff);
      return adr;
    }
    else
    {
      eep_write(adr,0xff);
    }
  }
  return 0;
}


/* -----------------------------------------------------------------
     Ende 24LCxx  EEProm
   ----------------------------------------------------------------- */

/* #################################################################
     Funktionen SSD1306 OLED-Display
   ################################################################# */

uint8_t aktxp= 0;
uint8_t aktyp= 0;
uint8_t doublechar;
uint8_t bkcolor= 0;
uint8_t textcolor= 1;


/* -------------------------------------------------------
                   ssd1306_writecmd

     sendet ein Kommandobyte an das Display
   ------------------------------------------------------- */
void ssd1306_writecmd(uint8_t cmd)
{
  i2c_start(ssd1306_addr);
  i2c_write(0x00);
  i2c_write(cmd);
  i2c_stop();
}

/* -------------------------------------------------------
                   ssd1306_writedata

    sendet ein Datenbyte an das Display
   ------------------------------------------------------- */
void ssd1306_writedata(uint8_t data)
{
  i2c_start(ssd1306_addr);
  i2c_write(0x40);
  i2c_write(data);
  i2c_stop();
}

/* -------------------------------------------------------
                     ssd1306_init

    Initialisiert das Display fuer den Gebrauch
   ------------------------------------------------------- */
void ssd1306_init(void)
{

  //Init LCD

  ssd1306_writecmd(0x8d);      // Ladungspumpe an
  ssd1306_writecmd(0x14);
  ssd1306_writecmd(0xaf);      // Display an
  delay(150);
  ssd1306_writecmd(0xa1);      // Segment Map
  ssd1306_writecmd(0xc0);      // Direction Map
}

/*  ---------------------------------------------------------
                              gotoxy

     legt die naechste Textausgabeposition auf dem
     Display fest. Koordinaten 0,0 bezeichnet linke obere
     Position
    --------------------------------------------------------- */
void gotoxy(uint8_t x, uint8_t y)
{
  aktxp= x;
  aktyp= y;
  x *= 8;
  y= 7-y;

  i2c_start(ssd1306_addr);
  i2c_write(0x00);

  i2c_write(0xb0 | (y & 0x0f));
  i2c_write(0x10 | (x >> 4 & 0x0f));
  i2c_write(x & 0x0f);

  i2c_stop();
}


/*  ---------------------------------------------------------
                           clrscr

      loescht den Displayinhalt mit der in bkcolor ange-
      gebenen "Farbe" (0 = schwarz, 1 = hell)
    --------------------------------------------------------- */
void clrscr(void)
{
  uint8_t x,y;

  i2c_start(ssd1306_addr);
  i2c_write(0x00);

  i2c_write(0x8d);                  // Ladungspumpe an
  i2c_write(0x14);

  i2c_write(0xaf);                  // Display on

  i2c_write(0xa1);                  // Segment Map
  i2c_write(0xc0);                  // Direction Map

  i2c_stop();

  for (y= 0; y< 8; y++)                  // ein Byte in Y-Achse = 8 Pixel...
                                         // 8*8Pixel = 64 Y-Reihen
  {
    i2c_start(ssd1306_addr);
    i2c_write(0x00);

    i2c_write(0xb0 | y);            // Pageadresse schreiben
    i2c_write(0x00);                // MSB X-Adresse
    i2c_write(0x00);                // LSB X-Adresse (+Offset)

    i2c_stop();

    i2c_start(ssd1306_addr);
    i2c_write(0x40);
    for (x= 0; x< 128; x++)
    {

      if (bkcolor) i2c_write(0xff); else i2c_write(0x00);

    }
    i2c_stop();

  }
  gotoxy(0,0);
}

/*  ---------------------------------------------------------
                         oled_putchar

     gibt ein Zeichen auf dem Display aus. Steuerzeichen
     (fuer bspw. printf) sind implementiert:

             13 = carriage return
             10 = line feed
              8 = delete last char
    --------------------------------------------------------- */
void oled_putchar(uint8_t ch)
{
  uint8_t  i, b;
  uint8_t  z1;
  uint16_t z2[8];
  uint16_t z;

  if (ch== 0) return;

  if (ch== 13)                                          // Fuer <printf> "/r" Implementation
  {
    aktxp= 0;
    gotoxy(aktxp, aktyp);
    return;
  }
  if (ch== 10)                                          // fuer <printf> "/n" Implementation
  {
    aktyp++;
    if (doublechar) aktyp++;
    gotoxy(aktxp, aktyp);
    return;
  }

  if (ch== 8)
  {
    if ((aktxp> 0))
    {

      aktxp--;
      gotoxy(aktxp, aktyp);

      i2c_start(ssd1306_addr);
      i2c_write(0x40);
      for (i= 0; i< 8; i++)
      {
       if ((!textcolor)) i2c_write(0xff); else i2c_write(0x00);
      }
      i2c_stop();
      gotoxy(aktxp, aktyp);
    }
    return;

  }

  if (doublechar)
  {
    for (i= 0; i< 8; i++)
    {
      // Zeichen auf ein 16x16 Zeichen vergroessern
      z1= font8x8h[ch-' '][i];
      z2[i]= 0;
      for (b= 0; b< 8; b++)
      {
        if (z1 & (1 << b))
        {
          z2[i] |= (1 << (b*2));
          z2[i] |= (1 << ((b*2)+1));
        }
      }
    }

    i2c_start(ssd1306_addr);
    i2c_write(0x40);
    for (i= 0; i< 8; i++)
    {
      z= z2[i];
      if ((!textcolor)) z= ~z;
      z= z >> 8;
      i2c_write(z);
      i2c_write(z);
    }
    i2c_stop();
    gotoxy(aktxp, aktyp+1);

    i2c_start(ssd1306_addr);
    i2c_write(0x40);
    for (i= 0; i< 8; i++)
    {
      z= z2[i];
      if ((!textcolor)) z= ~z;
      z= z & 0xff;
      i2c_write(z);
      i2c_write(z);
    }
    i2c_stop();

    aktyp--;
    aktxp +=2;
    if (aktxp> 15)
    {
      aktxp= 0;
      aktyp +=2;
    }
    gotoxy(aktxp,aktyp);
  }
  else
  {

    i2c_start(ssd1306_addr);
    i2c_write(0x40);
    for (i= 0; i< 8; i++)
    {
      if ((!textcolor)) i2c_write(~(font8x8h[ch-' '][i]));
                   else i2c_write(font8x8h[ch-' '][i]);
    }
    i2c_stop();
    aktxp++;
    if (aktxp> 15)
    {
      aktxp= 0;
      aktyp++;
    }
    gotoxy(aktxp,aktyp);
  }
}

/* -----------------------------------------------------------------
     Ende SSD1306 OLED Display
   ----------------------------------------------------------------- */

/* #################################################################
     Funktionen PCF8574 I/O-Expander
   ################################################################# */

void pcf8574_write(uint8_t value)
{
  i2c_start(pcf8574_addr);
  i2c_write(value);
  i2c_stop();
}

uint8_t pcf8574_read(void)
{
  uint8_t b;

  pcf8574_write(0xff);
  i2c_start(pcf8574_addr | 1);                  // zum lesen
  b= i2c_read_nack();
  i2c_stop();
  return b;
}
