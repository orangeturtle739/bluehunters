#include "pt_cornell_1_2_2.h"

//======================================================================
// vref confing (if used)
int CVRCON_setup;

// system time
volatile unsigned int time_tick_millsec;
// force full context save
// int w;
// void waste(void){w=1;};
// Timer 5 interrupt handler ///////
// ipl2 means "interrupt priority level 2"

void __ISR(_TIMER_5_VECTOR, IPL2AUTO) Timer5Handler(void)  //_TIMER_5_VECTOR
{
  // clear the interrupt flag
  mT5ClearIntFlag();
  // count milliseconds
  time_tick_millsec++;
  // waste();
}

void PT_setup(void) {
  // Configure the device for maximum performance but do not change the PBDIV
  // Given the options, this function will change the flash wait states, RAM
  // wait state and enable prefetch cache but will not change the PBDIV.
  // The PBDIV value is already set via the pragma FPBDIV option above..
  SYSTEMConfig(sys_clock, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

  ANSELA = 0;  // make sure analog is cleared
  ANSELB = 0;

#ifdef use_uart_serial
  // === init the uart2 ===================
  // SET UART i/o PINS
  // The RX pin must be one of the Group 2 input pins
  // RPA1, RPB1, RPB5, RPB8, RPB11
  PPSInput(2, U2RX, RPA1);  // Assign U2RX to pin RPA1 --
  // The TX pin must be one of the Group 4 output pins
  // RPA3, RPB0, RPB9, RPB10, RPB14
  PPSOutput(4, RPB10, U2TX);  // Assign U2TX to pin RPB10 --

  UARTConfigure(UART2, UART_ENABLE_PINS_TX_RX_ONLY);
  UARTSetLineControl(
      UART2, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
  UARTSetDataRate(UART2, pb_clock, BAUDRATE);
  UARTEnable(UART2, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));
  // Feel free to comment this out
  clrscr();
  printf("\n\r..protothreads 1_2_2 10/30/17..\n\r");
  // === set up DMA for UART output =========
  // configure the channel and enable end-on-match
  DmaChnOpen(DMA_CHANNEL1, DMA_CHN_PRI2, DMA_OPEN_MATCH);
  // trigger a byte everytime the UART is empty
  DmaChnSetEventControl(DMA_CHANNEL1, DMA_EV_START_IRQ_EN | DMA_EV_MATCH_EN |
                                          DMA_EV_START_IRQ(_UART2_TX_IRQ));
  // source and destination
  DmaChnSetTxfer(DMA_CHANNEL1, PT_send_buffer + 1, (void *)&U2TXREG, max_chars,
                 1, 1);
  // signal when done
  DmaChnSetEvEnableFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
  // set null as ending character (of a string)
  DmaChnSetMatchPattern(DMA_CHANNEL1, 0x00);
#endif  //#ifdef use_uart_serial

  // ===Set up timer5 ======================
  // timer 5: on,  interrupts, internal clock,
  // set up to count millsec
  OpenTimer5(T5_ON | T5_SOURCE_INT | T5_PS_1_1, pb_clock / 1000);
  // set up the timer interrupt with a priority of 2
  ConfigIntTimer5(T5_INT_ON | T5_INT_PRIOR_2);
  mT5ClearIntFlag();  // and clear the interrupt flag
  // zero the system time tick
  time_tick_millsec = 0;

//=== Set up VREF as a debugger output =======
#ifdef use_vref_debug
  // set up the Vref pin and use as a DAC
  // enable module| eanble output | use low range output | use internal
  // reference | desired step
  CVREFOpen(CVREF_ENABLE | CVREF_OUTPUT_ENABLE | CVREF_RANGE_LOW |
            CVREF_SOURCE_AVDD | CVREF_STEP_0);
  // And read back setup from CVRCON for speed later
  // 0x8060 is enabled with output enabled, Vdd ref, and 0-0.6(Vdd) range
  CVRCON_setup =
      CVRCON;  // CVRCON = 0x8060 from Tahmid http://tahmidmc.blogspot.com/

#endif  //#ifdef use_vref_debug
}