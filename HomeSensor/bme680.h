#ifndef BME680_H
#define BME680_H

#include "Adafruit_BME680.h"


void bme680_setup();


void bme680_update_screen();


extern Adafruit_BME680 bme;    // I2C


#endif