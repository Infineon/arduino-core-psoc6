/*
  Web client

 This sketch connects to a website (http://www.google.com)
 using the WiFi module.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.


 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe

 modified 2025
 by Infineon Technologies AG
 */


#include <WiFi.h>
#include "secrets.h" 
 
/* Add the SSID name and PASSWORD to the secrets.h file */
char ssid[] = NET_SECRET_SSID;
char pass[] = NET_SECRET_PASSWORD;    
int status = WL_IDLE_STATUS;

/* if you don't want to use DNS (and reduce your sketch size) */
/* use the numeric IP instead of the name for the server: */
//IPAddress server(74,125,232,128);
char server[] = "www.google.com";  

/* Initialize the Ethernet client library */
/* with the IP address and port of the server */
/* that you want to connect to (port 80 is default for HTTP): */
WiFiClient client;
 
void setup() {
  Serial.begin(9600);

  do {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  } while (status != WL_CONNECTED);

  wifiStatusPrint();

  Serial.println("\nStarting connection to server...");

  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
  }
}
 
/* Just wrap the received data up to 80 columns in the serial print */
void readResponse() {
  uint32_t received_data_num = 0;
  while (client.available()) {
    char c = client.read();
    Serial.print(c);
    /* wrap data to 80 columns*/
    received_data_num++;
    if(received_data_num % 80 == 0) { 
      Serial.println();
    }
  }  
}
 
void loop() {
  readResponse();

  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    /* Do nothing forever */
    while (true);
  }
}
 
 void wifiStatusPrint() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
 }