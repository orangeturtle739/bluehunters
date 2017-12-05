#define _SUPPRESS_PLIB_WARNING
#define _DISABLE_OPENADC10_CONFIGPORT_WARNING
#include <plib.h>
#include "imu.h"
#include <math.h>
#include <limits.h>
#include "servo.h"
#include "pt_cornell_1_2_2.h"

#define MPU_ADDRESS 0xd0
#define AK8963_ADDRESS (0x0C<<1)

#define MPU_INT_PIN_CFG 0x37
#define MPU_FIFO_EN 0x23
#define MPU_I2C_MST_CTRL 0x24
#define MPU_USER_CTRL 0x6a
#define MPU_INT_ENABLE 0x38
#define MPU_PWR_MGMT_1 0x6B

#define AK8963_WIA 0x00
#define AK8963_ST1 0x02
#define AK8963_XOUT_L 0x03
#define AK8963_XOUT_H 0x04
#define AK8963_YOUT_L 0x05
#define AK8963_YOUT_H 0x06
#define AK8963_ZOUT_L 0x07
#define AK8963_ZOUT_H 0x08
#define AK8963_ST2 0x09
#define AK8963_CNTL1 0x0A
#define AK8963_ASTC 0x0C
#define AK8963_ASAX 0x10
#define AK8963_ASAY 0x11
#define AK8963_ASAZ 0x12


int xmax = 0;
int xmin = 0;
int ymax = 0;
int ymin = 0;
static char error;

// Wait by executing nops

static void i2c_wait(unsigned int cnt) {
    while (--cnt) {
        asm("nop");
        asm("nop");
    }
}

static void i2c_write_byte(char device, char address, char data) {
    char i2c_header[2];
    i2c_header[0] = device | 0; // device address & WR
    i2c_header[1] = address; // register address

    StartI2C2(); // Send the Start Bit
    IdleI2C2(); // Wait to complete

    int i;
    for (i = 0; i < 2; i++) {
        MasterWriteI2C2(i2c_header[i]);
        IdleI2C2(); // Wait to complete

        // ACKSTAT is 0 when slave acknowledge,
        // if 1 then slave has not acknowledge the data.
        if (I2C2STATbits.ACKSTAT) break;
    }
    MasterWriteI2C2(data);
    IdleI2C2(); // Wait to complete

    StopI2C2(); // Send the Stop condition
    IdleI2C2(); // Wait to complete
}

static char i2c_read_device(char device, char address) {

    char i2c_header[2];
    i2c_header[0] = (device | 0); // device address & WR
    i2c_header[1] = address; // register address

    StartI2C2(); // Send the Start Bit
    IdleI2C2(); // Wait to complete

    int i;
    for (i = 0; i < 2; i++) {
        MasterWriteI2C2(i2c_header[i]);
        IdleI2C2(); // Wait to complete

        // ACKSTAT is 0 when slave acknowledge,
        // if 1 then slave has not acknowledge the data.
        if (I2C2STATbits.ACKSTAT) {
            break;
        }
    }

    // now send a start sequence again
    RestartI2C2(); // Send the Restart condition
    i2c_wait(10);
    // wait for this bit to go back to zero
    IdleI2C2(); // Wait to complete

    MasterWriteI2C2(device | 1); // transmit read command
    IdleI2C2(); // Wait to complete

    char data = MasterReadI2C2();

    IdleI2C2(); // Wait to complete

    StopI2C2(); // Send the Stop condition
    IdleI2C2(); // Wait to complete


    //    i2c_wait(10000000);
    return data;
}

int imu_mag_read_data(int* destination) {
    i2c_write_byte(AK8963_ADDRESS, AK8963_CNTL1, 0x01);
    // single read 14-bit

    // i2c_write_byte(AK8963_ADDRESS, AK8963_CNTL1, 1 << 4 | 0x01);
    // single read 16-bit
    // i2c_write_byte(AK8963_ADDRESS, AK8963_CNTL1, 0x02); 
    // cont mode 8 kHz

    char newMagData = (i2c_read_device(AK8963_ADDRESS, AK8963_ST1) & 0x01);
    //    if (newMagData) { // wait for magnetometer data ready bit to be set
    char rawdata[7];
    int i;
    for (i = 0; i < 7; i++) { // End data read by reading ST2 register
        rawdata[i] = i2c_read_device(AK8963_ADDRESS, AK8963_XOUT_L + i);
    }

    i2c_wait(100000);

    if (!(rawdata[6] & 0x08)) { // Check if magnetic sensor overflow set, if not then report data
        destination[0] = (rawdata[1] << 8) | rawdata[0]; // Turn the MSB and LSB into a signed 16-bit value
        destination[1] = (rawdata[3] << 8) | rawdata[2]; // Data stored as little Endian
        destination[2] = (rawdata[5] << 8) | rawdata[4];

        return 0;
    } else {
        return 1;
    }
}

int imu_get_heading() {

    int mag[3];
    float x, y;
    int i;
    //    for (i = 0; i < 3; i++) {
    error = imu_mag_read_data(mag);

    int xoffset = (xmax + xmin) / 2;
    int yoffset = (ymax + ymin) / 2;

    x = ((float) mag[0] - (float) xoffset) / (float) (xmax - xmin);
    y = ((float) mag[1] - (float) yoffset) / (float) (ymax - ymin);
    //    }
    return (int) (atan2(x / 3, y / 3) * (180 / 3.1416));
}

int imu_get_x_raw(int axis) {
    int mag[3];
    error = imu_mag_read_data(mag);

    return mag[axis];
}

int imu_last_error() {
    return error;
}

void imu_init() {
    mPORTBSetPinsDigitalOut(BIT_8 | BIT_9);
    OpenI2C2(I2C_ON, 48);

    i2c_write_byte(MPU_ADDRESS, MPU_INT_PIN_CFG, 0x22); //pass through enable
    i2c_write_byte(MPU_ADDRESS, MPU_INT_ENABLE, 0x01);
    i2c_write_byte(MPU_ADDRESS, MPU_I2C_MST_CTRL, 0x00); // Disable I2C master
    i2c_write_byte(MPU_ADDRESS, MPU_PWR_MGMT_1, 0x00); //power up sensors


}

int degree(int deg) {
    if (deg < -180) {
        return deg + 360;
    } else if (deg > 180) {
        return deg - 360;
    } else {
        return deg;
    }
}

int angle_diff(int source, int target) {
    int a = target - source;
    a += (a > 180) ? -360 : (a<-180) ? 360 : 0;
    return a;
}