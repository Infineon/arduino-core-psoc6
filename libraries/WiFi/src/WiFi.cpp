#include <WiFi.h>
#include "Socket.h"

/** 
 * @brief Macro to assert the return value of the cy_wcm APIs 
 *        and translate it to the mapped WiFi class error code.
 * @param cy_ret Return value of the cy_wcm API. 
 * @param ret_code The mapped WiFi class error code.
 */
#define wifi_assert_raise(cy_ret, ret_code)   if (cy_ret != CY_RSLT_SUCCESS) { \
        _status = ret_code; \
        return ret_code; \
}

WiFiClass & WiFiClass::get_instance() {
    static WiFiClass wifi_singleton;
    return wifi_singleton;
}

int WiFiClass::begin(const char* ssid) {
    return begin(ssid, nullptr);
}

int WiFiClass::begin(const char* ssid, const char *passphrase) {

    cy_rslt_t ret = wcm_init(CY_WCM_INTERFACE_TYPE_STA);
    wifi_assert_raise(ret, WL_CONNECT_FAILED);

    cy_wcm_connect_params_t connect_param;
    cy_wcm_ip_address_t ipaddress;
    /* Initialized all connect params to zero */
    memset(&connect_param, 0, sizeof(cy_wcm_connect_params_t));

    memcpy(connect_param.ap_credentials.SSID, ssid, strlen(ssid));

    /* Set security based on passphrase */
    if (passphrase == nullptr) {
        connect_param.ap_credentials.security = CY_WCM_SECURITY_OPEN;
    } else {
        memcpy(connect_param.ap_credentials.password, passphrase, strlen(passphrase));
        connect_param.ap_credentials.security = CY_WCM_SECURITY_UNKNOWN;
    }

    ret = CY_WCM_EVENT_CONNECT_FAILED;
    uint8_t retries = 3; /* This number has been selected arbitrarily. */
    _status = WL_IDLE_STATUS;
    do
    {
        ret = cy_wcm_connect_ap(&connect_param, &ipaddress);
    } while (--retries < 0 && ret != CY_RSLT_SUCCESS);
    wifi_assert_raise(ret, WL_CONNECT_FAILED);

    _status = WL_CONNECTED;
    _mode = CY_WCM_INTERFACE_TYPE_STA;

    return WL_CONNECTED;
}

void WiFiClass::end(void) {
    cy_wcm_deinit();
}

uint8_t WiFiClass::beginAP(const char *ssid) {
    return beginAP(ssid, nullptr, 1);
}

uint8_t WiFiClass::beginAP(const char *ssid, uint8_t channel) {
    return beginAP(ssid, nullptr, channel);
}
uint8_t WiFiClass::beginAP(const char *ssid, const char* passphrase) {
    return beginAP(ssid, passphrase, 1);
}

uint8_t WiFiClass::beginAP(const char *ssid, const char* passphrase, uint8_t channel) {

    cy_rslt_t ret = wcm_init(CY_WCM_INTERFACE_TYPE_AP);
    wifi_assert_raise(ret, WL_AP_FAILED);

    cy_wcm_ap_config_t ap_conf;
    /* Initialized all AP config params to zero */
    memset(&ap_conf, 0, sizeof(cy_wcm_ap_config_t));

    ap_conf.channel = channel;
    memcpy(ap_conf.ap_credentials.SSID, ssid, strlen(ssid));

    /* If no passphrase is provided set configuration to open. */
    if (passphrase != nullptr) {
        memcpy(ap_conf.ap_credentials.password, passphrase, strlen(passphrase));
        /* Default security is WPA2_AES_PSK */
        ap_conf.ap_credentials.security = CY_WCM_SECURITY_WPA2_AES_PSK; 
    } else {
        ap_conf.ap_credentials.security = CY_WCM_SECURITY_OPEN;
    }

    /* The AP requires a default some default IP settings */
    cy_wcm_set_ap_ip_setting(&(ap_conf.ip_settings), "192.168.0.1", "255.255.255.0", "192.168.0.1", CY_WCM_IP_VER_V4);

    ret = cy_wcm_start_ap(&ap_conf);
    wifi_assert_raise(ret, WL_AP_FAILED);

    _mode = CY_WCM_INTERFACE_TYPE_AP;
    _status = WL_AP_CONNECTED;

    return WL_AP_CONNECTED;
}

IPAddress WiFiClass::localIP() {
    /* If the WiFi interface has not been yet initialized. */
    if(_mode == CY_WCM_INTERFACE_TYPE_UNKNOWN) {
        return IPAddress(0, 0, 0, 0);
    }

    cy_wcm_ip_address_t ip_address;
    cy_rslt_t ret = cy_wcm_get_ip_addr(_mode, &ip_address);
    if(ret != CY_RSLT_SUCCESS) {
        return IPAddress(0, 0, 0, 0);
    }

    IPAddress ip(ip_address.ip.v4);
    return ip;
}

IPAddress WiFiClass::gatewayIP() {
    if(_mode == CY_WCM_INTERFACE_TYPE_UNKNOWN) {
        return IPAddress(0, 0, 0, 0);
    }

    cy_wcm_ip_address_t gateway_ip;
    cy_rslt_t ret = cy_wcm_get_gateway_ip_address(_mode, &gateway_ip);
    if(ret != CY_RSLT_SUCCESS) {
        return IPAddress(0, 0, 0, 0);
    }

    IPAddress ip(gateway_ip.ip.v4);
    return ip;
};

uint8_t WiFiClass::status() {
    return _status;
}

int WiFiClass::hostByName(const char* aHostname, IPAddress& aResult) {
    return Socket::hostByName(aHostname, aResult);
}   

WiFiClass::WiFiClass() {

}

WiFiClass::~WiFiClass() {

}

cy_rslt_t WiFiClass::wcm_init(cy_wcm_interface_t mode) {
    cy_wcm_config_t wcm_config = { .interface = mode };
    return cy_wcm_init(&wcm_config);
}

WiFiClass & WiFi = WiFiClass::get_instance();