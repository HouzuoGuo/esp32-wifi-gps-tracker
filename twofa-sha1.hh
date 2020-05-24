// SHA1 header file
//
// Copyright 2010 Google Inc.
// Author: Markus Gutschke
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// Houzuo Guo <guohouzuo@gmail.com> obtained the source code and made minor adjustments on 2020-05.
#ifndef ESP_WIFI_GPS_TRACKER_TWOFA_SHA1_H
#define ESP_WIFI_GPS_TRACKER_TWOFA_SHA1_H

#include <stdint.h>

#define SHA1_BLOCKSIZE 64
#define SHA1_DIGEST_LENGTH 20

typedef struct
{
  uint32_t digest[8];
  uint32_t count_lo, count_hi;
  uint8_t data[SHA1_BLOCKSIZE];
  int local;
} SHA1_INFO;

void sha1_init(SHA1_INFO *sha1_info);
void sha1_update(SHA1_INFO *sha1_info, const uint8_t *buffer, int count);
void sha1_final(SHA1_INFO *sha1_info, uint8_t digest[20]);

#endif // include guard