#include <WiFiClient.h>

#define wifi_client_assert_raise(cy_ret, ret_code)   if (cy_ret != CY_RSLT_SUCCESS) { \
            return ret_code; \
}

#define wifi_client_assert(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
            return; \
}

WiFiClient::WiFiClient() :
    socket(std::make_shared < Socket > ()) {

}

int WiFiClient::connect(IPAddress ip, uint16_t port) {
    socket->begin(false); // false = TCP

    socket->setReceiveOptCallback(receiveCallback, this);
    socket->setDisconnectOptCallback(disconnectionCallback, this);

    return socket->connect(ip, port);
}

int WiFiClient::connect(const char *host, uint16_t port) {
    socket->begin(false); // false = TCP

    socket->setReceiveOptCallback(receiveCallback, this);
    socket->setDisconnectOptCallback(disconnectionCallback, this);

    return socket->connect(host, port);
}

size_t WiFiClient::write(uint8_t data) {
    return write(&data, 1);
}

size_t WiFiClient::write(const uint8_t *buf, size_t size) {
    return (size_t)socket->send(buf, size);
}

int WiFiClient::available() {
    return socket->available();
}

int WiFiClient::read() {
    uint8_t data;
    uint32_t received_data = socket->receive(&data, 1);

    if (received_data == 0) {
        return -1;
    }

    return (int)data;
}

int WiFiClient::read(uint8_t *buf, size_t size) {
    uint32_t received_data = socket->receive(buf, size);

    if (received_data == 0) {
        return -1;
    }

    return (int)received_data;
}

int WiFiClient::peek() {
    return socket->peek();
}

void WiFiClient::flush() {
    socket->flush();
}

void WiFiClient::stop() {
    socket->end();
}

uint8_t WiFiClient::connected() {
    return socket->status() == SOCKET_STATUS_CONNECTED;
}

uint8_t WiFiClient::status() {
    return socket->status();
}

WiFiClient::operator bool() {
    if (socket->status() != SOCKET_STATUS_CONNECTED) {
        return false;
    }
    if (available() <= 0) {
        return false;
    }
    return true;
}

IPAddress WiFiClient::remoteIP() {
    return socket->remoteIP();
}
uint16_t WiFiClient::remotePort() {
    return socket->port();
};

WiFiClient::WiFiClient(const WiFiClient& other) : socket(other.socket) {
}

WiFiClient& WiFiClient::operator = (const WiFiClient& other) {
    if (this != &other) {
        socket = other.socket;
    }
    return *this;
}

cy_rslt_t WiFiClient::receiveCallback(cy_socket_t socket_handle, void *arg) {
    WiFiClient *client = (WiFiClient *)arg;

    client->socket->receiveCallback();

    return CY_RSLT_SUCCESS;
}

cy_rslt_t WiFiClient::disconnectionCallback(cy_socket_t socket_handle, void *arg) {
    WiFiClient *client = (WiFiClient *)arg;

    client->stop();

    return CY_RSLT_SUCCESS;
}

bool WiFiClient::isThisClient(cy_socket_t socket_handle) {
    if (socket->socket == socket_handle) {
        return true;
    }

    return false;
}
