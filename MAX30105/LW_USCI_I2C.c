#include <msp430g2553.h>
#include <stdint.h>
#include "LW_USCI_I2C.h"
#include "registers.h"


/*
 * TODO: WHICH ONE TO USE? Arduino code only uses one....
 */
const int writeAddr = 87; // particleSensor AEh, 10101110
const int readAddr = 175; // particleSensor AFh, 10101111

const int size8 = sizeof(uint8_t);

/*
 * The read write buffer we transfer to UCB0TXBUF/UCB0RXBUF
 */

uint8_t* gpTXPos = 0;
uint8_t* gpTXEnd = 0;
uint8_t* gpRXPos = 0;
uint8_t* gpRXEnd = 0;

const unsigned char gbWait = 1;
/**
 * Function Declarations
 */


/*
 * Particle Sensor
 *
 *
 */
 
void test(void)
{
    softReset();

    setPulseWidth(MAX30105_PULSEWIDTH_215);
    setSampleRate(MAX30105_SAMPLERATE_400);
    setPulseAmplitudeRed(0xFF);
}

void softReset(void) {
  bitMask(MAX30105_MODECONFIG, MAX30105_RESET_MASK, MAX30105_RESET);

  // Poll for bit to clear, reset is then complete
  // Timeout after 100ms
  while (1)
  {
    uint8_t response = readRegister(MAX30105_MODECONFIG);
    if ((response & MAX30105_RESET) == 0) break; //We're done!
//    __delay_cycles(1000); //Let's not over burden the I2C bus
  }
}
void setADCRange(uint8_t adcRange) {
  // adcRange: one of MAX30105_ADCRANGE_2048, _4096, _8192, _16384
  bitMask(MAX30105_PARTICLECONFIG, MAX30105_ADCRANGE_MASK, adcRange);
}


void setLEDMode(uint8_t mode) {
  // Set which LEDs are used for sampling -- Red only, RED+IR only, or custom.
  // See datasheet, page 19
  bitMask(MAX30105_MODECONFIG, MAX30105_MODE_MASK, mode);
}

void setPulseAmplitudeRed(uint8_t amplitude) {
  writeRegister(MAX30105_LED1_PULSEAMP, amplitude);
}

void setSampleRate(uint8_t sampleRate) {
  // sampleRate: one of MAX30105_SAMPLERATE_50, _100, _200, _400, _800, _1000, _1600, _3200
  bitMask(MAX30105_PARTICLECONFIG, MAX30105_SAMPLERATE_MASK, sampleRate);
}

void setPulseWidth(uint8_t pulseWidth) {
  // pulseWidth: one of MAX30105_PULSEWIDTH_69, _188, _215, _411
  bitMask(MAX30105_PARTICLECONFIG, MAX30105_PULSEWIDTH_MASK, pulseWidth);
}

void bitMask(uint8_t reg, uint8_t mask, uint8_t thing)
{
  // Grab current register context
  uint8_t originalContents = readRegister(reg);

  // Zero-out the portions of the register we're interested in
  originalContents = originalContents & mask;

  // Change contents
  writeRegister(reg, originalContents | thing);
}



/*
 * To read from Particle Sensor:
 *      1. Initialize slave address
 *      2. Send Start Message
 *      3. Send PS the register to be read
 *      4. Send a Re Start Message
 *      5. Read from Particle Sensor
 *
 */
uint8_t readRegister(uint8_t reg) {
    /*
     * Send particle Sensor the address of the register to be read.
     * Then read the value returned by the Particle Sensor
     */
    uint8_t out[10];
    I2CwriteRead(writeAddr, 2, &reg, 1, &out[0]);

    return out[0];
}

uint8_t writeRegister(uint8_t reg, uint8_t value) {
    uint8_t out[] = {reg, value};
    I2Cwrite(writeAddr, 2, &out[0]);

    return *gpRXPos;
}


// Die Temperature
// Returns temp in C
long readTemperature() {
    
    writeRegister(MAX30105_DIETEMPCONFIG, 0x01); // Tell the particle sensor to take 1 temperature sample.

    // Poll for bit to clear, reading is then complete
    // Timeout after 100ms
    //unsigned long startTime = millis();
    while (1) // TODO: Implement fail/timeout functionality
    {
        uint8_t response = readRegister(MAX30105_DIETEMPCONFIG);
        if ((response & 0x01) == 0) break; //We're done!
        __delay_cycles(1000000); //Let's not over burden the I2C bus
    }


    // Step 2: Read die temperature register (integer)
    int8_t tempInt = readRegister(MAX30105_DIETEMPINT);
    uint8_t tempFrac = readRegister(MAX30105_DIETEMPFRAC);

    // Step 3: Calculate temperature (datasheet pg. 23)
    return (long) tempInt + ((long)tempFrac * 0.0625);
}

// Returns die temp in F
float readTemperatureF() {
    float temp = readTemperature();

    if (temp != -999.0) temp = temp * 1.8 + 32.0;

    return (temp);
}


/*
 *
 * I2C Communications
 *
 *
 */


unsigned char USCI_I2C_NotReady()
{
    return (UCB0STAT & UCBBUSY);
}

void USCI_I2C_Init(unsigned char pinSel, unsigned char clkDiv)
{
    P1SEL |= pinSel;    // Assign I2C pins to USCI_B0
    P1SEL2|= pinSel;

    UCB0CTL1 |= UCSWRST;    // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;   // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;  // Use SMCLK, keep SW reset
    UCB0BR0 = clkDiv;   // fSCL = SMCLK/12 = ~100kHz
    UCB0BR1 = 0;
}


void I2CwriteRead(uint8_t slaveAddress,
               uint8_t sendCount,
               uint8_t* pSendData,
               uint8_t recvCount,
               uint8_t* pRecvData
)
{
    /*
     * Initialize pointers for start and end of read/write buffers.
     */
    gpTXPos = pSendData;
    gpTXEnd = pSendData + sendCount;
    gpRXPos = pRecvData;
    gpRXEnd = pRecvData + recvCount;

    UCB0CTL1 |= UCSWRST;        // Enable Software reset
    UCB0TXBUF = 0;              // Clear TX buffer (Not necessary?)
    UCB0I2CSA = slaveAddress;   // Set the Slave Address
    UCB0CTL1 &= ~UCSWRST;       // Clear SW reset, resume operation

    UCB0I2CIE = UCNACKIE;       // Enable NACK interrupt

    // TRANSMIT DATA
    IE2 |= UCB0TXIE;            // Enable TX interrupt
    UCB0CTL1 |= UCTR + UCTXSTT; // I2C TX, start condition

    if (gbWait)
        __bis_SR_register(CPUOFF + GIE);    // Enter LPM0 w/ interrupts for TX
}


void I2Cwrite(uint8_t slaveAddress,
               uint8_t sendCount,
               uint8_t* pSendData
)
{
    /*
     * Initialize pointers for start and end of read/write buffers.
     */
    gpTXPos = pSendData;
    gpTXEnd = pSendData + sendCount;
    gpRXPos = gpRXEnd;

    UCB0CTL1 |= UCSWRST;        // Enable Software reset
    UCB0TXBUF = 0;              // Clear TX buffer (Not necessary?)
    UCB0I2CSA = slaveAddress;   // Set the Slave Address
    UCB0CTL1 &= ~UCSWRST;       // Clear SW reset, resume operation

    UCB0I2CIE = UCNACKIE;       // Enable NACK interrupt

    // TRANSMIT DATA
    IE2 |= UCB0TXIE;            // Enable TX interrupt
    UCB0CTL1 |= UCTR + UCTXSTT; // I2C TX, start condition

    if (gbWait)
        __bis_SR_register(CPUOFF + GIE);    // Enter LPM0 w/ interrupts for TX
}


#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
    P1OUT ^= BIT0;
    if (IFG2 & UCB0RXIFG) {                  // (UCTR == 0) // RX mode
        *gpRXPos = UCB0RXBUF;                     // Read one int from RX buffer
        gpRXPos += size8;                         // Increment pointer to next uint8_t in array.
        if (gpRXPos + 1 == gpRXEnd)               // If NEXT to LAST send STOP
            UCB0CTL1 |= UCTXSTP;                  // send STOP
        if (gpRXPos == gpRXEnd) {                 // Last read SHUTDOWN
            IE2 &= ~UCB0RXIE;                     // Disable RX and TX interrupt
            IFG2 &= ~UCB0RXIFG;                   // Clear USCI_B0 TX int flag
            LPM0_EXIT;                            // Exit LPM0
        }
    } else {
    // if (UCB0CTL1 & UCTR && IFG2 & UCB0TXIFG) {    // TX mode (UCTR == 1)
        if (gpTXPos < gpTXEnd) {                    // TRUE if more bytes remain
            UCB0TXBUF = *(gpTXPos);               // Load TX buffer TODO: how much it sends??
            gpTXPos += size8;                     // Increment pointer to next uint8_t in array.
        } else {                                    // no more bytes to send
            IE2 &= ~UCB0TXIE;                     // Disable TX interrupt
            IFG2 &= ~UCB0TXIFG;                   // Clear USCI_B0 TX int flag
            if (gpRXPos < gpRXEnd) {              // RX setup
                //UCB0I2CSA = readAddr;               // TODO: Does this work??? Do we have to switch to Slave read Address
                UCB0CTL1 &= ~UCTR;                // Set Receive Clear UCTR
                UCB0CTL1 |= UCTXSTT;              // I2C restart condition
                IE2 |= UCB0RXIE;                  // Enable RX interrupt
                if (gpRXPos + 1 == gpRXEnd) {     // 1 RX byte left
                while (UCB0CTL1 & UCTXSTT);       // wait for START to send STOP (this is dumb!)
                    UCB0CTL1 |= UCTXSTP;          // send STOP
                }
            }
            else {                                // TX only
                UCB0CTL1 |= UCTXSTP;              // send STOP
                LPM0_EXIT;                        // Exit LPM0
            }
        }
    }
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
    if (UCB0STAT & UCNACKIFG) {                   // I2C Busy/Error & Not acknowledged interrupt flag trigered
        UCB0CTL1 |= UCTXSTP;                      // Generate I2C stop condition
        UCB0STAT &= ~UCNACKIFG;                   // Clear IFG
        LPM0_EXIT;                                // Exit LPM0
    }
}

