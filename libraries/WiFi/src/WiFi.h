#ifndef WIFI_H
#define WIFI_H

#include <stdint.h>

#include "IPAddress.h"
#include "WiFiClient.h"
#include "WiFiServer.h"

#include "cy_wcm.h"

/**
 * TODO: 
 *  - Adapt the enum list conveniently. Do we need to keep all?
 *  - Move it to some header file?
*/
typedef enum {
    WL_NO_SHIELD = 255,
    WL_NO_MODULE = 255,
    WL_IDLE_STATUS = 0,
    WL_NO_SSID_AVAIL,
    WL_SCAN_COMPLETED,
    WL_CONNECTED,
    WL_CONNECT_FAILED,
    WL_CONNECTION_LOST,
    WL_DISCONNECTED,
    WL_AP_LISTENING,
    WL_AP_CONNECTED,
    WL_AP_FAILED
} wl_status_t;

class WiFiClass {

    public:

        /* Return the WiFi class singleton */
        static WiFiClass & get_instance();

        /* Start Wifi connection for OPEN networks
        *
        * param ssid: Pointer to the SSID string.
        */
        int begin(const char *ssid);

        /* Start WiFi connection with passphrase
        * the most secure supported mode will be automatically selected
        *
        * param ssid: Pointer to the SSID string.
        * param passphrase: Passphrase. Valid characters in a passphrase
        *        must be between ASCII 32-126 (decimal).
        */
        int begin(const char *ssid, const char *passphrase);

        /* WEP is not supported by WCM class as considered an insecure encryption. */

        // /* Start WiFi connection with WEP encryption.
        // * Configure a key into the device. The key type (WEP-40, WEP-104)
        // * is determined by the size of the key (5 bytes for WEP-40, 13 bytes for WEP-104).
        // *
        // * param ssid: Pointer to the SSID string.
        // * param key_idx: The key index to set. Valid values are 0-3.
        // * param key: Key input buffer.
        // */
        // int begin(const char* ssid, uint8_t key_idx, const char* key);

        void end(void);

        /* Connect as Access Point with  a standard passphrase */
        uint8_t beginAP(const char *ssid);
        uint8_t beginAP(const char *ssid, uint8_t channel);
        uint8_t beginAP(const char *ssid, const char* passphrase);
        uint8_t beginAP(const char *ssid, const char* passphrase, uint8_t channel);

        /* Change IP configuration settings disabling the DHCP client
        *
        * param local_ip:   Static IP configuration
        * param dns_server: IP configuration for DNS server 1
        * param gateway:    Static gateway configuration
        * param subnet:     Static Subnet mask
        */
        void config(IPAddress local_ip);
        void config(IPAddress local_ip, IPAddress dns_server);
        void config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
        void config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);

        /*
        * Get the interface IP address.
        * Currently only IPv4 is supported.
        *
        * return: IP address value. Returns 0.0.0.0 in case of error.
        */
        IPAddress localIP();

        /*
        * Get the gateway IP address.
        *
        * return: gateway IP address value
        */
        IPAddress gatewayIP();

        /**
         * Return Connection status
         * TODO: Clean up unused or irrelevant status codes for this core.
         * 
         *  WL_CONNECTED: assigned when connected to a WiFi network;
         *  WL_AP_CONNECTED : assigned when a device is connected in Access Point mode;
         *  WL_AP_LISTENING : assigned when the listening for connections in Access Point mode;
         *  WL_NO_SHIELD: assigned when no WiFi shield is present;
         *  WL_NO_MODULE: assigned when the communication with an integrated WiFi module fails;
         *  WL_IDLE_STATUS: it is a temporary status assigned when WiFi.begin() is called and remains active until the number of attempts expires (resulting in WL_CONNECT_FAILED) or a connection is established (resulting in WL_CONNECTED);
         *  WL_NO_SSID_AVAIL: assigned when no SSID are available;
         *  WL_SCAN_COMPLETED: assigned when the scan networks is completed;
         *  WL_CONNECT_FAILED: assigned when the connection fails for all the attempts;
         *  WL_CONNECTION_LOST: assigned when the connection is lost;
         *  WL_DISCONNECTED: assigned when disconnected from a network;
         */
        uint8_t status();

        int hostByName(const char* aHostname, IPAddress& aResult);

    private: 

        /* Extension of cy_wcm_interface_t enums. */
        #define CY_WCM_INTERFACE_TYPE_UNKNOWN   (cy_wcm_interface_t)0xFF

        cy_wcm_interface_t _mode = CY_WCM_INTERFACE_TYPE_UNKNOWN;
        volatile wl_status_t _status = WL_IDLE_STATUS;


        /* The WiFi class is implemented as singleton.
        The constructor and destructor are private. */
        WiFiClass();
        ~WiFiClass();

        /* Delete copy constructor and assignment operator  */
        WiFiClass(const WiFiClass&) = delete;
        WiFiClass& operator = (const WiFiClass&) = delete;

        /**
        * Initialize the WiFi connection manager.
        * It support both AP and STA interfaces.
        */
        static cy_rslt_t wcm_init(cy_wcm_interface_t mode);
};

extern WiFiClass & WiFi;

#endif /* WIFI_H */
