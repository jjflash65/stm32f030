    --------------------------------------
    "Sinn von tbasic", kurze Beschreibung:
    --------------------------------------

    Grundsaetzlich ging es darum, so etwas einmal zu realisieren.
    Dieser einfacher Interpreter kann evtl. dazu dienen, einfachste
    Programme zu erstellen. Vllt. erweitertert man diese Firmware
    hier in der Art, dass beim Start ein GPIO abgefragt wird und je
    nach Logigzustand (der von aussen ueber Jumper gesetzt werden
    kann, wird das gespeicherte Programm ausgefuehrt).

    Ebenso ist denkbar, den Befehl "scall" um eigene "Systemfunktionen"
    in C zu erweitern, die dann aus diesem Basic hier aufgerufen
    werden (im Moment gibt es nur ein einziger SCALL: Ausgabe eines
    Ascii-Zeichens).

    Die Struktur dieses Programmes hier ist, sagen wir es einmal
    freundlich, etwas chaotisch. Ich habe das ganze nur an einen
    STM32F030 angepasst (und ein bisschen erweitert). Vllt. raeume
    ich es auch einmal auf, damit es besser ausschaut.

    Variablenamen koennen nur einen einzigen Buchstaben haben (a-z),
    somit sind 26 Variable vorhanden.

    Besonderheiten:
    ---------------------------------------------------------------
        - Ein laufendes Programm (in einer Endlosschleife) wird mit
          senden des ESC-Zeichens (Ascii 27) beendet.

        - Um nicht ein Basicprogramm ueber ein Terminal eingeben
          zu muessen, kann eine Textdatei erstellt werden und diese
          mittels "sendbas" geschickt werden. Hierzu wird ein
          Terminal fuer den Interpreter geoeffnet und in einem
          anderen Konsolenfenster "sendbas" mit den entsprechenden
          Parametern gestartet.

          Beispiel:

                sendbas testprog.bas /dev/ttyUSB0

          Uebersetzen des Programms "sendbas" mit:

                ./compilesendbas

    Zusaetzliche Befehle:
    ---------------------------------------------------------------

    SCALL
    -----

    Zusaetzlich zu den elementaren Basic Befehlen wurde SCALL ein-
    gefuehrt. SCALL wird mit 2 Argumenten aufgerufen:

        - Argument 1 ist die Systemfunktion die aufgerufen werden
          soll
        - Argument 2 ist ein Parameter fuer diese Funktion

    Im Moment gibt es nur eine einzige Systemfunktion, die der Aus-
    gabe eines Ascii-Zeichens:

        SCALL 1,65

    gibt hier den Buchstaben 'A' aus (65 ist der Ascii-Code fuer A)
    In der Funktion void sysfunc(int16_t v1, int16_t v2) koennen
    weitere Systemcalls definiert werden

    IN / OUT
    --------

    Um mit dem Basicinterpreter etwas anfangen zu koennen, wurden
    ihm die Befehle IN bzw. OUT hinzugefuegt.

    Ein Einlesen des Logikzustands an PA5 geschieht folgenderweise:

        if in(5)= 1 print"PA5= 1"

    Ein Setzen von PA7 auf logisch 1 erfolgt durch:

        out(7)= 1

    Die Zuordnung der IO-Nummern zu den Portbits ist wie folgt:


       0   : PA0
       1   : PA1
       4   : PA4
       5   : PA5
       6   : PA6
       7   : PA7
       8   : PB1
       9   : PA9
       10  : PA10
       13  : PA13
       14  : PA14

    Array
    -----

    Es ist nur ein einziges Array verfuegbar. Dieses Array wird
    mit dem '@' Zeichen adressiert.

    REM  Das 5. Element im Array erhaelt den Wert 12
    @(5) = 12

    REM  Das 9. Element im Array erhaelt den Wert 12
    @(5) = 25

    print @(5)

    REM Die Ausgabe ist 12

    ---------------------------------------------------------------

