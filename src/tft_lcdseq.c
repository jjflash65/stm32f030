/* ------------------------------------------------------------------------------------
                                 tft_lcdseq.c

     Initialisierungssequenzen fuer farbige TFT - Displays

     Unterstuetzte Displaycontroller:
           ili9163
           ili9340
           st7735r
           s6d02a1
           ili9225



     27.01.2019  R. Seelig
   ----------------------------------------------------------------------------------- */


#include "tftdisplay.h"

#define delay_flag 0x80                                // Markierungsflag: bestimmt, ob nach einem Kommando

/* ----------------------------------------------------------------------
                     TFT-Displays mit SPI-Interfaces
   ---------------------------------------------------------------------- */
#if ( (s6d02a1 == 1) || (ili9163 == 1) )
  static const uint8_t  lcdinit_seq[] =         // Initialisierungssequenzen
   {
    30,                                                // Anzahl Gesamtkommandos

  /*
    Byte 0 | Byte 1       | Byte 2 u. folgende | evtl. Delaytime-Byte
    CMD    | Anzahl Datas | Datas | evtl. Delaytime
  */

    0xf0, 2,0x5a,0x5a,
    0xfc, 2,0x5a,0x5a,                                                                   // Excommand3
    0x26, 1,0x01,                                                                        // Gammaset
    0xfa, 15,0x02,0x1f,0x00,0x10,0x22,0x30,0x38,0x3A,0x3A,0x3A,0x3A,0x3A,0x3d,0x02,0x01, // Positivegammacontrol
    0xfb, 15,0x21,0x00,0x02,0x04,0x07,0x0a,0x0b,0x0c,0x0c,0x16,0x1e,0x30,0x3f,0x01,0x02, // Negativegammacontrol
    0xfd, 11,0x00,0x00,0x00,0x17,0x10,0x00,0x01,0x01,0x00,0x1f,0x1f,                     // Analogparametercontrol
    0xf4, 15,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x07,0x00,0x3C,0x36,0x00,0x3C,0x36,0x00, // Powercontrol
    0xf5, 13,0x00,0x70,0x66,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6d,0x66,0x06,           // VCOMcontrol
    0xf6, 11,0x02,0x00,0x3f,0x00,0x00,0x00,0x02,0x00,0x06,0x01,0x00,                     // Sourcecontrol
                                                                                         // Displaycontrol
    0xf2, 17,0x00,0x01,0x03,0x08,0x08,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x04,0x08,0x08,
    0xf8, 1, 0x11,                                                                       // Gatecontrol
    0xf7, 4,0xc8,0x20,0x00,0x00,                                                         //Interfacecontrol
    0xf3, 2,0x00,0x00,                                                                   //Powersequencecontrol
    0x11, delay_flag,50,                                                                 //Wake
    0xf3, 2+delay_flag,0x00,0x01,50,                                                     //Powersequencecontrol
    0xf3, 2+delay_flag,0x00,0x03,50,
    0xf3, 2+delay_flag,0x00,0x07,50,
    0xf3, 2+delay_flag,0x00,0x0f,50,
    0xf4, 15+delay_flag,0x00,0x04,0x00,0x00,0x00,0x3f,0x3f,0x07,0x00,0x3C,0x36,0x00,0x3C,0x36,0x00,50,
    0xf3, 2+delay_flag,0x00,0x1f,50,
    0xf3, 2+delay_flag,0x00,0x7f,50,
    0xf3, 2+delay_flag,0x00,0xff,50,
    0xfd, 11,0x00,0x00,0x00,0x17,0x10,0x00,0x00,0x01,0x00,0x16,0x16,                     // Analogparametercontrol
    0xf4, 15,0x00,0x09,0x00,0x00,0x00,0x3f,0x3f,0x07,0x00,0x3C,0x36,0x00,0x3C,0x36,0x00,
    0x36, 1,0x08,                                                                        //Memoryaccessdatacontrol
    0x35, 1,0x00,                                                                        //Tearingeffectlineon
    0x3a, 1+delay_flag,0x05,150,                                                         //Interfacepixelcontrol

    #if ( negativout == 1)
      0x21, 0,
    #else
      0x20, 0,
    #endif

    #if ( rgbseq == 0 )
      0x36, 1, 0x00,                                      // 00 fuer auf dem Kopf stehend
                                                          // 00 fuer Kopf stehend, BGR Farbfolge
                                                          // 08 fuer Kopf stehend, RGB Farbfolge
    #endif

    #if ( rgbseq == 1 )
      0x36, 1, 0x08,                                      // 00 fuer auf dem Kopf stehend
                                                          // 00 fuer Kopf stehend, BGR Farbfolge
                                                          // 08 fuer Kopf stehend, RGB Farbfolge
    #endif

    0x29,0,                                                                              //Displayon
    0x2c,0                                                                               //Memorywrite
  };


#endif

#if (st7735r == 1)
  static const uint8_t  lcdinit_seq[] =            // Initialisierungssequenzen
  {
      21,                                                 // Anzahl Gesamtkommandos

      0x01, delay_flag,150,
      0x11, delay_flag,255,

      #if ( flickerreduce == 1)
        0xb1, 3, 0x00, 0x00, 0x00,
      #else
        0xb1, 3, 0x01, 0x2C, 0x2D,                                                         // Framerate Controll
      #endif
      0xb2, 3, 0x01, 0x2C, 0x2D,
      0xb3, 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,
      0xb4, 1, 0x07,
      #if ( flickerreduce == 1)
        0xc0, 3, 0xA2, 0x02, 0x44,                                                         // Power Control 1
      #else
        0xc0, 3, 0xA2, 0x02, 0x84,
      #endif
      0xc1, 1, 0xC5,
      0xc2, 2, 0x0A, 0x00,
      0xc3, 2, 0x8A, 0x2A,
      0xc4, 2, 0x8A, 0xEE,
      0xc5, 1, 0x0E,
      0x20, 0,

      #if ( rgbseq == 0 )
        0x36, 1, 0xc0,                                      // 00 fuer auf dem Kopf stehend
                                                            // C0 fuer Normal, BGR Farbfolge
                                                            // C8 fuer Normal, RGB Farbfolge
      #endif

      #if ( rgbseq == 1 )
        0x36, 1, 0xc8,                                      // 00 fuer auf dem Kopf stehend
                                                            // C0 fuer Normal, BGR Farbfolge
                                                            // C8 fuer Normal, RGB Farbfolge
      #endif

      0x3a, 1+delay_flag, 0x05,10,

      0x2a, 4, 0x00, 0x00, 0x00, 0x7F,
      0x2b, 4, 0x00, 0x00, 0x00, 0x9F,

      0xe0, 16, 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
                0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10,
      0xe1, 16, 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
                0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,
      0x13, delay_flag,10,
      0x29, delay_flag,100,
  };

#endif

#if (ili9340 == 1)
  static const uint8_t  lcdinit_seq[] =              // Initialisierungssequenzen
  {
    22,
                                                            // Anzahl Gesamtkommandos
  //
  //  Byte 0 | Byte 1       | Byte 2 u. folgende | evtl. Delaytime-Byte
  //  CMD    | Anzahl Datas | Datas | evtl. Delaytime
  //


    0x01,     delay_flag, 200,
    0xcb, 5,  0x39, 0x2c, 0x00, 0x34, 0x02,
    0xcf, 3,  0x00, 0xc1, 0x30,
    0xe8, 3,  0x85, 0x00, 0x78,
    0xea, 2,  0x00, 0x00,
    0xed, 4,  0x64, 0x03, 0x12, 0x81,
    0xf7, 1,  0x20,
    0xc0, 1,  0x23,                                // power control => VRH[5:0]
    0xc1, 1,  0x10,                                // power control => SAP[2:0]; BT[3:0]
    0xc5, 2,  0x3e, 0x28,                          // VCM control
    0xc7, 1,  0x86,                                // VCM control2
    0x36, 1,  0x48,                                // memory access controll
    0x3a, 1,  0x55,
    0xb1, 2,  0x00, 0x18,
    0xb6, 3,  0x08, 0x82, 0x27,                    // display control
    0xf2, 1,  0x00,                                // gamma function
    0x26, 1,  0x01,                                // gamma curve selected

    // set gama
    0xe0, 15, 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0x4e, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00,
    0xe1, 15, 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f,

    0x11,     delay_flag, 130,
    0x29,     delay_flag, 10,
    0x2c,     delay_flag, 10
  };
#endif

#if (ili9225 == 1)
  static const uint8_t  lcdinit_seq[] =              // Initialisierungssequenzen
      {
        34,                                                // Anzahl Gesamtkommandos

      /*
        Byte 0 | Byte 1       | Byte 2 u. folgende | evtl. Delaytime-Byte
        CMD    | Anzahl Datas | Datas | evtl. Delaytime
      */

        0x01, 2+delay_flag, 0x01,0x1c,50,
        0x02, 2, 0x01,0x00,
        0x03, 2, 0x10,0x38,
        0x08, 2, 0x08,0x08,

        0x0c, 2, 0x00,0x00,
        0x0f, 2, 0x0e,0x01,
        0x20, 2, 0x00,0x00,
        0x21, 2+delay_flag, 0x00,0x00,50,

        	//power ON sequence
        0x10, 2, 0x09,0x00,
        0x11, 2+delay_flag,0x10,0x38,50,

        0x12, 2, 0x11,0x21,
        0x13, 2, 0x00,0x65,
        0x14, 2, 0x50,0x58,

     	//set GRAM area
        0x30, 2, 0x00,0x00,
        0x31, 2, 0x00,0xdb,
        0x32, 2, 0x00,0x00,
        0x33, 2, 0x00,0x00,
        0x34, 2, 0x00,0xdb,
        0x35, 2, 0x00,0x00,
        0x36, 2, 0x00,0xaf,
        0x37, 2, 0x00,0x00,
        0x38, 2, 0x00,0xdb,
        0x39, 2, 0x00,0x00,

     	//adjust the gamma curve
        0x50, 2, 0x04,0x00,
        0x51, 2, 0x06,0x0b,
        0x52, 2, 0x0c,0x0a,
        0x53, 2, 0x01,0x05,
        0x54, 2, 0x0a,0x0c,
        0x55, 2, 0x0b,0x06,
        0x56, 2, 0x00,0x04,
        0x57, 2, 0x05,0x01,
        0x58, 2, 0x0e,0x00,
        0x59, 2+delay_flag, 0x00,0x0e,50,

        0x07, 2,0x10,0x17

      };
#endif

#if (st7789 == 1)
  static const uint8_t lcdinit_seq[] =              // Initialisierungssequenzen
      {
        9,                                                // Anzahl Gesamtkommandos

      /*
        Byte 0 | Byte 1       | Byte 2 u. folgende | evtl. Delaytime-Byte
        CMD    | Anzahl Datas | Datas | evtl. Delaytime
      */

        0x01, delay_flag, 150,
        0x11, delay_flag, 255,
        0x3a, 1+delay_flag, 0x55, 10,
        0x36, 1, 0x04,
        0x2a, 4, 0x00, 0x00, 0x00, 240,                        // Displaygroesse x
        0x2b, 4, 0x00, 0x00, 0x00, 240,                        // Displaygroesse Y
        0x21, delay_flag, 10,
        0x13, delay_flag, 10,
        0x29, delay_flag, 255

      };
#endif


/* ----------------------------------------------------------------------
                     TFT-Displays mit 8-Bit Parallelinterface
   ---------------------------------------------------------------------- */
#if (ili9341 == 1)
  static const uint8_t lcdinit_seq[] =                 // Initialisierungssequenzen
  {
    11,                                                // Anzahl Gesamtkommandos

  //  Byte 0 | Byte 1       | Byte 2 u. folgende | evtl. Delaytime-Byte
  //  CMD    | Anzahl Datas | Datas | evtl. Delaytime

    0xc0, 1,0x23,                   // Power control 1  : 4.6 Volt
    0xc1, 1,0x10,                   // Power control 2
    0xc5, 2,0x3e,0x28,              // VCOM  ctrl1 (Contrast: VMH= 4.25V, VML= -1.5V
    0xc7, 1,0x86,                   // VCOM  ctrl2
    0x36, 1,0x48,                   // MADCTL (memory acess ctrl): 0x48 RGB Mode
    0x3a, 1,0x55,                   // COLMOD (RGB Interface, 16-Bit Farben)
    0xb1, 2,0x00,0x18,              // Frame Rate: 0x00 = kein Teiler, 0x18 = 79Hz
    0xb6, 3,0x08,0x82,0x27,         // LCD Function ctrl , 0x08 = scann mode in none-display area, 0x82 = 85ms
    0x11, delay_flag, 120,          // LCD enable und 120 mS warten
    0x29, 0,                        // LCD an
    0x2c, 0                         // memory write
  };
#endif

#if (ili9481 == 1)
  static const uint8_t lcdinit_seq[] =                 // Initialisierungssequenzen
  {
    12,                                                // Anzahl Gesamtkommandos

  //  Byte 0 | Byte 1       | Byte 2 u. folgende | evtl. Delaytime-Byte
  //  CMD    | Anzahl Datas | Datas | evtl. Delaytime

    0x11,  delay_flag, 20,
    0xd0,  3,0x07,0x42,0x18,
    0xd1,  3,0x00,0x07,0x10,
    0xd2,  2,0x01,0x02,
    0xc0,  5,0x10,0x3b,0x00,0x02,0x11,
    0xc5,  1,0x03,
    0xc8,  12,0x00,0x32,0x36,0x45,0x06,0x16,0x37,0x75,0x77,0x54,0x0c,0x00,
    0x36,  1,0x0a,
    0x3a,  1,0x55,
    0x2a,  4,0x00,0x00,0x01,0x3f,
    0x2b,  4+delay_flag,0x00,0x00,0x01,0xe0,120,
    0x29,  0
  };
#endif

#if (ili9486 == 1)
  static const uint8_t lcdinit_seq[] =                 // Initialisierungssequenzen
  {
    14,                                                // Anzahl Gesamtkommandos

  //  Byte 0 | Byte 1       | Byte 2 u. folgende | evtl. Delaytime-Byte
  //  CMD    | Anzahl Datas | Datas | evtl. Delaytime

    0x0b,  2,0x00,0x00,
    0x11,  delay_flag, 20,
    0x3a,  1,0x55,
    0x36,  1,0x88,
    0xc2,  1,0x44,
    0xc5,  4,0x00,0x00,0x00,0x00,
    0xe0, 15,0x0f,0x1f,0x1c,0x0c,0x0f,0x08,0x48,0x98,0x37,0x0a,0x13,0x04,0x11,0x0d,0x00,
    0xe1, 15,0x0f,0x32,0x2e,0x0b,0x0d,0x05,0x47,0x75,0x37,0x06,0x10,0x03,0x24,0x20,0x00,
    0xe2, 15,0x0f,0x32,0x2e,0x0b,0x0d,0x05,0x47,0x75,0x37,0x06,0x10,0x03,0x24,0x20,0x00,
    0x11,  delay_flag, 20,
    0x29,  0,
    0x2a,  4,0x00,0x00,0x01,0x3f,
    0x2b,  4+delay_flag,0x00,0x00,0x01,0xe0,120,
    0x2c,  delay_flag, 20
  };
#endif
