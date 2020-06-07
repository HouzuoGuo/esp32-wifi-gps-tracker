#ifndef ESP_WIFI_GPS_TRACKER_LAITOS_CMD_H
#define ESP_WIFI_GPS_TRACKER_LAITOS_CMD_H

#define SUBJ_REPORT_FIELD_SEP '\x1f'
#define SUBJ_REPORT_LINE_SEP '\x1e'

/*
 * laitos_encode_dtmf_sequences translates symbols and digits from the input laitos
 * app command into DTMF sequences that laitos can understand and decode. The function
 * returns an entire app command after tranlation.
 * Caller must free the return value when it is no longer used.
 */
char *laitos_encode_dtmf_sequences(char *cmd);

/*
 * laitos_get_dns_query_name returns a complete DNS query name consisting of laitos
 * app command prefix, app command input (after encoding DTMF sequences) split into labels,
 * and domain name suffix.
 * Caller must free the return value after it is no longer used.
 */
char *laitos_get_dns_query_name(char *app_cmd, char *domain_name);

/**
 * laitos_get_tracking_cmd returns a laitos app command constructed from current WiFi name,
 * GPS info, and nearby WiFi names, encapsulated in a laitos subject report app command.
 * Caller should pass the return value to laitos_encode_dtmf_sequences().
 * Caller must free the return value when it is no longer used.
 */
char *laitos_get_tracking_cmd(const char *current_wifi_name, struct gps_data gps_data, char **nearby_aps);

#endif