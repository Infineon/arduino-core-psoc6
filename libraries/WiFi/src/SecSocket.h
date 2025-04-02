#ifndef CY_SECURE_SOCKET_H
#define CY_SECURE_SOCKET_H

#include "cy_secure_sockets.h"
#include "api/IPAddress.h"
#include "api/RingBuffer.h"

typedef enum {
    SOCKET_STATUS_UNINITED = 0,
    SOCKET_STATUS_CREATED,
    SOCKET_STATUS_DELETED,
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

    void setTimeout(uint32_t timeout);
    void setConnectOptCallback(cy_socket_callback_t cback, void *arg);
    void setReceiveOptCallback(cy_socket_callback_t cback, void *arg);
    void setDisconnectOptCallback(cy_socket_callback_t cback, void *arg);

    void bind(uint16_t port);
    bool connect(IPAddress ip, uint16_t port);
    bool connect(const char *host, uint16_t port);

    void listen(int max_connections);
    bool accept(Socket & client_socket);
    uint32_t send(const void *data, uint32_t len);
    uint32_t available();
    int peek();
    uint32_t receive(uint8_t *data, uint32_t len);
    void flush();

    IPAddress remoteIP();
    uint16_t port();

    static int hostByName(const char *aHostname, IPAddress& ip);

    uint8_t status();
    cy_rslt_t getLastError();

private:

    cy_socket_t socket;
    socket_status_t _status;
    cy_rslt_t _last_error;

    IPAddress remote_ip;
    uint16_t _port;

    void setOptCallback(int optname, cy_socket_callback_t cback, void *arg);

    static const uint16_t RX_BUFFER_SIZE = 256;
    arduino::RingBufferN < RX_BUFFER_SIZE > rx_buf;

    bool connect(cy_socket_sockaddr_t *addr);
    void receiveCallback();

    static bool global_socket_initialized;
    static uint32_t global_socket_count;
    static cy_rslt_t global_sockets_init();
    static cy_rslt_t global_sockets_deinit();

    friend class WiFiServer;
    friend class WiFiClient;
};

#endif /* CY_SECURE_SOCKET_H */
