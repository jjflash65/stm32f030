/* -----------------------------------------------------------------------------
                                  math_fixed.h

     Header fuer mathematische Funktionen mittels Festkommaberechnung realisiert.

     Ausgangsprojekt:
         Copyright (c) 2010-2012 Ivan Voras <ivoras@freebsd.org>
         Copyright (c) 2012 Tim Hartrick <tim@edgecast.com>

     Damit mit diesem Softwaremodul wie "gewohnt" umgegangen werden kann,
     sind - obwohl das hier eine Festkommabibliothek ist - am Ende Funktionen
     hinzugefuegt, die die Festkommazahlen wieder in einen float konvertieren.

     Standardmaessig ist die Bibliothek so eingestellt, dass der Vorkommateil
     mit 18, der Nachkommateil mit 14 Bit rechnet.


     Modifikation fuer Mikrocontroller (c) 2019 R. Seelig

  ----------------------------------------------------------------------------- */


 /* -----------------------------------------------------------------------------
                                Originalbeschreibung
    ----------------------------------------------------------------------------- */

/*
 * fixedptc.h is a 32-bit or 64-bit fixed point numeric library.
 *
 * The symbol FIXEDPT_BITS, if defined before this library header file
 * is included, governs the number of bits in the data type (its "width").
 * The default width is 32-bit (FIXEDPT_BITS=32) and it can be used
 * on any recent C99 compiler. The 64-bit precision (FIXEDPT_BITS=64) is
 * available on compilers which implement 128-bit "long long" types. This
 * precision has been tested on GCC 4.2+.
 *
 * Since the precision in both cases is relatively low, many complex
 * functions (more complex than div & mul) take a large hit on the precision
 * of the end result because errors in precision accumulate.
 * This loss of precision can be lessened by increasing the number of
 * bits dedicated to the fraction part, but at the loss of range.
 *
 * Adventurous users might utilize this library to build two data types:
 * one which has the range, and one which has the precision, and carefully
 * convert between them (including adding two number of each type to produce
 * a simulated type with a larger range and precision).
 *
 * The ideas and algorithms have been cherry-picked from a large number
 * of previous implementations available on the Internet.
 * Tim Hartrick has contributed cleanup and 64-bit support patches.
 *
 * == Special notes for the 32-bit precision ==
 * Signed 32-bit fixed point numeric library for the 24.8 format.
 * The specific limits are -8388608.999... to 8388607.999... and the
 * most precise number is 0.00390625. In practice, you should not count
 * on working with numbers larger than a million or to the precision
 * of more than 2 decimal places. Make peace with the fact that PI
 * is 3.14 here. :)
 */

/*-
 * Copyright (c) 2010-2012 Ivan Voras <ivoras@freebsd.org>
 * Copyright (c) 2012 Tim Hartrick <tim@edgecast.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _FIXEDPTC_H_
  #define _FIXEDPTC_H_

  #include <stdint.h>


  #ifndef FIXEDPT_BITS
    #define FIXEDPT_BITS  32
  #endif

  #if FIXEDPT_BITS == 32
    typedef int32_t fixedpt;
    typedef  int64_t  fixedptd;
    typedef  uint32_t fixedptu;
    typedef  uint64_t fixedptud;
  #elif FIXEDPT_BITS == 64
    typedef int64_t fixedpt;
    typedef  __int128_t fixedptd;
    typedef  uint64_t fixedptu;
    typedef  __uint128_t fixedptud;
  #else
    #error "FIXEDPT_BITS must be equal to 32 or 64"
  #endif

  #ifndef FIXEDPT_WBITS
    #define FIXEDPT_WBITS  18
  #endif

  // Umrechnung von float zu Festkomma
  #define F2Q(f)    (int)((f) * (1<< (32-FIXEDPT_WBITS) ))
  // Umrechnung von Festkomma zu float
  #define Q2F(i)    (float)(i) / (float)(1<<(32-FIXEDPT_WBITS))


  #if FIXEDPT_WBITS >= FIXEDPT_BITS
    #error "FIXEDPT_WBITS must be less than or equal to FIXEDPT_BITS"
  #endif

  #define FIXEDPT_FBITS  (FIXEDPT_BITS - FIXEDPT_WBITS)
  #define FIXEDPT_FMASK  (((fixedpt)1 << FIXEDPT_FBITS) - 1)

  #define fixedpt_rconst(R) ((fixedpt)((R) * (((fixedptd)1 << FIXEDPT_FBITS) \
    + ((R) >= 0 ? 0.5 : -0.5))))
  #define fixedpt_fromint(I) ((fixedptd)(I) << FIXEDPT_FBITS)
  #define fixedpt_toint(F) ((F) >> FIXEDPT_FBITS)
  #define fixedpt_add(A,B) ((A) + (B))
  #define fixedpt_sub(A,B) ((A) - (B))
  #define fixedpt_xmul(A,B)            \
    ((fixedpt)(((fixedptd)(A) * (fixedptd)(B)) >> FIXEDPT_FBITS))
  #define fixedpt_xdiv(A,B)            \
    ((fixedpt)(((fixedptd)(A) << FIXEDPT_FBITS) / (fixedptd)(B)))
  #define fixedpt_fracpart(A) ((fixedpt)(A) & FIXEDPT_FMASK)

  #define FIXEDPT_ONE  ((fixedpt)((fixedpt)1 << FIXEDPT_FBITS))
  #define FIXEDPT_ONE_HALF (FIXEDPT_ONE >> 1)
  #define FIXEDPT_TWO  (FIXEDPT_ONE + FIXEDPT_ONE)
  #define FIXEDPT_PI  fixedpt_rconst(3.14159265358979323846)
  #define FIXEDPT_TWO_PI  fixedpt_rconst(2 * 3.14159265358979323846)
  #define FIXEDPT_HALF_PI  fixedpt_rconst(3.14159265358979323846 / 2)
  #define FIXEDPT_E  fixedpt_rconst(2.7182818284590452354)

  #define fixedpt_abs(A) ((A) < 0 ? -(A) : (A))

  #define MY_PI   3.1415926535897932

 /* -----------------------------------------------------------------------------
                                   Prototypen
    ----------------------------------------------------------------------------- */

  fixedpt fixedpt_mul(fixedpt A, fixedpt B);
  fixedpt fixedpt_div(fixedpt A, fixedpt B);
  void fixedpt_str(fixedpt A, char *str, int max_dec);
  char *fixedpt_cstr(const fixedpt A, const int max_dec);
  fixedpt fixedpt_sqrt(fixedpt A);
  fixedpt fixedpt_sin(fixedpt fp);
  fixedpt fixedpt_cos(fixedpt A);
  fixedpt fixedpt_tan(fixedpt A);
  fixedpt fixedpt_exp(fixedpt fp);
  fixedpt fixedpt_ln(fixedpt x);
  fixedpt fixedpt_log(fixedpt x, fixedpt base);
  fixedpt fixedpt_pow(fixedpt n, fixedpt exp);

  /* -----------------------------------------------------------------------
         ab hier Umrechnung der Fixkommaberechnungen zurueck zu float
     ----------------------------------------------------------------------- */

  float fk_sin(float value);
  float fk_cos(float value);
  float fk_tan(float value);
  float fk_sqrt(float value);
  float fk_exp(float value);
  float fk_log10(float value);
  float fk_log(float value);
  float fk_pow(float value, float ex);



#endif
