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
    
    socket.set_receive_opt_callback(receive_callback, this); 

    return socket.connect(ip, port);
}

int WiFiClient::connect(const char *host, uint16_t port) {
    /*TODO: First we need to get host by name in WiFi class */
    return 0;
}

size_t WiFiClient::write(uint8_t data) {
    return write(&data, 1);
}

size_t WiFiClient::write(const uint8_t *buf, size_t size) {
    return (size_t)socket.send(buf, size);
}

int WiFiClient::available() {
    return socket.available();
}

int WiFiClient::read()  {
    uint8_t data;
    uint32_t received_data = socket.receive(&data, 1);

    if(received_data == 0) {
        return -1;
    }

    return (int)data;
}

int WiFiClient::read(uint8_t *buf, size_t size) {
    uint32_t received_data = socket.receive(buf, size);

    if(received_data == 0) {
        return -1;
    }

    return (int)received_data;
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
    if(available() <= 0) {
        return false;
    }
    return true;
}

cy_rslt_t WiFiClient::receive_callback(cy_socket_t socket_handle, void * arg) {
    WiFiClient *client = (WiFiClient *)arg;

    client->socket.receive_callback();

    return CY_RSLT_SUCCESS;
}