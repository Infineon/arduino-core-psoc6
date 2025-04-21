#ifndef WIFI_H
#define WIFI_H

#include <stdint.h>

#include "api/IPAddress.h"
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

typedef enum {
    WIFI_STATUS_UNINITED = 0,
    WIFI_STATUS_INITED = WL_IDLE_STATUS,
    WIFI_STATUS_STA_CONNECTED = WL_CONNECTED,
    WIFI_STATUS_AP_CONNECTED = WL_AP_LISTENING,
    WIFI_STATUS_STA_DISCONNECTED = WIFI_STATUS_INITED,
    WIFI_STATUS_AP_DISCONNECTED = WIFI_STATUS_INITED
} wifi_status_t;

typedef enum {
    WIFI_ERROR_NONE = 0,
    WIFI_ERROR_INIT_FAILED = WL_NO_SHIELD,
    WIFI_ERROR_SCAN_FAILED,
    WIFI_ERROR_STA_CONNECT_FAILED = WL_CONNECT_FAILED,
    WIFI_ERROR_AP_LISTENING_FAILED = WL_AP_FAILED,
    WIFI_ERROR_STA_DISCONNECT_FAILED,
    WIFI_ERROR_AP_DISCONNECT_FAILED,
    WIFI_ERROR_STA_AP_MODE_INCOMPATIBLE,
    WIFI_ERROR_STATUS_INVALID,
    WIFI_ERROR_DEINIT_FAILED,
    WIFI_ERROR_READ_WRITE_CONFIG_FAILED,
    WIFI_ERROR_PING_FAILED
} wifi_error_t;

typedef enum {
    AUTH_MODE_INVALID,
    AUTH_MODE_AUTO,
    AUTH_MODE_OPEN_SYSTEM,
    AUTH_MODE_SHARED_KEY,
    AUTH_MODE_WPA,
    AUTH_MODE_WPA2,
    AUTH_MODE_WPA_WPA2,
    AUTH_MODE_WPA3
}wl_auth_mode;

class WiFiClass {

public:

    /* Return the WiFi class singleton */
    static WiFiClass & get_instance();


    int begin(const char *ssid);
    int begin(const char *ssid, const char *passphrase);

    /* WEP is not supported by WCM class as considered an insecure encryption. */
    // int begin(const char* ssid, uint8_t key_idx, const char* key);

    uint8_t beginAP(const char *ssid);
    uint8_t beginAP(const char *ssid, uint8_t channel);
    uint8_t beginAP(const char *ssid, const char *passphrase);
    uint8_t beginAP(const char *ssid, const char *passphrase, uint8_t channel);

    void config(IPAddress local_ip);
    void config(IPAddress local_ip, IPAddress dns_server);
    void config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
    void config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);

    int disconnect(void);
    void end(void);
    void setTimeout(unsigned long timeout);

    uint8_t * macAddress(uint8_t *mac);
    IPAddress localIP();
    IPAddress subnetMask();
    IPAddress gatewayIP();
    IPAddress dnsIP(int n = 0);
    const char * SSID();
    uint8_t * BSSID(uint8_t *bssid);
    int32_t RSSI();
    uint8_t encryptionType();

    int8_t scanNetworks();
    const char * SSID(uint8_t networkItem);
    uint8_t * BSSID(uint8_t networkItem, uint8_t *bssid);
    int32_t RSSI(uint8_t networkItem);
    uint8_t encryptionType(uint8_t networkItem);
    uint8_t channel(uint8_t networkItem);

    uint8_t status();

    int hostByName(const char *aHostname, IPAddress& ip);

    int ping(const char *hostname, uint8_t ttl = 128);
    int ping(const String &hostname, uint8_t ttl = 128);
    int ping(IPAddress host, uint8_t ttl = 128);

    uint8_t reasonCode();

    wifi_error_t getLastError();

    void setDNS(IPAddress dns_server1);
    void setDNS(IPAddress dns_server1, IPAddress dns_server2);

private:

    /* Extension of cy_wcm_interface_t enums. */
    #define CY_WCM_INTERFACE_TYPE_UNKNOWN   (cy_wcm_interface_t)0xFF

    cy_wcm_interface_t _mode = CY_WCM_INTERFACE_TYPE_UNKNOWN;
    volatile wifi_status_t _status = WIFI_STATUS_UNINITED;
    wifi_error_t _last_error = WIFI_ERROR_NONE;

    /* IP settings */
    bool user_static_ip_settings = false;
    cy_wcm_ip_setting_t ip_settings = {{}, {}, {}};

    /* AP configuration */
    const char *DEFAULT_AP_IP = "192.168.0.1";
    const char *DEFAULT_AP_GATEWAY_IP = "192.168.0.1";
    const char *DEFAULT_AP_SUBNET_MASK = "255.255.255.0";
    cy_wcm_ap_config_t ap_conf;

    /* STA configuration */
    cy_wcm_associated_ap_info_t ap_info;

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
    wifi_error_t wcm_init(cy_wcm_interface_t mode);
    wifi_error_t wcm_assert_interface_mode(cy_wcm_interface_t mode);

    int disconnectSTA(void);
    int disconnectAP(void);

    static void set_connect_params_sta(cy_wcm_connect_params_t *connect_params, const char *ssid, const char *passphrase);
    void set_params_ap(const char *ssid, const char *passphrase, uint8_t channel);

    void set_ip_settings_ap();
    void set_ip_settings_sta(cy_wcm_connect_params_t *connect_params);

    // timeout configuration
    unsigned long timeout_ms = 10000;

    const char * SSID_STA();
    const char * SSID_AP();

    static wl_auth_mode convertEncryptType(cy_wcm_security_t wcm_sec);

    static const uint8_t CY_MAX_SCAN_RESULTS = 10;
    typedef struct {
        cy_wcm_scan_status_t status;
        int8_t result_count;
        cy_wcm_scan_result_t results[CY_MAX_SCAN_RESULTS];
    }scan_results_t;

    volatile scan_results_t scan_results = {CY_WCM_SCAN_INCOMPLETE, 0, {}};

    static void wcm_scan_cb(cy_wcm_scan_result_t *result_ptr, void *user_data, cy_wcm_scan_status_t status);
};

extern WiFiClass & WiFi;

#endif /* WIFI_H */
