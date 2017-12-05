#ifndef J3_4760_IMU_H
#define J3_4760_IMU_H

void imu_init();

// return degree of heading
int imu_get_heading();

int imu_last_error();

int imu_get_raw(int axis);

int imu_mag_read_data(int * destination);

int angle_diff(int source, int target);
int degree(int deg);

#endif