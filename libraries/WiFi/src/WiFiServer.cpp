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
    socket.set_connect_opt_callback(connection_callback, this);
    socket.set_receive_opt_callback(receive_callback, this);

    socket.bind(port);
    socket.listen(SERVER_MAX_CLIENTS);
}

void WiFiServer::begin() {
    begin(0);
}

WiFiClient WiFiServer::available() {
    WiFiClient client;

    if (connected_clients.size() > 0) {
        client = connected_clients.front();
        /* If client connected and data available */
        if (client) {
            return client;
        }

        //We cannot delete it, because we have to keep in the 
        // list for received connections. 
        // We can only deleted from the list when disconnecting.
        // connected_clients.erase(connected_clients.begin());
        /**
         * TODO: We need to check later how to handle the client
         * removal from the list upon disconnection etc.*/
    }

    return client;
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

cy_rslt_t WiFiServer::connection_callback(cy_socket_t socket_handle, void *arg) {
    WiFiServer *server = (WiFiServer *)arg;

    WiFiClient new_client;
    if(!server->socket.accept(new_client.socket)) {
        return server->socket.get_last_error();
    }

    server->connected_clients.push_back(new_client);

    return CY_RSLT_SUCCESS;
}

cy_rslt_t WiFiServer::receive_callback(cy_socket_t socket_handle, void * arg) {
    WiFiServer *server = (WiFiServer *)arg;

    /** 
     * Find the client which has triggered the callback
     * and receive the data from the client. 
     */
    std::vector<WiFiClient>::iterator client = server->connected_clients.begin();    
    for( ;client != server->connected_clients.end(); client++) {
        if(client->socket.socket == socket_handle) {
            client->socket.receive_callback();
            break;
        }
    }

    return CY_RSLT_SUCCESS;
}
