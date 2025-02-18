#include <Socket.h>

#define socket_assert(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
        s_status = SOCKET_STATUS_ERROR; \
        return; \
}

#define socket_status_update(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
        s_status = SOCKET_STATUS_ERROR; \
}

#define socket_assert_raise(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
        return cy_ret; \
}

Socket::Socket(): 
socket(nullptr), 
s_status(SOCKET_STATUS_UNINITED),
s_last_error(CY_RSLT_SUCCESS) {

}

void Socket::begin() {
    s_last_error = Socket::global_sockets_init();
    socket_assert(s_last_error);

    s_last_error = cy_socket_create(CY_SOCKET_DOMAIN_AF_INET, CY_SOCKET_TYPE_STREAM,
                              CY_SOCKET_IPPROTO_TCP, &socket);
    socket_assert(s_last_error);

    s_status = SOCKET_STATUS_CREATED;
}

void Socket::end() {
    /** 
     * TODO: This sequence of disconnect delete requires review
     * { 
     */
    s_last_error = cy_socket_disconnect(socket, 0);
    socket_assert(s_last_error);
    /* } */
    s_status = SOCKET_STATUS_DISCONNECTED;

    s_last_error = cy_socket_delete(socket);
    socket_assert(s_last_error);
    s_status = SOCKET_STATUS_CLOSED;

    s_last_error = Socket::global_sockets_deinit();
}

void Socket::set_timeout(uint32_t timeout) {
    s_last_error = cy_socket_setsockopt(socket, CY_SOCKET_SOL_SOCKET,
                                    CY_SOCKET_SO_RCVTIMEO, &timeout,
                                    sizeof(timeout)); 
    socket_assert(s_last_error);
}

void Socket::set_connect_opt_callback(cy_socket_callback_t cback, void * arg) {
    set_opt_callback(CY_SOCKET_SO_CONNECT_REQUEST_CALLBACK, cback, arg);
}

void Socket::set_receive_opt_callback(cy_socket_callback_t cback, void * arg) {
    set_opt_callback(CY_SOCKET_SO_RECEIVE_CALLBACK, cback, arg);
}

void Socket::bind(uint16_t port) {
    cy_socket_sockaddr_t address = {
        .port = port, 
        .ip_address = {
            .version = CY_SOCKET_IP_VER_V4,
            .ip = { .v4 = 0 }
        }
    };

    s_last_error = cy_socket_bind(socket, &address, sizeof(cy_socket_sockaddr_t));
    socket_assert(s_last_error);

    s_status = SOCKET_STATUS_BOUND;
}

bool Socket::connect(IPAddress ip, uint16_t port) {
    uint32_t ip_u32 = (ip[3] << 24) | (ip[2] << 16) | (ip[1] << 8) | ip[0];
    cy_socket_sockaddr_t address = {
        .port = port, 
        .ip_address = {
            .version = CY_SOCKET_IP_VER_V4,
            .ip = { .v4 = ip_u32 }
        }
    };

    s_last_error = cy_socket_connect(socket, &address, sizeof(cy_socket_sockaddr_t));
    if(s_last_error != CY_RSLT_SUCCESS) {
        return false;
    }

    s_status = SOCKET_STATUS_CONNECTED;
    return true;
}

void Socket::listen(int max_connections) {
    s_last_error = cy_socket_listen(socket, max_connections);
    socket_assert(s_last_error);

    s_status = SOCKET_STATUS_LISTENING;
}

bool Socket::accept(Socket & client_socket) {
    /**
     * TODO: check if we need to keep track of the addresses,
     * and of the port number in the accepted socket */
    cy_socket_sockaddr_t peer_addr;
    uint32_t peer_addr_len;

    s_last_error = cy_socket_accept(socket, &peer_addr, &peer_addr_len, &(client_socket.socket));
    if(s_last_error != CY_RSLT_SUCCESS) {
        s_status = SOCKET_STATUS_ERROR;
        return false;
    }

    client_socket.s_status = SOCKET_STATUS_CONNECTED;
    return true;
}

uint32_t Socket::send(const void * data, uint32_t len) {
    uint32_t bytes_sent = 0;
    s_last_error =  cy_socket_send(socket, data, len,
                                CY_SOCKET_FLAGS_NONE, &bytes_sent);
    socket_status_update(s_last_error);

    return bytes_sent;
}
uint32_t Socket::available(){
    return rx_buf.available();
}

uint32_t Socket::receive(uint8_t * data, uint32_t len) {
    uint32_t bytes_to_read = (uint32_t)rx_buf.available() > len ? len : rx_buf.available();

    for(uint32_t i = 0; i < bytes_to_read; i++) {
        data[i] = rx_buf.read_char();
    }

    return bytes_to_read;
}

uint8_t Socket::status() {
    return s_status;
}   

cy_rslt_t Socket::get_last_error() {
    return s_last_error;
}   

void Socket::receive_callback() {
    if (!rx_buf.isFull()) {
        uint32_t bytes_rcvd_request = rx_buf.availableForStore();
        uint8_t temp_rx_buff [bytes_rcvd_request] = {0};

        uint32_t bytes_received = 0;
        s_last_error = cy_socket_recv(socket, temp_rx_buff, bytes_rcvd_request,
                                    CY_SOCKET_FLAGS_NONE, &bytes_received);
        socket_assert(s_last_error);

        for(uint32_t i = 0; i < bytes_received; i++) {
            rx_buf.store_char(temp_rx_buff[i]);
        }
    }
}

void Socket::set_opt_callback(int optname, cy_socket_callback_t cback, void * arg) {
    cy_socket_opt_callback_t cy_opt_callback;

    cy_opt_callback.callback = cback; 
    cy_opt_callback.arg = arg;

    s_last_error = cy_socket_setsockopt(socket, CY_SOCKET_SOL_SOCKET,
                                    optname,
                                    &cy_opt_callback, sizeof(cy_socket_opt_callback_t));
    socket_assert(s_last_error); 
}

bool Socket::global_socket_initialized = false;
uint32_t Socket::global_socket_count = 0;

cy_rslt_t Socket::global_sockets_init() {
    if (!Socket::global_socket_initialized) {
        cy_rslt_t ret = cy_socket_init();
        socket_assert_raise(ret);
        Socket::global_socket_initialized = true;
    }
    Socket::global_socket_count++;

    return CY_RSLT_SUCCESS;
}

cy_rslt_t Socket::global_sockets_deinit() {
    Socket::global_socket_count--;
    if (Socket::global_socket_count == 0) {
        cy_rslt_t ret = cy_socket_deinit();
        socket_assert_raise(ret);
        Socket::global_socket_initialized = false;
    }

    return CY_RSLT_SUCCESS;
}
