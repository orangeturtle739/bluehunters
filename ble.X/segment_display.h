/*
 * File:   segment_display.h
 * Author: cmr
 *
 * Created on November 13, 2017, 1:33 PM
 */

#ifndef SEGMENT_DISPLAY_H
#define SEGMENT_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#define _DISABLE_OPENADC10_CONFIGPORT_WARNING
#define _SUPPRESS_PLIB_WARNING
#include <plib.h>

#define SEGMENT_DPORT IOPORT_B
#define SEGMENT_DPIN BIT_13
#define SEGMENT_EPORT IOPORT_B
#define SEGMENT_EPIN BIT_14
#define SEGMENT_BPORT IOPORT_B
#define SEGMENT_BPIN BIT_15
#define SEGMENT_APORT IOPORT_B
#define SEGMENT_APIN BIT_5
#define SEGMENT_FPORT IOPORT_A
#define SEGMENT_FPIN BIT_4
#define SEGMENT_GPORT IOPORT_B
#define SEGMENT_GPIN BIT_4
#define SEGMENT_DPPORT IOPORT_B
#define SEGMENT_DPPIN BIT_12
#define SEGMENT_CPORT IOPORT_B
#define SEGMENT_CPIN BIT_11

#define SEGMENT_OPEN_(PORT, PIN) PORTSetPinsDigitalOut(PORT, PIN)
#define SEGMENT_SET_(PORT, PIN) PORTClearBits(PORT, PIN)
#define SEGMENT_CLEAR_(PORT, PIN) PORTSetBits(PORT, PIN)

#define SEGMENT_OPEN(LETTER) \
  SEGMENT_OPEN_(SEGMENT_##LETTER##PORT, SEGMENT_##LETTER##PIN)
#define SEGMENT_SET(LETTER) \
  SEGMENT_SET_(SEGMENT_##LETTER##PORT, SEGMENT_##LETTER##PIN)
#define SEGMENT_CLEAR(LETTER) \
  SEGMENT_CLEAR_(SEGMENT_##LETTER##PORT, SEGMENT_##LETTER##PIN)
#define SEGMENT_WRITE_(PORT, PIN, VALUE) \
  do {                                   \
    if (VALUE)                           \
      PORTSetBits(PORT, PIN);            \
    else                                 \
      PORTClearBits(PORT, PIN);          \
  } while (0)
#define SEGMENT_WRITE(LETTER, VALUE) \
  SEGMENT_WRITE_(SEGMENT_##LETTER##PORT, SEGMENT_##LETTER##PIN, VALUE)

#define SEGMENT_DISPLAY_OPEN() \
  SEGMENT_OPEN(A);             \
  SEGMENT_OPEN(B);             \
  SEGMENT_OPEN(C);             \
  SEGMENT_OPEN(D);             \
  SEGMENT_OPEN(E);             \
  SEGMENT_OPEN(F);             \
  SEGMENT_OPEN(G);             \
  SEGMENT_OPEN(DP)

#define SEGMENT_DISPLAY_WRITE(BYTE)   \
  SEGMENT_WRITE(A, BYTE &(0x1 << 0)); \
  SEGMENT_WRITE(B, BYTE &(0x1 << 1)); \
  SEGMENT_WRITE(C, BYTE &(0x1 << 2)); \
  SEGMENT_WRITE(D, BYTE &(0x1 << 3)); \
  SEGMENT_WRITE(E, BYTE &(0x1 << 4)); \
  SEGMENT_WRITE(F, BYTE &(0x1 << 5)); \
  SEGMENT_WRITE(G, BYTE &(0x1 << 6)); \
  SEGMENT_WRITE(DP, BYTE &(0x1 << 7))

extern const char SEGMENT_NUMBERS[];

#define SEGMENT_DISPLAY_NUMBER(NUM) SEGMENT_DISPLAY_WRITE(SEGMENT_NUMBERS[NUM])

#ifdef __cplusplus
}
#endif

#endif /* SEGMENT_DISPLAY_H */
