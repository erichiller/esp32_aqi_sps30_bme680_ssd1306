#ifndef CONFIG_H
#define CONFIG_H
// Config here

#define SanMateo 0    // Your location
#define NECTEC 1
#define TEST 2
#define BEDROOM 3
#define LIVINGROOM 4
#define NECTECCO2 5

#define LAT 0
#define LON 1

#define API_ENABLED 0
#define THINGSPEAK_ENABLED 1

#define WPA2EN false         // true if use WPA2 Enterprise
#define WIFI_SECURE false    // use WiFiClient or WiFiClientSecure

#define DEBUG true    // enable debug function (print to Serial)



#if WIFI_SECURE
#include <WiFiClientSecure.h>
#define CLIENT_TYPE WiFiClientSecure
extern CLIENT_TYPE client;
#else
#include <WiFiClient.h>
#define CLIENT_TYPE WiFiClient
extern CLIENT_TYPE client;
#endif

#if DEBUG
#define debugln( x ) Serial.println( x )
#define debug( x ) Serial.print( x )
#else
#define debugln( x ) (void)0
#define debug( x ) (void)0
#endif

#endif