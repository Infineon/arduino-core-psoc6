
#ifndef WIFI_CLIENT_H
#define WIFI_CLIENT_H

#include <memory>
#include "SecSocket.h"
#include "api/Client.h"
#include "api/Print.h"

class WiFiClient: public arduino::Client {

public:

    WiFiClient();

    int connect(IPAddress ip, uint16_t port);
    int connect(const char *host, uint16_t port);
    size_t write(uint8_t data) override;
    size_t write(const uint8_t *buf, size_t size) override;
    int available();
    int read();
    int read(uint8_t *buf, size_t size);
    int peek();
    void flush();
    void stop();
    uint8_t connected();
    uint8_t status();
    operator bool();

    IPAddress remoteIP();
    uint16_t remotePort();

    using Print::print;
    using Print::println;

    WiFiClient(const WiFiClient& other);
    WiFiClient& operator = (const WiFiClient& other);


private:

    /**
     * Implemented as shared ptr as this class is used
     * in the WiFiServer to keep the WiFiClient objects
     * as a list of connected clients.
     * Keeping objects will lead to copies which won´t
     * update the socket rx_buffer upon interrupts.
    */
    std::shared_ptr < Socket > socket = nullptr;

    static cy_rslt_t receiveCallback(cy_socket_t socket_handle, void *arg);
    static cy_rslt_t disconnectionCallback(cy_socket_t socket_handle, void *arg);

    bool isThisClient(cy_socket_t socket_handle);

    friend class WiFiServer;
};

#endif /* WIFI_CLIENT_H */
