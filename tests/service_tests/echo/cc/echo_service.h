// Copyright (c) 2015, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

// Generated file. Do not edit.

#ifndef ECHO_SERVICE_H
#define ECHO_SERVICE_H

class EchoService {
 public:
  static void Setup();
  static void TearDown();
  static int Echo(int n);
  static void EchoAsync(int n, void (*callback)(int));
  static int Sum(short x, int y);
  static void SumAsync(short x, int y, void (*callback)(int));
};

#endif  // ECHO_SERVICE_H
