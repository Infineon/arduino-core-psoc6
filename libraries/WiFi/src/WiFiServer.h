#include <stdint.h>
#include <vector>
#include <WiFiClient.h>
#include "Server.h"
#include "cy_secure_sockets.h"

class WiFiServer : public arduino::Server {

    public:

        WiFiServer();

        void begin(uint16_t port);
        void begin();

        size_t write(uint8_t);

        void end();

    private:

        #define TCP_SERVER_RECV_TIMEOUT_MS  (500u)
        
        cy_socket_sockaddr_t address;
        cy_socket_t socket;

        std::vector <WiFiClient> connected_clients;

        void set_socket_timeout(uint32_t timeout);
        void set_socket_connect_opts();
        void set_socket_receive_opts();
        void set_socket_disconnect_opts();

        static cy_rslt_t tcp_connection_handler(cy_socket_t socket_handle, void *arg);
        static cy_rslt_t tcp_receive_msg_handler(cy_socket_t socket_handle, void *arg);
        static cy_rslt_t tcp_disconnection_handler(cy_socket_t socket_handle, void *arg);
};