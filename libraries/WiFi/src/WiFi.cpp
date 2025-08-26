#include <WiFi.h>
#include "api/Common.h"
#include "SecSocket.h"
#include "lwip/dns.h"

#define wcm_assert_raise(cy_ret, ret_code)   if (cy_ret != CY_RSLT_SUCCESS) { \
            _last_error = ret_code; \
            return ret_code; \
}

#define wcm_assert(cy_ret, ret_code)   if (cy_ret != CY_RSLT_SUCCESS) { \
            _last_error = ret_code; \
            return; \
}

#define wcm_assert_raise_ret(cy_ret, ret_code, ret_value)   if (cy_ret != CY_RSLT_SUCCESS) { \
            _last_error = ret_code; \
            return ret_value; \
}

#define wifi_assert_raise(ret_code)  if (ret_code != WIFI_ERROR_NONE) { \
            _last_error = ret_code; \
            return ret_code; \
}

#define wifi_assert_raise_ret(ret_code, ret_value)  if (ret_code != WIFI_ERROR_NONE) { \
            _last_error = ret_code; \
            return ret_value; \
}

#define wcm_config_assert_raise(cy_ret, ret_value)   if (cy_ret != CY_RSLT_SUCCESS) { \
            _last_error = WIFI_ERROR_READ_WRITE_CONFIG_FAILED; \
            return ret_value; \
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
        set_ip_settings_sta(&connect_params);

        cy_wcm_ip_address_t ipaddress;
        cy_rslt_t ret = CY_WCM_EVENT_CONNECT_FAILED;

        uint32_t start_time = millis();

        while ((millis() - start_time) < timeout_ms && ret != CY_RSLT_SUCCESS) {
            ret = cy_wcm_connect_ap(&connect_params, &ipaddress);
        }
        wcm_assert_raise(ret, WIFI_ERROR_STA_CONNECT_FAILED);

        _status = WIFI_STATUS_STA_CONNECTED;
    }

    return _status;
}

int WiFiClass::connected(void) {
    cy_wcm_mac_t sta_list[NETWORK_WLAN_MAX_AP_STATIONS];
    cy_wcm_mac_t not_conn_sta = {0, 0, 0, 0, 0, 0};
    int num_of_station = 0;

    uint32_t ret = cy_wcm_get_associated_client_list(&sta_list[0], NETWORK_WLAN_MAX_AP_STATIONS);
    wcm_assert_raise(ret, WIFI_ERROR_STATUS_INVALID);

    for (int i = 0; i < NETWORK_WLAN_MAX_AP_STATIONS; ++i) {
        if (memcmp(&sta_list[i], &not_conn_sta, CY_WCM_MAC_ADDR_LEN) != 0) {
            num_of_station += 1;
        }
    }

    return num_of_station;
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

void WiFiClass::setTimeout(unsigned long timeout) {
    timeout_ms = timeout;
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
        set_params_ap(ssid, passphrase, channel);
        set_ip_settings_ap();

        cy_rslt_t ret = cy_wcm_start_ap(&ap_conf);
        wcm_assert_raise(ret, WIFI_ERROR_AP_LISTENING_FAILED);

        _status = WIFI_STATUS_AP_CONNECTED;
    }

    return _status;
}

void WiFiClass::config(IPAddress local_ip) {
    const ip_addr_t *dns = dns_getserver(0);
    config(local_ip, IPAddress(dns->addr));
}

void WiFiClass::config(IPAddress local_ip, IPAddress dns_server) {
    config(local_ip, dns_server, IPAddress(DEFAULT_AP_GATEWAY_IP));
}

void WiFiClass::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway) {
    config(local_ip, dns_server, gateway, IPAddress(DEFAULT_AP_SUBNET_MASK));
}

void WiFiClass::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet) {
    user_static_ip_settings = true;

    ip_settings.ip_address.version = CY_WCM_IP_VER_V4;
    ip_settings.ip_address.ip.v4 = uint32_t(local_ip);

    const ip_addr_t dns = {.addr = uint32_t(dns_server)};
    dns_setserver(0, &dns);

    ip_settings.netmask.version = CY_WCM_IP_VER_V4;
    ip_settings.netmask.ip.v4 = uint32_t(subnet);

    ip_settings.gateway.version = CY_WCM_IP_VER_V4;
    ip_settings.gateway.ip.v4 = uint32_t(gateway);
}

uint8_t * WiFiClass::macAddress(uint8_t *mac) {
    cy_wcm_mac_t wcm_mac;
    memset(mac, 0, CY_WCM_MAC_ADDR_LEN);

    cy_rslt_t ret = cy_wcm_get_mac_addr(_mode, &wcm_mac);
    wcm_config_assert_raise(ret, mac);

    memcpy(mac, wcm_mac, CY_WCM_MAC_ADDR_LEN);
    return mac;
}

IPAddress WiFiClass::localIP() {
    if (_status == WIFI_STATUS_UNINITED) {
        _last_error = WIFI_ERROR_STATUS_INVALID;
        return IPAddress(0, 0, 0, 0);
    }

    cy_wcm_ip_address_t ip_address;
    cy_rslt_t ret = cy_wcm_get_ip_addr(_mode, &ip_address);
    wcm_config_assert_raise(ret, IPAddress(0, 0, 0, 0));

    IPAddress ip(ip_address.ip.v4);
    return ip;
}

IPAddress WiFiClass::subnetMask() {
    if (_status == WIFI_STATUS_UNINITED) {
        _last_error = WIFI_ERROR_STATUS_INVALID;
        return IPAddress(0, 0, 0, 0);
    }

    cy_wcm_ip_address_t subnet_mask;
    cy_rslt_t ret = cy_wcm_get_ip_netmask(_mode, &subnet_mask);
    wcm_config_assert_raise(ret, IPAddress(0, 0, 0, 0));

    return IPAddress(subnet_mask.ip.v4);
}

IPAddress WiFiClass::gatewayIP() {
    if (_status == WIFI_STATUS_UNINITED) {
        _last_error = WIFI_ERROR_STATUS_INVALID;
        return IPAddress(0, 0, 0, 0);
    }

    cy_wcm_ip_address_t gateway_ip;
    cy_rslt_t ret = cy_wcm_get_gateway_ip_address(_mode, &gateway_ip);
    wcm_config_assert_raise(ret, IPAddress(0, 0, 0, 0));

    IPAddress ip(gateway_ip.ip.v4);
    return ip;
};

IPAddress WiFiClass::dnsIP(int n) {
    const ip_addr_t *dns = dns_getserver(n);
    return IPAddress(dns->addr);
}

const char * WiFiClass::SSID() {
    switch (_mode)
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

uint8_t * WiFiClass::BSSID(uint8_t *bssid) {
    memset(bssid, 0, CY_WCM_MAC_ADDR_LEN);
    switch (_mode)
    {
        case CY_WCM_INTERFACE_TYPE_STA: {
            cy_rslt_t ret = cy_wcm_get_associated_ap_info(&ap_info);
            wcm_config_assert_raise(ret, bssid);

            memcpy(bssid, ap_info.BSSID, CY_WCM_MAC_ADDR_LEN);
            return bssid;
        }
        case CY_WCM_INTERFACE_TYPE_AP:
            return macAddress(bssid);
        default:
            /* The instance has not yet called begin() or beginAP().
            Return nullptr. */
            _last_error = WIFI_ERROR_STA_AP_MODE_INCOMPATIBLE;
            memset(bssid, 0, CY_WCM_MAC_ADDR_LEN);
            return bssid;
    }
}

int32_t WiFiClass::RSSI() {
    switch (_mode)
    {
        case CY_WCM_INTERFACE_TYPE_STA: {
            cy_rslt_t ret = cy_wcm_get_associated_ap_info(&ap_info);
            wcm_config_assert_raise(ret, INT32_MIN);

            return (int32_t)(ap_info.signal_strength);
        }
        case CY_WCM_INTERFACE_TYPE_AP:
            /* This function is not applicable. The return is not meaningful.
               Return the lowest possible value, the closest to -inf dBm (no power).
            */
            _last_error = WIFI_ERROR_STA_AP_MODE_INCOMPATIBLE;
            return INT32_MIN;
        default:
            /* The instance has not yet called begin() or beginAP().
            Return the lowest possible value, the closest to -inf dBm (no power). */
            _last_error = WIFI_ERROR_STATUS_INVALID;
            return INT32_MIN;
    }
}

uint8_t WiFiClass::encryptionType() {
    switch (_mode)
    {
        case CY_WCM_INTERFACE_TYPE_STA: {
            cy_rslt_t ret = cy_wcm_get_associated_ap_info(&ap_info);
            wcm_config_assert_raise(ret, AUTH_MODE_INVALID);

            return convertEncryptType(ap_info.security);
        }
        case CY_WCM_INTERFACE_TYPE_AP:
            return convertEncryptType(ap_conf.ap_credentials.security);
        default:
            /* The instance has not yet called begin() or beginAP().*/
            _last_error = WIFI_ERROR_STA_AP_MODE_INCOMPATIBLE;
            return AUTH_MODE_INVALID;
    }
}

int8_t WiFiClass::scanNetworks() {
    /* This function can be called before setting
    the instance as access point (begin()) or station
    (beginAP()). This is the unitialization status.
    Therefore we have to initialize the WCM. */
    bool uninited = (_status == WIFI_STATUS_UNINITED);
    if (uninited) {
        /* Set mode as Access Point + Station.
           It does not matter. */
        _last_error = wcm_init(CY_WCM_INTERFACE_TYPE_AP_STA);
        wifi_assert_raise_ret(_last_error, 0);
    }

    cy_rslt_t ret = cy_wcm_start_scan(WiFiClass::wcm_scan_cb, (void *)&scan_results, NULL);
    wcm_assert_raise_ret(ret, WIFI_ERROR_SCAN_FAILED, 0);

    constexpr uint16_t timeout = 1000; /* 1000 times by 10 ms delay = 10 seconds */
    uint16_t timer = 0;
    while (scan_results.status == CY_WCM_SCAN_INCOMPLETE && timer < timeout) {
        delay(10);
        timer++;
    }

    /* Deinitialize if it was not already assigned as access
    point or station. */
    if (uninited) {
        end();
    }

    return scan_results.result_count;
}

const char * WiFiClass::SSID(uint8_t networkItem) {
    if (networkItem >= scan_results.result_count) {
        return "";
    }

    return (const char *)scan_results.results[networkItem].SSID;
}

uint8_t * WiFiClass::BSSID(uint8_t networkItem, uint8_t *bssid) {
    if (networkItem >= scan_results.result_count) {
        memset(bssid, 0, CY_WCM_MAC_ADDR_LEN);
        return bssid;
    }

    memcpy(bssid, (uint8_t *)scan_results.results[networkItem].BSSID, CY_WCM_MAC_ADDR_LEN);
    return bssid;
}

int32_t WiFiClass::RSSI(uint8_t networkItem) {
    if (networkItem >= scan_results.result_count) {
        return INT32_MIN;
    }

    return (int32_t)scan_results.results[networkItem].signal_strength;
}

uint8_t WiFiClass::encryptionType(uint8_t networkItem) {
    if (networkItem >= scan_results.result_count) {
        return AUTH_MODE_INVALID;
    }

    return convertEncryptType(scan_results.results[networkItem].security);
}

uint8_t WiFiClass::channel(uint8_t networkItem) {
    if (networkItem >= scan_results.result_count) {
        return 0;
    }

    return scan_results.results[networkItem].channel;
}

uint8_t WiFiClass::status() {
    return _status;
}

int WiFiClass::hostByName(const char *aHostname, IPAddress& ip) {
    return Socket::hostByName(aHostname, ip);
}

int WiFiClass::ping(const char *hostname, uint8_t ttl) {
    IPAddress ip;

    if (!hostByName(hostname, ip)) {
        return WIFI_ERROR_PING_FAILED;
    }

    return ping(ip, ttl);
}

int WiFiClass::ping(const String &hostname, uint8_t ttl) {
    return ping(hostname.c_str(), ttl);
}

int WiFiClass::ping(IPAddress host, uint8_t ttl) {
    (void)ttl; /* Unused parameters. Not available in wcm ping() API.
                  Preserved for compatibility with other cores. */

    if (_status == WIFI_STATUS_UNINITED) {
        _last_error = WIFI_ERROR_STATUS_INVALID;
        return WIFI_ERROR_PING_FAILED;
    }

    const uint32_t timeout_ms = 10000;
    uint32_t elapsed_ms = 0; /* Unused */
    uint32_t ip_u32 = (host[3] << 24) | (host[2] << 16) | (host[1] << 8) | host[0];
    cy_wcm_ip_address_t ip_addr = { .version = CY_WCM_IP_VER_V4, .ip = {.v4 = ip_u32} };

    cy_rslt_t ret = cy_wcm_ping(_mode, &ip_addr, timeout_ms, &elapsed_ms);
    wcm_assert_raise(ret, WIFI_ERROR_PING_FAILED);

    return WIFI_ERROR_NONE;
}

wifi_error_t WiFiClass::getLastError() {
    return _last_error;
}

uint8_t WiFiClass::reasonCode() {
    return getLastError();
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

void WiFiClass::set_params_ap(const char *ssid, const char *passphrase, uint8_t channel) {
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
}

void WiFiClass::set_ip_settings_ap() {
    IPAddress ip;
    IPAddress gateway_ip;
    IPAddress subnet_mask;

    if (user_static_ip_settings) {
        ip = IPAddress(ip_settings.ip_address.ip.v4);
        gateway_ip = IPAddress(ip_settings.gateway.ip.v4);
        subnet_mask = IPAddress(ip_settings.netmask.ip.v4);
    } else {
        ip = IPAddress(DEFAULT_AP_IP);
        gateway_ip = IPAddress(DEFAULT_AP_GATEWAY_IP);
        subnet_mask = IPAddress(DEFAULT_AP_SUBNET_MASK);
    }

    cy_wcm_set_ap_ip_setting(&(ap_conf.ip_settings), ip.toString().c_str(), subnet_mask.toString().c_str(), gateway_ip.toString().c_str(), CY_WCM_IP_VER_V4);
}

void WiFiClass::set_ip_settings_sta(cy_wcm_connect_params_t *connect_params) {
    if (user_static_ip_settings) {
        connect_params->static_ip_settings = &ip_settings;
    }
}

const char * WiFiClass::SSID_STA() {
    cy_rslt_t ret = cy_wcm_get_associated_ap_info(&ap_info);
    wcm_config_assert_raise(ret, "");

    return (const char *)(&(ap_info.SSID));
}

const char * WiFiClass::SSID_AP() {
    return (const char *)(&(ap_conf.ap_credentials.SSID));
}

void WiFiClass::setDNS(IPAddress dns_server1) {
    const ip_addr_t dns1 = {.addr = uint32_t(dns_server1)};
    dns_setserver(0, &dns1);
}

void WiFiClass::setDNS(IPAddress dns_server1, IPAddress dns_server2) {
    const ip_addr_t dns1 = {.addr = uint32_t(dns_server1)};
    const ip_addr_t dns2 = {.addr = uint32_t(dns_server2)};

    dns_setserver(0, &dns1);
    dns_setserver(1, &dns2);
}

wl_auth_mode WiFiClass::convertEncryptType(cy_wcm_security_t wcm_sec) {
    wl_auth_mode sec_type = AUTH_MODE_INVALID;

    switch (wcm_sec)
    {
        case CY_WCM_SECURITY_OPEN:
            sec_type = AUTH_MODE_OPEN_SYSTEM;
            break;
        case CY_WCM_SECURITY_WEP_PSK:
        case CY_WCM_SECURITY_WEP_SHARED:
        case CY_WCM_SECURITY_IBSS_OPEN:
            sec_type = AUTH_MODE_SHARED_KEY;
            break;
        case CY_WCM_SECURITY_WPA_AES_PSK:
        case CY_WCM_SECURITY_WPA_MIXED_PSK:
        case CY_WCM_SECURITY_WPA_TKIP_PSK:
        case CY_WCM_SECURITY_WPA_TKIP_ENT:
        case CY_WCM_SECURITY_WPA_AES_ENT:
        case CY_WCM_SECURITY_WPA_MIXED_ENT:
            sec_type = AUTH_MODE_WPA;
            break;
        case CY_WCM_SECURITY_WPA2_AES_PSK:
        case CY_WCM_SECURITY_WPA2_TKIP_PSK:
        case CY_WCM_SECURITY_WPA2_MIXED_PSK:
        case CY_WCM_SECURITY_WPA2_FBT_PSK:
        case CY_WCM_SECURITY_WPA2_TKIP_ENT:
        case CY_WCM_SECURITY_WPA2_AES_ENT:
        case CY_WCM_SECURITY_WPA2_MIXED_ENT:
        case CY_WCM_SECURITY_WPA2_FBT_ENT:
            sec_type = AUTH_MODE_WPA2;
            break;
        case CY_WCM_SECURITY_WPA2_WPA_AES_PSK:
        case CY_WCM_SECURITY_WPA2_WPA_MIXED_PSK:
            sec_type = AUTH_MODE_WPA_WPA2;
            break;
        case CY_WCM_SECURITY_WPA3_SAE:
        case CY_WCM_SECURITY_WPA3_WPA2_PSK:
            sec_type = AUTH_MODE_WPA3;
            break;
        case CY_WCM_SECURITY_WPS_SECURE:
        case CY_WCM_SECURITY_UNKNOWN:
        default:
            sec_type = AUTH_MODE_INVALID;
            break;
    }
    return sec_type;
}
/* Callback function for scanNetworks() method. After each scan result, the scan callback is executed.*/
void WiFiClass::wcm_scan_cb(cy_wcm_scan_result_t *result_ptr, void *user_data, cy_wcm_scan_status_t status) {
    scan_results_t *scan_user_data = (scan_results_t *)user_data;

    /* Stop after if more results than the maximum are available */
    if (scan_user_data->result_count > CY_MAX_SCAN_RESULTS) {
        cy_wcm_stop_scan();
        scan_user_data->status = CY_WCM_SCAN_COMPLETE;
        return;
    }

    if (status == CY_WCM_SCAN_INCOMPLETE) {
        memcpy(&(scan_user_data->results[scan_user_data->result_count]), result_ptr, sizeof(cy_wcm_scan_result_t));
        (scan_user_data->result_count)++;
    }

    scan_user_data->status = status;
}

WiFiClass & WiFi = WiFiClass::get_instance();
