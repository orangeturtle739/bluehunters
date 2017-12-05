#include "main.h"
#include "ble.h"
#include "config_1_2_2.h"
#include "pt_cornell_1_2_2.h"
//#include "segment_display.h"
#include "servo.h"
#include <stdlib.h>
#include "imu.h"
#include <limits.h>

#define TARGET_MAGIC_ID 2
const char *BLE_MACS[] = {"3CA30807276A", "3CA30807B62E", "3CA3080738E3"};
const char *BLE_NAMES[] = {"PIRATE", "EAGLE", "NYMERIA"};
#define OTHER_ADDR (BLE_MACS[TARGET_MAGIC_ID])
#define STR(s) #s
#define XSTR(s) STR(s)
#define MAGIC_BLE_NAME(ID) (BLE_NAMES[ID])
#define SPEED 10
#define QUANTA 500
#define SEGMENTS 20 
#define SHORTDRIVE 20
#define END_RSSI (-40)
#define TURN_THRESHOLD 30
#define CALIB_TIME 7000
#define TURN_TIMEOUT 2000
#define ALPHA 0.2

#define LED_PORTA IOPORT_B
#define LED_PINA BIT_8
#define LED_PORTB IOPORT_B
#define LED_PINB BIT_9
#define LED_PORTC IOPORT_B
#define LED_PINC BIT_10

#define SW_PORT0 IOPORT_B
#define SW_PIN0 BIT_7
#define SW_PORT1 IOPORT_B
#define SW_PIN1 BIT_13

#define COMMAND_DELAY 100

static int lastturn = 0;

static pt_t pt_blinky, pt_ble, pt_ble_receive;

static int my_id;

static int get_id() {
    PORTSetPinsDigitalIn(SW_PORT0, SW_PIN0);
    PORTSetPinsDigitalIn(SW_PORT1, SW_PIN1);
    CNPUB = SW_PIN0 | SW_PIN1;
    CNPDB = 0;

    char bit_0 = PORTReadBits(SW_PORT0, SW_PIN0) == 0;
    char bit_1 = PORTReadBits(SW_PORT1, SW_PIN1) == 0;

    my_id = bit_1 << 1 | bit_0;
    return my_id;
}

static PT_THREAD(blinky(struct pt *pt)) {
    static char buf[500];
    PT_BEGIN(pt);

    //    PORTClearBits(LED_PORTA, LED_PINA);
    //    PORTClearBits(LED_PORTB, LED_PINB);
    //    PORTClearBits(LED_PORTC, LED_PINC);

    //    PORT_WRITE(LED_PORTA, LED_PINA, my_id & (0x1 << 0));
    //    PORT_WRITE(LED_PORTB, LED_PINB, my_id & (0x1 << 1));

    while (my_id == TARGET_MAGIC_ID) {
        PT_YIELD_TIME_msec(250);
        PORT_WRITE(LED_PORTC, LED_PINC, 0);
        PT_YIELD_TIME_msec(250);
        PORT_WRITE(LED_PORTC, LED_PINC, 1);
    }
    PORT_WRITE(LED_PORTC, LED_PINC, 1);
    PT_END(pt);
}

static PT_THREAD(drive(struct pt *pt, int rssi)) {
    static int last_rssi;
    static int state;
    static char buf[500];
    static int i;
    static int start_heading, goal_heading;
    static int left;
    static int straight_time;
    static unsigned int timestamp;
    PT_BEGIN(pt);


    sprintf(buf, "IMU reading: %d", imu_get_heading());
    BLE_UART_PRINTLN(pt, COM_UART, buf);

    if (last_rssi == 0) {
        servo_drive(20, 0);
        state = 0b11;
    } else if (rssi != 0) {
        if (rssi > last_rssi) {
            state = 0b00;
            sprintf(buf, "SERVO straight");
            BLE_UART_PRINTLN(pt, COM_UART, buf);
            straight_time = QUANTA;
        } else {
            //            if (lastturn) {
            //                lastturn = 0;
            //                left = 2;
            //            } else
            if (rand() & 0x01) {
                left = 1;
                state = 0b01;
                lastturn = 1;
            } else {
                left = -1;
                state = 0b10;
                lastturn = 1;
            }
            start_heading = imu_get_heading();
            goal_heading = degree(start_heading + left * 90);

            sprintf(buf, "SERVO turn; start: %d, goal: %d left: %d, fun: %d", start_heading, goal_heading, left, start_heading + left * 90);
            BLE_UART_PRINTLN(pt, COM_UART, buf);


            timestamp = PT_GET_TIME();
            while (abs(angle_diff(imu_get_heading(), goal_heading)) > TURN_THRESHOLD && PT_GET_TIME() - timestamp < TURN_TIMEOUT) {
                //                sprintf(buf, "IMU curr: %d, goal: %d angle diff: %d,", imu_get_heading(), goal_heading, angle_diff(imu_get_heading(), goal_heading));
                //                BLE_UART_PRINTLN(pt, COM_UART, buf);
                servo_drive(SPEED * 2, left * 100);
                PT_YIELD(pt);
            }
            sprintf(buf, "SERVO turn end: heading: %d", imu_get_heading());
            BLE_UART_PRINTLN(pt, COM_UART, buf);

            sprintf(buf, "SERVO done turning");
            BLE_UART_PRINTLN(pt, COM_UART, buf);

            servo_drive(0, 0);
            PT_YIELD_TIME_msec(100);
            straight_time = QUANTA / 2;
        }

        start_heading = imu_get_heading();
        for (i = 0; i < SEGMENTS; i++) {
            servo_drive_straight(SPEED, start_heading);
            if (straight_time == QUANTA) {
                PT_YIELD_TIME_msec(straight_time / SEGMENTS / 2);
            } else {
                PT_YIELD_TIME_msec(straight_time / SEGMENTS * 2);
            }
        }
        //        servo_drive(SPEED, 0);
        PT_YIELD_TIME_msec(straight_time);
    }


    last_rssi = rssi;
    servo_drive(0, 0);

    PORT_WRITE(LED_PORTA, LED_PINA, state & (0x1 << 0));
    PORT_WRITE(LED_PORTB, LED_PINB, state & (0x1 << 1));
    PT_END(pt);
}

static PT_THREAD(ble1(struct pt *pt)) {
    static char buf[500];
    static int i;
    static char* ROLE_STRING;
    static char* IMME_STRING;
    static pt_t pt_drive;
    static int rssi_1;
    static int rssi_2;
    static int mag[3];
    static unsigned int start_time;
    static int lastx, lasty;
    PT_BEGIN(pt);

    sprintf(buf, "Hello World");
    BLE_UART_PRINTLN(pt, COM_UART, buf);
    PT_YIELD_TIME_msec(COMMAND_DELAY);

    start_time = PT_GET_TIME();

    //    xmax = 40;
    //    xmin = -10;
    //    ymax = 50;
    //    ymin = -10;
    xmax = INT_MIN;
    xmin = INT_MAX;
    ymax = INT_MIN;
    ymin = INT_MAX;

    while (PT_GET_TIME() < start_time + CALIB_TIME) {
        servo_drive(SPEED, 100);

        imu_mag_read_data(mag);

        lastx = mag[0] * ALPHA + lastx * (1 - ALPHA);
        lasty = mag[1] * ALPHA + lasty * (1 - ALPHA);

        if (lastx > xmax) xmax = lastx;
        if (lastx < xmin) xmin = lastx;
        if (lasty > ymax) ymax = lasty;
        if (lasty < ymin) ymin = lasty;
    }
    sprintf(buf, "xmax %d xmin %d ymax %d ymin %d", xmax, xmin, ymax, ymin);
    BLE_UART_PRINTLN(pt, COM_UART, buf);
    servo_drive(0, 0);

    BLE_UART_PRINTLN(pt, COM_UART, "Configuring Chip");
    PT_YIELD_TIME_msec(COMMAND_DELAY);


    BLE_UART_PRINTLN(pt, BLE_UART, "AT+RESET");
    BLE_UART_PRINTLN(pt, COM_UART, "AT+RESET");
    PT_YIELD_TIME_msec(COMMAND_DELAY);

    BLE_UART_PRINTLN(pt, BLE_UART, "AT+IBEA1");
    BLE_UART_PRINTLN(pt, COM_UART, "AT+IBEA1");
    PT_YIELD_TIME_msec(COMMAND_DELAY);
    BLE_UART_PRINTLN(pt, BLE_UART, "AT+IBEA?");
    BLE_UART_PRINTLN(pt, COM_UART, "AT+IBEA?");
    PT_YIELD_TIME_msec(COMMAND_DELAY);

    ROLE_STRING = my_id == TARGET_MAGIC_ID ? "AT+ROLE0" : "AT+ROLE1";

    BLE_UART_PRINTLN(pt, BLE_UART, ROLE_STRING);
    BLE_UART_PRINTLN(pt, COM_UART, ROLE_STRING);
    PT_YIELD_TIME_msec(COMMAND_DELAY);
    BLE_UART_PRINTLN(pt, BLE_UART, "AT+ROLE?");
    BLE_UART_PRINTLN(pt, COM_UART, "AT+ROLE?");
    PT_YIELD_TIME_msec(COMMAND_DELAY);

    IMME_STRING = my_id == TARGET_MAGIC_ID ? "AT+IMME0" : "AT+IMME1";

    BLE_UART_PRINTLN(pt, BLE_UART, IMME_STRING);
    BLE_UART_PRINTLN(pt, COM_UART, IMME_STRING);
    PT_YIELD_TIME_msec(COMMAND_DELAY);
    BLE_UART_PRINTLN(pt, BLE_UART, "AT+IMME?");
    BLE_UART_PRINTLN(pt, COM_UART, "AT+IMME?");
    PT_YIELD_TIME_msec(COMMAND_DELAY);

    sprintf(buf, "AT+NAME%s", MAGIC_BLE_NAME(my_id));
    BLE_UART_PRINTLN(pt, BLE_UART, buf);
    BLE_UART_PRINTLN(pt, COM_UART, buf);
    PT_YIELD_TIME_msec(COMMAND_DELAY);
    BLE_UART_PRINTLN(pt, BLE_UART, "AT+NAME?");
    BLE_UART_PRINTLN(pt, COM_UART, "AT+NAME?");
    PT_YIELD_TIME_msec(COMMAND_DELAY);
    BLE_UART_PRINTLN(pt, BLE_UART, "AT+SHOW3");
    BLE_UART_PRINTLN(pt, COM_UART, "AT+SHOW3");
    PT_YIELD_TIME_msec(COMMAND_DELAY);
    BLE_UART_PRINTLN(pt, BLE_UART, "AT+SHOW?");
    BLE_UART_PRINTLN(pt, COM_UART, "AT+SHOW?");
    PT_YIELD_TIME_msec(COMMAND_DELAY);
    BLE_UART_PRINTLN(pt, BLE_UART, "AT+ADDR?");
    BLE_UART_PRINTLN(pt, COM_UART, "AT+ADDR?");
    PT_YIELD_TIME_msec(COMMAND_DELAY);
    BLE_UART_PRINTLN(pt, COM_UART, "=====================");

    while (1) {
        ble_enable_capture("OK+DISCE");
        BLE_UART_PRINTLN(pt, BLE_UART, "AT+DISI?");
        BLE_WAIT_FOR_CAPTURE(pt);
        ble_disable_capture();
        rssi_1 = ble_rssi_from_capture(OTHER_ADDR);
        ble_enable_capture("OK+DISCE");
        BLE_UART_PRINTLN(pt, BLE_UART, "AT+DISI?");
        BLE_WAIT_FOR_CAPTURE(pt);
        ble_disable_capture();
        rssi_2 = ble_rssi_from_capture(OTHER_ADDR);

        rssi_2 = (rssi_1 + rssi_2) / 2;
        sprintf(buf, "RSSI: %03d", rssi_2);
        BLE_UART_PRINTLN(pt, COM_UART, buf);
        if (rssi_2 < END_RSSI) {
            PT_SPAWN(pt, &pt_drive, drive(&pt_drive, rssi_2));
        } else { //goal reached, blinky celebration
            while (1) {
                PORT_WRITE(LED_PORTC, LED_PINC, rand() & 0x01);
                PORT_WRITE(LED_PORTA, LED_PINA, rand() & 0x01);
                PORT_WRITE(LED_PORTB, LED_PINB, rand() & 0x01);
                PT_YIELD_TIME_msec(500);
            }
        }
    }
    PT_END(pt);
}

int main() {
    PT_setup();
    PT_INIT(&pt_blinky);
    PT_INIT(&pt_ble);
    PT_INIT(&pt_ble_receive);

    INTEnableSystemMultiVectoredInt();

    ble_init();
    get_id();
    if (my_id != TARGET_MAGIC_ID) {
        servo_init();
        imu_init();
    }

    servo_set(0, 0);

    PORTSetPinsDigitalOut(LED_PORTA, LED_PINA);
    PORTSetPinsDigitalOut(LED_PORTB, LED_PINB);
    PORTSetPinsDigitalOut(LED_PORTC, LED_PINC);

    //    SEGMENT_DISPLAY_OPEN();
    //    SEGMENT_DISPLAY_NUMBER(0);

    while (1) {
        PT_SCHEDULE(blinky(&pt_blinky));
        PT_SCHEDULE(ble1(&pt_ble));
        PT_SCHEDULE(ble_receive((&pt_ble_receive)));
    }
}
