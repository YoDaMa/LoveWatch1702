#ifndef USCI_I2C_G2553_H_
#define USCI_I2C_G2553_H_

#define SDA_PIN 0x06                                  // msp430x261x UCB0SDA pin
#define SCL_PIN 0x07                                  // msp430x261x UCB0SCL pin

void USCI_I2C_receiveinit(unsigned char slave_address, unsigned char prescale);
void USCI_I2C_transmitinit(unsigned char slave_address, unsigned char prescale);


void USCI_I2C_receive(unsigned char byteCount, unsigned char *field);
void USCI_I2C_transmit(unsigned char byteCount, unsigned char *field);


unsigned char USCI_I2C_slave_present(unsigned char slave_address);
unsigned char USCI_I2C_notready();


#endif // USCI_I2C_G2553_H_
