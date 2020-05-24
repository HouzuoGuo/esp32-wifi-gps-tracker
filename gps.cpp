#include <time.h>
#include <HardwareSerial.h>
// GPS library came installed with "TinyGPS++ by Mikal Hart Version 1.0.2"
#include <TinyGPS++.h>
#include "gps.hh"

TinyGPSPlus gps;

void gps_setup()
{
    // On TTGO T-Beam v0.7 the pins are 12 and 15. Newer board revisions use other pins.
    Serial1.begin(9600, SERIAL_8N1, 12, 15);
}

void gps_read()
{
    long start = millis();
    while (millis() - start < 1000)
    {
        while (Serial1.available())
            gps.encode(Serial1.read());
    }
    Serial.printf("%s: took %d ms to read GPS data\n", __func__, millis() - start);
}

struct gps_data gps_get_data()
{
    struct gps_data ret;
    ret.valid_pos = false;
    ret.valid_time = false;
    if (gps.charsProcessed() < 20)
    {
        // GPS is not ready
        return ret;
    }
    ret.satellites = (int)gps.satellites.value();
    ret.valid_time = gps.time.isValid();
    if (ret.valid_time)
    {
        ret.utc_hour = gps.time.hour();
        ret.utc_minute = gps.time.minute();
        ret.utc_second = gps.time.second();
    }
    if (gps.date.isValid())
    {
        // Q: why would the library return a readable time but not readable date?
        ret.utc_year = gps.date.year();
        ret.utc_month = gps.date.month();
        ret.utc_day = gps.date.day();
    }
    ret.valid_pos = gps.location.isValid();
    if (ret.valid_pos)
    {
        ret.latitude = gps.location.lat();
        ret.longitude = gps.location.lng();
        ret.altitude_metres = gps.altitude.feet() / 3.2808;
        ret.pos_age_sec = gps.location.age() / 1000;
    }
    // Calculate unix timestamp based on the individual date elements
    ret.unix_time = 0;
    if (ret.valid_time)
    {
        struct tm unix_tm;
        memset(&unix_tm, NULL, sizeof(struct tm));
        unix_tm.tm_hour = ret.utc_hour;
        unix_tm.tm_min = ret.utc_minute;
        unix_tm.tm_sec = ret.utc_second;
        unix_tm.tm_year = ret.utc_year - 1900;
        unix_tm.tm_mon = ret.utc_month - 1;
        unix_tm.tm_mday = ret.utc_day;
        ret.unix_time = mktime(&unix_tm);
    }
    Serial.printf("%s: unix time is %d, valid time? %d valid position? %d\n", __func__, ret.unix_time, ret.valid_time, ret.valid_pos);

    return ret;
}