#ifndef ESP_WIFI_GPS_TRACKER_WIFI_HH
#define ESP_WIFI_GPS_TRACKER_WIFI_HH

#include <WiFiType.h>
#include <esp_wifi_types.h>

// WIFI_MAX_DISCOVERED_APS is the maximum number of access points to be discovered during a scan.
#define WIFI_MAX_DISCOVERED_APS 100

// wifi_scan_result is a detailed record of a recently discovered WiFi access point.
struct wifi_access_point
{
    int32_t rssi;
    String ssid;
    String mac;
    wifi_auth_mode_t auth_mode;
};

// wifi_aps is an array of discovered WiFi stations after the latest scanning round.
extern struct wifi_access_point wifi_aps[WIFI_MAX_DISCOVERED_APS];
// wifi_scan_results_next_index is the index number of array which the next discovered station will occupy. In between each scan it is reset to 0;
extern int wifi_ap_result_next;

// wifi_scan conducts a fresh round of discovery of nearby WiFi networks. It may take several seconds to return.
void wifi_scan();

/*
wifi_connect connects to a WPA-protected AP if the password is provided, or an open AP if the password is NULL.
A connection attempt may take several seconds to complete. The connection attempt aborts after specified timeout seconds elapses.
The function returns the last connection status (WL_CONNECTED for success) or error status (e.g. WL_CONNECT_FAILED).
*/
wl_status_t wifi_connect(char *ssid, char *pass, int timeout_sec);

/*
wifi_get_random_open_ap returns a randomly chosen open access point from recent scan result.
If an open access point exists, the details will be assigned to the output pointer and the function will return the index
of the access point amid the scan result.
If no open access point exists, the function will return -1.
*/
int wifi_get_random_open_ap(struct wifi_access_point *out);

#endif // include