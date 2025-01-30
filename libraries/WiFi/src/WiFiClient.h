
#include "Client.h"
#include "cy_secure_sockets.h"

class WiFiClient : arduino::Client {

    public:
        WiFiClient();

        int connect(IPAddress ip, uint16_t port);
        int connect(const char *host, uint16_t port);
        size_t write(uint8_t);
        size_t write(const uint8_t *buf, size_t size);
        int available();
        int read();
        int read(uint8_t *buf, size_t size);
        int peek();
        void flush();
        void stop();
        uint8_t connected();
        operator bool();
    

    private:
        cy_socket_t socket; 

        friend class WiFiServer;
};