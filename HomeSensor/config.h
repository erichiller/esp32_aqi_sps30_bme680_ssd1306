#ifndef CONFIG_H
#define CONFIG_H
// Config here

#define ESP32       // for influx library

#define Denver 0    // Your location

#define LAT 0
#define LON 1

#define API_ENABLED 0
#define THINGSPEAK_ENABLED 0

#define WPA2EN false         // true if use WPA2 Enterprise
#define WIFI_SECURE false    // use WiFiClient or WiFiClientSecure

#define DEBUG false    // enable debug function (print to Serial)


#define HOSTNAME "aqi01.hiller.pro"

/*** INFLUX DB ***/
#define INFLUX_DB_ENABLED 1
#define INFLUXDB_HOST "192.168.10.125"
// #define INFLUXDB_PORT "8086"
#define INFLUXDB_DATABASE "home"
//if used with authentication
// #define INFLUXDB_USER "user"
// #define INFLUXDB_PASS "password"


// #if WIFI_SECURE
// #include <WiFiClientSecure.h>
// #define CLIENT_TYPE WiFiClientSecure
// extern CLIENT_TYPE client;
// #else
// #include <WiFiClient.h>
// #define CLIENT_TYPE WiFiClient
// extern CLIENT_TYPE client;
// #endif

#if DEBUG
#define debugln( x ) Serial.println( x )
#define debug( x ) Serial.print( x )
#else
#define debugln( x ) (void)0
#define debug( x ) (void)0
#endif

#endif