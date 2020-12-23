/* ------------------------------------------------
                    tm1638_calc.c

     Demonstriert einen sehr einfachen "Ganzzahlen-
     taschenrechner mit umgekehrt polnischer
     Notation" in Verbindung mit einem TM1638
     Baustein an den 8 Stck. 7-Segmentanzeigen
     und 16 Tasten angeschlossen ist (China-Modul)

     MCU  : STM32F030
     Takt :

     Eingabe erfolgt nach diesem Schema:

     Zahl_1 - Enter - Zahl_2 *

     um Zahl_1 mit Zahl_2 zu multiplizieren.

     Tastenlayout ist:

         Calculator Keymap:

         7    8    9    /
         4    5    6    *
         1    2    3    -
        ent   0   clr   +

     Anschluesse STM32 nach TM1638 siehe
     tm1638.h

     02.03.2020  R. Seelig
   ----------------------------------------------- */


#include <libopencm3.h>

#include "sysf030_init.h"
#include "tm1638.h"


const uint8_t lauflseq [20] =
{ 0,1,2,3,4,5,6,7, 47, 39, 31,30,29,28,27,26,25,24,
  16, 8 };


/*  -----------------------------------------------------------------------------
                                          MAIN
    -----------------------------------------------------------------------------  */
int main(void)
{
  uint8_t i, k;
  uint32_t op1, op2;
  uint8_t op1read;
  uint8_t cmd;

  sys_init();

  tm1638_init();
  tm1638_brightness= 3;

  // Demo einzelne Segmente im Lauflicht auf- und abblenden
  fb1638_clr();
  for (i= 0; i< 20; i++)
  {
    k= lauflseq[i];
    fb1638_putseg(k,1);
    tm1638_showbuffer();
    delay(70);
  }
  for (i= 0; i< 20; i++)
  {
    k= lauflseq[i];
    fb1638_putseg(k,0);
    tm1638_showbuffer();
    delay(70);
  }
  delay(200);

  // Demo Text einscrollen
  for (i= 0; i< 8; i++)
  {
    fb1638_clr();
    fb1638_puts("1nt CALC",i);
    tm1638_showbuffer();
    delay(100);
  }

  // und Dezimalpunkt setzen
  tm1638_setdp(5,1);
  delay(1300);

  // Demo Integer Calculator
  op1read= 1;
  while(1)
  {
    if (op1read)
    {
      do
      {
        op1= 0;
        cmd= tm1638_readint(&op1);
        tm1638_clear();
        delay(200);
        tm1638_setdez(op1,0,1);
      } while (cmd != 0x0d);          // erster Operand muss mit Enter abgeschlossen sein
      op1read= 0;
    }
    op2= op1;
    do
    {
      cmd= tm1638_readint(&op2);
      tm1638_clear();
      delay(200);
      if (cmd == 0x0d) op2= 0;
    } while(cmd == 0x0d);             // zweiter Operand darf nicht mit ENTER abgeschlossen sein

    switch (cmd)
    {
      case 0x18 :                     // clr
      {
        op1= 0; op2= 0; op1read= 1;   // Operande loeschen und flag fuer ersten Operanten lesen
        break;
      }
      case '+' :
      {
        op1 += op2;
        tm1638_setdez(op1,0,1);
        break;
      }
      case '-' :
      {
        op1 -= op2;
        tm1638_setdez(op1,0,1);
        break;
      }
      case '*' :
      {
        op1 *= op2;
        tm1638_setdez(op1,0,1);
        break;
      }
      case '/' :
      {
        if (!op2)                       // division by zero
        {
          fb1638_puts("Err     ", 7);
          tm1638_showbuffer();
          op1= 0; op2= 0; op1read= 1;   // Operande loeschen und flag fuer ersten Operanten lesen
          do
          {
            k= tm1638_readkeys();
            k= calckeymap[k-1];
          } while (k != 0x18);
        }
        else
        {
          op1 /= op2;
          tm1638_setdez(op1,0,1);
        }
        break;
      }
      default : break;
    }
  }

  while(1);
}
