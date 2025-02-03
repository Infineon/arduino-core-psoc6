#include <WiFiServer.h>
#include <Arduino.h>

#define wifi_server_assert(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
        return; \
}

WiFiServer::WiFiServer(){

}

void WiFiServer::begin(uint16_t port) {
    
    socket.begin();
    
    socket.set_timeout(SERVER_RECV_TIMEOUT_MS);
    socket.set_connect_opts(tcp_connection_handler, this);
    socket.set_receive_opts(tcp_receive_msg_handler, this);
    socket.set_disconnect_opts(tcp_disconnection_handler, this);

    socket.bind(port);
    socket.listen(SERVER_MAX_CLIENTS);
}

void WiFiServer::begin() {
    begin(0);
}


size_t WiFiServer::write(uint8_t) {
    return 0;
}

void WiFiServer::end() {

    /**
     * TODO: We need to disconnect all the open connected socket.
     * */
    // ret = cy_socket_shutdown(socket, CY_SOCKET_SHUT_RDWR);
    // wifi_server_assert(ret);

    socket.end();
}


cy_rslt_t WiFiServer::tcp_connection_handler(cy_socket_t socket_handle, void *arg) {
    
    Serial.print("We received some connection!");
    /**
     * TODO: Verify this properly when implementing the 
     * available method. This for now shows the connection
     * callback being called.
    */
    WiFiServer *server = (WiFiServer *)arg;

    WiFiClient new_client;

    if(!server->socket.accept(new_client.socket)) {
        return CY_RSLT_MODULE_SECURE_SOCKETS_NOMEM;
    }

    server->connected_clients.push_back(new_client);

    return CY_RSLT_SUCCESS;
}

cy_rslt_t WiFiServer::tcp_receive_msg_handler(cy_socket_t socket_handle, void *arg) {

    return 0;
}

cy_rslt_t WiFiServer::tcp_disconnection_handler(cy_socket_t socket_handle, void *arg) {
    
    return 0;
}