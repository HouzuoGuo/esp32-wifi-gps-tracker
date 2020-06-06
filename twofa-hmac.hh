// HMAC_SHA1 implementation
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
// Houzuo Guo <guohouzuo@gmail.com> obtained the source code and made minor adjustments in 2020-05.

#ifndef ESP_WIFI_GPS_TRACKER_HMAC_H
#define ESP_WIFI_GPS_TRACKER_HMAC_H

#include <stdint.h>

void goog_hmac_sha1(const uint8_t *key, int keyLength, const uint8_t *data, int dataLength, uint8_t *result, int resultLength);

#endif // include guard
