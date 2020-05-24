// OLED libraries came installed with "ESP8266 and ESP32 Oled Driver for SSD1306 display by Daniel Eichhorn, Fabrice Weinberg Version 4.1.0".
#include <SSD1306Wire.h>
#include <OLEDDisplay.h>
#include <WiFi.h>
#include <WiFiType.h>
#include "wifi.hh"
#include "gps.hh"
#include "oled.hh"

SSD1306Wire oled(OLED_I2C_ADDR, OLED_I2C_SDA, OLED_I2C_SCL);

void oled_setup()
{
    oled.init();
    oled.clear();
    oled.flipScreenVertically();
    oled.setTextAlignment(TEXT_ALIGN_LEFT);
    oled.setFont(ArialMT_Plain_10);
    Serial.printf("%s: OLED setup completed\n", __func__);
}

void oled_disp_open_aps()
{
    oled.clear();
    // Display a heading line
    oled.drawStringMaxWidth(0, 1 * OLED_FONT_HEIGHT_PX, 200, "Open APs:");
    // Display AP information on the remaining four lines
    int line = 2;
    for (int i = 0; i < wifi_ap_result_next && line <= 5; ++i)
    {
        struct wifi_access_point ap = wifi_aps[i];
        if (ap.auth_mode == WIFI_AUTH_OPEN)
        {
            oled.drawStringMaxWidth(0, line++ * OLED_FONT_HEIGHT_PX, 200, String() + ap.rssi + " " + ap.ssid);
        }
    }
    oled.display();
    Serial.printf("%s: displaying open APs among %d APs\n", __func__, wifi_ap_result_next + 1);
}

void oled_disp_all_aps()
{
    oled.clear();
    // Display a heading line
    oled.drawStringMaxWidth(0, 1 * OLED_FONT_HEIGHT_PX, 200, "All APs:");
    // Display AP information on the remaining four lines
    int line = 2;
    for (int i = 0; i < wifi_ap_result_next && line <= 5; ++i)
    {
        struct wifi_access_point ap = wifi_aps[i];
        oled.drawStringMaxWidth(0, line++ * OLED_FONT_HEIGHT_PX, 200, String() + ap.rssi + " " + ap.ssid);
    }
    oled.display();
    Serial.printf("%s: displaying %d APs\n", __func__, wifi_ap_result_next + 1);
}

void oled_disp_wifi_ap_status()
{
    char text[OLED_MAX_LINE_LEN];
    oled.clear();
    if (WiFi.isConnected())
    {
        oled.drawStringMaxWidth(0, 1 * OLED_FONT_HEIGHT_PX, 200, String("WiFi is connected to:"));
        oled.drawStringMaxWidth(0, 2 * OLED_FONT_HEIGHT_PX, 200, String(WiFi.RSSI()) + " " + WiFi.SSID());
        oled.drawStringMaxWidth(0, 3 * OLED_FONT_HEIGHT_PX, 200, String("GW: ") + WiFi.gatewayIP().toString());
        oled.drawStringMaxWidth(0, 4 * OLED_FONT_HEIGHT_PX, 200, String("My IP: ") + WiFi.localIP().toString());
        oled.drawStringMaxWidth(0, 5 * OLED_FONT_HEIGHT_PX, 200, String("DNS: ") + WiFi.dnsIP().toString());
    }
    else
    {
        oled.drawStringMaxWidth(0, 1 * OLED_FONT_HEIGHT_PX, 200, "WiFi is not connected");
        oled.drawStringMaxWidth(0, 2 * OLED_FONT_HEIGHT_PX, 200, String("Last status: ") + (int)WiFi.status());
    }
    oled.display();
    Serial.printf("%s: displaying WiFi status\n", __func__);
}

void oled_disp_gps_data()
{
    oled.clear();
    struct gps_data data = gps_get_data();
    // No. of satellites on the first line
    oled.drawStringMaxWidth(0, 1 * OLED_FONT_HEIGHT_PX, 200, String("GPS sats: ") + data.satellites);
    // Date and time on the second line
    char text[OLED_MAX_LINE_LEN];
    snprintf(text, OLED_MAX_LINE_LEN, "UTC %d-%d-%d %d:%d:%d", data.utc_year, data.utc_month, data.utc_day, data.utc_hour, data.utc_minute, data.utc_second);
    oled.drawStringMaxWidth(0, 2 * OLED_FONT_HEIGHT_PX, 200, text);
    // Coordinates are converted from DD.DDDDDD to DD MM SS
    char north_south = 'N';
    if (data.latitude < 0)
    {
        north_south = 'S';
    }
    char east_west = 'E';
    if (data.longitude < 0)
    {
        east_west = 'W';
    }
    double lat_d, lat_m, lat_s, throw_away;
    lat_m = modf(data.latitude, &lat_d) * 60;
    lat_s = modf(lat_m, &throw_away) * 60;
    snprintf(text, OLED_MAX_LINE_LEN, "Lat: %c %dd %dm %ds", north_south, (int)lat_d, (int)lat_m, (int)lat_s);
    oled.drawStringMaxWidth(0, 3 * OLED_FONT_HEIGHT_PX, 200, text);

    double lon_d, lon_m, lon_s;
    lon_m = modf(data.longitude, &lon_d) * 60;
    lon_s = modf(lon_m, &throw_away) * 60;
    snprintf(text, OLED_MAX_LINE_LEN, "Lon: %c %dd %dm %ds", east_west, (int)lon_d, (int)lon_m, (int)lon_s);
    oled.drawStringMaxWidth(0, 4 * OLED_FONT_HEIGHT_PX, 200, text);
    oled.drawStringMaxWidth(0, 5 * OLED_FONT_HEIGHT_PX, 200, String("Altitude (m): ") + data.altitude_metres);
    oled.display();
    Serial.printf("%s: displaying GPS status\n", __func__);
}