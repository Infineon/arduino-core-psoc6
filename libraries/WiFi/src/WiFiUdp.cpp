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
    _last_error(CY_RSLT_SUCCESS) {
}

uint8_t WiFiUDP::begin(uint16_t port) {
    _last_error = WiFiUDP::global_sockets_init();
    udp_assert_raise(_last_error);

    /* Create a UDP socket */
    _last_error = cy_socket_create(CY_SOCKET_DOMAIN_AF_INET, CY_SOCKET_TYPE_DGRAM, CY_SOCKET_IPPROTO_UDP, &client_handle);
    udp_assert_raise(_last_error);

    _status = SOCKET_STATUS_CREATED;
    return 1;
}


void WiFiUDP::stop() {

}

int WiFiUDP::beginPacket(IPAddress ip, uint16_t port) {
    return 0;
}

int WiFiUDP::beginPacket(const char *host, uint16_t port) {
    return 0;
}

int WiFiUDP::endPacket() {
    return 0;
}

size_t WiFiUDP::write(uint8_t) {
    return 0;
}

size_t WiFiUDP::write(const uint8_t *buffer, size_t size) {
    return 0;
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

bool WiFiUDP::global_socket_initialized = false;
uint32_t WiFiUDP::global_socket_count = 0;

cy_rslt_t WiFiUDP::global_sockets_init() {
    if (!WiFiUDP::global_socket_initialized) {
        cy_rslt_t ret = cy_socket_init();
        if (ret != CY_RSLT_SUCCESS) {
            return ret;
        }
        WiFiUDP::global_socket_initialized = true;
    }
    WiFiUDP::global_socket_count++;
    return CY_RSLT_SUCCESS;
}
