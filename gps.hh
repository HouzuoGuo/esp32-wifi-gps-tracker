#ifndef ESP_WIFI_GPS_TRACKER_GPS_HH
#define ESP_WIFI_GPS_TRACKER_GPS_HH

// gps_setup initialises GPS chip, the one producing location data over a serial connection.
void gps_setup();
// gps_read spends at most 1 second to read and interpret the latest position output from GPS chip.
void gps_read();

// gps_data describes the latest coordinates and clock time read from GPS.
struct gps_data
{
    double latitude, longitude, altitude_metres;
    int satellites, pos_age_sec;
    bool valid_pos;

    int unix_time, utc_year, utc_month, utc_day, utc_hour, utc_minute, utc_second;
    bool valid_time;
};

/*
gps_get_data returns the latest coordinates and clock time read from GPS. If GPS info is not available,
the "valid" property of the return value will be false.
*/
struct gps_data gps_get_data();

#endif // include guard