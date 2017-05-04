#include <stdint.h>

#ifndef LEDBACKPACK_H_
#define LEDBACKPACK_H_


#define LED_ON 1
#define LED_OFF 0

#define LED_RED 1
#define LED_YELLOW 2
#define LED_GREEN 3
 


#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_BLINK_OFF 0
#define HT16K33_BLINK_2HZ  1
#define HT16K33_BLINK_1HZ  2
#define HT16K33_BLINK_HALFHZ  3

#define HT16K33_CMD_BRIGHTNESS 0xE0

#define SEVENSEG_DIGITS 5


uint16_t displaybuffer[8]; 

void setBrightness(uint8_t b);
void blinkRate(uint8_t b);
void begin();
void writeDisplay(void);
void clear(void);
void writeDigitRaw(uint8_t n, uint16_t bitmask);


#endif // LEDBackpack_h

