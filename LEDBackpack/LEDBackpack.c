#include "LEDBackpack.h"
#include "USCI_I2C_G2553.h"

static uint8_t numbertable[] = {
	0x3F, /* 0 */
	0x06, /* 1 */
	0x5B, /* 2 */
	0x4F, /* 3 */
	0x66, /* 4 */
	0x6D, /* 5 */
	0x7D, /* 6 */
	0x07, /* 7 */
	0x7F, /* 8 */
	0x6F, /* 9 */
	0x77, /* a */
	0x7C, /* b */
	0x39, /* C */
	0x5E, /* d */
	0x79, /* E */
	0x71, /* F */
};

unsigned char i2c_addr = 0x70;


void setBrightness(uint8_t b) {
  if (b > 15) b = 15;
  unsigned char data = HT16K33_CMD_BRIGHTNESS | b;

  // USCI_I2C_transmitinit(i2c_addr, 0x12);
  while( USCI_I2C_notready() );
  USCI_I2C_transmit((unsigned char) 1, &data);

}

void blinkRate(uint8_t b) {
  // USCI_I2C_transmitinit(i2c_addr, 0x12);
  if (b > 3) b = 0; // turn off if not sure
  unsigned char data = HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (b << 1);
  USCI_I2C_transmit(1, &data);
  
}

void begin() {
  i2c_addr = i2c_addr;
  USCI_I2C_transmitinit(i2c_addr, 0x12);
  while( USCI_I2C_notready() );
  unsigned char data = 0x21;
  USCI_I2C_transmit(1, &data);  // turn on oscillator
  blinkRate(HT16K33_BLINK_OFF);
  
  setBrightness(15); // max brightness
}

void writeDisplay(void) {
  // USCI_I2C_transmitinit(i2c_addr, 0x12);
  while( USCI_I2C_notready() );
  unsigned char abuffer[17] = {0x00, displaybuffer[0] & 0xFF, displaybuffer[0] >> 8,
                               displaybuffer[1] & 0xFF, displaybuffer[1] >> 8,
                               displaybuffer[2] & 0xFF, displaybuffer[2] >> 8,
                               displaybuffer[3] & 0xFF, displaybuffer[3] >> 8,
                               displaybuffer[4] & 0xFF, displaybuffer[4] >> 8,
                               displaybuffer[5] & 0xFF, displaybuffer[5] >> 8,
                               displaybuffer[6] & 0xFF, displaybuffer[6] >> 8,
                               displaybuffer[7] & 0xFF, displaybuffer[7] >> 8};

  USCI_I2C_transmit(17, &abuffer[0]);
}

void clear(void) {
  uint8_t i;
  for (i=0; i<8; i++) {
    displaybuffer[i] = 0;
  }
}

void writeDigitRaw(uint8_t n, uint16_t bitmask) {
  displaybuffer[n] = bitmask;
}

// void writeDigitAscii(uint8_t n, uint8_t a,  boolean d) {
//   uint16_t font = pgm_read_word(alphafonttable+a);

//   displaybuffer[n] = font;

//   /*
//   Serial.print(a, DEC);
//   Serial.print(" / '"); Serial.write(a);
//   Serial.print("' = 0x"); Serial.println(font, HEX);
//   */

//   if (d) displaybuffer[n] |= (1<<14);
// }
