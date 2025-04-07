#include "WiFiServer.h"

#define wifi_server_assert(cy_ret)   \
    if (cy_ret != CY_RSLT_SUCCESS) { \
        return;                      \
    }

WiFiServer::WiFiServer() {
    connected_clients.reserve(SERVER_MAX_CLIENTS);
}

void WiFiServer::begin(uint16_t port) {
    socket.begin();

    socket.setTimeout(SERVER_RECV_TIMEOUT_MS);
    socket.setConnectOptCallback(connectionCallback, this);
    socket.setReceiveOptCallback(receiveCallback, this);
    socket.setDisconnectOptCallback(disconnectionCallback, this);

    socket.bind(port);
    socket.listen(SERVER_MAX_CLIENTS);
}

void WiFiServer::begin() {
    begin(0);
}

WiFiClient& WiFiServer::available() {
    /**
     * Keep track of the last returned available client.
     * Subsequent calls to available would return different
     * client objects pointers.
     * If the returned client is the same you can compare
     * using the WiFiClient::operator== or directly the pointer value.
     */
    static uint16_t last_available_client = 0;
    last_available_client = last_available_client % connected_clients.size();

    for (uint16_t i = last_available_client; i < connected_clients.size(); i++) {
        if (connected_clients[i]) {
            last_available_client = i;
            return connected_clients[i];
        }
    }

    /**
     * If there is no available client, this sentinel object is returned.
     * As required by the specification, it will evaluate to false
     * in a if-statement.
     */
    static WiFiClient unavailable_client;
    return unavailable_client;
}

size_t WiFiServer::write(uint8_t data) {
    return write(&data, 1);
}

size_t WiFiServer::write(const uint8_t* buf, size_t size) {
    size_t written_bytes = 4294967295; /* Maximum value of uint32_t */

    for (WiFiClient& client : connected_clients) {
        uint16_t written_bytes_i = client.write(buf, size);
        /**
         * Keep as written_bytes the smallest value of written bytes to any clients.
         *
         * If the size passed by argument is equal to the written bytes
         * for all the connecting clients, that value can be used to check that all the
         * all the bytes passed to write() have been written to all clients successfully.
         *
         * If not, the information is that at least one of the clients have not been written
         * successfully all specified bytes.
         * The there is no way to know which client has received the least bytes,
         * neither to know which if/which other clients have received all the expected bytes.
         *
         * This is a limitation of the API function.
         * The user can always opt to communicate with the client via its own WiFiClient object.
         */
        written_bytes = written_bytes_i > written_bytes ? written_bytes : written_bytes_i;
    }

    return written_bytes;
}

uint8_t WiFiServer::status() {
    return socket.status();
}

void WiFiServer::end() {
    for (uint16_t i = 0; i < connected_clients.size(); i++) {
        connected_clients[i].stop();
        connected_clients.erase(connected_clients.begin() + i);
    }

    socket.end();
}

WiFiServer::operator bool() {
    if (socket.status() == SOCKET_STATUS_LISTENING) {
        return true;
    }

    return false;
}

uint8_t WiFiServer::connectedSize() {
    return connected_clients.size();
}

cy_rslt_t WiFiServer::connectionCallback(cy_socket_t socket_handle, void* arg) {
    WiFiServer* server = (WiFiServer*)arg;

    /**
     * Add a client to the end of the vector
     * and use the reference to accept the connection.
     */
    server->connected_clients.emplace_back();
    WiFiClient& new_client = server->connected_clients.back();

    if (!server->socket.accept(*(new_client.socket))) {
        server->connected_clients.pop_back();
        return server->socket.getLastError();
    }

    return CY_RSLT_SUCCESS;
}

cy_rslt_t WiFiServer::receiveCallback(cy_socket_t socket_handle, void* arg) {
    WiFiServer* server = (WiFiServer*)arg;

    /**
     * Find the client which has triggered the callback
     * and receive the data from the client.
     */
    for (WiFiClient& client : server->connected_clients) {
        if (client.isThisClient(socket_handle)) {
            client.socket->receiveCallback();
            break;
        }
    }

    return CY_RSLT_SUCCESS;
}

cy_rslt_t WiFiServer::disconnectionCallback(cy_socket_t socket_handle, void* arg) {
    WiFiServer* server = (WiFiServer*)arg;

    /**
     * Find the client which has triggered the callback
     * and stop the client.
     */
    for (uint16_t i = 0; i < server->connected_clients.size(); i++) {
        WiFiClient& disconnecting_client = server->connected_clients[i];
        if (disconnecting_client.isThisClient(socket_handle)) {
            disconnecting_client.stop();
            server->connected_clients.erase(server->connected_clients.begin() + i);
            break;
        }
    }

    return CY_RSLT_SUCCESS;
}
