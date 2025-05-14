#include "WiFiUdp.h"

#define udp_assert(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
            _status = SOCKET_STATUS_ERROR; \
            return; \
}

#define udp_assert_raise(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
            return cy_ret; \
}

WiFiUDP::WiFiUDP() :
    client_handle(nullptr),
    _status(SOCKET_STATUS_UNINITED),
    _last_error(CY_RSLT_SUCCESS),
    remote_ip(0, 0, 0, 0),
    _port(0) {
}

uint8_t WiFiUDP::begin(uint16_t port) {
    _port = port;

    // Initialize the socket for UDP
    socket.begin(SOCKET_PROTOCOL_UDP);
    if (socket.status() != SOCKET_STATUS_CREATED) {
        return 0; // Return 0 if socket creation fails
    }
    // Bind the socket to the specified port
    socket.bind(port);
    if (socket.status() != SOCKET_STATUS_BOUND) {
        return 0; // Return 0 if binding fail

    }
    return socket.status(); // Return the socket status
}


void WiFiUDP::stop() {
    // Stop the UDP socket
    socket.end();
}

int WiFiUDP::beginPacket(IPAddress ip, uint16_t port) {
    remote_ip = ip;
    _port = port;
    return 1; // Return 1 if successful
}

int WiFiUDP::beginPacket(const char *host, uint16_t port) {
    // Resolve the hostname to an IP address
    IPAddress ip;
    if (socket.hostByName(host, ip)) {
        return beginPacket(ip, port); // Call the IP-based version
    }

    // Return 0 if hostname resolution fails
    return 0;
}

int WiFiUDP::endPacket() {
    size_t size = txBuffer.available() > WIFI_UDP_BUFFER_SIZE ? WIFI_UDP_BUFFER_SIZE : txBuffer.available();
    uint8_t temp_buffer[size];
    for (size_t i = 0; i < size; i++) {
        temp_buffer[i] = txBuffer.read_char();
    }
    size_t bytes_sent = socket.send(temp_buffer, size, remote_ip, _port);
    if (bytes_sent != size) {
        return 0; // Return 0 if not all bytes were sent
    }
    return 1; // Return 1 if all bytes were sent successfully
}

size_t WiFiUDP::write(uint8_t byte) {
    return write(&byte, 1);
}

size_t WiFiUDP::write(const uint8_t *buffer, size_t size) {
    size_t bytesStored = 0;

    for (size_t i = 0; i < size; i++) {
        if (!txBuffer.isFull()) {
            txBuffer.store_char(buffer[i]);
            bytesStored++;
        } else {
            break;
        }
    }

    return bytesStored;
}

int WiFiUDP::parsePacket() {
    return 0;
}

int WiFiUDP::available() {
    return 0;
}

int WiFiUDP::read() {
    return 0;
}

int WiFiUDP::read(unsigned char *buffer, size_t len) {
    return 0;
}

int WiFiUDP::read(char *buffer, size_t len) {
    return 0;
}

int WiFiUDP::peek() {
    return 0;
}

void WiFiUDP::flush() {
    return;
}

IPAddress WiFiUDP::remoteIP() {
    return IPAddress();
}

uint16_t WiFiUDP::remotePort() {
    return 0;
}
