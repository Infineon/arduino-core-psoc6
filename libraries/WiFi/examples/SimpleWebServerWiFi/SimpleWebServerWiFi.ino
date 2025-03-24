/*
  WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi module (once connected)
 to the Serial Monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin LED_BUILTIN.

 If the IP address of your board is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption.

 created 25 Nov 2012
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
WiFiServer server;

void setup() {
  Serial.begin(9600);      
  pinMode(LED_BUILTIN, OUTPUT);      

  do {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  } while (status != WL_CONNECTED);

  server.begin(80);                         
  wifiStatusPrint();                      
}

void loop() {
  WiFiClient client = server.available();  

  if (client) {                             
    Serial.println("new client");           
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {             
        char c = client.read();            
        Serial.write(c);                    
        if (c == '\n') {                   

          /* I the current line is blank, you got two newline characters in a row. */
          /* That's the end of the client HTTP request, so send a response: */
          if (currentLine.length() == 0) {
            /* HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK) */
            /* and a content-type so the client knows what's coming, then a blank line: */
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            /* The content of the HTTP response follows the header: */
            client.print("Click <a href=\"/H\">here</a> turn the LED on pin LED_BUILTIN on<br>");
            client.print("Click <a href=\"/L\">here</a> turn the LED on pin LED_BUILTIN off<br>");

            /* The HTTP response ends with another blank line: */
            client.println();
            break;
          } else {    /* if you got a newline, then clear currentLine: */
            currentLine = "";
          }
        } else if (c != '\r') {  /* if you got anything else but a carriage return character, */
          currentLine += c;      /* add it to the end of the currentLine */
        }

        /* Check to see if the client request was "GET /H" or "GET /L": */
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);               /* GET /H turns the LED on */
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);                /* GET /L turns the LED off */
        }
      }
    }

    client.stop();
    Serial.println("client disconnected");
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

  /* Browser url to connected to server */
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}