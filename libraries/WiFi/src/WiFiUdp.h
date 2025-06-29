#ifndef WIFI_UDP_H
#define WIFI_UDP_H

#include "SecSocket.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "WiFiServer.h"
#include "api/Udp.h"
#include <stdint.h>
#include <string.h>

class WiFiUDP : public arduino::UDP {
public:
    static const size_t WIFI_UDP_BUFFER_SIZE = 256;

    WiFiUDP();
    uint8_t begin(uint16_t);
    uint8_t beginMulticast(IPAddress, uint16_t);
    void stop();
    int beginPacket(IPAddress ip, uint16_t port);
    int beginPacket(const char* host, uint16_t port);
    int endPacket();
    size_t write(uint8_t byte);
    size_t write(const uint8_t* buffer, size_t size);
    using Print::write;
    int parsePacket();
    int available();
    int read();
    int read(unsigned char* buffer, size_t len);

    int read(char* buffer, size_t len) {
        return read((unsigned char*)buffer, len);
    };

    int peek();
    void flush();
    IPAddress remoteIP();
    uint16_t remotePort();

private:
    Socket socket = {};

    uint8_t beginInternal(uint16_t port, IPAddress multicastIP = IPAddress());
    static cy_rslt_t receiveCallback(cy_socket_t socket_handle, void* arg);

    typedef struct {
        arduino::RingBufferN<WIFI_UDP_BUFFER_SIZE> rx_buf;
        IPAddress _senderIP;  // Stores the sender's IP address
        uint16_t _senderPort; // Stores the sender's port
    } udp_rx_packet_t;

    std::vector<udp_rx_packet_t> rx_packets = {};
    udp_rx_packet_t current_packet = {{}, IPAddress(0, 0, 0, 0), 0};

    socket_status_t _status = SOCKET_STATUS_UNINITED;
    cy_rslt_t _last_error = CY_RSLT_SUCCESS;
    IPAddress remote_ip = {0, 0, 0, 0};
    uint16_t _port = 0;
    arduino::RingBufferN<WIFI_UDP_BUFFER_SIZE> txBuffer = {};
};

#endif /* WIFI_UDP_H */
