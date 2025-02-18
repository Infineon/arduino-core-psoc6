

#ifndef CY_SECURE_SOCKET_H
#define CY_SECURE_SOCKET_H

#include "cy_secure_sockets.h"
#include "IPAddress.h"
#include "RingBuffer.h"  

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
        void set_connect_opt_callback(cy_socket_callback_t cback, void * arg);
        void set_receive_opt_callback(cy_socket_callback_t cback, void * arg);

        void bind(uint16_t port);
        bool connect(IPAddress ip, uint16_t port);
        
        void listen(int max_connections);
        bool accept(Socket & client_socket);
        uint32_t send(const void * data, uint32_t len);
        uint32_t available();
        uint32_t receive(uint8_t * data, uint32_t len);

        uint8_t status();
        cy_rslt_t get_last_error();

    private:

        cy_socket_t socket;
        socket_status_t s_status;
        cy_rslt_t s_last_error;

        void set_opt_callback(int optname, cy_socket_callback_t cback, void * arg);

        static const uint16_t RX_BUFFER_SIZE = 256;
        arduino::RingBufferN<RX_BUFFER_SIZE> rx_buf;

        void receive_callback();

        static bool global_socket_initialized;
        static uint32_t global_socket_count;
        static cy_rslt_t global_sockets_init();
        static cy_rslt_t global_sockets_deinit();

        friend class WiFiServer;
        friend class WiFiClient;
};

#endif /* CY_SECURE_SOCKET_H */ 
