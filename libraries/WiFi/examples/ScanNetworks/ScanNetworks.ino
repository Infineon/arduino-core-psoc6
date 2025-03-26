/*
  This example prints the board's MAC address, and
  scans for available WiFi networks.
  Every ten seconds, it scans again. It doesn't actually
  connect to any network, so no encryption scheme is specified.

  created 13 July 2010
  by dlf (Metodo2 srl)
  modified 21 Junn 2012
  by Tom Igoe and Jaymes Dec
  
  modified 2025
  by Infineon Technologies AG
 */

 #include <WiFi.h>

 void setup() {
   Serial.begin(9600);

 }
 
 void loop() {
   uint8_t mac[6];
   // scan for existing networks:
   Serial.println("Scanning available networks...");
   networksList();
   
   WiFi.macAddress(mac);
   Serial.println();
   Serial.print("Your MAC Address is: ");
   macAddressPrint(mac);
   
   delay(10000);
 }
 
 void networksList() {
   Serial.println("** Scan Networks **");
   int numSsid = WiFi.scanNetworks();
   if (numSsid == -1) {
     Serial.println("Couldn't get a WiFi connection");
     while (true);
   }
 
   Serial.print("number of available networks:");
   Serial.println(numSsid);
 
   for (int thisNet = 0; thisNet < numSsid; thisNet++) {
     Serial.print(thisNet);
     Serial.print(") ");
     Serial.print(WiFi.SSID(thisNet));
     Serial.print(" Signal: ");
     Serial.print(WiFi.RSSI(thisNet));
     Serial.print(" dBm");
     Serial.print(" Encryption: ");
     encryptionTypePrint(WiFi.encryptionType(thisNet));
   }
 }
 
 void encryptionTypePrint(int thisType) {
   switch (thisType) {
     case AUTH_MODE_SHARED_KEY:
       Serial.println("WEP");
       break;
     case AUTH_MODE_WPA:
       Serial.println("WPA");
       break;
     case AUTH_MODE_WPA2:
       Serial.println("WPA2");
       break;
     case AUTH_MODE_WPA3:
       Serial.println("WPA3");
       break;   
     case AUTH_MODE_OPEN_SYSTEM:
       Serial.println("None");
       break;
     case AUTH_MODE_AUTO:
       Serial.println("Auto");
       break;
     case AUTH_MODE_INVALID:
     default:
       Serial.println("Unknown");
       break;
   }
 }
 
 void macAddressPrint(uint8_t *mac) {
  for (int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    if (i < 5) {
      Serial.print(":");
    }
  }
  Serial.println();
}