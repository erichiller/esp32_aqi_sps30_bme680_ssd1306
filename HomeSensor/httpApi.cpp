// #include <Arduino.h>
// #include "config.h"




// // post to secure api https
// void httpsPost(const char *host, String url, String data) {
//   if (client.connect(host, 443)) {
//     client.println("POST " + url + " HTTP/1.1");
//     client.println("Host: " + (String)host);
//     client.println("User-Agent: ESP32SPS30/1.0");
//     client.println("Connection: close");
//     client.print("Content-Length: ");
//     client.println(data.length());
//     client.println("Content-Type: application/x-www-form-urlencoded;");
//     client.println();
//     client.println(data);
//     // String response = client.readString(); // uncomment for debug
//     client.stop();

//     // return response;
//     // Serial.println(response); // uncomment for debug
//   }
// }