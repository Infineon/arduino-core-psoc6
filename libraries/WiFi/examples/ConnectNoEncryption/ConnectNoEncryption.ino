/*
 This example connects to an unencrypted WiFi network.
 Then it prints the MAC address of the board,
 the IP address obtained, and other network details.

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe

 modified 2025
 by Infineon Technologies AG
 */
#include <WiFi.h>
#include "secrets.h" 

/* Add the SSID name to the secrets.h file */
char ssid[] = NET_SECRET_SSID;              
int status = WL_IDLE_STATUS;    

void setup() {
  Serial.begin(9600);

  /* Wait for serial initialization */
  delay(500);

  do {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid);
  } while (status != WL_CONNECTED);

  Serial.print("Connected to network with settings: ");
  wifiConfigPrint();
}

void loop() {
  delay(10000);
  wifiConfigPrint();
}

void wifiConfigPrint() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  /* TODO: Enable when functions are available. Not yet enabled */
  /** 
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  */
 
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NetMask: ");
  Serial.println(subnet);
 
  IPAddress gateway = WiFi.gatewayIP();
  Serial.print("Gateway: ");
  Serial.println(gateway);

  uint8_t mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  macAddressPrint(mac);

  uint8_t bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  macAddressPrint(bssid);
 
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
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