#include <Arduino.h>
#include <U8g2lib.h>
#include "ssh1106.h"
#include "config.h"
#include "Adafruit_BME680.h"
#include "bme680.h"

// pinMode( 22,  );

U8G2_SH1106_128X64_NONAME_1_4W_HW_SPI u8g2( U8G2_R0, /* cs=*/5, /* dc=*/33, /* reset=*/32 );

/**
 * Menu
 */
uint8_t MENU_pin_select = 4;    // button. must connect other side to ground
uint8_t MENU_pin_next   = 0;    // button. must connect other side to ground
uint8_t MENU_pin_prev   = 2;    // button. must connect other side to ground
uint8_t MENU_pin_up     = 26;

char print_action[20] = "none";

char buf_d1[32];
char buf_d2[32];
char bufTime[9];

void setup_ssh1106(){
	pinMode( MENU_pin_select, INPUT_PULLDOWN );
	pinMode( MENU_pin_next, INPUT_PULLUP );
    u8g2.begin( analogInputToDigitalPin(MENU_pin_select), analogInputToDigitalPin(MENU_pin_next), analogInputToDigitalPin(MENU_pin_prev), MENU_pin_up, U8X8_PIN_NONE, U8X8_PIN_NONE );    // https://github.com/olikraus/u8g2/wiki/u8g2reference#begin

}

void message_ssh1106( char *message, char *result ) {
	sprintf( buf_d1, message );
	sprintf( buf_d2, result );

	u8g2.firstPage( );
	do {
		u8g2.setFont( u8g2_font_6x10_tf );

		u8g2.setCursor( 5, 15 );
		// const char
		u8g2.print( F( buf_d1 ) );
		u8g2.setCursor( 5, 62 );
		u8g2.setFont( u8g2_font_6x10_tf );
		u8g2.print( F( buf_d2 ) );
	} while( u8g2.nextPage( ) );
}

void update_ssh1106( sps30_measurement *data, String ntpFormattedTime) {
	// u8g2.setFont( u8g2_font_logisoso30_tn );
	sprintf( buf_d1, "%f", data->mc_2p5 );
	sprintf( bufTime, "%s", ntpFormattedTime );
	// sprintf( buf_d1, "%i", 3 );
	// sprintf( buf_d2, "%f \xfe", bme.temperature );
	String( bme.temperature, 2 ).toCharArray( buf_d2, 5 );
	// sprintf( buf_d2, String( bme.temperature, 2 ).toCharArray( buf_d2, 4 ) );

	// debug("sensor is:");
	// debugln(data->nc_2p5);

	u8g2.firstPage( );
	do {
		u8g2.setFont( u8g2_font_9x15B_mn );

		u8g2.setCursor( 5, 15 );
		// const char
		u8g2.print( F( buf_d2 ) );
		u8g2.setCursor( 58, 15 );
		// const char
		u8g2.print( F( bufTime ) );
		// u8g2.setCursor( 5, 31 );
		// u8g2.print( F( print_action ) );
		u8g2.setCursor( 5, 62 );
		u8g2.setFont( u8g2_font_inb30_mr );
		u8g2.print( F( buf_d1 ) );
	} while( u8g2.nextPage( ) );

    
	/******************************************
	 ****       check for menu input       ****
	 ******************************************/
	int8_t event = u8g2.getMenuEvent( );
	switch( event ) {
		case U8X8_MSG_GPIO_MENU_SELECT:
			sprintf( print_action, "%s", "menu_select" );
			Serial.println( print_action );
			break;
		case U8X8_MSG_GPIO_MENU_NEXT:
			sprintf( print_action, "%s", "MENU_NEXT" );
			Serial.println( print_action );
			break;
		case U8X8_MSG_GPIO_MENU_PREV:
			sprintf( print_action, "%s", "MENU_PREV" );
			Serial.println( print_action );
			break;
		case U8X8_MSG_GPIO_MENU_UP:
			sprintf( print_action, "%s", "MENU_UP" );
			Serial.println( print_action );
			break;
		case U8X8_MSG_GPIO_MENU_DOWN:
			sprintf( print_action, "%s", "MENU_down" );
			Serial.println( print_action );
			break;
		case U8X8_MSG_GPIO_MENU_HOME:
			sprintf( print_action, "%s", "MENU_home" );
			Serial.println( print_action );
			break;
		default:
			sprintf( print_action, "%s %i", "DEFAULT pressed:", event );
			// Serial.println( print_action );
			break;
	}
}