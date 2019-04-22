
// #include <WiFi.h>

// //--------------------------------------------------------------------------------------------------
// // Function Details
// //--------------------------------------------------------------------------------------------------
// // Name : wifiClear
// // Purpose : clears all wifi settings before connecting. must be run before every wifiConnect
// // Argument(s) : void
// // Returns : void

// void ESP32Wifi::wifiClear()
// {
//     WiFi.mode(WIFI_STA);
//     WiFi.disconnect();
//     wifiStatus = WiFi.status() == WL_CONNECTED;
//     delay(100);
// }
// //--------------------------------------------------------------------------------------------------

// //--------------------------------------------------------------------------------------------------
// // Function Details
// //--------------------------------------------------------------------------------------------------
// // Name : wifiConnect
// // Purpose : connects to given ssid using password for given number of tries. updates the wifiStatus.
// // Argument(s) : char *SSID, char *PASS, unsigned short tries
// // Returns : void

// void ESP32Wifi::wifiConnect(char *SSID, char *PASS, unsigned short tries)
// {
//     _SSID = SSID;
//     _PASS = PASS;
//     wifiStatus = WiFi.status() == WL_CONNECTED;
//     while ((tries > 0) && (!wifiStatus))
//     {
//         wifiClear();
//         WiFi.begin(_SSID, _PASS);
//         delay(5000);
//         wifiStatus = WiFi.status() == WL_CONNECTED;
//         tries--;
//     }
// }
// //--------------------------------------------------------------------------------------------------

// //--------------------------------------------------------------------------------------------------
// // Function Details
// //--------------------------------------------------------------------------------------------------
// // Name : wifiDisconnect
// // Purpose : disconnects from the network and switches off the radio. sets wifiStatus to false.
// // Argument(s) : void
// // Returns : void

// void ESP32Wifi::wifiDisconnect()
// {
//     WiFi.disconnect();
//     WiFi.mode(WIFI_OFF);
//     wifiStatus = false;
// }
// //--------------------------------------------------------------------------------------------------
