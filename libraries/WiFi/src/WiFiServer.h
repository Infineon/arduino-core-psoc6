#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#include <stdint.h>
#include <vector>
#include <WiFiClient.h>
#include <SecSocket.h>
#include "Server.h"

class WiFiServer: public arduino::Server {

public:

    WiFiServer();

    void begin(uint16_t port);
    void begin();

    WiFiClient & available();
    size_t write(uint8_t) override;
    size_t write(const uint8_t *buf, size_t size) override;

    uint8_t status();
    void end();
    operator bool();

    using Print::print;
    using Print::println;

    /* This function is not part of the API in other (official) ports */
    uint8_t connectedSize();

private:

    Socket socket;

    static const uint16_t SERVER_RECV_TIMEOUT_MS = 500;
    static const uint32_t SERVER_MAX_CLIENTS = 32;
    std::vector < WiFiClient > connected_clients;

    static cy_rslt_t connectionCallback(cy_socket_t socket_handle, void *arg);
    static cy_rslt_t receiveCallback(cy_socket_t socket_handle, void *arg);
    static cy_rslt_t disconnectionCallback(cy_socket_t socket_handle, void *arg);
};

#endif /* WIFI_SERVER_H */
