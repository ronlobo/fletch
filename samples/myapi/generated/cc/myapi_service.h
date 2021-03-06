// Copyright (c) 2015, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

// Generated file. Do not edit.

#ifndef MYAPI_SERVICE_H
#define MYAPI_SERVICE_H

#include <inttypes.h>

class MyApiService {
 public:
  static void setup();
  static void tearDown();
  static int32_t create();
  static void createAsync(void (*callback)(int32_t));
  static void destroy(int32_t api);
  static void destroyAsync(int32_t api, void (*callback)());
  static int32_t foo(int32_t api);
  static void fooAsync(int32_t api, void (*callback)(int32_t));
  static void MyObject_funk(int32_t api, int32_t id, int32_t o);
  static void MyObject_funkAsync(int32_t api, int32_t id, int32_t o, void (*callback)());
};

#endif  // MYAPI_SERVICE_H
