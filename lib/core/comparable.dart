// Copyright (c) 2015, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

part of dart.core;

// Matches dart:core on Jan 21, 2015.
typedef int Comparator<T>(T a, T b);

// Matches dart:core on Jan 21, 2015.
abstract class Comparable<T> {
  int compareTo(T other);

  static int compare(Comparable a, Comparable b) => a.compareTo(b);
}
