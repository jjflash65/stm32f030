/* --------------------------------------------------------------------------
                                set_dtr_rts.c

     Bitbanging der DTR und RTS Leitungen einer seriellen Schnittstelle,
     funktioniert auch mit einem CH340G USB-Chib

     23.03.2016     R. Seelig

   -------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

struct termios tio;                         // Zugriff um HUPCK Bit loeschen zu koennen

int main(int argc, char *argv[])
{
  int fd;
  int status;

  if (argc != 4)
  {
    printf("Syntax: set_dtr_rts  port                  DTR RTS\n");
    printf("Syntax: set_dtr_rts  /dev/ttyS0|/dev/ttyS1 0|1 0|1\n");
    exit( 1 );
  }

  if ((fd = open(argv[1],O_RDWR)) < 0)
  {
    printf("Failure: %s not present\n",argv[1]);
    exit(1);
  }

  tcgetattr(fd, &tio);
  tio.c_cflag &= ~HUPCL;             // HUPCL - Bit loeschen
  tcsetattr(fd, TCSANOW, &tio);

  ioctl(fd, TIOCMGET, &status);      // Status Serielle Schnittstelle

  if ( argv[2][0] == '1' )           // DTR setzen
    status &= ~TIOCM_DTR;
  else
    status |= TIOCM_DTR;             // ... oder loeschen

  if ( argv[3][0] == '1' )           // RTS setzen
    status &= ~TIOCM_RTS;
  else
    status |= TIOCM_RTS;             // ... oder loeschen

  ioctl(fd, TIOCMSET, &status);      // ... und neuen Status setzen

  close(fd);                         // Device schliessen
}

