#include "ble.h"

pt_t pt_uart;

char ble_receive_buf[BLE_BUF_SIZE];
static size_t ble_receive_buf_idx;
static char ble_capture;
static char *ble_sentinel;

PT_THREAD(ble_uart_print(struct pt *pt, UART_MODULE uart, char *buf)) {
    PT_BEGIN(pt);
    BLE_PRINT(pt, uart, buf);
    PT_EXIT(pt);
    PT_END(pt);
}

PT_THREAD(ble_uart_println(struct pt *pt, UART_MODULE uart, char *buf)) {
    const static char newline[] = "\r\n";
    PT_BEGIN(pt);
    BLE_PRINT(pt, uart, buf);
    BLE_PRINT(pt, uart, newline);
    PT_EXIT(pt);
    PT_END(pt);
}

PT_THREAD(ble_uart_readln(struct pt *pt, UART_MODULE uart, char *buf,
        size_t len, char end_char)) {
    static size_t i;
    static int done;
    static char data;
    PT_BEGIN(pt);
    done = 0;
    for (i = 0; i < len - 1 && !done; i++) {
        BLE_RECEIVE_CHAR(pt, uart, data);
        if (data == end_char) {
            done = 1;
            i--; // so the \r gets overwritten by 0
        } else {
            buf[i] = data;
        }
    }
    // Set the terminal 0
    buf[i] = 0;
    PT_EXIT(pt);
    PT_END(pt);
}

PT_THREAD(ble_receive(struct pt *pt)) {
    PT_BEGIN(pt);
    static char data;
    while (1) {
        BLE_RECEIVE_CHAR(pt, BLE_UART, data);
        if (ble_capture && ble_receive_buf_idx < BLE_BUF_SIZE - 1) {
            ble_receive_buf[ble_receive_buf_idx++] = data;
            // Make the next one a null byte to signify the end of the string
            ble_receive_buf[ble_receive_buf_idx] = 0;
            if (ble_receive_buf_idx > strlen(ble_sentinel)) {
                if (strcmp(&ble_receive_buf[ble_receive_buf_idx - strlen(ble_sentinel)],
                        ble_sentinel) == 0) {
                    ble_disable_capture();
                }
            }
        } else {
            BLE_TRANSMIT_CHAR(pt, COM_UART, data);
        }
    }
    PT_END(pt);
}

void ble_init() {
    // RX, TX
    BLE_OPEN(BLE_UART_ID, 3, RPA2, 1, RPA0);
    BLE_OPEN(COM_UART_ID, 2, RPA1, 4, RPA3);
    ble_disable_capture();
}

void ble_enable_capture(char *sentinel) {
    ble_receive_buf_idx = 0;
    ble_capture = 1;
    ble_receive_buf[ble_receive_buf_idx] = 0;
    ble_sentinel = sentinel;
}

void ble_disable_capture() {
    ble_capture = 0;
}

char ble_is_capturing() {
    return ble_capture;
}

int ble_rssi_from_capture(const char *mac) {
    // OK+DISC:00000000:00000000000000000000000000000000:0000000000:6832A3801EBE:-080
    int start = strlen("OK+DISIS");
    const char init = '+';
    int mac_offset =
            strlen("+DISC:00000000:00000000000000000000000000000000:0000000000:");
    int mac_len = 12;
    int rssi_offset = strlen(
            "+DISC:00000000:00000000000000000000000000000000:0000000000:"
            "6832A3801EBE:");
    int rssi_len = 4;
    int line_length = strlen(
            "+DISC:00000000:00000000000000000000000000000000:0000000000:"
            "6832A3801EBE:-080");
    int i;
    for (i = start; i < strlen(ble_receive_buf); i++) {
        char* found = strchr(&ble_receive_buf[i], init);
        if (found == NULL) continue;
        i = found - ble_receive_buf;
        if (strncmp(&ble_receive_buf[i + mac_offset], mac, mac_len) == 0) {
            char rssi[5];
            strncpy(rssi, &ble_receive_buf[i + rssi_offset], rssi_len);
            rssi[4] = 0;
            return atoi(rssi);
        }
    }
    return 0;
}