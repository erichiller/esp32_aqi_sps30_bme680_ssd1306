#include "config.h"
#include "secrets.h"
#include <WebServer.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// #include "ThingSpeak.h"

#include "sensirion_uart.h"
#include "sps30.h"
#include "ssh1106.h"
#include "bme680.h"

#include "index.h"    // html template

#include "InfluxDb.h"


#if WPA2EN
#include "esp_wpa2.h"
#include <Wire.h>
const char *ssid     = SSIDNAME;
const char *username = "Username";
const char *password = PASSWORD;
#else
const char *ssid     = SSIDNAME;
const char *password = PASSWORD;
#endif


int LOCATE = Denver;    // Select location

struct sps30_measurement val;

s16 ret;


WiFiUDP   ntpUDP;
NTPClient timeClient( ntpUDP );


String label[] = {
    "Yates"};
String sname = "SPS30";    // Sensor name

int counter = 0;

uint8_t error_cnt = 0;

unsigned long Timer1;

WebServer   server( 80 );

#if INFLUX_DB_ENABLED
Influxdb influx( INFLUXDB_HOST );
#endif

// Read CPU Temp
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read( );
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read( );

// Functions for handle uri request

void handleSetAutoClean( ) {
	// Serial.println(server.arg(0));
	s16 status;
	u8  d = ( server.arg( 0 ) ).toInt( );

	status = sps30_set_fan_auto_cleaning_interval_days( d );
	server.send( 200, "text/html", ( status == 0 ) ? "ok" : "error" );
}
void handleRoot( ) {
	String s = MAIN_page;
	server.send( 200, "text/html", s );
}

void handlePM( ) {
	// struct sps30_measurement val;
	s16 ret;
	u16 fanspeed;
	// u32 cleanhr;
	u8 cleanday;

	int temp = ( ( temprature_sens_read( ) - 32 ) / 1.8 );    // CPU Temp
	sps30_read_fan_speed( &fanspeed );                        // fan speed in rpm
	// sps30_get_fan_auto_cleaning_interval(&cleanhr); // auto clean
	sps30_get_fan_auto_cleaning_interval_days( &cleanday );    // auto clean in day

	do {
		ret = sps30_read_measurement( &val );
		delay( 200 );
	} while( ret < 0 );

	// JSON Format

	String data = "[{\"name\":\"pm1\",\"val\":\"" + String( val.mc_1p0, 2 ) + "\"}";
	data += ",{\"name\":\"pm2\",\"val\":\"" + String( val.mc_2p5, 2 ) + "\"}";
	data += ",{\"name\":\"pm4\",\"val\":\"" + String( val.mc_4p0, 2 ) + "\"}";
	data += ",{\"name\":\"pm10\",\"val\":\"" + String( val.mc_10p0, 2 ) + "\"}";
	data += ",{\"name\":\"nc05\",\"val\":\"" + String( val.nc_0p5, 2 ) + "\"}";
	data += ",{\"name\":\"nc1\",\"val\":\"" + String( val.nc_1p0, 2 ) + "\"}";
	data += ",{\"name\":\"nc2\",\"val\":\"" + String( val.nc_2p5, 2 ) + "\"}";
	data += ",{\"name\":\"nc4\",\"val\":\"" + String( val.nc_4p0, 2 ) + "\"}";
	data += ",{\"name\":\"nc10\",\"val\":\"" + String( val.nc_10p0, 2 ) + "\"}";
	data += ",{\"name\":\"size\",\"val\":\"" + String( val.typical_particle_size ) +
	        "\"}";
	data += ",{\"name\":\"temp\",\"val\":\"" + String( temp ) + "\"}";
	data += ",{\"name\":\"fanspeed\",\"val\":\"" + String( fanspeed ) + "\"}";
	// data += ",{\"name\":\"cleanhr\",\"val\":\"" + String(cleanhr/(1000 * 60 *
	// 60)) +"\"}";
	data += ",{\"name\":\"cleanday\",\"val\":\"" + String( cleanday ) + "\"}";
	data += ",{\"name\":\"nodename\",\"val\":\"" + label[LOCATE] + "\"}]";

	server.send( 200, "application/json", data );
}

// Check wifi connection and get data from sensor and sent to api server

void CheckWifi( ) {
	if( WiFi.status( ) == WL_CONNECTED ) {
		counter = 0;
	} else if( WiFi.status( ) != WL_CONNECTED ) {
#if WPA2EN
		esp_wifi_sta_wpa2_ent_set_identity( (uint8_t *)username, strlen( username ) );
		esp_wifi_sta_wpa2_ent_set_username( (uint8_t *)username, strlen( username ) );
		esp_wifi_sta_wpa2_ent_set_password( (uint8_t *)password, strlen( password ) );
		esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT( );
		esp_wifi_sta_wpa2_ent_enable( &config );
		WiFi.begin( ssid );
#else
		WiFi.begin( ssid, password );
#endif
		while( WiFi.status( ) != WL_CONNECTED ) {
			delay( 500 );
			Serial.print( "." );
			counter++;

			if( counter >= 60 ) {    // after 30 seconds timeout - reset board
				ESP.restart( );
			}
		}
	}
}

void measure_sps30( ) {
	do {
		ret = sps30_read_measurement( &val );
		delay( 500 );
#if DEBUG
		Serial.println( "measure_sps30, 500ms" );
#else
		Serial.print( "*" );
#endif
	} while( ret > 0 );

	if( SPS_IS_ERR_STATE( ret ) ) {
		printf( "Chip state: %u - measurements may not be accurate\n",
		        SPS_GET_ERR_STATE( ret ) );
	}
}

void PushToApi( ) {
	u16 fanspeed;
	// u32 cleanhr;
	u8  auto_clean_days;
	u32 auto_clean;
	
	Serial.println( );
	Serial.print( "time:\t");
	Serial.println( timeClient.getFormattedTime( ) );


	int temp = ( ( temprature_sens_read( ) - 32 ) / 1.8 );    // CPU Temp

	ret = sps30_read_fan_speed( &fanspeed );    // fan speed in rpm
	if( ret )
		printf( "error %d retrieving the auto-clean interval\n", ret );
	else
		printf( "auto-cleaning interval is %d seconds\n", fanspeed );


	ret = sps30_get_fan_auto_cleaning_interval( &auto_clean );
	if( ret )
		printf( "error %d retrieving the auto-clean interval\n", ret );
	else
		printf( "auto-cleaning interval is %d seconds\n", auto_clean );

	ret = sps30_get_fan_auto_cleaning_interval_days( &auto_clean_days );
	if( ret )
		printf( "error retrieving the auto-clean interval\n" );
	else
		printf( "auto-cleaning interval set to %u days\n", auto_clean_days );

	// print out for debug
	printf( "\nmeasured values:\n"
	        "\t%0.2f pm1.0\n"
	        "\t%0.2f pm2.5\n"
	        "\t%0.2f pm4.0\n"
	        "\t%0.2f pm10.0\n"
	        "\t%0.2f nc0.5\n"
	        "\t%0.2f nc1.0\n"
	        "\t%0.2f nc2.5\n"
	        "\t%0.2f nc4.5\n"
	        "\t%0.2f nc10.0\n"
	        "\t%0.2f typical particle size\n"
	        "\t%0.2f temp\n"
	        "\t%0.2f fanspeed\n"
	        "\t%d auto_clean\n"
	        "\t%d auto_clean_days\n"
	        "\n",
	        val.mc_1p0,
	        val.mc_2p5,
	        val.mc_4p0,
	        val.mc_10p0,
	        val.nc_0p5,
	        val.nc_1p0,
	        val.nc_2p5,
	        val.nc_4p0,
	        val.nc_10p0,
	        val.typical_particle_size,
	        temp,
	        fanspeed,
	        auto_clean,
	        auto_clean_days );
	// Serial.print( val.mc_1p0 );
	// Serial.print( F( "\t" ) );
	// Serial.print( val.mc_2p5 );
	// Serial.print( F( "\t" ) );
	// Serial.print( val.mc_4p0 );
	// Serial.print( F( "\t" ) );
	// Serial.print( val.mc_10p0 );
	// Serial.println( );
	// Serial.print( val.nc_0p5 );
	// Serial.print( F( "\t" ) );
	// Serial.print( val.nc_1p0 );
	// Serial.print( F( "\t" ) );
	// Serial.print( val.nc_2p5 );
	// Serial.print( F( "\t" ) );
	// Serial.print( val.nc_4p0 );
	// Serial.print( F( "\t" ) );
	// Serial.print( val.nc_10p0 );
	// Serial.print( F( "\t" ) );
	// Serial.print( val.typical_particle_size );

	// Serial.println( );

	// sent to api server

//  if (API_ENABLED) {
//    String url = "/api/input";
//    String data = "pm1=" + String(val.mc_1p0, 2);
//    data += "&pm2=" + String(val.mc_2p5, 2);
//    data += "&pm4=" + String(val.mc_4p0, 2);
//    data += "&pm10=" + String(val.mc_10p0, 2);
//    data += "&lat=" + String(locations[LOCATE][LAT], 6);
//    data += "&lon=" + String(locations[LOCATE][LON], 6);
//    data += "&label=" + label[LOCATE];
//    data += "&sname=" + sname;
//  }
#if THINGSPEAK_ENABLED
	ThingSpeak.setField( 1, val.mc_1p0 );
	ThingSpeak.setField( 2, val.mc_2p5 );
	ThingSpeak.setField( 3, val.mc_4p0 );
	ThingSpeak.setField( 4, val.mc_10p0 );
	ThingSpeak.setField( 5, bme.temperature );
	ThingSpeak.setField( 6, (float)( bme.pressure / 100.0 ) );
	ThingSpeak.setField( 7, bme.humidity );
	ThingSpeak.setField( 8, (float)( bme.gas_resistance / 1000.0 ) );
	//  ThingSpeak.setField(5, label[LOCATE]);

	// write to the ThingSpeak channel
	int x = ThingSpeak.writeFields( tspk_chan, tspk_key );
	if( x == 200 ) {
		Serial.println( "Channel update successful." );
	} else {
		Serial.println( "Problem updating channel. HTTP error code " + String( x ) );
	}
#endif
#if INFLUX_DB_ENABLED

	InfluxData measurement1( "air_quality_1p0" );
	measurement1.addTag( "device", HOSTNAME );
	measurement1.addTag( "sensor", "sps30" );
	measurement1.addValue( "value", val.mc_1p0 );
	influx.prepare( measurement1 );

	InfluxData measurement2( "air_quality_2p5" );
	measurement2.addTag( "device", HOSTNAME );
	measurement2.addTag( "sensor", "sps30" );
	measurement2.addValue( "value", val.mc_2p5 );
	influx.prepare( measurement2 );

	InfluxData measurement3( "air_quality_4p0" );
	measurement3.addTag( "device", HOSTNAME );
	measurement3.addTag( "sensor", "sps30" );
	measurement3.addValue( "value", val.mc_4p0 );
	influx.prepare( measurement3 );

	InfluxData measurement4( "air_quality_10p0" );
	measurement4.addTag( "device", HOSTNAME );
	measurement4.addTag( "sensor", "sps30" );
	measurement4.addValue( "value", val.mc_10p0 );
	influx.prepare( measurement4 );



	InfluxData measurement5( "temperature" );
	measurement5.addTag( "device", HOSTNAME );
	measurement5.addTag( "sensor", "bme680" );
	measurement5.addValue( "value", bme.temperature );
	influx.prepare( measurement5 );

	InfluxData measurement6( "pressure" );
	measurement6.addTag( "device", HOSTNAME );
	measurement6.addTag( "sensor", "bme680" );
	measurement6.addValue( "value", (float)( bme.pressure / 100.0 ) );
	influx.prepare( measurement6 );

	InfluxData measurement7( "humidity" );
	measurement7.addTag( "device", HOSTNAME );
	measurement7.addTag( "sensor", "bme680" );
	measurement7.addValue( "value", bme.humidity );
	influx.prepare( measurement7 );

	InfluxData measurement8( "gas_resistance" );
	measurement8.addTag( "device", HOSTNAME );
	measurement8.addTag( "sensor", "bme680" );
	measurement8.addValue( "value", (float)( bme.gas_resistance / 1000.0 ) );
	influx.prepare( measurement8 );

	debug( "writing to influxdb \n" );

	if( influx.write( ) ) {
		debug( "successfully wrote to influxdb \n" );
	} else {
		debug( " error during write to influxdb\n" );
	}


#endif
}




/**
 * setup
 */
void setup( ) {
	Serial.begin( 115200 );

	// Setup screen for message display
	debug( "Setting up screen\n" );
	setup_ssh1106( );
	message_ssh1106( "Setup Screen", "Complete" );
	message_ssh1106( "Setup SPS30", "..." );

	sensirion_uart_open( );    // start sensor
	message_ssh1106( "Set Up SPS30", "Complete" );
	delay( 1000 );

	message_ssh1106( "Probe SPS30", "..." );
	while( sps30_probe( ) != 0 ) {
		Serial.println( "probe failed" );
		message_ssh1106( "Probe SPS30", "ERROR" );
		delay( 1000 );
	}
	message_ssh1106( "Probe SPS30", "Complete" );

	message_ssh1106( "Start SPS30", "..." );
	if( sps30_start_measurement( ) != 0 ) {
		message_ssh1106( "Start SPS30", "ERROR" );
		Serial.println( "error starting measurement" );
	}
	message_ssh1106( "Start SPS30", "Complete" );


	// ret = sps30_reset( );
	// if( ret )
	// 	printf( "error retrieving the auto-clean interval\n" );

	ret = sps30_set_fan_auto_cleaning_interval( 86400 );
	if( ret )
		printf( "error %d setting the auto-clean interval\n", ret );


	u8 auto_clean_days;
	ret = sps30_get_fan_auto_cleaning_interval_days( &auto_clean_days );
	if( ret )
		printf( "error retrieving the auto-clean interval\n" );
	else
		printf( "auto-cleaning interval set to %u days\n", auto_clean_days );

	// print out for debug
	message_ssh1106( "Connect WiFi", "..." );
	Serial.println( );
	Serial.println( );
	Serial.print( "Connecting to " );
	Serial.println( ssid );

	WiFi.disconnect( true );    // make sure
	WiFi.mode( WIFI_STA );

	message_ssh1106( "Check WiFi", "..." );
	CheckWifi( );

	// print out for debug
	Serial.print( "IP address: " );
	message_ssh1106( "Receive IP", "..." );
	Serial.println( WiFi.localIP( ) );

	// implement server handler
	server.on( "/", handleRoot );
	server.on( "/pm2", handlePM );
	server.on( "/setautoclean", handleSetAutoClean );

	// start http server
	message_ssh1106( "Starting HTTP Server", "..." );
	server.begin( );
	message_ssh1106( "Starting HTTP Server", "Complete" );
	Serial.println( "HTTP server started" );

	Serial.println( "Setting up NTP...");
	timeClient.begin( );
	if( timeClient.forceUpdate( ) ) {
		Serial.println( "Setting up NTP... Complete" );
		timeClient.setTimeOffset(-6 * 60 * 60);
		Serial.print( "\tPresent time is " );
		Serial.println( timeClient.getFormattedTime( ) );
	} else {
		Serial.println( "Setting up NTP... ERROR updating time" );
	}

	debug( "Setting up BME680 sensor\n" );
	message_ssh1106( "Setup BME680", "..." );
	setup_bme680( );
	message_ssh1106( "Setup BME680", "Complete" );

#if INFLUX_DB_ENABLED
	message_ssh1106( "Setup InfluxDB", "..." );
	influx.setDb( INFLUXDB_DATABASE );
	message_ssh1106( "Setup InfluxDB", "Complete" );
#endif
#if THINGSPEAK_ENABLED
	ThingSpeak.begin( client );
#endif
	// send frist data to api server
	// PushToApi( );
}

void loop( ) {
	// Alway CheckWifi
	CheckWifi( );

	// http server always wait for client
	server.handleClient( );

	measure_sps30( );
	measure_bm680( );
	// send data to api every 1 min
	if( millis( ) - Timer1 >= 60000 ) {
		Timer1 = millis( );
		PushToApi( );
	}
	update_ssh1106( &val , timeClient.getFormattedTime());

	delay( 1 );
}
