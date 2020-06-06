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
    //oled_disp_open_aps();
    // gps_read(5);
    // oled_disp_all_aps();
    // gps_read(5);
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
        struct gps_data data = gps_get_data();
        int code1 = twofa_generate_code(LAITOS_PASS, data.unix_time / 30);
        Serial.printf("twofa code: %d\n", code1);
        int code2 = twofa_generate_code(LAITOS_PASS_REVERSE, data.unix_time / 30);
        Serial.printf("twofa code: %d\n", code2);

        char cmd[254] = {};
        Serial.printf("ssid: %s\n", WiFi.SSID().c_str());
        snprintf(cmd, sizeof(cmd), "%d%d.0m%s%c", code1, code2, WiFi.SSID().c_str(), SUBJ_REPORT_FIELD_SEP);
        Serial.printf("cmd: %s\n", cmd);

        char *app_cmd = get_app_cmd_with_dtmf(cmd);
        Serial.printf("app_cmd: %s\n", app_cmd);

        char *dns_q = get_laitos_dns_query(app_cmd, LAITOS_DOMAIN_NAME);
        Serial.printf("query: %s\n", dns_q);

        char dns_srv[32] = {};
        strcpy(dns_srv, WiFi.dnsIP().toString().c_str());
        Serial.printf("dns_srv: %s\n", dns_srv);

        char **result = resolve_txt(dns_srv, 53, dns_q, 3);
        if (result == NULL)
        {
            Serial.println("resolution failure");
        }
        else
        {
            Serial.println("resolution succeeded");
            for (char **it = result; *it != NULL; ++*it)
            {
                Serial.printf("resolution result: %s\n", *it);
            }
        }
        free(dns_q);
        free(app_cmd);
    }
}
