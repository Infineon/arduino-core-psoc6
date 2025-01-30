#include <WiFiServer.h>
#include <Arduino.h>

#define wifi_server_assert(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
        return; \
}

WiFiServer::WiFiServer(){

}

void WiFiServer::begin(uint16_t port) {

    cy_rslt_t ret;

    /** TODO: This needs to be initialized only once for the entire library.
     * As this includes client and server that this probably ends being part 
     * of a common Socket class as a static function. 
     * { 
     * */
    ret = cy_socket_init();
    wifi_server_assert(ret);
    /* } */

    ret = cy_socket_create(CY_SOCKET_DOMAIN_AF_INET, CY_SOCKET_TYPE_STREAM,
                              CY_SOCKET_IPPROTO_TCP, &socket);
    wifi_server_assert(ret);

    set_socket_timeout(TCP_SERVER_RECV_TIMEOUT_MS);
    set_socket_connect_opts();
    set_socket_receive_opts();
    set_socket_disconnect_opts();

    address.port = port;
    ret = cy_socket_bind(socket, &address, sizeof(address));
    wifi_server_assert(ret);
}

void WiFiServer::begin() {
    begin(0);
}


size_t WiFiServer::write(uint8_t) {
    return 0;
}

void WiFiServer::end() {
    cy_rslt_t ret;

    /**
     * TODO: We need to disconnect all the open connected socket.
     * */
    ret = cy_socket_shutdown(socket, CY_SOCKET_SHUT_RDWR);
    wifi_server_assert(ret);

    /** 
     * TODO: This sequence of disconnect delete requires review
     * { 
     */
    ret = cy_socket_disconnect(socket, 0);
    wifi_server_assert(ret);
    /* } */

    ret = cy_socket_delete(socket);
    wifi_server_assert(ret);

    /** TODO: This needs to be initialized only once for the entire library.
     * As this includes client and server that this probably ends being part 
     * of a common Socket class as a static function. 
     * { 
     * */
    ret = cy_socket_deinit();
    wifi_server_assert(ret);
    /* } */
}

void WiFiServer::set_socket_timeout(uint32_t timeout) {
    /** 
     * TODO: Set a reasonable default configuration. The rest 
     * should be done by an additional user function to configure the timeout.
     * */
    cy_rslt_t ret;
    uint32_t tcp_recv_timeout = TCP_SERVER_RECV_TIMEOUT_MS;

    ret = cy_socket_setsockopt(socket, CY_SOCKET_SOL_SOCKET,
                                 CY_SOCKET_SO_RCVTIMEO, &tcp_recv_timeout,
                                 sizeof(tcp_recv_timeout));
    wifi_server_assert(ret);
}

void WiFiServer::set_socket_connect_opts() {   
    cy_rslt_t ret;
    cy_socket_opt_callback_t tcp_connection_option;

    tcp_connection_option.callback = tcp_connection_handler;
    tcp_connection_option.arg = (void *)this;

    ret = cy_socket_setsockopt(socket, CY_SOCKET_SOL_SOCKET,
                                  CY_SOCKET_SO_CONNECT_REQUEST_CALLBACK,
                                  &tcp_connection_option, sizeof(cy_socket_opt_callback_t));
    wifi_server_assert(ret);
}

void WiFiServer::set_socket_receive_opts() {
    cy_rslt_t ret;
    cy_socket_opt_callback_t tcp_receive_option;

    tcp_receive_option.callback = tcp_receive_msg_handler;
    tcp_receive_option.arg = (void *)this;

    ret = cy_socket_setsockopt(socket, CY_SOCKET_SOL_SOCKET,
                                  CY_SOCKET_SO_RECEIVE_CALLBACK,
                                  &tcp_receive_option, sizeof(cy_socket_opt_callback_t));
    wifi_server_assert(ret);
}

void WiFiServer::set_socket_disconnect_opts() {
    cy_rslt_t ret;
    cy_socket_opt_callback_t tcp_disconnection_option;

    tcp_disconnection_option.callback = tcp_disconnection_handler;
    tcp_disconnection_option.arg = (void *)this;

    ret = cy_socket_setsockopt(socket, CY_SOCKET_SOL_SOCKET,
                                  CY_SOCKET_SO_DISCONNECT_CALLBACK,
                                  &tcp_disconnection_option, sizeof(cy_socket_opt_callback_t));
    wifi_server_assert(ret);
}

cy_rslt_t WiFiServer::tcp_connection_handler(cy_socket_t socket_handle, void *arg) {
    
    WiFiServer *server = (WiFiServer *)arg;
    /**
     * TODO: check if we need to keep track of the addresses,
     * and of the port number in the WiFiClient */
    cy_socket_sockaddr_t peer_addr;
    uint32_t peer_addr_len;

    /* Create a new client and add it to the list of connected clients */
    WiFiClient new_client;
    server->connected_clients.push_back(new_client);
    cy_rslt_t ret = cy_socket_accept(socket_handle, &peer_addr, &peer_addr_len, &(new_client.socket));

    /* Delete the new client in case of error */
    if (ret != CY_RSLT_SUCCESS) {
        server->connected_clients.pop_back();
    }

    return ret;
}

cy_rslt_t WiFiServer::tcp_receive_msg_handler(cy_socket_t socket_handle, void *arg) {

    return 0;
}

cy_rslt_t WiFiServer::tcp_disconnection_handler(cy_socket_t socket_handle, void *arg) {
    
    return 0;
}