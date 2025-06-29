#include "WiFiUdp.h"

#define udp_assert(cy_ret)             \
    if (cy_ret != CY_RSLT_SUCCESS) {   \
        _status = SOCKET_STATUS_ERROR; \
        return;                        \
    }

#define udp_assert_raise(cy_ret)     \
    if (cy_ret != CY_RSLT_SUCCESS) { \
        return cy_ret;               \
    }

WiFiUDP::WiFiUDP()
    : _status(SOCKET_STATUS_UNINITED),
      _last_error(CY_RSLT_SUCCESS),
      remote_ip(0, 0, 0, 0),
      _port(0) {
}

uint8_t WiFiUDP::beginInternal(uint16_t port, IPAddress multicastIP) {

    socket.begin(SOCKET_PROTOCOL_UDP);
    if (socket.status() != SOCKET_STATUS_CREATED) {
        return 0;
    }

    socket.setReceiveOptCallback(receiveCallback, this);

    socket.bind(port);
    if (socket.status() != SOCKET_STATUS_BOUND) {
        return 0;
    }

    if (multicastIP != IPAddress(0, 0, 0, 0)) {
        IPAddress local_ip = WiFi.localIP(); // Get the local IP address

        // Bind the socket to the specified multicast address and port
        cy_rslt_t result = socket.joinMulticastGroup(multicastIP, local_ip);
        udp_assert_raise(result);
    }

    return socket.status(); // Return the socket status
}

uint8_t WiFiUDP::begin(uint16_t port) {
    // Start the UDP socket on the specified port
    return beginInternal(port, IPAddress(0, 0, 0, 0));
}

uint8_t WiFiUDP::beginMulticast(IPAddress ip, uint16_t port) {
    return beginInternal(port, ip);
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

int WiFiUDP::beginPacket(const char* host, uint16_t port) {
    // Resolve the hostname to an IP address
    IPAddress ip;
    if (socket.hostByName(host, ip)) {
        return beginPacket(ip, port); // Call the IP-based version
    }

    // Return 0 if hostname resolution fails
    return 0;
}

int WiFiUDP::endPacket() {
    size_t size = static_cast<size_t>(txBuffer.available()) > WIFI_UDP_BUFFER_SIZE
                      ? WIFI_UDP_BUFFER_SIZE
                      : static_cast<size_t>(txBuffer.available());
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

size_t WiFiUDP::write(const uint8_t* buffer, size_t size) {
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

    // Check if there are any packets in the rx_packets vector
    if (rx_packets.empty()) {
        current_packet = {{}, IPAddress(0, 0, 0, 0), 0};
        return 0;
    }

    current_packet = rx_packets.front();
    rx_packets.erase(rx_packets.begin());
    return current_packet.rx_buf.available();
}

int WiFiUDP::available() {
    return current_packet.rx_buf.available();
}

int WiFiUDP::read() {
    if (current_packet.rx_buf.available() < 1) {
        return -1;
    }
    char buffer[1];
    read((unsigned char*)&buffer, 1);
    return buffer[0];
}

int WiFiUDP::read(unsigned char* buffer, size_t len) {

    size_t rx_available = (size_t)current_packet.rx_buf.available();
    if (rx_available < 1) {
        return -1;
    }

    len = len > rx_available ? rx_available : len;
    for (size_t i = 0; i < len; i++) {
        buffer[i] = current_packet.rx_buf.read_char();
    }
    return len;
}

int WiFiUDP::peek() {
    if (current_packet.rx_buf.available() < 1) {
        return -1;
    }
    return current_packet.rx_buf.peek();
}

void WiFiUDP::flush() {
    current_packet.rx_buf.clear();
}

IPAddress WiFiUDP::remoteIP() {
    return current_packet._senderIP;
}

uint16_t WiFiUDP::remotePort() {
    return current_packet._senderPort;
}

cy_rslt_t WiFiUDP::receiveCallback(cy_socket_t socket_handle, void* arg) {
    WiFiUDP* udp = (WiFiUDP*)arg;

    cy_socket_sockaddr_t peer_addr;

    udp->socket.receiveCallback(&peer_addr);

    udp_rx_packet_t new_packet;
    new_packet._senderIP = IPAddress(peer_addr.ip_address.ip.v4);
    new_packet._senderPort = peer_addr.port;

    uint32_t bytes_received = udp->socket.rx_buf.available();
    for (uint32_t i = 0; i < bytes_received; i++) {
        new_packet.rx_buf.store_char(udp->socket.rx_buf.read_char());
    }
    udp->rx_packets.push_back(new_packet);
    return CY_RSLT_SUCCESS;
}
