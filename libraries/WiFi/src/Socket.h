

#ifndef CY_SECURE_SOCKET_H
#define CY_SECURE_SOCKET_H

#include "cy_secure_sockets.h"
#include "IPAddress.h"

class Socket {

    public:
        Socket();

        void begin();
        void end();

        void set_timeout(uint32_t timeout);
        void set_connect_opts(cy_socket_callback_t cback, void *arg);
        void set_receive_opts(cy_socket_callback_t cback, void *arg);
        void set_disconnect_opts(cy_socket_callback_t cback, void *arg);

        void bind(uint16_t port);
        bool connect(IPAddress ip, uint16_t port);
        
        void listen(int max_connections);
        bool accept(Socket & client_socket);

        int get_status();

    private:

        cy_socket_t socket;
        cy_rslt_t status;

        void set_status(cy_rslt_t ret);

        static bool global_socket_begun;
        static uint32_t global_socket_count;
        static cy_rslt_t global_sockets_begin();
        static cy_rslt_t global_sockets_end();

        friend class WiFiServer;
        friend class WiFiClient;
};

#endif /* CY_SECURE_SOCKET_H */ 
