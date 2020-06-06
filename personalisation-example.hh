#ifndef ESP_WIFI_GPS_TRACKER_PERSONALISATION_H
#define ESP_WIFI_GPS_TRACKER_PERSONALISATION_H

/*
HOME_WIFI_NAME defines a WiFi network name in close proximity to the developer's location.
In the absence of an open WiFi network, the program will connect to this home network instead and
transmits the tracking information. This helps with development activities.
*/
#define HOME_WIFI_NAME "my-home-wifi-name"
#define HOME_WIFI_PASS "my-home-wifi-password"

// LAITOS_PASS defines the password PIN string used to enable laitos program command access.
#define LAITOS_PASS "my-laitos-command-password"

// LAITOS_PASS defines the reversed password PIN string used to enable laitos program command access.
#define LAITOS_PASS_REVERSE "drowssap-dnammoc-sotial-ym"

// LAITOS_DOMAIN_NAME is the domain name running laitos DNS server that collects WiFi reports over DNS requests.
#define LAITOS_DOMAIN_NAME ""

#endif // include guard