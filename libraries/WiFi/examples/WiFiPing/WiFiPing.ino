 /*
  Web ICMP Ping

  This sketch pings a device based on the IP address or the hostname
  using the WiFi library.

  This example is written for a network using WPA encryption.

  created 14 February 2024
  by paulvha
   
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
}
 
void loop() {
  /* Ping IP */
  const IPAddress remote_ip(140,82,121,4);
  Serial.print("Trying to ping github.com on IP: ");
  Serial.println(remote_ip);

  int res = WiFi.ping(remote_ip);
  
  if (res != WIFI_ERROR_NONE){
    Serial.println("Ping failed !");
  } else {
    Serial.println("Ping successful !");
  }

  /* Ping Host */
  const char* remote_host = "www.google.com";
  Serial.print("Trying to ping host: ");
  Serial.println(remote_host);

  res = WiFi.ping(remote_host);

  if (res != WIFI_ERROR_NONE){
    Serial.println("Ping failed !");
  } else {
    Serial.println("Ping successful !");
  }

  delay(1000);
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