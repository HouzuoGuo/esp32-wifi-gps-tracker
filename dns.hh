#ifndef ESP_WIFI_GPS_TRACKER_DNS_HH
#define ESP_WIFI_GPS_TRACKER_DNS_HH

// DNS_TXT_TYPE_CLASS are four byte of a DNS query indicating the query type is
// TXT (0, 16) and class is Internet (0, 1).
#define DNS_TXT_TYPE_CLASS 0, 16, 0, 1

/*
 * dns_resolve_txt sends the DNS recursive resolver a TXT query and returns TXT
 * response entries in a string array, terminated with NULL.
 * Caller must free the string array elements and the array itself.
 * In case of invalid parameter input or IO error, the function returns NULL.
 */
char **dns_resolve_txt(char *resolver_ip, int port, char *name, int timeout_sec);

#endif // include guard