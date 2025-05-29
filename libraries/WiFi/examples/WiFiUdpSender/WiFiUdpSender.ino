/*
WiFiUdpSender Example

This sketch sends a UDP packet to a server on remote port and waits for a reply.
This example works together with the WiFiUdpReceiver example.

created 2025
by Infineon Technologies AG
*/

#include <WiFi.h>
#include <WiFiUdp.h>
#include "secrets.h"  // Include your secrets file with SSID and password

int status = WL_IDLE_STATUS;
char ssid[] = NET_SECRET_SSID;
char pass[] = NET_SECRET_PASSWORD;

unsigned int localPort = 4321; // Local port to listen for replies
const char* serverIP = " 192.168.38.107"; // Change to your server's IP. You can get it by doing WiFi.localIP() on server side
unsigned int serverPort = 1234;       // Server's listening port

char packetBuffer[255]; //buffer to hold incoming packet

WiFiUDP Udp;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Udp.begin(localPort);
  Serial.println("UDP client started");
}

void loop() {
  // Send a packet to the server
  const char* msg = "Hello from client!";
  Udp.beginPacket(serverIP, serverPort);
  Udp.write(msg);
  Udp.endPacket();
  Serial.println("Packet sent to server");

  // Wait for a reply (with timeout)
  unsigned long start = millis();
  bool gotReply = false;
  while (millis() - start < 2000) { // 2 second timeout
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      int len = Udp.read(packetBuffer, 254);
      if (len > 0) packetBuffer[len] = 0;
      Serial.print("Received reply: ");
      Serial.println(packetBuffer);
      gotReply = true;
      break;
    }
    delay(10);
  }
  if (!gotReply) {
    Serial.println("No reply received.");
  }

  delay(5000); // Wait before sending again
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi local IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}