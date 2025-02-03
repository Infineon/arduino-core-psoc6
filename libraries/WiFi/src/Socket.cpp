#include <Socket.h>

#define socket_assert(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
        return; \
}

#define socket_assert_raise(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
        return cy_ret; \
}

Socket::Socket(): socket(nullptr), status(CY_RSLT_SUCCESS) {

}

void Socket::begin() {
    cy_rslt_t ret;
    ret = Socket::global_sockets_begin();
    set_status(ret);
    socket_assert(ret);

    ret = cy_socket_create(CY_SOCKET_DOMAIN_AF_INET, CY_SOCKET_TYPE_STREAM,
                              CY_SOCKET_IPPROTO_TCP, &socket);
    set_status(ret);
}

void Socket::end() {
    cy_rslt_t ret;
    /** 
     * TODO: This sequence of disconnect delete requires review
     * { 
     */
    ret = cy_socket_disconnect(socket, 0);
    set_status(ret);
    socket_assert(ret);
    /* } */

    ret = cy_socket_delete(socket);
    set_status(ret);
    socket_assert(ret);

    ret = Socket::global_sockets_end();

}

void Socket::set_timeout(uint32_t timeout) {
    cy_rslt_t ret = cy_socket_setsockopt(socket, CY_SOCKET_SOL_SOCKET,
                                    CY_SOCKET_SO_RCVTIMEO, &timeout,
                                    sizeof(timeout));
    set_status(ret);              
}

void Socket::set_connect_opts(cy_socket_callback_t cback, void *arg) {   
    cy_socket_opt_callback_t tcp_connection_option;

    tcp_connection_option.callback = cback;
    tcp_connection_option.arg = arg;

    cy_rslt_t ret = cy_socket_setsockopt(socket, CY_SOCKET_SOL_SOCKET,
                                    CY_SOCKET_SO_CONNECT_REQUEST_CALLBACK,
                                    &tcp_connection_option, sizeof(cy_socket_opt_callback_t));
    set_status(ret); 
}

void Socket::set_receive_opts(cy_socket_callback_t cback, void *arg) {
    cy_socket_opt_callback_t tcp_receive_option;

    tcp_receive_option.callback = cback;
    tcp_receive_option.arg = arg;

    cy_rslt_t ret = cy_socket_setsockopt(socket, CY_SOCKET_SOL_SOCKET,
                                    CY_SOCKET_SO_RECEIVE_CALLBACK,
                                    &tcp_receive_option, sizeof(cy_socket_opt_callback_t));
    set_status(ret); 
}

void Socket::set_disconnect_opts(cy_socket_callback_t cback, void *arg) {
    cy_socket_opt_callback_t tcp_disconnection_option;

    tcp_disconnection_option.callback = cback;
    tcp_disconnection_option.arg = arg;

    cy_rslt_t ret = cy_socket_setsockopt(socket, CY_SOCKET_SOL_SOCKET,
                                    CY_SOCKET_SO_DISCONNECT_CALLBACK,
                                    &tcp_disconnection_option, sizeof(cy_socket_opt_callback_t));
    set_status(ret); 
}

void Socket::bind(uint16_t port) {
    cy_socket_sockaddr_t address = {
        .port = port, 
        .ip_address = {
            .version = CY_SOCKET_IP_VER_V4,
            .ip = { .v4 = 0 }
        }
    };

    cy_rslt_t ret = cy_socket_bind(socket, &address, sizeof(cy_socket_sockaddr_t));
    set_status(ret);
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

    cy_rslt_t ret = cy_socket_connect(socket, &address, sizeof(cy_socket_sockaddr_t));
    set_status(ret);
    if(ret != CY_RSLT_SUCCESS) {
        return false;
    }

    return true;
}

void Socket::listen(int max_connections) {
    cy_rslt_t ret = cy_socket_listen(socket, max_connections);
    set_status(ret);
}

bool Socket::accept(Socket & client_socket) {
    /**
     * TODO: check if we need to keep track of the addresses,
     * and of the port number in the accepted socket */
    cy_socket_sockaddr_t peer_addr;
    uint32_t peer_addr_len;

    cy_rslt_t ret = cy_socket_accept(socket, &peer_addr, &peer_addr_len, &(client_socket.socket));
    set_status(ret);
    if(ret != CY_RSLT_SUCCESS) {
        return false;
    }

    return true;
}

int Socket::get_status() {
    return status;
}   

void Socket::set_status(cy_rslt_t ret) {
    status = ret;
}   

bool Socket::global_socket_begun = false;
uint32_t Socket::global_socket_count = 0;

cy_rslt_t Socket::global_sockets_begin() {
    if (!Socket::global_socket_begun) {
        cy_rslt_t ret = cy_socket_init();
        socket_assert_raise(ret);
        Socket::global_socket_begun = true;
    }
    Socket::global_socket_count++;

    return CY_RSLT_SUCCESS;
}

cy_rslt_t Socket::global_sockets_end() {
    Socket::global_socket_count--;
    if (Socket::global_socket_count == 0) {
        cy_rslt_t ret = cy_socket_deinit();
        socket_assert_raise(ret);
        Socket::global_socket_begun = false;
    }

    return CY_RSLT_SUCCESS;
}