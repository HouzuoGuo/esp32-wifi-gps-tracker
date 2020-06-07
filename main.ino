// esp32-wifi-gps-tracker is a GPS tracker program that uses nearby open WiFi hotspots to transmit tracked object's location.

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "personalisation.hh"
#include "twofa-code-gen.hh"
#include "gps.hh"
#include "oled.hh"
#include "wifi.hh"
#include "dns.hh"
#include "laitos-cmd.hh"

void setup()
{
    // Use floating analogue pins to seed the RNG. DNS query uses a randomly generated transaction ID.
    unsigned long rand_seed = 0;
    for (int i = 0; i < 32; i++)
    {
        rand_seed += analogRead(i);
    }
    randomSeed(rand_seed);
    // Initialise everything else only after having read from pins
    Serial.begin(115200);
    Serial.printf("%s: random seed is %d\n", __func__, rand_seed);
    oled_setup();
    gps_setup();
    Serial.printf("%s: setup completed\n", __func__);
}

void loop()
{
    // Scan for open access points
    oled.clear();
    oled.drawStringMaxWidth(0, 1 * OLED_FONT_HEIGHT_PX, 200, "Scanning APs");
    oled.display();
    wifi_scan();
    // Display open APs and all APs
    oled_disp_open_aps();
    gps_read(5);
    oled_disp_all_aps();
    gps_read(5);
    // Connect to an open AP or the fallback home AP
    oled.clear();
    struct wifi_access_point open_ap;
    int i = wifi_get_random_open_ap(&open_ap);
    if (i == -1)
    {
        // Connect to the default access point at home for a developer to tweak the program
        oled.drawStringMaxWidth(0, 1 * OLED_FONT_HEIGHT_PX, 200, "Connecting to " HOME_WIFI_NAME);
        oled.display();
        wifi_connect(HOME_WIFI_NAME, HOME_WIFI_PASS, 15);
    }
    else
    {
        oled.drawStringMaxWidth(0, 1 * OLED_FONT_HEIGHT_PX, 200, "Connecting to open AP");
        oled.drawStringMaxWidth(0, 2 * OLED_FONT_HEIGHT_PX, 200, open_ap.ssid);
        oled.drawStringMaxWidth(0, 3 * OLED_FONT_HEIGHT_PX, 200, open_ap.mac);
        oled.drawStringMaxWidth(0, 4 * OLED_FONT_HEIGHT_PX, 200, String(open_ap.rssi));
        oled.display();
        wifi_connect(open_ap.ssid, NULL, 15);
    }
    // Display AP connection status
    oled_disp_wifi_ap_status();
    gps_read(5);
    // Display GPS status
    oled_disp_gps_data();
    gps_read(5);
    // Send a message processor command to laitos server
    if (WiFi.isConnected())
    {
        struct gps_data gps_data = gps_get_data();
        /*
        Collect a short list of nearby WiFi APs to send along with the tracker report.
        Not all of them are going to fit into a DNS query, that is OK, the functions will
        truncate the report command to the maximum accepted length.
        */
        char *nearby_aps[5] = {};
        int nearby_aps_i = 0;
        for (int i = 0; i < wifi_ap_result_next && i < 5; ++i)
        {
            if (strcmp(wifi_aps[i].ssid, WiFi.SSID().c_str()) == 0)
            {
                continue; // skip the currently connected AP
            }
            nearby_aps[nearby_aps_i] = wifi_aps[i].ssid;
            Serial.printf("%s: nearby wifi %d: %s\n", __func__, i, nearby_aps[nearby_aps_i]);
            ++nearby_aps_i;
        }

        char *tracking_cmd = laitos_get_tracking_cmd(WiFi.SSID().c_str(), gps_data, nearby_aps);
        Serial.printf("%s: laitos tracking report cmd: %s\n", __func__, tracking_cmd);

        char *encoded_cmd = laitos_encode_dtmf_sequences(tracking_cmd);
        Serial.printf("%s: laitos command after DTMF encoding: %s\n", __func__, encoded_cmd);

        char *dns_q = laitos_get_dns_query_name(encoded_cmd, LAITOS_DOMAIN_NAME);
        Serial.printf("%s: DNS query is: %s\n", __func__, dns_q);

        char dns_srv[32] = {};
        strcpy(dns_srv, WiFi.dnsIP().toString().c_str());
        Serial.printf("%s: DNS server is: %s\n", __func__, dns_srv);

        // Let user know we're about to contact laitos DNS server
        oled.clear();
        oled.drawStringMaxWidth(0, 1 * OLED_FONT_HEIGHT_PX, 200, String("Sending ") + strlen(dns_q) + " bytes of info");
        oled.drawStringMaxWidth(0, 2 * OLED_FONT_HEIGHT_PX, 200, String("To: ") + LAITOS_DOMAIN_NAME);
        oled.drawStringMaxWidth(0, 3 * OLED_FONT_HEIGHT_PX, 200, String("Via WiFi: ") + WiFi.SSID());
        oled.drawStringMaxWidth(0, 4 * OLED_FONT_HEIGHT_PX, 200, String("DNS server: ") + WiFi.dnsIP().toString());

        char **result = dns_resolve_txt(dns_srv, 53, dns_q, 3);
        if (result == NULL)
        {
            Serial.printf("%s: failed to send laitos command over DNS\n", __func__);
            oled.drawStringMaxWidth(0, 5 * OLED_FONT_HEIGHT_PX, 200, "IO failure :(");
        }
        else
        {
            Serial.printf("%s: successfully sent laitos command over DNS\n", __func__);
            // Throw away the TXT record results for now
            bool displayed = false;
            for (char **it = result; *it != NULL; ++it)
            {
                if (!displayed)
                {
                    oled.drawStringMaxWidth(0, 5 * OLED_FONT_HEIGHT_PX, 200, String("OK :) ") + *it);
                    displayed = true;
                }
                Serial.printf("%s: TXT response: %s\n", __func__, *it);
                free(*it);
            }
            free(result);
        }
        oled.display();
        free(dns_q);
        free(encoded_cmd);
        free(tracking_cmd);
        gps_read(5);
    }
}
