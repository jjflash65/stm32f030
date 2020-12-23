/* -----------------------------------------------------
                        i2c_explore.c

    Demo fuer softwareimplementierten (Bitbanging)
    I2C-Bus.

    Unterstuetzte I2C-Devices:
         - DS1307  (real time clock)
         - RDA5807 (UKW receiver)
         - LM75    (Temperatursensor)
         - 24LCxx  (EEProm)
         - SSD1306 (OLED-Display)


    Hardware  : STM32F030F4P6
    IDE       : make - Projekt
    Library   : libopencm3
    Toolchain : arm-none-eabi

    02.03.2020   R. Seelig
  ------------------------------------------------------ */


#include <stdint.h>
#include <string.h>

#include <libopencm3.h>
#include "sysf030_init.h"
#include "uart.h"
#include "my_printf.h"

#include "i2c_devices_soft.h"

#define printf   my_printf

enum { uart_stream, oled_stream };

uint8_t outstream = uart_stream;


/* ########################################################
                   "Helferfunktionen"
   ######################################################## */

/* --------------------------------------------------
      DEZ2BCD

      wandelt eine dezimale Zahl in eine BCD
      Bsp: value = 45
      Rueckgabe    0x45
   -------------------------------------------------- */
uint8_t dez2bcd(uint8_t value)
{
  uint8_t hiz,loz,c;

  hiz= value / 10;
  loz= (value -(hiz*10));
  c= (hiz << 4) | loz;
  return c;
}

/* --------------------------------------------------
      binout

      gibt einen Bytewert binaer auf der seriellen
      Schnittstelle aus
   -------------------------------------------------- */
void binout(uint8_t value)
{
  uint8_t b;

  for (b= 8; b != 0; b--)
  {
    if (value & (1 << (b-1))) uart_putchar('o'); else uart_putchar('.');
  }
}

/* --------------------------------------------------
     readint

     liest einen Integerzahlenwert ueber die RS-232
     ein. Ein Punkt '.' kann zwar eingegeben werden,
     wird aber vor der Rueckgabe von den eingelesenen
     Zeichen entfernt.

     Bsp.:
     Eingabe von 99.8 wird als 998 zurueckgegeben

     Parameter:
         outtext : Zeiger auf einen String, der
                   vor einer Werteeingabe ausge-
                   geben wird.
         maxanz  : Anzahl maximal eingebarer Ziffern
   -------------------------------------------------- */
int readint(char *outtext, char maxanz)
{
  char  ch;
  char  inbuf[11];
  char  *inptr;
  char  inanz;
  char  dotflag;
  int   z;

  inanz= 0;
  inptr= &inbuf[0];
  *inptr= 0;
  dotflag= 0;
  do
  {
     printf("\r%s%s ", outtext, inbuf);
     printf("\r%s%s", outtext, inbuf);
     ch= uart_getchar();
     if ( (ch== '.') && (!dotflag))
     {
       dotflag++;
       *inptr= ch;
       inptr++;
       *inptr= 0;
       inanz++;
     }
     if ( (ch >= '0') && (ch<= '9') && (inanz< maxanz) )
     {
       *inptr= ch;
       inptr++;
       *inptr= 0;
       inanz++;
     }

     // Back delete
     if ((ch== 0x7f) && (inanz> 0))
     {
       inptr--;
       if (*inptr== '.') dotflag--;
       *inptr= 0;
       inanz--;
     }
  } while( !((ch== 0x0d) || (ch== 0x0a)) );

  inptr= strchr(inbuf, '.');
  if (inptr) strcpy(inptr, inptr+1);
  z= atoi(inbuf);
  return z;
}

/* --------------------------------------------------
     readstr

     liest einen Text ueber RS232 ein. Eine
     Korrektur des eingegebenen Textes ist NICHT
     moeglich.

     Uebergabe:
       *buf   : Zeiger auf einen Speicherbereich, der
                den eingegebenen Text aufnimmt
       maxnz  : maximale Zeichenanzahl, die ein-
                gegeben werden kann.
   -------------------------------------------------- */
void readstring(uint8_t *buf, int maxanz)
{
  char ch;
  int  cx;

  memset(buf,0, maxanz+1);              // Buffer mit Endekennungen auffuellen
  cx= 0;
  do
  {
    ch= uart_getchar();
    if (!((ch == 0x0d) || (ch == 0x0a)))
    {
      if (cx< maxanz-2)
      {
        if (ch== '#')
        {
          *buf = '\n';
          buf++;
          *buf = '\r';
          buf++;
          cx += 2;
          printf("\n\r");
        }
        else
        {
          *buf = ch;
          buf++;
          my_putchar(ch);
          cx++;
        }
      }
    }
  } while( !((ch== 0x0d) || (ch== 0x0a)) );
  buf++;
  *buf= 0;
}

/* --------------------------------------------------------
                  Ende Helferfunktioneen
   -------------------------------------------------------- */

/* ########################################################
     Radio - Bedienerinterface (mittels UART)
   ######################################################## */

/* --------------------------------------------------
     show_tune

     zeigt die Empfangsfrequenz und die aktuell
     eingestellte Lautstaerke an
   -------------------------------------------------- */
void show_tune(void)
{
  char i;

  if (aktfreq < 1000) { my_putchar(' '); }
  printf("  %k MHz  |  Volume: ",aktfreq);

  if (aktvol)
  {
    my_putchar('0');
    for (i= 0; i< aktvol-1; i++) { my_putchar('-'); }
    my_putchar('x');
    i= aktvol;

    while (i< 15)
    {
      my_putchar('-');
      i++;
    }
  }
  else
  {
    printf("x--------------");
  }
  printf("  \r");
}

/* --------------------------------------------------
     setnewtune

     setzt eine neue Empfangsfrequenz und zeigt die
     neue Frequenz an.
   -------------------------------------------------- */
void setnewtune(uint16_t channel)
{
  aktfreq= channel;
  rda5807_setfreq(aktfreq);
  show_tune();
}


/* --------------------------------------------------
     radio_ctrl

     Bedienerinterface UKW-Radio
   -------------------------------------------------- */
void radio_ctrl(void)
{
  char ch;

  char spacestr[] = "\r                                        ";

  printfkomma= 1;                       // my_printf verwendet mit Formatter %k eine Kommastelle

  printf("\n\n\r  ----------------------------------\n\r");
  printf(      "    UKW-Radio mit I2C-Chip RDA5807\n\r");
  printf(      "  ----------------------------------\n\n\r");
  printf(      "      (+)     Volume+\n\r");
  printf(      "      (-)     Volume-\n\n\r");
  printf(      "      (u)     Empfangsfrequenz hoch\n\r");
  printf(      "      (d)     Empfangsfrequenz runter\n\r");
  printf(      "      (f)     Senderdirekteingabe\n\n\r");
  printf(      "      (1..6)  Stationstaste\n\n\r");
  printf(      "      (e)     Radio aus (Ende)\n\n\r");

  rda5807_reset();
  rda5807_poweron();
  rda5807_setmono();
  rda5807_setfreq(aktfreq);
  rda5807_setvol(aktvol);

  show_tune();

  while(1)
  {
    ch= 0;
    ch= uart_getchar();
    switch (ch)
    {
      case '+' :                            // Volume erhoehen
        {
          if (aktvol< 15)
          {
            aktvol++;
            rda5807_setvol(aktvol);
            my_putchar('\r');
            show_tune();
          }
          break;
        }

      case '-' :
        {
          if (aktvol> 0)                      // Volume verringern
          {
            aktvol--;
            rda5807_setvol(aktvol);
            my_putchar('\r');
            show_tune();
          }
          break;
        }

      case 'd' :                           // Empfangsfrequenz nach unten
        {
          if (aktfreq > fbandmin)
          {
            aktfreq--;
            setnewtune(aktfreq);
            delay(20);
            show_tune();
          }
        break;
        }
      case 'u' :                           // Empfangsfrequenz nach oben
        {
          if (aktfreq < fbandmax)
          {
            aktfreq++;
            setnewtune(aktfreq);
            delay(20);
            show_tune();
          }
          break;
        }
      case 'f' :                           // Senderdirekteingabe
        {
          printf("%s", spacestr);
          aktfreq= readint("Eingabe Freq.: ",5);
          printf("%s\r", spacestr);
          setnewtune(aktfreq);
          delay(20);
          show_tune();
          break;
        }


      case 'e' :
        {
          rda5807_setvol(0);
          rda5807_reset();
          return ;
          break;
        }

      default  : break;
    }

    if ((ch >= '1') && (ch <= '6'))
    {
      setnewtune(festfreq[ch-'0'-1]);
      show_tune();
    }
  }

}

//   Ende Radio Bedienerinterface
// -------------------------------------------------------------------------


/* --------------------------------------------------
      RTC_SHOWTIME

      liest das DS1307 Uhrenmodul aus und zeigt die
      Zeit an.
   -------------------------------------------------- */
void rtc_showtime(void)
{
  struct my_datum date;

  char tagnam[7][3] =
  {
    "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"
  };

  date= rtc_readdate();

  printf("\r %s  ", tagnam[date.dow]);
  printf("%x.%x.20%x  %x.%x:%x ", date.tag, date.monat, date.jahr,       \
                                  date.std, date.min, date.sek);

}


/* --------------------------------------------------
     rtc_stellen

     die Uhr benutzerabgefragt stellen
   -------------------------------------------------- */
void rtc_stellen(void)
{
  uint8_t   b;

  struct my_datum date;

  do
  {
    printf("\n\r");
    b= readint("Tag: ", 2);
  } while ( !((b > -1) && (b < 32)));
  b= dez2bcd(b);
  date.tag= b;

  do
  {
    printf("\n\r");
    b= readint("Monat: ", 2);
  } while ( !((b > -1) && (b < 13)));
  b= dez2bcd(b);
  date.monat= b;

  do
  {
    printf("\n\r");
    b= readint("Jahr (0..99): ",2);
  } while ( !((b > -1) && (b < 100)));
  b= dez2bcd(b);
  date.jahr= b;

  do
  {
    printf("\n\r");
    b= readint("Stunde: ",2);
  } while ( !((b > -1) && (b < 24)));
  b= dez2bcd(b);
  date.std= b;

  do
  {
    printf("\n\r");
    b= readint("Minute: ",2);
  } while ( !((b > -1) && (b < 60)));
  b= dez2bcd(b);
  date.min= b;

  do
  {
    printf("\n\r");
    b= readint("Sekunde: ",2);
  } while ( !((b > -1) && (b < 60)));
  b= dez2bcd(b);
  date.sek= b;

  rtc_writedate(&date);

}

/* --------------------------------------------------
     rtc_ctrl

     Bedienerinterface RTC Uhr
   -------------------------------------------------- */
void rtc_ctrl(void)
{
  char ch;

  struct my_datum date;
  uint8_t oldsek;

  ch= 0;
  oldsek= 0;

  delay(50);

  date= rtc_readdate();
  oldsek= date.sek-1;
  while(1)
  {
    printf("\n\n\r  ----------------------------------\n\r");
    printf(      "    DS1307 RTC\n\r");
    printf(      "  ----------------------------------\n\n\r");
    printf(      "      (z)     Zeige Uhrzeit\n\r");
    printf(      "      (s)     Uhr stellen\n\r");
    printf(      "      (e)     Ende\n\n\r");

    ch= uart_getchar();
    switch (ch)
    {
      case 'e' : return; break;
      case 's' : rtc_stellen(); break;
      case 'z' :
      {
        do
        {
          ch= 0;
          if ((uart_ischar())) ch= uart_getchar();
          delay(10);

          date= rtc_readdate();
          if (oldsek != date.sek)
          {
            rtc_showtime();
            oldsek= date.sek;
          }
        }while(!(ch));
        break;
      }
      default : break;
    }
  }
}

/* --------------------------------------------------
     eep_ctrl

     Bedienerinterface RTC Uhr
   -------------------------------------------------- */

void eep_ctrl(void)
{
  char  ch;
  uint8_t  textbuf[129];

  do
  {
    printf("\n\n\r  ----------------------------------\n\r");
    printf(      "    EEPROM 24LCxx\n\r");
    printf(      "  ----------------------------------\n\n\r");
    printf(      "      (d)     EEProm loeschen\n\r");
    printf(      "      (w)     Text eingeben und speichern\n\r");
    printf(      "      (r)     Text aus EEProm anzeigen\n\r");
    printf(      "      (s)     Scan memorysize\n\n\r");
    printf(      "      (e)     Ende\n\r");

    ch= uart_getchar();
    switch(ch)
    {
      case 'd' :
      {
        printf("\n\r EEProm loeschen... ");
        eep_erase();
        printf("\n\r done...\n\r");
        break;
      }
      case 's' :
      {
        printf("\n\r Scan memorysize ");
        printf("\n\n\r Dieser Vorgang schreibt auf das EEProm");
        printf(  "\n\r und zerstoert somit einige Inhalte !!!");
        printf("\n\n\r Vorgang fortsetzen ( J / N )");
        ch= uart_getchar();
        if ((ch == 'j') || (ch == 'J'))
        {
          printf("\n\n\r ############################################");
          printf("\n\n\r Speichergroesse EEProm: %d Bytes", eep_getmemsize());
          printf("\n\n\r ############################################");
        }
        printf("\n\r done...\n\r");
        break;
      }
      case 'w' :
      {
        printf("\n\rText eingeben, der im EEProm gespeichert wird.");
        printf("\n\rEin Zeilenumbruch kann mit dem '#' Zeichen");
        printf("\n\reingegeben werden. Beenden der Texteingabe");
        printf("\n\rmit Enter.\n\n\r");
        readstring(&textbuf[0], 128);
        eep_writebuf(0x10, &textbuf[0], 128);
        break;
      }
      case 'r' :
      {
        eep_readbuf(0x10, &textbuf[0], 128);
        printf("\n\n\r%s\n\r", textbuf);
        ch= uart_getchar();
        break;
      }
      case 'e' :
      {
        break;
      }
      default : break;
    }
  }while (ch != 'e');

}

/* --------------------------------------------------
     oled_txtoutput

     Das OLED-Display als "Displayschreibmaschine"
     verwenden: Jeder Tastendruck der ueber den
     UART empfangen wird, wird auf dem Display
     ausgegeben
   -------------------------------------------------- */
void oled_txtoutput(void)
{
  char ch;
  char first = 1;

  printf("\n\r Jede Tasteneingabe die getaetigt wird, wird");
  printf("\n\r auch auf dem Display ausgegeben.\n\r");
  printf("\n\r Mit '#' ist ein Zeilenumbruch moeglich,");
  printf("\n\r ein '*' loescht den Displayinhalt,");
  printf("\n\r die Entertaste beendet die Anzeige\n\n\r");

  clrscr();
  gotoxy(0,0);

  outstream= oled_stream;
  printf("Geben sie einen\n\rText ein !!!");
  outstream= uart_stream;

  do
  {
    ch= uart_getchar();
    if (first) { clrscr(); first= 0; }
    if (ch == '#')
    {
      printf("\n\r");
      oled_putchar(0x0d); oled_putchar(0x0a);
    }
    else
    {
      if (ch== '*')
      {
        clrscr();
        gotoxy(0,0);
        printf("\n\r");
      }
      else
      {
        uart_putchar(ch);
        oled_putchar(ch);
      }
    }
  }while ((ch != 0x0a) && (ch != 0x0d));
}

/* --------------------------------------------------
     oled_txtfromeep

     Text, der im EEProm gespeichert ist, auf dem
     Display anzeigen
   -------------------------------------------------- */
void oled_txtfromeep()
{
  uint8_t ack;
  uint8_t textbuf[129];

  ack= i2c_start(eep_addr);
  if (!(ack))
  {
    printf("\n\r EEProm nicht verfuegbar oder nicht angeschlossen...\n\r");
    return;
  }
  i2c_stop();

  eep_readbuf(0x10, &textbuf[0], 128);

  outstream= oled_stream;
  ssd1306_init();
  clrscr();

  printf("%s", textbuf);
  outstream= uart_stream;
  printf("\n\r done...\n\r");
}

/* --------------------------------------------------
     oled_txtfromeep

     Zeigt Temperatur (aus LM75), Datum und einge-
     stellter Sender (aus RDA5807) auf dem Display
     an
   -------------------------------------------------- */
void oled_rtc_temp_ukw(void)
{
  struct my_datum date;

  uint8_t  is_rda, is_lm75, is_rtc;

  uint8_t abr;
  char    ch;

  char tagnam[7][3] =
  {
    "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"
  };


  is_rda= i2c_start(rda5807_adrr << 1);
  i2c_stop();
  is_lm75= i2c_start(lm75_addr);
  i2c_stop();
  is_rtc= i2c_start(rtc_addr);
  i2c_stop();
  outstream= oled_stream;
  clrscr();
  printf("I2C-Device check\n\r---------------\n\r");
  printf("RDA5807: ");
  if (is_rda) printf("ok"); else printf("n.a.");
  printf("\n\rLM75   : ");
  if (is_lm75) printf("ok"); else printf("n.a.");
  printf("\n\rDS1307 : ");
  if (is_rtc) printf("ok"); else printf("n.a.");
  delay(1000);

  clrscr();
  printfkomma= 1;
  doublechar= 1;
  if (is_rda)
  {
    printf("%k",aktfreq);
    doublechar= 0;
    printf(" MHz");

    rda5807_reset();
    rda5807_poweron();
    rda5807_setmono();
    rda5807_setfreq(aktfreq);
    rda5807_setvol(aktvol);
  }
  else
  {
    printf("n.a.");
    doublechar= 0;
    printf("MHz");
  }
  gotoxy(0,2); printf("----------------");
  gotoxy(0,3);
  printf("Temp.:");

  abr= 0;
  while(!(abr))
  {
    if (tick_ms > 500)
    {
      tick_ms= 0;
      gotoxy(7,3);
      if (is_lm75) printf("%k %cC", lm75_read(), 0x81); else printf("n.a.");   // 0x81 Zeichen fuer hochgestelltes o

      gotoxy(3,5);
      if (is_rtc)
      {
        date= rtc_readdate();

        printf("%x.%x:%x\n\n\r",date.std, date.min, date.sek);

        printf("%s  ", tagnam[date.dow]);
        printf("%x.%x.20%x", date.tag, date.monat, date.jahr);
      }
      else
      {
        printf("RTC n.a.");
      }
    }
    if (uart_ischar())
    {
      ch= uart_getchar();
      switch (ch)
      {
        case '+' :                            // Volume erhoehen
          {
            if (aktvol< 15)
            {
              aktvol++;
              rda5807_setvol(aktvol);
            }
            break;
          }

        case '-' :
          {
            if (aktvol> 0)                      // Volume verringern
            {
              aktvol--;
              rda5807_setvol(aktvol);
            }
            break;
          }
        default  : abr= 1; break;
      }
    }

  }
  if (is_rda)
  {
    rda5807_setvol(0);
    rda5807_reset();
  }

  clrscr();
  printf("SSD1306 - OLED");

  outstream= uart_stream;
  printf("\n\r done...");
}

/* --------------------------------------------------
     oled_ctrl

     Menu zur Steuerung der Displayfunktionen
   -------------------------------------------------- */
void oled_ctrl(void)
{
  char  ch;
  uint8_t ack;

  ack= i2c_start(ssd1306_addr);
  if (!(ack))
  {
    printf("\n\r SSD1306 nicht verfuegbar oder nicht angeschlossen...\n\r");
    return;
  }
  i2c_stop();
  ssd1306_init();
  clrscr();
  gotoxy(0,0);
  outstream= oled_stream;
  printf("SSD1306 - OLED");
  outstream= uart_stream;

  do
  {
    printf("\n\n\r  ----------------------------------\n\r");
    printf(      "    SSD1306 OLED-Demo\n\r");
    printf(      "  ----------------------------------\n\n\r");
    printf(      "      (w)     Displayschreibmaschine\n\r");
    printf(      "      (t)     Text aus EEProm anzeigen\n\r");
    printf(      "      (c)     Display loeschen\n\r");
    printf(      "      (a)     Temp., Uhr und Radiofreq. anzeigen\n\n\r");
    printf(      "      (e)     Ende\n\r");

    ch= uart_getchar();
    switch(ch)
    {
      case 'w' : oled_txtoutput(); break;
      case 't' : oled_txtfromeep(); break;
      case 'c' : clrscr(); break;
      case 'a' : oled_rtc_temp_ukw(); break;
      default : break;
    }
  }while (ch != 'e');
}

/* --------------------------------------------------
     i2c_scanbus

     scant den I2C-Bus nach angeschlossenen
     Einheiten
   -------------------------------------------------- */
void i2c_scanbus(void)
{
  uint8_t   ack;
  uint16_t  cx;

  printf("\n\n\rI2C Bus scanning\n\r--------------------------\n\n\r" \
         "Devices found at address:\n\n\r");

  for (cx= 2; cx< 255; cx+= 2)
  {
    ack= i2c_start(cx);
    delay(1);
    i2c_stop();
    if (ack)
    {
      switch (cx)
      {
        case 0xC0 : printf("Adr. %xh : TEA5767 UKW-Radio\n\r", cx); break;
        case 0x20 :
        case 0x22 : printf("Adr. %xh : RDA5807 UKW-Radio\n\r", cx); break;
        case 0x40 :
        case 0x42 :
        case 0x44 :
        case 0x46 :
        case 0x48 :
        case 0x4A :
        case 0x4C :
        case 0x4E : printf("Adr. %xh : PCF8574 I/O Expander\n\r", cx); break;
        case 0x90 :
        case 0x92 :
        case 0x94 :
        case 0x96 :
        case 0x98 :
        case 0x9A :
        case 0x9C :
        case 0x9E : printf("Adr. %xh : LM75 Temp.-Sensor\n\r", cx); break;
        case 0xA0 :
        case 0xA2 :
        case 0xA4 :
        case 0xA6 :
        case 0xA8 :
        case 0xAA :
        case 0xAC :
        case 0xAE : printf("Adr. %xh : EEProm\n\r", cx); break;
        case 0x78 : printf("Adr. %xh : SSD13016 I2C-OLED Display\n\r", cx); break;
        case 0xD0 : printf("Adr. %xh : RTC - DS1307\n\r", cx); break;

        default   : printf("Adr. %xh : unknown\n\r",cx); break;
      }
    }
    delay(1);
  }
  printf("\n\n\rEnd of I2C-bus scanning... \n\r");
  i2c_stop();

  printf("\n\r... done ! \n\r");
}

/* --------------------------------------------------
     lm75_show

     zeigt die Temperatur aus LM75 ueber den
     UART an
   -------------------------------------------------- */
void lm75_show(void)
{
  char ch;
  int  temp, cx;

  cx= 0;
  printfkomma= 1;                       // my_printf verwendet mit Formatter %k eine Kommastelle
  printf("\n\r");
  do
  {
    ch= 0;
    if ((uart_ischar())) ch= uart_getchar();
    delay(10);

    if (!cx)
    {
      cx= 0;
      temp= lm75_read();
      printf("\r Temp.: %k [Grad Celsius]", temp);
    }
    cx++;
    cx= cx % 50;
  }while(!(ch));
  printf("\n\r");
}

/* --------------------------------------------------
     pcf8574_toggleoutput

     toggelt die Ausgabepins des I/O Bausteins
     PCF8574
   -------------------------------------------------- */
void pcf8574_toggleoutput(void)
{
  static uint8_t outbyte = 0x00;
  uint8_t b, ch;

  printf("\n\r Tasten 0..7 zum ein/ausschalten der Ausgaenge");
  printf("\n\r Taste e fuer Ende\n\r");
  pcf8574_write(outbyte);
  do
  {
    ch= uart_getchar();
    switch (ch)
    {
      case '0' : case '1' : case '2' : case '3' :
      case '4' : case '5' : case '6' : case '7' :
      {
        b= ch - '0';
        outbyte= outbyte ^ (1 << b);
        pcf8574_write(outbyte);
      }
    }
  }while (ch != 'e');
}

/* --------------------------------------------------
     pcf8574 ctrl

     Menu fuer die Funktionen des I/O Bausteins
   -------------------------------------------------- */
void pcf8574_ctrl(void)
{
  char    ch;
  uint8_t ack;
  uint8_t l, lr;

  ack= i2c_start(pcf8574_addr);
  if (!(ack))
  {
    printf("\n\r PCF8574 nicht verfuegbar oder nicht angeschlossen...\n\r");
    return;
  }
  i2c_stop();

  do
  {
    printf("\n\n\r  ----------------------------------\n\r");
    printf(      "    PCF8574 I/O - Demo\n\r");
    printf(      "  ----------------------------------\n\n\r");
    printf(      "      (l)     Lauflich (LED gegen +)\n\r");
    printf(      "      (o)     Ausgaenge schalten\n\r");
    printf(      "      (i)     Pins einlesen\n\r");
    printf(      "      (e)     Ende\n\r");

    ch= uart_getchar();
    switch(ch)
    {
      case 'l' :                            // Knightrider-Lauflich
      {
        l= 0x01; lr= 0xff;
        do
        {
          ch= 0;
          if ((uart_ischar())) ch= uart_getchar();
          pcf8574_write(~l);
          if (lr) l= l << 1; else l= l >> 1;
          if (!(l))
          {
            if (lr) l= 0x40; else l= 0x02;
            lr= ~lr;
          }
          delay(100);
        }while(!(ch));
        ch= 0;
        break;
      }
      case 'i' :
      {
        printf("\n\r Einlesen Ende mit beliebiger Taste...\n\n\r");
        do
        {
          l= pcf8574_read();
          printf("\r Input (hex): %xh   ", l);
          binout(l);
          uart_putchar(' ');
          delay(150);
        }while(!(uart_ischar()));
        ch= uart_getchar();
        ch= 0;
        break;
      }
      case 'o' :
      {
        pcf8574_toggleoutput();
        break;
      }
      default : break;
    }
  }while (ch != 'e');
}

/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf einen Bytestream sendet !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  switch (outstream)
  {
    case uart_stream :  uart_putchar(ch); break;
    case oled_stream :  oled_putchar(ch); break;
    default          :  break;
  }
}


/* -------------------------------------------------------------
                              main
   ------------------------------------------------------------- */
int main(void)
{
  char  ch;

  sys_init();
  delay(150);
  uart_init(19200);
  i2c_master_init();

  outstream= uart_stream;
  i2c_scanbus();

  ssd1306_init();
  clrscr();

  while(1)
  {
    printf("\n\n\r  ----------------------------------\n\r");
    printf(      "    I2C - Demo\n\r");
    printf(      "  ----------------------------------\n\n\r");
    printf(      "      (l)     List I2C - Devices (scan) bus\n\r");
    printf(      "      (u)     Uhr (DS1307)\n\r");
    printf(      "      (t)     Temperatur (LM75)\n\r");
    printf(      "      (p)     EEProm (24LCxx)\n\r");
    printf(      "      (o)     SSD1306 OLED-Display\n\r");
    printf(      "      (i)     PCF8574 I/O-Expander\n\r");
    printf(      "      (r)     UKW-Radio (RDA5807) bus\n\n\r");

    ch = uart_getchar();
    switch(ch)
    {
      case 'l' : i2c_scanbus(); break;
      case 'r' : radio_ctrl(); break;
      case 'u' : rtc_ctrl(); break;
      case 't' : lm75_show(); break;
      case 'o' : oled_ctrl(); break;
      case 'i' : pcf8574_ctrl(); break;
      case 'p' : eep_ctrl(); break;
      default  : break;
    }
  }
}
