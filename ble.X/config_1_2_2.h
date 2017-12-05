/*
 * File:   config_1_2_2.h
 * Author: Syed Tahmid Mahbub
 * Modifed by: Bruce Land
 * Created on October 30, 2017
 *
 */

#ifndef CONFIG_H
#define CONFIG_H
#define _SUPPRESS_PLIB_WARNING
#define _DISABLE_OPENADC10_CONFIGPORT_WARNING
#include <plib.h>

//=============================================================
// 60 MHz
#pragma config FNOSC = FRCPLL, POSCMOD = OFF
#pragma config FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPLLODIV = DIV_2  // 40 MHz
#pragma config FPBDIV = DIV_1  // PB 40 MHz
#pragma config FWDTEN = OFF, JTAGEN = OFF
#pragma config FSOSCEN = OFF  // PINS 11 and 12 to secondary oscillator!
#pragma config DEBUG = OFF    // RB4 and RB5
//==============================================================

#endif /* CONFIG_H */
