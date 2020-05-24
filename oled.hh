#ifndef ESP_WIFI_GPS_TRACKER_OLED_HH
#define ESP_WIFI_GPS_TRACKER_OLED_HH

#include <SSD1306Wire.h>
#include <OLEDDisplay.h>

// I soldered OLED SCL and SDA pins to board pin 22 and 21 (same side as the LoRA antenna), the OLED uses I2C for control.
#define OLED_I2C_ADDR 0x3c
#define OLED_I2C_SCL 22
#define OLED_I2C_SDA 21
// OLED_MAX_LINE_LEN is the approximate maximum text length that the on-board OLED can display on a single line using font ArialMT_Plain_10.
#define OLED_MAX_LINE_LEN 23
// OLED_FONT_HEIGHT_PX is the height of a line of text displayed on the on-board OLED using font ArialMT_Plain_10.
#define OLED_FONT_HEIGHT_PX 10

// oled is initialised by oled_setup and controls the on-board OLED display.
extern SSD1306Wire oled;

// oled_setup resets and initialises OLED hardware and its user interface library.
void oled_setup();
// oled_disp_open_aps displays the recently discovered open access points on screen.
void oled_disp_open_aps();
// oled_disp_all_aps displays the recently discovered access points on screen regardless of whether they are open.
void oled_disp_all_aps();
// oled_disp_wifi_ap_status displays the currently connected access point name and IP address.
void oled_disp_wifi_ap_status();
// oled_disp_gps_data displays the latest GPS time, date, and coordiantes data.
void oled_disp_gps_data();

#endif // include guard