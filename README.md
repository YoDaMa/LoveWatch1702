# LoveWatch 1702

### ELEC 327 Final Project
### Yoseph Maguire, Yovahn Hoole, Chandler Burke

## Abstract

Love Watch 1702 is a smart watch capable of measuring heart rate, oxygen saturation and most importantly the presence of loovvee. We three members of the Lonely Hearts Club, Yoseph Maguire, Yovahn Hoole and Chandler Burke have come up with a unique and state of the art system to help a user detect if they are in love by detecting a spike in heart rate. This research has far reaching consequences in protecting young ELECs from the distractions of college life.  


## Final Product

After pivoting drastically due to the failure of the I2C MSP430 Library we wrote, among other things, we ended up with a design based on an Arduino. Represented in breadboard version, the system uses I2C to read infrared values from the MAX30105 sensor, and then after using those values to calculate heart rate, send back graphical representations of heart rate to the 14-Segment Display. To model a simple threshold algorithm, if the user heart rate exceeds 80 BPM, it triggers a “Love” animation, indicating to the user that they have the potential to be in love (note, this system has an extremely high false positive rate).

![Image of Arduino Circuit](https://github.com/YoDaMa/LoveWatch1702/blob/master/img/arduinocircuit.jpg)


