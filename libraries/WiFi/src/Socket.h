

#ifndef CY_SECURE_SOCKET_H
#define CY_SECURE_SOCKET_H

#include "cy_secure_sockets.h"
#include "IPAddress.h"

typedef enum {
    SOCKET_STATUS_UNINITED = 0,
    SOCKET_STATUS_CREATED,
    SOCKET_STATUS_CLOSED,
    SOCKET_STATUS_DISCONNECTED,
    SOCKET_STATUS_CONNECTED,
    SOCKET_STATUS_BOUND,
    SOCKET_STATUS_LISTENING,
    SOCKET_STATUS_ERROR = -1
} socket_status_t;

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

        uint8_t status();
        cy_rslt_t get_last_error();

    private:

        cy_socket_t socket;
        socket_status_t s_status;
        cy_rslt_t s_last_error;


        static bool global_socket_initialized;
        static uint32_t global_socket_count;
        static cy_rslt_t global_sockets_init();
        static cy_rslt_t global_sockets_deinit();

        friend class WiFiServer;
        friend class WiFiClient;
};

#endif /* CY_SECURE_SOCKET_H */ 
