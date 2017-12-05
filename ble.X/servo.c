#include "servo.h"
#include "pt_cornell_1_2_2.h"
#include "imu.h"

#include <math.h>
#include <stdlib.h>

#define MIN_PWM_VALUE 1280
#define MAX_PWM_VALUE 2560
#define ZERO_PWM_VALUE 1880 //1920 //((MAX_PWM_VALUE - MIN_PWM_VALUE) / 2 + MIN_PWM_VALUE)
#define ZERO_PWM_RANGE (MAX_PWM_VALUE - ZERO_PWM_VALUE)

#define IMU_NOISE 5
#define IMU_BUFSIZE 5
static int heading_buf[IMU_BUFSIZE];

void servo_raw_set_1(int val) {
    SetDCOC3PWM(val);
}

void servo_raw_set_2(int val) {
    SetDCOC2PWM(val);
}

int servo_norm_to_raw(int val) {
    if (val == 0) {
        return 0;
    }
    int result = val * ZERO_PWM_RANGE / 100 + ZERO_PWM_VALUE;
    if (result > ZERO_PWM_VALUE + ZERO_PWM_RANGE) {
        result = ZERO_PWM_VALUE + ZERO_PWM_RANGE;
    }
    if (result < ZERO_PWM_VALUE - ZERO_PWM_RANGE) {
        result = ZERO_PWM_VALUE - ZERO_PWM_RANGE;
    }
    return result;
}

void servo_set_1(int val) {
    servo_raw_set_1(servo_norm_to_raw(val));
}

void servo_set_2(int val) {
    servo_raw_set_2(servo_norm_to_raw(val));
}

void servo_set(int left, int right) {
    servo_set_2(left);
    servo_set_1(-right);
}

void servo_drive(int power, int twist) {
    servo_set(power + power * twist / 100, power - power * twist / 100);
}

int servo_drive_straight(int speed, double fwd_heading) {
    int left = speed * 4;
    int right = speed * 4;
    int curr_heading = imu_get_heading();

//    if (angle_diff(curr_heading, fwd_heading) < -IMU_NOISE) {
//        left /= 2;
//    } else if (angle_diff(curr_heading, fwd_heading) > IMU_NOISE) {
//        right /= 2;
//    } // else, left and right stay the same  
    servo_set(left, right*1.5);
}
//Nothing

void __ISR(_TIMER_2_VECTOR, IPL2AUTO) Timer2Handler(void) {
    mT2ClearIntFlag();
}

void servo_init() {
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_32, 25600);
    ConfigIntTimer2(T2_INT_ON | T2_INT_PRIOR_2);

    mT2ClearIntFlag();

    // set up compare3 for PWM mode
    OpenOC3(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0);
    OpenOC2(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0);

    PPSOutput(4, RPB0, OC3);
    PPSOutput(2, RPB1, OC2);

    // initialize random
    int i;
    for (i = 0; i < IMU_BUFSIZE; i++) {
        heading_buf[i] = (rand() & 0x1FF) % 360 - 180;
    }
}