#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#include <stdint.h>
#include <vector>
#include <WiFiClient.h>
#include <Socket.h>
#include "Server.h"

class WiFiServer : public arduino::Server {

    public:

        WiFiServer();

        void begin(uint16_t port);
        void begin();

        WiFiClient available();
        size_t write(uint8_t);

        void end();

    private:

        Socket socket;

        static const uint16_t SERVER_RECV_TIMEOUT_MS = 500;
        static const uint32_t SERVER_MAX_CLIENTS = 255;

        std::vector <WiFiClient> connected_clients;

        static cy_rslt_t connection_callback(cy_socket_t socket_handle, void * arg);
        static cy_rslt_t receive_callback(cy_socket_t socket_handle, void * arg);
};

#endif /* WIFI_SERVER_H */