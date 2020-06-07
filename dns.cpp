#include <Arduino.h>
#include <lwip/inet.h>
#include <lwip/sockets.h>
#include "dns.hh"

char **dns_resolve_txt(char *resolver_ip, int port, char *name, int timeout_sec)
{
    struct timeval io_timeout = {.tv_sec = timeout_sec, .tv_usec = 0};
    ssize_t recv_size;
    int recv_entry_i = 0;
    /*
   * According to "What is the real maximum length of a DNS name?"
   * https://devblogs.microsoft.com/oldnewthing/20120412-00/?p=7873
   * The maximum length of a DNS name is 253 characters long.
   * The function validates the length of the entire DNS name, however it does
   * not validate individual label length.
   */
    if (resolver_ip == NULL || port < 1 || name == NULL || strlen(name) < 2 || strlen(name) > 253 || timeout_sec < 1)
    {
        return NULL;
    }
    Serial.printf("%s: resolving %s using resolver %s:%d\n", __func__, name, resolver_ip, port);
    char **ret = NULL;
    // The DNS query uses a randomly generated transaction ID
    char tr_id[] = {rand() % 128, rand() % 128};
    char query_headers[] = {// transaction ID
                            tr_id[0], tr_id[1],
                            // standard recursive query
                            0x1, 0x20,
                            // 1 question
                            0x0, 0x1,
                            // 0 answers, 0 authority RRs, 0 additional RRs.
                            0, 0, 0, 0, 0, 0};
    size_t query_len = sizeof(query_headers);
    char *query_packet = (char *)malloc(query_len);
    memcpy(query_packet, query_headers, query_len);
    // Break down the query name into labels
    char name_copy[254] = {};
    strncpy(name_copy, name, sizeof(name_copy));
    for (char *label = strtok(name_copy, "."); label != NULL; label = strtok(NULL, "."))
    {
        size_t label_len = strlen(label);
        if (label_len > 0)
        {
            // Size of the label goes into the query packet
            query_packet = (char *)realloc(query_packet, query_len + 1 + label_len);
            query_packet[query_len] = label_len;
            // Label content goes into the query packet
            memcpy(&query_packet[query_len + 1], label, label_len);
            query_len += 1 + label_len;
        }
    }
    // There is an extra 0x0 after all labels
    query_packet = (char *)realloc(query_packet, query_len + 1);
    query_packet[query_len] = 0;
    ++query_len;
    // Append query type and class
    static const char match_type_class[] = {DNS_TXT_TYPE_CLASS};
    query_packet = (char *)realloc(query_packet, query_len + 4);
    memcpy(&query_packet[query_len], match_type_class, 4);
    query_len += 4;
    // Prepare a socket for UDP client
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        Serial.printf("%s: failed to create UDP socket\n", __func__);
        goto clean_up;
    }
    struct sockaddr_in resolver_addr;
    memset(&resolver_addr, 0, sizeof(resolver_addr));
    resolver_addr.sin_family = AF_INET;
    resolver_addr.sin_addr.s_addr = inet_addr(resolver_ip);
    resolver_addr.sin_port = htons(port);

    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &io_timeout, sizeof(io_timeout)) < 0)
    {
        Serial.printf("%s: failed to set socket write timeout\n", __func__);
        goto clean_up;
    }
    recv_size =
        sendto(sock, query_packet, query_len, 0,
               (const struct sockaddr *)&resolver_addr, sizeof(resolver_addr));
    if (recv_size < 0)
    {
        Serial.printf("%s: failed to send TXT query packet\n", __func__);
        goto clean_up;
    }
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &io_timeout, sizeof(io_timeout)) < 0)
    {
        Serial.printf("%s: failed to set socket read timeout\n", __func__);
        goto clean_up;
    }
    // Accept up to 1KB of response even though it is typically 512 bytes or less
    char resp_packet[1024];
    recv_size = recvfrom(sock, (char *)resp_packet, sizeof(resp_packet), 0, NULL, NULL);
    if (recv_size < 2)
    {
        Serial.printf("%s: failed to receive TXT query response\n", __func__);
        goto clean_up;
    }
    // Validate transaction ID of the response
    if (resp_packet[0] != tr_id[0] || resp_packet[1] != tr_id[1])
    {
        goto clean_up;
    }
    ret = (char **)calloc(1, sizeof(char *));
    for (ssize_t i = 0; i < recv_size - 2; ++i)
    {
        // Look for c0 0c - pointer to the original query and marks the beginning of
        // the TXT responses
        if (resp_packet[i] != (char)0xc0 || resp_packet[i + 1] != (char)0x0c)
        {
            continue;
        }
        i += 2;
        // Look for type TXT and class Internet at the beginning of TXT responses
        if (i > (long)(recv_size - sizeof(match_type_class)) || memcmp(match_type_class, &resp_packet[i], sizeof(match_type_class)) != 0)
        {
            continue;
        }
        i += sizeof(match_type_class);
        // Skip TTL (4 bytes) and data length (2 bytes) to find the TXT length byte
        if (i > recv_size - 7)
        {
            break;
        }
        char txt_len = resp_packet[i + 4 + 2];
        i += 7;
        // Read TXT entry
        if (i > recv_size - txt_len)
        {
            break; // malformed entry size
        }
        char *txt = (char *)calloc(txt_len + 1, 1); // with a trailing NULL byte
        memcpy(txt, &resp_packet[i], txt_len);
        ret[recv_entry_i++] = txt;
        ret = (char **)realloc(ret, (recv_entry_i + 1) * sizeof(char *));
        ret[recv_entry_i] = NULL;
    }
clean_up:
    if (sock != -1)
    {
        close(sock);
    }
    free(query_packet);
    return ret;
}