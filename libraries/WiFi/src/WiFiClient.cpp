#include <WiFiClient.h>

/** 
 * @brief Macro to assert the return value of the cy_wcm APIs 
 *        and translate it to the mapped WiFi class error code.
 * @param cy_ret Return value of the cy_wcm API. 
 * @param ret_code The mapped WiFi class error code.
 */
#define wifi_client_assert_raise(cy_ret, ret_code)   if (cy_ret != CY_RSLT_SUCCESS) { \
        return ret_code; \
}

#define wifi_client_assert(cy_ret)   if (cy_ret != CY_RSLT_SUCCESS) { \
        return; \
}

WiFiClient::WiFiClient() {

}

int WiFiClient::connect(IPAddress ip, uint16_t port) {
    socket.begin();
    
    socket.set_receive_opts(tcp_receive_msg_handler, this);
    socket.set_disconnect_opts(tcp_disconnection_handler, this);

    return socket.connect(ip, port);
}

int WiFiClient::connect(const char *host, uint16_t port) {
    return 0;
}

size_t WiFiClient::write(uint8_t) {
    return 0;
}

size_t WiFiClient::write(const uint8_t *buf, size_t size) {
    return 0;
}

int WiFiClient::available() {
    return true;
}

int WiFiClient::read()  {
    return 0;
}

int WiFiClient::read(uint8_t *buf, size_t size) {
    return 0;   
}

int WiFiClient::peek() {
    return 0;
}

void WiFiClient::flush() {

}

void WiFiClient::stop() {

}

uint8_t WiFiClient::connected() {
    return (socket.status() == SOCKET_STATUS_CONNECTED); 
}

uint8_t WiFiClient::status() {
    return socket.status();
}

WiFiClient::operator bool() {
    if (socket.status() != SOCKET_STATUS_CONNECTED) {
        return false;
    }
    if(available() <= 0) 
    {
        return false;
    }
    return true;
}

cy_rslt_t WiFiClient::tcp_receive_msg_handler(cy_socket_t socket_handle, void *arg) {
    return 0;
}

cy_rslt_t WiFiClient::tcp_disconnection_handler(cy_socket_t socket_handle, void *arg) {
    
    return 0;
}