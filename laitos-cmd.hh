#ifndef ESP_WIFI_GPS_TRACKER_LAITOS_CMD_H
#define ESP_WIFI_GPS_TRACKER_LAITOS_CMD_H

#define SUBJ_REPORT_FIELD_SEP '\x1f'
#define SUBJ_REPORT_LINE_SEP '\x1e'

/*
 * get_app_cmd_with_dtmf translates symbols and digits from the input laitos app
 * command into DTMF sequences and returns the entire command after translation.
 * Caller must free the return value when it is no longer used.
 */
char *get_app_cmd_with_dtmf(char *cmd);

/*
 * get_laitos_dns_query returns a complete DNS query name consisting of prefix,
 * input app command split into labels, and the domain name as suffix.
 * Caller must free the return value after it is no longer used.
 */
char *get_laitos_dns_query(char *app_cmd, char *domain_name);

#endif