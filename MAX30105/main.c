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
#include <stdint.h>
#include "LW_USCI_I2C.h"



unsigned char timercounter;
unsigned char array[40] = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb };
unsigned char store[40] = { 13, 13, 13, 13, 13, 13, 13, 0, 0, 0};

void main(void) {

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    /*
     * TODO: Clock initialization
     */

    P1DIR |= BIT0;
    P1OUT &= ~BIT0;

    USCI_I2C_Init(BIT6 + BIT7, 0x12);
    BCSCTL1 = CALBC1_8MHZ;
    DCOCTL = CALDCO_8MHZ;


    // TODO: USCI_I2C_Init(unsigned char pinSel, unsigned char clkDiv);
    uint8_t temp;
    while (1) {
        test();
        __delay_cycles(1000);
        // TODO: Set some LED to test if this is working
    }
}

