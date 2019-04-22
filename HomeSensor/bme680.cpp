// https://github.com/adafruit/Adafruit_BME680
#include "Adafruit_BME680.h"

extern Adafruit_BME680 bme = {};    // I2C

// setup
void bme680_setup( ) {
	Serial.println( F( "BME680 test" ) );
	if( !bme.begin( ) ) {
		Serial.println( "Could not find a valid BME680 sensor, check wiring!" );
		while( 1 )
			;
	}
	// Set up oversampling and filter initialization
	bme.setTemperatureOversampling( BME680_OS_8X );
	bme.setHumidityOversampling( BME680_OS_2X );
	bme.setPressureOversampling( BME680_OS_4X );
	bme.setIIRFilterSize( BME680_FILTER_SIZE_3 );
	bme.setGasHeater( 320, 150 );    // 320*C for 150 ms
}

void bme680_update_screen( ) {
	// display.setCursor( 0, 0 );
	// display.clearDisplay( );

	if( !bme.performReading( ) ) {
		Serial.println( "Failed to perform reading on BME680 !" );
		return;
	}
	Serial.print( "Temperature = " );
	Serial.print( bme.temperature );
	Serial.println( " *C" );
	// display.print( "Temperature: " );
	// display.print( bme.temperature );
	// display.println( " *C" );

	Serial.print( "Pressure = " );
	Serial.print( bme.pressure / 100.0 );
	Serial.println( " hPa" );
	// display.print( "Pressure: " );
	// display.print( bme.pressure / 100 );
	// display.println( " hPa" );

	Serial.print( "Humidity = " );
	Serial.print( bme.humidity );

	Serial.println( " %" );
	// display.print( "Humidity: " );
	// display.print( bme.humidity );
	// display.println( " %" );

	Serial.print( "Gas = " );
	Serial.print( bme.gas_resistance / 1000.0 );
	Serial.println( " KOhms" );
	// display.print( "Gas: " );
	// display.print( bme.gas_resistance / 1000.0 );
	// display.println( " KOhms" );

	// #define SEALEVELPRESSURE_HPA (1013.25)
	// readAltitude

	Serial.println( );
	// display.display( );
	delay( 2000 );
}