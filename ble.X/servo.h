#ifndef J3_4760_SERVO_H
#define	J3_4760_SERVO_H

#define _SUPPRESS_PLIB_WARNING
#define _DISABLE_OPENADC10_CONFIGPORT_WARNING
#include <plib.h>

extern int xmax;
extern int xmin;
extern int ymax;
extern int ymin;

void servo_raw_set_1(int val);
void servo_raw_set_2(int val);
int servo_norm_to_raw(int val);
void servo_set_1(int val);
void servo_set_2(int val);
void servo_set(int left, int right);
void servo_drive(int power, int twist);
int servo_drive_straight(int speed, double fwd_heading);

void servo_init();

#endif
