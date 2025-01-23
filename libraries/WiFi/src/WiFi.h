#include <stdint.h>

#include "cy_wcm.h"

/**
 * TODOs: 
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
        int begin(const char* ssid);

        /* Start WiFi connection with passphrase
        * the most secure supported mode will be automatically selected
        *
        * param ssid: Pointer to the SSID string.
        * param passphrase: Passphrase. Valid characters in a passphrase
        *        must be between ASCII 32-126 (decimal).
        */
        int begin(const char* ssid, const char *passphrase);

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

    private: 

        /* The WiFi class is implemented as singleton. 
           The constructor and destructor are private. */
        WiFiClass();
        ~WiFiClass();

        /* Delete copy constructor and assignment operator  */
        WiFiClass(const WiFiClass&) = delete;
        WiFiClass& operator=(const WiFiClass&) = delete;

        /**
         * Initialize the WiFi connection manager.
         * It support both AP and STA interfaces.
         */
        static cy_rslt_t wcm_init();
};

extern WiFiClass & WiFi;