#include <WiFiClient.h>

WiFiClient::WiFiClient() {

}

int WiFiClient::connect(IPAddress ip, uint16_t port) {
    return 0;
}

int WiFiClient::connect(const char *host, uint16_t port) {
    return 0;
}

size_t WiFiClient::write(uint8_t) {
    return 0;
}

size_t WiFiClient::write(const uint8_t *buf, size_t size) {
    return 0;
}

int WiFiClient::available() {
    return 0;
}

int WiFiClient::read()  {
    return 0;
}

int WiFiClient::read(uint8_t *buf, size_t size) {
    return 0;   
}

int WiFiClient::peek() {
    return 0;
}

void WiFiClient::flush() {

}

void WiFiClient::stop() {

}

uint8_t WiFiClient::connected() {
    return 0;
}

WiFiClient::operator bool() {
 return true;
}