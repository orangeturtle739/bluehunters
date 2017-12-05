#ifndef J3_4760_BLE_H
#define J3_4760_BLE_H

#include "pt_cornell_1_2_2.h"

#define BLE_BAUD 9600
#define BLE_BUF_SIZE 4096

#define BLE_UART_ID 1
#define BLE_UART UART_CONCAT(BLE_UART_ID)
#define COM_UART_ID 2
#define COM_UART UART_CONCAT(COM_UART_ID)

#define UART_CONCAT(NUMBER) UART_CONCAT_(NUMBER)
#define UART_CONCAT_(NUMBER) UART##NUMBER

#define BLE_OPEN(ID, RXGROUP, RXPIN, TXGROUP, TXPIN) \
  BLE_OPEN_(ID, RXGROUP, RXPIN, TXGROUP, TXPIN)

#define BLE_OPEN_(ID, RXGROUP, RXPIN, TXGROUP, TXPIN)                         \
  PPSInput(RXGROUP, U##ID##RX, RXPIN);                                        \
  PPSOutput(TXGROUP, TXPIN, U##ID##TX);                                       \
  UARTConfigure(UART##ID, UART_ENABLE_PINS_TX_RX_ONLY);                       \
  UARTSetLineControl(                                                         \
      UART##ID, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1); \
  UARTSetDataRate(UART##ID, pb_clock, BLE_BAUD);                              \
  UARTEnable(UART##ID, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX))

#define BLE_TRANSMIT_CHAR(PT, ID, CHAR) BLE_TRANSMIT_CHAR_(PT, ID, CHAR);

#define BLE_TRANSMIT_CHAR_(PT, ID, CHAR) \
  do {                                   \
    while (!UARTTransmitterIsReady(ID))  \
      ;                                  \
    UARTSendDataByte(ID, CHAR);          \
  } while (0)


#define BLE_PRINT(PT, ID, BUF) BLE_PRINT_(PT, ID, BUF)

#define BLE_PRINT_(PT, ID, BUF)          \
  do {                                   \
    int i;                               \
    for (i = 0; BUF[i] != 0; i++) {      \
      BLE_TRANSMIT_CHAR(PT, ID, BUF[i]); \
    }                                    \
  } while (0)

#define BLE_RECEIVE_CHAR(PT, ID, DATA) BLE_RECEIVE_CHAR_(PT, ID, DATA)

#define BLE_RECEIVE_CHAR_(PT, ID, DATA)                    \
  do {                                                     \
    while (!UARTReceivedDataIsAvailable(ID)) PT_YIELD(PT); \
    DATA = UARTGetDataByte(ID);                            \
  } while (0)

#define BLE_UART_PRINT(PT, UART, STR) \
  PT_SPAWN(pt, &pt_uart, ble_uart_print(&pt_uart, UART, STR))

#define BLE_UART_PRINTLN(PT, UART, STR) \
  PT_SPAWN(pt, &pt_uart, ble_uart_println(&pt_uart, UART, STR))

#define BLE_UART_READLN(PT, UART, STR, LEN, ENDCHAR) \
  PT_SPAWN(pt, &pt_uart, ble_uart_readln(&pt_uart, UART, STR, LEN, ENDCHAR))

#define PORT_WRITE(PORT, PIN, VALUE) \
  do {                               \
    if (VALUE)                       \
      PORTSetBits(PORT, PIN);        \
    else                             \
      PORTClearBits(PORT, PIN);      \
  } while (0)

#define BLE_WAIT_FOR_CAPTURE(PT) PT_WAIT_UNTIL(pt, !ble_is_capturing())

extern pt_t pt_uart;
extern char ble_receive_buf[BLE_BUF_SIZE];

PT_THREAD(ble_uart_print(struct pt *pt, UART_MODULE uart, char *buf));
PT_THREAD(ble_uart_println(struct pt *pt, UART_MODULE uart, char *buf));
PT_THREAD(ble_uart_readln(struct pt *pt, UART_MODULE uart, char *buf,
                          size_t len, char end_char));
PT_THREAD(ble_receive(struct pt *pt));

void ble_init();
void ble_enable_capture(char *sentinel);
void ble_disable_capture();
char ble_is_capturing();
int ble_rssi_from_capture(const char *mac);

#endif
