#include "config.h"
#include "secrets.h"
#include <WebServer.h>
#include <WiFi.h>

#if WIFI_SECURE == TRUE
#include <WiFiClientSecure.h>
#else
#include <WiFiClient.h>
#endif
#include "ThingSpeak.h"

#include "sensirion_uart.h"
#include "sps30.h"
#include "ssh1106.cpp"

#include "index.h" // html template

// Config here

#define SanMateo 0      // Your location
#define NECTEC 1
#define TEST 2
#define BEDROOM 3
#define LIVINGROOM 4
#define NECTECCO2 5

#define LAT 0
#define LON 1

#define API_ENABLED 0
#define THINGSPEAK_ENABLED 1

#if WPA2EN
#include "esp_wpa2.h"
#include <Wire.h>

const char *ssid = SSIDNAME;
const char *username = "Username";
const char *password = PASSWORD;
#else
const char *ssid = SSIDNAME;
const char *password = PASSWORD;
#endif


int LOCATE = SanMateo; // Select location


String label[] = {
    "SanMateo",         "NECTEC-MrChoke",     "TEST-MrChoke",
    "BEDROOM-MrChoke", "LIVINGROOM-MrChoke", "NECTEC-CoWorking-F2"};
String sname = "SPS30"; // Sensor name





//const char *host1 = "api.thingspeak.com";
int counter = 0;

uint8_t error_cnt = 0;

unsigned long Timer1;

//WiFiClientSecure client;
WiFiClient client;
WebServer server(80);

// post to secure api https

void httpsPost(const char *host, String url, String data) {
  if (client.connect(host, 443)) {
    client.println("POST " + url + " HTTP/1.1");
    client.println("Host: " + (String)host);
    client.println("User-Agent: ESP32SPS30/1.0");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.println();
    client.println(data);
    // String response = client.readString(); // uncomment for debug
    client.stop();

    // return response;
    // Serial.println(response); // uncomment for debug
  }
}

// Read CPU Temp

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

// Functions for handle uri request

void handleSetAutoClean() {
  // Serial.println(server.arg(0));
  s16 status;
  u8 d = (server.arg(0)).toInt();

  status = sps30_set_fan_auto_cleaning_interval_days(d);
  server.send(200, "text/html", (status == 0) ? "ok" : "error");
}
void handleRoot() {
  String s = MAIN_page;
  server.send(200, "text/html", s);
}

void handlePM() {
  struct sps30_measurement val;
  s16 ret;
  u16 fanspeed;
  // u32 cleanhr;
  u8 cleanday;

  int temp = ((temprature_sens_read() - 32) / 1.8); // CPU Temp
  sps30_read_fan_speed(&fanspeed);                  // fan speed in rpm
  // sps30_get_fan_auto_cleaning_interval(&cleanhr); // auto clean
  sps30_get_fan_auto_cleaning_interval_days(&cleanday); // auto clean in day

  do {
    ret = sps30_read_measurement(&val);
    delay(200);
  } while (ret < 0);

  // JSON Format

  String data = "[{\"name\":\"pm1\",\"val\":\"" + String(val.mc_1p0, 2) + "\"}";
  data += ",{\"name\":\"pm2\",\"val\":\"" + String(val.mc_2p5, 2) + "\"}";
  data += ",{\"name\":\"pm4\",\"val\":\"" + String(val.mc_4p0, 2) + "\"}";
  data += ",{\"name\":\"pm10\",\"val\":\"" + String(val.mc_10p0, 2) + "\"}";
  data += ",{\"name\":\"nc05\",\"val\":\"" + String(val.nc_0p5, 2) + "\"}";
  data += ",{\"name\":\"nc1\",\"val\":\"" + String(val.nc_1p0, 2) + "\"}";
  data += ",{\"name\":\"nc2\",\"val\":\"" + String(val.nc_2p5, 2) + "\"}";
  data += ",{\"name\":\"nc4\",\"val\":\"" + String(val.nc_4p0, 2) + "\"}";
  data += ",{\"name\":\"nc10\",\"val\":\"" + String(val.nc_10p0, 2) + "\"}";
  data += ",{\"name\":\"size\",\"val\":\"" + String(val.typical_particle_size) +
          "\"}";
  data += ",{\"name\":\"temp\",\"val\":\"" + String(temp) + "\"}";
  data += ",{\"name\":\"fanspeed\",\"val\":\"" + String(fanspeed) + "\"}";
  // data += ",{\"name\":\"cleanhr\",\"val\":\"" + String(cleanhr/(1000 * 60 *
  // 60)) +"\"}";
  data += ",{\"name\":\"cleanday\",\"val\":\"" + String(cleanday) + "\"}";
  data += ",{\"name\":\"nodename\",\"val\":\"" + label[LOCATE] + "\"}]";

  server.send(200, "application/json", data);
}

// Check wifi connection and get data from sensor and sent to api server

void CheckWifi() {

  if (WiFi.status() == WL_CONNECTED) {
    counter = 0;
  } else if (WiFi.status() != WL_CONNECTED) {

#if WPA2EN

    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)username, strlen(username));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)username, strlen(username));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)password, strlen(password));
    esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
    esp_wifi_sta_wpa2_ent_enable(&config);
    WiFi.begin(ssid);

#else

    WiFi.begin(ssid, password);

#endif

    //  while(WiFi.waitForConnectResult() != WL_CONNECTED){
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      counter++;

      if (counter >= 60) { // after 30 seconds timeout - reset board
        ESP.restart();
      }
    }
  }
}

void PushToApi() {
  struct sps30_measurement val;
  s16 ret;

  do {
    ret = sps30_read_measurement(&val);
    delay(500);
  } while (ret < 0);

  // print out for debug

  Serial.print(val.mc_1p0);
  Serial.print(F("\t"));
  Serial.print(val.mc_2p5);
  Serial.print(F("\t"));
  Serial.print(val.mc_4p0);
  Serial.print(F("\t"));
  Serial.print(val.mc_10p0);
  Serial.println();

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
  ThingSpeak.setField(1, val.mc_1p0);
  ThingSpeak.setField(2, val.mc_2p5);
  ThingSpeak.setField(3, val.mc_4p0);
  ThingSpeak.setField(4, val.mc_10p0);
//  ThingSpeak.setField(5, label[LOCATE]);


  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(tspk_chan, tspk_key);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  #endif
}

void setup() {

  Serial.begin(115200);
  sensirion_uart_open(); // start sensor
  delay(1000);

  while (sps30_probe() != 0) {
    Serial.println("probe failed");
    delay(1000);
  }

  if (sps30_start_measurement() != 0) {
    Serial.println("error starting measurement");
  }

  // print out for debug
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.disconnect(true); // make sure
  WiFi.mode(WIFI_STA);

  CheckWifi();

  // print out for debug
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // imprement server handler
  server.on("/", handleRoot);
  server.on("/pm2", handlePM);
  server.on("/setautoclean", handleSetAutoClean);

  // start http server
  server.begin();
  Serial.println("HTTP server started");

  ThingSpeak.begin(client); 
  // send frist data to api server
  PushToApi();
}

void loop() {

  // Alway CheckWifi
  CheckWifi();

  // http server alway wait for client
  server.handleClient();
  // sent data to api every 1 min

  if (millis() - Timer1 >= 60000) {
    Timer1 = millis();
    PushToApi();
  }
  delay(1);
}
