#include <WiFi.h>
#include "SecSocket.h"

#define wcm_assert_raise(cy_ret, ret_code)   if (cy_ret != CY_RSLT_SUCCESS) { \
            _last_error = ret_code; \
            return ret_code; \
}

#define wcm_assert(cy_ret, ret_code)   if (cy_ret != CY_RSLT_SUCCESS) { \
            _last_error = ret_code; \
            return; \
}

#define wifi_assert_raise(ret_code)  if (ret_code != WIFI_ERROR_NONE) { \
            _last_error = ret_code; \
            return ret_code; \
}

WiFiClass & WiFiClass::get_instance() {
    static WiFiClass wifi_singleton;
    return wifi_singleton;
}

int WiFiClass::begin(const char *ssid) {
    return begin(ssid, nullptr);
}

int WiFiClass::begin(const char *ssid, const char *passphrase) {
    _last_error = wcm_init(CY_WCM_INTERFACE_TYPE_STA);
    wifi_assert_raise(_last_error);

    /* Ensure begin() is only called from an uninitialized WiFi
    instance or from an instance in station mode. */
    _last_error = wcm_assert_interface_mode(CY_WCM_INTERFACE_TYPE_STA);
    wifi_assert_raise(_last_error);

    if (_status == WIFI_STATUS_INITED ||
        _status == WIFI_STATUS_STA_DISCONNECTED) {
        cy_wcm_connect_params_t connect_params;
        set_connect_params_sta(&connect_params, ssid, passphrase);
        
        cy_wcm_ip_address_t ipaddress;
        cy_rslt_t ret = CY_WCM_EVENT_CONNECT_FAILED;
        uint8_t retries = 3; /* This number has been selected arbitrarily. */
        do
        {
            ret = cy_wcm_connect_ap(&connect_params, &ipaddress);
        } while (--retries < 0 && ret != CY_RSLT_SUCCESS);
        wcm_assert_raise(ret, WIFI_ERROR_STA_CONNECT_FAILED);

        _status = WIFI_STATUS_STA_CONNECTED;
    }

    return _status;
}

int WiFiClass::disconnect(void) {
    switch (_mode)
    {
        case CY_WCM_INTERFACE_TYPE_STA:
            return disconnectSTA();
        case CY_WCM_INTERFACE_TYPE_AP:
            return disconnectAP();
        default:
            return WIFI_ERROR_STA_AP_MODE_INCOMPATIBLE;
    }
}

void WiFiClass::end(void) {
    cy_rslt_t ret = cy_wcm_deinit();
    wcm_assert(ret, WIFI_ERROR_DEINIT_FAILED);

    _status = WIFI_STATUS_UNINITED;
}

uint8_t WiFiClass::beginAP(const char *ssid) {
    return beginAP(ssid, nullptr, 1);
}

uint8_t WiFiClass::beginAP(const char *ssid, uint8_t channel) {
    return beginAP(ssid, nullptr, channel);
}
uint8_t WiFiClass::beginAP(const char *ssid, const char *passphrase) {
    return beginAP(ssid, passphrase, 1);
}

uint8_t WiFiClass::beginAP(const char *ssid, const char *passphrase, uint8_t channel) {
    _last_error = wcm_init(CY_WCM_INTERFACE_TYPE_AP);
    wifi_assert_raise(_last_error);

    /* Ensure beginAP() is only called from an uninitialized WiFi
    instance or from an instance in access point mode. */
    _last_error = wcm_assert_interface_mode(CY_WCM_INTERFACE_TYPE_AP);
    wifi_assert_raise(_last_error);

    if (_status == WIFI_STATUS_INITED ||
        _status == WIFI_STATUS_AP_DISCONNECTED) {
        set_params_ap(&ap_conf, ssid, passphrase, channel);

        /** TODO: This can be added to set_params_ap? Wait for the development of config function() */
        /* The AP requires some default IP settings */
        cy_wcm_set_ap_ip_setting(&(ap_conf.ip_settings), "192.168.0.1", "255.255.255.0", "192.168.0.1", CY_WCM_IP_VER_V4);

        cy_rslt_t ret = cy_wcm_start_ap(&ap_conf);
        wcm_assert_raise(ret, WIFI_ERROR_AP_LISTENING_FAILED);

        _status = WIFI_STATUS_AP_CONNECTED;
    }

    return _status;
}

IPAddress WiFiClass::localIP() {
    if (_status == WIFI_STATUS_UNINITED) {
        _last_error = WIFI_ERROR_STATUS_INVALID;
        return IPAddress(0, 0, 0, 0);
    }

    cy_wcm_ip_address_t ip_address;
    cy_rslt_t ret = cy_wcm_get_ip_addr(_mode, &ip_address);
    if (ret != CY_RSLT_SUCCESS) {
        return IPAddress(0, 0, 0, 0);
    }

    IPAddress ip(ip_address.ip.v4);
    return ip;
}

IPAddress WiFiClass::gatewayIP() {
    if (_status == WIFI_STATUS_UNINITED) {
        _last_error = WIFI_ERROR_STATUS_INVALID;
        return IPAddress(0, 0, 0, 0);
    }

    cy_wcm_ip_address_t gateway_ip;
    cy_rslt_t ret = cy_wcm_get_gateway_ip_address(_mode, &gateway_ip);
    if (ret != CY_RSLT_SUCCESS) {
        return IPAddress(0, 0, 0, 0);
    }

    IPAddress ip(gateway_ip.ip.v4);
    return ip;
};

const char* WiFiClass::SSID() {
    switch(_mode)
    {
        case CY_WCM_INTERFACE_TYPE_STA:
            return SSID_STA();
        case CY_WCM_INTERFACE_TYPE_AP:
            return SSID_AP();
        default:
            /* The instance has not yet called begin()
            or beginAP(). Return empty string. */
            _last_error = WIFI_ERROR_STATUS_INVALID;
            return "";
    }
}

uint8_t WiFiClass::status() {
    return _status;
}

int WiFiClass::hostByName(const char *aHostname, IPAddress& ip) {
    return Socket::hostByName(aHostname, ip);
}

wifi_error_t WiFiClass::getLastError() {
    return _last_error;
}


WiFiClass::WiFiClass() {

}

WiFiClass::~WiFiClass() {

}

wifi_error_t WiFiClass::wcm_init(cy_wcm_interface_t mode) {
    if (_status == WIFI_STATUS_UNINITED) {
        cy_wcm_config_t wcm_config = { .interface = mode };
        cy_rslt_t ret = cy_wcm_init(&wcm_config);
        wcm_assert_raise(ret, WIFI_ERROR_INIT_FAILED);
        _mode = mode;
        _status = WIFI_STATUS_INITED;
    }

    return WIFI_ERROR_NONE;
}

wifi_error_t WiFiClass::wcm_assert_interface_mode(cy_wcm_interface_t mode) {
    if (_mode != mode) {
        return WIFI_ERROR_STA_AP_MODE_INCOMPATIBLE;
    }

    return WIFI_ERROR_NONE;
}

int WiFiClass::disconnectSTA(void) {
    if (_status == WIFI_STATUS_STA_CONNECTED) {
        cy_rslt_t ret = cy_wcm_disconnect_ap();
        wcm_assert_raise(ret, WIFI_ERROR_STA_DISCONNECT_FAILED);
        _status = WIFI_STATUS_STA_DISCONNECTED;
        return _status;
    }

    return WIFI_ERROR_STATUS_INVALID;
}

int WiFiClass::disconnectAP(void) {
    if (_status == WIFI_STATUS_AP_CONNECTED) {
        cy_rslt_t ret = cy_wcm_stop_ap();
        wcm_assert_raise(ret, WIFI_ERROR_AP_DISCONNECT_FAILED);
        _status = WIFI_STATUS_AP_DISCONNECTED;
        return _status;
    }

    return WIFI_ERROR_STATUS_INVALID;
}

void WiFiClass::set_connect_params_sta(cy_wcm_connect_params_t *connect_params, const char *ssid, const char *passphrase) {
    /* Initialized all connect params to zero */
    memset(connect_params, 0, sizeof(cy_wcm_connect_params_t));

    memcpy(connect_params->ap_credentials.SSID, ssid, strlen(ssid));

    /* Set security based on passphrase */
    if (passphrase == nullptr) {
        connect_params->ap_credentials.security = CY_WCM_SECURITY_OPEN;
    } else {
        memcpy(connect_params->ap_credentials.password, passphrase, strlen(passphrase));
        connect_params->ap_credentials.security = CY_WCM_SECURITY_UNKNOWN;
    }
}

void WiFiClass::set_params_ap(cy_wcm_ap_config_t *ap_config, const char *ssid, const char *passphrase, uint8_t channel) {
    /* Initialized all AP config params to zero */
    memset(ap_config, 0, sizeof(cy_wcm_ap_config_t));

    ap_config->channel = channel;
    memcpy(ap_config->ap_credentials.SSID, ssid, strlen(ssid));

    /* If no passphrase is provided set configuration to open. */
    if (passphrase != nullptr) {
        memcpy(ap_config->ap_credentials.password, passphrase, strlen(passphrase));
        /* Default security is WPA2_AES_PSK */
        ap_config->ap_credentials.security = CY_WCM_SECURITY_WPA2_AES_PSK;
    } else {
        ap_config->ap_credentials.security = CY_WCM_SECURITY_OPEN;
    }
}

const char * WiFiClass::SSID_STA() {
    cy_rslt_t ret = cy_wcm_get_associated_ap_info(&ap_info);
    if(ret != CY_RSLT_SUCCESS) {
        return "";
    }
    return (const char *)(&(ap_info.SSID));
}

const char * WiFiClass::SSID_AP() {
    return (const char *)(&(ap_conf.ap_credentials.SSID));
}

WiFiClass & WiFi = WiFiClass::get_instance();
