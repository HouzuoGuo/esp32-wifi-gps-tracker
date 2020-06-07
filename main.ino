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
        // Collect list of nearly WiFi APs
        char *nearby_aps[5] = {};
        for (int i = 0; i < wifi_ap_result_next && i < 5; ++i)
        {
            nearby_aps[i] = wifi_aps[i].ssid;
            Serial.printf("nearby wifi %d: %s\n", i, nearby_aps[i]);
        }

        char *tracking_cmd = laitos_get_tracking_cmd(WiFi.SSID().c_str(), gps_data, nearby_aps);
        Serial.printf("tracking cmd: %s\n", tracking_cmd);

        char *encoded_cmd = laitos_encode_dtmf_sequences(tracking_cmd);
        Serial.printf("after encoding: %s\n", encoded_cmd);

        char *dns_q = laitos_get_dns_query_name(encoded_cmd, LAITOS_DOMAIN_NAME);
        Serial.printf("query: %s\n", dns_q);

        char dns_srv[32] = {};
        strcpy(dns_srv, WiFi.dnsIP().toString().c_str());
        Serial.printf("dns_srv: %s\n", dns_srv);

        char **result = dns_resolve_txt(dns_srv, 53, dns_q, 3);
        if (result == NULL)
        {
            Serial.println("resolution failure");
        }
        else
        {
            Serial.println("resolution succeeded");
            for (char **it = result; *it != NULL; ++it)
            {
                Serial.printf("resolution result: %s\n", *it);
                free(*it);
            }
        }
        free(result);
        free(dns_q);
        free(encoded_cmd);
        free(tracking_cmd);
    }
}
