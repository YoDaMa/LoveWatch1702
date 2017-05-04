/*
 * LW_USCI_I2C.h
 *
 *  Created on: May 3, 2017
 *      Author: Yoseph
 */
 #include <stdint.h>

#ifndef LW_USCI_I2C_H_
#define LW_USCI_I2C_H_

void I2CwriteRead(uint8_t slaveAddress,
               uint8_t sendCount,
               uint8_t* pSendData,
               uint8_t recvCount,
               uint8_t* pRecvData
);


void test(void);

void softReset(void);

void setADCRange(uint8_t adcRange);

void setLEDMode(uint8_t mode);

void setPulseAmplitudeRed(uint8_t amplitude);

void setSampleRate(uint8_t sampleRate);

void setPulseWidth(uint8_t pulseWidth);

void bitMask(uint8_t reg, uint8_t mask, uint8_t thing);

uint8_t readRegister(uint8_t reg);

uint8_t writeRegister(uint8_t reg, uint8_t value);

long readTemperature();

float readTemperatureF();

unsigned char USCI_I2C_NotReady();

void USCI_I2C_Init(unsigned char pinSel, unsigned char clkDiv);

void I2CwriteRead(uint8_t slaveAddress,
               uint8_t sendCount,
               uint8_t* pSendData,
               uint8_t recvCount,
               uint8_t* pRecvData
);

void I2Cwrite(uint8_t slaveAddress,
               uint8_t sendCount,
               uint8_t* pSendData
);


#endif /* LW_USCI_I2C_H_ */
