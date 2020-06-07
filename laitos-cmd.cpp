#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gps.hh"
#include "twofa-code-gen.hh"
#include "laitos-cmd.hh"
#include "personalisation.hh"

char *laitos_encode_dtmf_sequences(char *cmd)
{
    // A symbol gets translated into at most 3 individual characters
    size_t max_len = strlen(cmd) * 3;
    char *out = (char *)calloc(max_len, 1);
    size_t o = 0;
    for (char *c = cmd; *c != '\0'; c++)
    {
        if (o >= max_len - 3)
        {
            break;
        }
        if (*c >= '0' && *c <= '9')
        {
            // Digit becomes "1" + digit + "0" according to laitos DTMF decoder
            out[o++] = '1';
            out[o++] = *c;
            if (*c != '0')
            {
                // For the input digit 0 there is not going to be an extra 0 suffix
                out[o++] = '0';
            }
        }
        else if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z'))
        {
            // Latin letters remain as-is
            out[o++] = *c;
        }
        else
        {
            // Symbols become DTMF sequence + "0" according to laitos DTMF decoder
            switch (*c)
            {
            case ' ':
                break;
            case '\x1f':
                out[o++] = '1';
                out[o++] = '4';
                out[o++] = '6';
                break;
            case '\x1e':
                out[o++] = '1';
                out[o++] = '4';
                out[o++] = '7';
                break;
            case '!':
                out[o++] = '1';
                out[o++] = '1';
                out[o++] = '1';
                break;
            case '@':
                out[o++] = '1';
                out[o++] = '1';
                out[o++] = '2';
                break;
            case '#':
                out[o++] = '1';
                out[o++] = '1';
                out[o++] = '3';
                break;
            case '$':
                out[o++] = '1';
                out[o++] = '1';
                out[o++] = '4';
                break;
            case '%':
                out[o++] = '1';
                out[o++] = '1';
                out[o++] = '5';
                break;
            case '^':
                out[o++] = '1';
                out[o++] = '1';
                out[o++] = '6';
                break;
            case '&':
                out[o++] = '1';
                out[o++] = '1';
                out[o++] = '7';
                break;
            case '*':
                out[o++] = '1';
                out[o++] = '1';
                out[o++] = '8';
                break;
            case '(':
                out[o++] = '1';
                out[o++] = '1';
                out[o++] = '9';
                break;
            case '`':
                out[o++] = '1';
                out[o++] = '2';
                out[o++] = '1';
                break;
            case '~':
                out[o++] = '1';
                out[o++] = '2';
                out[o++] = '2';
                break;
            case ')':
                out[o++] = '1';
                out[o++] = '2';
                out[o++] = '3';
                break;
            case '-':
                out[o++] = '1';
                out[o++] = '2';
                out[o++] = '4';
                break;
            case '_':
                out[o++] = '1';
                out[o++] = '2';
                out[o++] = '5';
                break;
            case '=':
                out[o++] = '1';
                out[o++] = '2';
                out[o++] = '6';
                break;
            case '+':
                out[o++] = '1';
                out[o++] = '2';
                out[o++] = '7';
                break;
            case '[':
                out[o++] = '1';
                out[o++] = '2';
                out[o++] = '8';
                break;
            case '{':
                out[o++] = '1';
                out[o++] = '2';
                out[o++] = '9';
                break;
            case ']':
                out[o++] = '1';
                out[o++] = '3';
                out[o++] = '1';
                break;
            case '}':
                out[o++] = '1';
                out[o++] = '3';
                out[o++] = '2';
                break;
            case '\\':
                out[o++] = '1';
                out[o++] = '3';
                out[o++] = '3';
                break;
            case '|':
                out[o++] = '1';
                out[o++] = '3';
                out[o++] = '4';
                break;
            case ';':
                out[o++] = '1';
                out[o++] = '3';
                out[o++] = '5';
                break;
            case ':':
                out[o++] = '1';
                out[o++] = '3';
                out[o++] = '6';
                break;
            case '\'':
                out[o++] = '1';
                out[o++] = '3';
                out[o++] = '7';
                break;
            case '"':
                out[o++] = '1';
                out[o++] = '3';
                out[o++] = '8';
                break;
            case ',':
                out[o++] = '1';
                out[o++] = '3';
                out[o++] = '9';
                break;
            case '<':
                out[o++] = '1';
                out[o++] = '4';
                out[o++] = '1';
                break;
            case '.':
                out[o++] = '1';
                out[o++] = '4';
                out[o++] = '2';
                break;
            case '>':
                out[o++] = '1';
                out[o++] = '4';
                out[o++] = '3';
                break;
            case '/':
                out[o++] = '1';
                out[o++] = '4';
                out[o++] = '4';
                break;
            case '?':
                out[o++] = '1';
                out[o++] = '4';
                out[o++] = '5';
                break;
            default:
                // Unknown symbol becomes a whitespace represented by a single character
                // '0'
                break;
            }
            // Character '0' terminates a DTMF sequence
            out[o++] = '0';
        }
    }
    return out;
}

char *laitos_get_dns_query_name(char *app_cmd, char *domain_name)
{
    // Be on the safe size and avoid filling up all 253 characters of a DNS name
    size_t max_q_len = 246 - 2 - strlen(domain_name);
    if (max_q_len > strlen(app_cmd))
    {
        max_q_len = strlen(app_cmd);
    }
    char *ret = (char *)calloc(260, 1);
    int ret_i = 0;
    // laitos recognises the prefix underscore for an app command in a DNS query
    ret[ret_i++] = '_';
    ret[ret_i++] = '.';
    // Break apart the app command into DNS labels no more than 60 characters each
    for (size_t i_label = 0; i_label < (max_q_len / 60) + 1; ++i_label)
    {
        size_t label_begin = i_label * 60;
        size_t label_end = (i_label + 1) * 60;
        if (label_end > max_q_len)
        {
            label_end = max_q_len;
        }
        if (label_end - label_begin == 0)
        {
            continue;
        }
        // Put the label into query along with a full-stop
        strncpy(&ret[ret_i], &app_cmd[label_begin], label_end - label_begin);
        ret_i += label_end - label_begin;
        ret[ret_i++] = '.';
    }
    // Put the domain name into the query
    strcpy(&ret[ret_i], domain_name);
    return ret;
}

char *laitos_get_tracking_cmd(const char *current_wifi_name, struct gps_data gps_data, char **nearby_aps)
{
    /*
    laitos uses the following convention for the fields, separated by SUBJ_REPORT_FIELD_SEP in between the fields:
    - Host / object name
    - Command request command content
    - Command response command content
    - Command response result
    - Platform info
    - Comment text
    - IP address (what the subject considers to be its IP address, it is not the one observed by laitos server)
    - Command response timestamp
    - Command response run-duration
    */
    int code1 = twofa_generate_code(LAITOS_PASS, gps_data.unix_time / 30);
    int code2 = twofa_generate_code(LAITOS_PASS_REVERSE, gps_data.unix_time / 30);
    // The WiFi names are artifically capped at 20 characters each
    char *ret = (char *)calloc(300, 1);
    snprintf(ret, 300 - 5,
             "%06d%06d"
             ".0m%.20s%c"
             "%c"
             "%c"
             "%c"
             "esp32%c"
             "%dsats A%dm %f %f near ",
             code1, code2,
             current_wifi_name, SUBJ_REPORT_FIELD_SEP,
             SUBJ_REPORT_FIELD_SEP,
             SUBJ_REPORT_FIELD_SEP,
             SUBJ_REPORT_FIELD_SEP,
             SUBJ_REPORT_FIELD_SEP,
             gps_data.satellites, (int)gps_data.altitude_metres, gps_data.latitude, gps_data.longitude, SUBJ_REPORT_FIELD_SEP);

    for (int i = 0; i < 4 && nearby_aps[i] != NULL; i++)
    {
        char shortened_wifi_name[20] = {};
        snprintf(shortened_wifi_name, sizeof(shortened_wifi_name) - 1, nearby_aps[i]);
        strcat(ret, shortened_wifi_name);
        strcat(ret, ",");
    }
    return ret;
}