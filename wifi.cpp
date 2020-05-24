#include <WiFi.h>
#include "wifi.hh"

struct wifi_access_point wifi_aps[WIFI_MAX_DISCOVERED_APS];
int wifi_ap_result_next = 0;

void wifi_scan()
{
    wifi_ap_result_next = 0;
    WiFi.scanDelete();
    // WiFi scan must be done in station mode without any active connection
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_MODE_STA);
    // Use maximum transmission power to perform an foreground active scan
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    int num_stations = WiFi.scanNetworks(false, false, false);
    // Store up to a predefined number of discovered networks
    if (num_stations < 1)
    {
        return;
    }
    if (num_stations > WIFI_MAX_DISCOVERED_APS)
    {
        num_stations = WIFI_MAX_DISCOVERED_APS;
    }
    for (; wifi_ap_result_next < num_stations; ++wifi_ap_result_next)
    {
        struct wifi_access_point ap;
        ap.ssid = WiFi.SSID(wifi_ap_result_next);
        ap.rssi = WiFi.RSSI(wifi_ap_result_next);
        ap.mac = WiFi.BSSIDstr(wifi_ap_result_next);
        ap.auth_mode = WiFi.encryptionType(wifi_ap_result_next);
        wifi_aps[wifi_ap_result_next] = ap;
    }
}

wl_status_t wifi_connect(char *ssid, char *pass, int timeout_sec)
{
    // Disconnect from previously associated AP
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_MODE_STA);
    // Use maximum transmission power
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    // Wait for connection attempt to complete
    WiFi.begin(ssid, pass);
    wl_status_t ret;
    for (int ms = 0; ms < timeout_sec * 1000; ms += 200)
    {
        ret = WiFi.status();
        if (ret == WL_CONNECTED)
        {
            return ret;
        }
        delay(200);
    }
    // Disconnect and abort after timeout
    WiFi.disconnect(true, true);
    return ret;
}

int wifi_get_random_open_ap(struct wifi_access_point *out)
{
    // Go through scan results and see how many open access points there are
    int open_ap_count = 0;
    for (int i = 0; i < wifi_ap_result_next; i++)
    {
        if (wifi_aps[i].auth_mode == WIFI_AUTH_OPEN)
        {
            ++open_ap_count;
        }
    }
    if (open_ap_count == 0)
    {
        return -1;
    }
    // Pick a random open AP
    int chosen_ap = random(open_ap_count);
    for (int i = 0; i < wifi_ap_result_next; i++)
    {
        if (wifi_aps[i].auth_mode == WIFI_AUTH_OPEN)
        {
            if (chosen_ap == 0)
            {
                *out = wifi_aps[i];
                return i;
            }
            --chosen_ap;
        }
    }
}