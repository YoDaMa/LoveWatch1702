/*** USCI master library ************************************************************

In this file the usage of the USCI I2C master library without DMA support is 
shown. This library uses pointers to specify what data is to be sent. 

When calling the TI_USCI_I2C_receive or TI_USCI_I2C_transmit routines 
the number of bytes, which are to be transmitted or received have to be passed as 
well as a pointer to a data field, that contains(or stores) the data.

This code checks if there is a slave with address 0x50 is connected to the I2C
bus and if the slave device is present, bytes are received and transmitted.

Uli Kretzschmar
MSP430 Systems
Freising
*******************************************************************************/
#include "msp430g2553.h"
#include "LEDBackpack.h"
#include "USCI_I2C_G2553.h"


void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;

  __enable_interrupt();
  USCI_I2C_transmitinit(0x70, 0x12);
  if (!USCI_I2C_slave_present(0x70))
      while (1);


  while(1) {

      begin();
      writeDigitRaw(3, 0xFFFF);
      writeDigitRaw(0, 0xFFFF);
      writeDisplay();
      __delay_cycles(1000);
  }



}

