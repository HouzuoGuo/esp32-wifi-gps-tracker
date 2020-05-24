#ifndef ESP_WIFI_GPS_TRACKER_TWOFA_CODE_GEN_H
#define ESP_WIFI_GPS_TRACKER_TWOFA_CODE_GEN_H

#define VERIFICATION_CODE_MODULUS (1000 * 1000) // Six digits
#define BITS_PER_BASE32_CHAR 5                  // Base32 expands space by 8/5

/*
twofa_generate_code uses the input secret key and timestamp to generate a TOTP code.
The input timestamp has to be a unix timestamp in UTC zone and already divided by 30 - the common TOTP interval.
The function returns the TOTP code, or -1 in case of error.
*/
int twofa_generate_code(const char *key, unsigned long tm);

#endif // include guard