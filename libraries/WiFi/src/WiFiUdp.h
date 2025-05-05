#ifndef WIFI_UDP_H
#define WIFI_UDP_H

#include <stdint.h>
#include <string.h>
#include <api/Udp.h>
#include "WiFiClient.h"
#include "WiFi.h"
#include "WiFiServer.h"
#include "SecSocket.h"

class WiFiUDP: public arduino::UDP {
public:
    WiFiUDP();
    uint8_t begin(uint16_t);

    // yet to implement these functions
    void stop();
    int beginPacket(IPAddress ip, uint16_t port);
    int beginPacket(const char *host, uint16_t port);
    int endPacket();
    size_t write(uint8_t);
    size_t write(const uint8_t *buffer, size_t size);

    using Print::write;
    int parsePacket();
    int available();
    int read();
    int read(unsigned char *buffer, size_t len);
    int read(char *buffer, size_t len);
    int peek();
    void flush();
    IPAddress remoteIP();
    uint16_t remotePort();



private:
    cy_socket_t client_handle;
    socket_status_t _status;
    cy_rslt_t _last_error;

    static bool global_socket_initialized;
    static uint32_t global_socket_count;
    cy_rslt_t global_sockets_init();
    cy_rslt_t global_sockets_deinit();
};

#endif /* WIFI_UDP_H */
