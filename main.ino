// esp32-wifi-gps-tracker is a GPS tracker program that uses nearby open WiFi hotspots to transmit tracked object's location.

#include <Arduino.h>
#include <time.h>
#include "personalisation.hh"
#include "gps.hh"
#include "oled.hh"
#include "wifi.hh"
#include "twofa-code-gen.hh"

int yisleap(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int get_yday(int mon, int day, int year)
{
    static const int days[2][13] = {
        {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
        {0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}};
    int leap = yisleap(year);

    return days[leap][mon] + day;
}

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
    gps_read(); // takes a second
    delay(4000);
    oled_disp_all_aps();
    gps_read(); // takes a second
    delay(4000);
    // Connect to an open AP
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
        wifi_connect((char *)open_ap.ssid.c_str(), NULL, 15);
    }
    // Display AP connection status
    oled_disp_wifi_ap_status();
    gps_read(); // takes a second
    delay(4000);
    // Display GPS status
    oled_disp_gps_data();
    gps_read(); // takes a second
    delay(4000);

    struct gps_data data = gps_get_data();
    int n = generateCode("abcdefg", 1590246816 / 30);
    Serial.printf("twofa code: %d\n", n);
}
