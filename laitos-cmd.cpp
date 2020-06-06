#include <stdlib.h>
#include <string.h>
#include "laitos-cmd.hh"

/*
 * get_app_cmd_with_dtmf translates symbols and digits from the input laitos app
 * command into DTMF sequences and returns the entire command after translation.
 * Caller must free the return value when it is no longer used.
 */
char *get_app_cmd_with_dtmf(char *cmd)
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
            // Digit becomes digit + "0" according to laitos DTMF decoder
            out[o++] = *c;
            out[o++] = '0';
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

/*
 * get_laitos_dns_query returns a complete DNS query name consisting of prefix,
 * input app command split into labels, and the domain name as suffix.
 * Caller must free the return value after it is no longer used.
 */
char *get_laitos_dns_query(char *app_cmd, char *domain_name)
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
        // Put the label into query along with a full-stop
        strncpy(&ret[ret_i], &app_cmd[label_begin], label_end - label_begin);
        ret_i += label_end - label_begin;
        ret[ret_i++] = '.';
    }
    // Put the domain name into the query
    strcpy(&ret[ret_i], domain_name);
    return ret;
}