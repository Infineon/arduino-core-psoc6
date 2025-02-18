
#ifndef WIFI_CLIENT_H
#define WIFI_CLIENT_H

#include <Socket.h>
#include "Client.h"

class WiFiClient : arduino::Client {

    public:
        WiFiClient();

        int connect(IPAddress ip, uint16_t port);
        int connect(const char *host, uint16_t port);
        size_t write(uint8_t data);
        size_t write(const uint8_t *buf, size_t size);
        int available();
        int read();
        int read(uint8_t *buf, size_t size);
        int peek();
        void flush();
        void stop();
        uint8_t connected();
        uint8_t status();
        operator bool();
    

    private:
        Socket socket;

        static cy_rslt_t receive_callback(cy_socket_t socket_handle, void * arg);

        friend class WiFiServer;
};

#endif /* WIFI_CLIENT_H */