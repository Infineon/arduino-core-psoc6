#include "WiFiUdp.h"


#define udp_assert(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
            _status = SOCKET_STATUS_ERROR; \
            return; \
}

#define udp_assert_raise(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
            return cy_ret; \
}

WiFiUDP::WiFiUDP() :
    _status(SOCKET_STATUS_UNINITED),
    _last_error(CY_RSLT_SUCCESS),
    remote_ip(0, 0, 0, 0),
    _port(0),
    _parsedPacketSize(0) {
}

uint8_t WiFiUDP::begin(uint16_t port) {
    _parsedPacketSize = 0;

    // Initialize the socket for UDP
    socket.begin(SOCKET_PROTOCOL_UDP);
    if (socket.status() != SOCKET_STATUS_CREATED) {
        return 0;
    }

    socket.setReceiveOptCallback(receiveCallback, this);

    // Bind the socket to the specified port
    socket.bind(port);
    if (socket.status() != SOCKET_STATUS_BOUND) {
        return 0;

    }
    return socket.status(); // Return the socket status
}


void WiFiUDP::stop() {
    socket.end();
}

int WiFiUDP::beginPacket(IPAddress ip, uint16_t port) {
    remote_ip = ip;
    _port = port;
    txBuffer.clear();
    return 1;
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
        return 0;
    }
    return 1;
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
    if (_parsedPacketSize > 0) {
        // discard previously parsed packet data
        while (available()) {
            read();
        }
    }
    _parsedPacketSize = socket.available();
    return _parsedPacketSize;
}

int WiFiUDP::available() {
    return socket.available();
}

int WiFiUDP::read() {

    if (_parsedPacketSize < 1) {
        return -1;
    }
    uint8_t data;
    uint32_t received_data = socket.receive(&data, 1);
    if (received_data == 0) {
        return -1;
    }
    _parsedPacketSize--;
    return (int)data;
}

int WiFiUDP::read(unsigned char *buffer, size_t len) {

    if (_parsedPacketSize < 1) {
        return -1;
    }
    uint32_t received_data = socket.receive((uint8_t *)buffer, len);
    if (received_data == 0) {
        return -1;
    }
    _parsedPacketSize -= received_data;
    return (int)received_data;
}

int WiFiUDP::peek() {
    if (_parsedPacketSize < 1) {
        return -1;
    }
    return socket.peek();
}

void WiFiUDP::flush() {
    socket.flush();
    _parsedPacketSize = 0;
}

IPAddress WiFiUDP::remoteIP() {
    return _senderIP;
}

uint16_t WiFiUDP::remotePort() {
    return _senderPort;
}

cy_rslt_t WiFiUDP::receiveCallback(cy_socket_t socket_handle, void *arg) {
    WiFiUDP *udp = (WiFiUDP *)arg;

    cy_socket_sockaddr_t peer_addr;
    udp->socket.receiveCallback(&peer_addr);

    // store the sender's IP address and port
    udp->_senderIP = IPAddress(peer_addr.ip_address.ip.v4);
    udp->_senderPort = peer_addr.port;

    return CY_RSLT_SUCCESS;
}
