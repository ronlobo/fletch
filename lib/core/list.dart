// Copyright (c) 2014, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

part of dart.core;

// Matches dart:core on Jan 21, 2015.
abstract class List<E> implements Iterable<E> {
  factory List([int length]) {
    return (length == null) ? new _GrowableList() : new _FixedList(length);
  }

  factory List.filled(int length, E fill) {
    var result = new List<E>(length);
    if (fill != null) {
      for (var i = 0; i < length; ++i) {
        result[i] = fill;
      }
    }
    return result;
  }

  factory List.from(Iterable other, { bool growable: true }) {
    List result = [];
    other.forEach((each) => result.add(each));
    return growable ? result : result.toList(false);
  }

  factory List.generate(int length, E generator(int index),
                       { bool growable: true }) {
    var result;
    if (growable) {
      result = <E>[];
      result.length = length;
    } else {
      result = new List<E>(length);
    }
    for (var i = 0; i < length; ++i) {
      result[i] = generator(i);
    }
    return result;
  }

  E operator[](int index);

  void operator[]=(int index, E value);

  int get length;

  void set length(int newLength);

  void add(E value);

  void addAll(Iterable<E> iterable);

  Iterable<E> get reversed;

  void sort([int compare(E a, E b)]);

  void shuffle([Random random]);

  int indexOf(E element, [int start = 0]);

  int lastIndexOf(E element, [int start]);

  void clear();

  void insert(int index, E element);

  void insertAll(int index, Iterable<E> iterable);

  void setAll(int index, Iterable<E> iterable);

  bool remove(Object value);

  E removeAt(int index);

  E removeLast();

  void removeWhere(bool test(E element));

  void retainWhere(bool test(E element));

  List<E> sublist(int start, [int end]);

  Iterable<E> getRange(int start, int end);

  void setRange(int start, int end, Iterable<E> iterable, [int skipCount = 0]);

  void removeRange(int start, int end);

  void fillRange(int start, int end, [E fillValue]);

  void replaceRange(int start, int end, Iterable<E> replacement);

  Map<int, E> asMap();
}

class _ConstantList<E> implements List<E> {
  final _list;

  _ConstantList(int length) : this._list = _new(length);

  Iterator<E> get iterator => new _ListIterator(this);

  Iterable map(f(E element)) {
    throw new UnimplementedError("_ConstantList.map");
  }

  Iterable<E> where(bool test(E element)) {
    throw new UnimplementedError("_ConstantList.where");
  }

  Iterable expand(Iterable f(E element)) {
    throw new UnimplementedError("_ConstantList.expand");
  }

  bool contains(Object element) {
    for (int i = 0; i < length; i++) {
      if (this[i] == element) return true;
    }
    return false;
  }

  void forEach(void f(E element)) {
    for (int i = 0; i < length; i++) {
      f(this[i]);
    }
  }

  E reduce(E combine(E value, E element)) {
    if (length == 0) throw new StateError("No element");
    var value = this[0];
    for (int i = 1; i < length; i++) {
      value = combine(value, this[i]);
    }
    return value;
  }

  dynamic fold(var initialValue,
               dynamic combine(var previousValue, E element)) {
    for (int i = 0; i < length; i++) {
      initialValue = combine(initialValue, this[i]);
    }
    return initialValue;
  }

  bool every(bool test(E element)) {
    for (int i = 0; i < length; i++) {
      if (!test(this[i])) return false;
    }
    return true;
  }

  String join([String separator = ""]) {
    StringBuffer buffer = new StringBuffer();
    buffer.writeAll(this, separator);
    return buffer.toString();
  }

  bool any(bool test(E element)) {
    for (int i = 0; i < length; i++) {
      if (test(this[i])) return true;
    }
    return false;
  }

  List<E> toList({ bool growable: true }) {
    var result;
    if (growable) {
      result = <E>[];
      result.length = length;
    } else {
      result = new List<E>(length);
    }
    for (int i = 0; i < length; i++) {
      list[i] = this[i];
    }
    return result;
  }

  Set<E> toSet() => new Set<E>.from(this);

  int get length native;

  int get isEmpty => length == 0;

  int get isNotEmpty => length != 0;

  Iterable<E> take(int n) {
    throw new UnimplementedError("_ConstantList.take");
  }

  Iterable<E> takeWhile(bool test(E value)) {
    throw new UnimplementedError("_ConstantList.takeWhile");
  }

  Iterable<E> skip(int n) {
    throw new UnimplementedError("_ConstantList.skip");
  }

  Iterable<E> skipWhile(bool test(E value)) {
    throw new UnimplementedError("_ConstantList.skipWhile");
  }

  E get first {
    if (length == 0) throw new StateError("No element");
    return this[0];
  }

  E get last {
    if (length == 0) throw new StateError("No element");
    return this[length - 1];
  }

  E get single {
    throw new UnimplementedError("_ConstantList.single");
  }

  E firstWhere(bool test(E element), { E orElse() }) {
    throw new UnimplementedError("_ConstantList.firstWhere");
  }

  E lastWhere(bool test(E element), {E orElse()}) {
    throw new UnimplementedError("_ConstantList.lastWhere");
  }

  E singleWhere(bool test(E element)) {
    throw new UnimplementedError("_ConstantList.singleWhere");
  }

  E elementAt(int index) => this[index];

  E operator[](int index) native catch (error) {
    switch (error) {
      case _wrongArgumentType:
        throw new ArgumentError();
      case _indexOutOfBounds:
        throw new IndexError(index, this);
    }
  }

  void operator[]=(int index, E value) {
    throw new UnsupportedError("Cannot modify an unmodifiable list");
  }

  void set length(int newLength) {
    throw new UnsupportedError("Cannot change length of fixed-length list");
  }

  void add(E value) {
    throw new UnsupportedError("Cannot add to fixed-length list");
  }

  void addAll(Iterable<E> iterable) {
    throw new UnsupportedError("Cannot add to fixed-length list");
  }

  Iterable<E> get reversed {
    throw new UnimplementedError("_ConstantList.reversed");
  }

  void sort([int compare(E a, E b)]) {
    throw new UnsupportedError("Cannot modify an unmodifiable list");
  }

  void shuffle([Random random]) {
    throw new UnsupportedError("Cannot modify an unmodifiable list");
  }

  int indexOf(E element, [int start = 0]) {
    if (start >= length) return -1;
    if (start < 0) start = 0;
    for (int i = start; i < length; i++) {
      if (this[i] == element) return i;
    }
    return -1;
  }

  int lastIndexOf(E element, [int start]) {
    if (start == null) start = length - 1;
    if (start < 0) return -1;
    if (start >= length) start = length - 1;
    for (int i = start; i >= 0; i--) {
      if (this[i] == element) {
        return i;
      }
    }
    return -1;
  }

  void clear() {
    throw new UnsupportedError("Cannot remove from fixed-length list");
  }

  void insert(int index, E element) {
    throw new UnsupportedError("Cannot add to fixed-length list");
  }

  void insertAll(int index, Iterable<E> iterable) {
    throw new UnsupportedError("Cannot add to fixed-length list");
  }

  void setAll(int index, Iterable<E> iterable) {
    throw new UnsupportedError("Cannot add to fixed-length list");
  }

  bool remove(Object value) {
    throw new UnsupportedError("Cannot remove from fixed-length list");
  }

  E removeAt(int index) {
    throw new UnsupportedError("Cannot remove from fixed-length list");
  }

  E removeLast() {
    throw new UnsupportedError("Cannot remove from fixed-length list");
  }

  void removeWhere(bool test(E element)) {
    throw new UnsupportedError("Cannot remove from fixed-length list");
  }

  void retainWhere(bool test(E element)) {
    throw new UnsupportedError("Cannot remove from fixed-length list");
  }

  List<E> sublist(int start, [int end]) {
    var result = new List();
    for (int i = start; i < end; i++) {
      result.add(this[i]);
    }
    return result;
  }

  Iterable<E> getRange(int start, int end) {
    throw new UnimplementedError("_ConstantList.getRange");
  }

  void setRange(int start, int end, Iterable<E> iterable, [int skipCount = 0]) {
    throw new UnsupportedError("Cannot modify an unmodifiable list");
  }

  void removeRange(int start, int end) {
    throw new UnsupportedError("Cannot remove from fixed-length list");
  }

  void fillRange(int start, int end, [E fillValue]) {
    throw new UnsupportedError("Cannot modify an unmodifiable list");
  }

  void replaceRange(int start, int end, Iterable<E> replacement) {
    throw new UnsupportedError("Cannot remove from fixed-length list");
  }

  Map<int, E> asMap() {
    throw new UnimplementedError("_ConstantList.asMap");
  }

  String toString() => "List";

  static _ConstantList _new(int length) native;
}

class _FixedList<E> extends _ConstantList<E> {
  _FixedList([int length]) : super(length);

  void sort([int compare(E a, E b)]) {
    throw new UnimplementedError("_FixedList.sort");
  }

  void shuffle([Random random]) {
    throw new UnimplementedError("_FixedList.shuffle");
  }

  void setRange(int start, int end, Iterable<E> iterable, [int skipCount = 0]) {
    throw new UnimplementedError("_FixedList.setRange");
  }

  void fillRange(int start, int end, [E fillValue]) {
    RangeError.checkValidRange(start, end, length);
    for (int i = start; i < end; i++) {
      this[i] = fillValue;
    }
  }

  E operator[]=(int index, value) native catch (error) {
    switch (error) {
      case _wrongArgumentType:
        throw new ArgumentError();
      case _indexOutOfBounds:
        throw new IndexError(index, this);
    }
  }
}

class _GrowableList<E> implements List<E> {
  int _length;
  _FixedList _list;

  _GrowableList() : _length = 0, _list = new _FixedList(4);

  Iterator<E> get iterator => new _ListIterator(this);

  Iterable map(f(E element)) {
    throw new UnimplementedError("_GrowableList.map");
  }

  Iterable<E> where(bool test(E element)) {
    throw new UnimplementedError("_GrowableList.where");
  }

  Iterable expand(Iterable f(E element)) {
    throw new UnimplementedError("_GrowableList.expand");
  }

  bool contains(Object element) {
    for (int i = 0; i < length; i++) {
      if (this[i] == element) return true;
    }
    return false;
  }

  void forEach(void f(E element)) {
    for (int i = 0; i < length; i++) {
      f(this[i]);
    }
  }

  E reduce(E combine(E value, E element)) {
    if (length == 0) throw new StateError("No element");
    var value = this[0];
    for (int i = 1; i < length; i++) {
      value = combine(value, this[i]);
    }
    return value;
  }

  dynamic fold(var initialValue,
               dynamic combine(var previousValue, E element)) {
    for (int i = 0; i < length; i++) {
      initialValue = combine(initialValue, this[i]);
    }
    return initialValue;
  }

  bool every(bool test(E element)) {
    for (int i = 0; i < length; i++) {
      if (!test(this[i])) return false;
    }
    return true;
  }

  String join([String separator = ""]) {
    StringBuffer buffer = new StringBuffer();
    buffer.writeAll(this, separator);
    return buffer.toString();
  }

  bool any(bool test(E element)) {
    for (int i = 0; i < length; i++) {
      if (test(this[i])) return true;
    }
    return false;
  }

  List<E> toList({ bool growable: true }) {
    var result;
    if (growable) {
      result = <E>[];
      result.length = length;
    } else {
      result = new List<E>(length);
    }
    for (int i = 0; i < length; i++) {
      list[i] = this[i];
    }
    return result;
  }

  Set<E> toSet() {
    throw new UnimplementedError("_GrowableList.toSet");
  }

  int get length => _length;

  int get isEmpty => _length == 0;

  int get isNotEmpty => _length != 0;

  Iterable<E> take(int n) {
    throw new UnimplementedError("_GrowableList.take");
  }

  Iterable<E> takeWhile(bool test(E value)) {
    throw new UnimplementedError("_GrowableList.takeWhile");
  }

  Iterable<E> skip(int n) {
    throw new UnimplementedError("_GrowableList.skip");
  }

  Iterable<E> skipWhile(bool test(E value)) {
    throw new UnimplementedError("_GrowableList.skipWhile");
  }

  E get first {
    if (length == 0) throw new StateError("No element");
    return this[0];
  }

  E get last {
    if (length == 0) throw new StateError("No element");
    return this[length - 1];
  }

  E get single {
    throw new UnimplementedError("_GrowableList.single");
  }

  E firstWhere(bool test(E element), { E orElse() }) {
    throw new UnimplementedError("_GrowableList.firstWhere");
  }

  E lastWhere(bool test(E element), {E orElse()}) {
    throw new UnimplementedError("_GrowableList.lastWhere");
  }

  E singleWhere(bool test(E element)) {
    throw new UnimplementedError("_GrowableList.singleWhere");
  }

  E elementAt(int index) => this[index];

  E operator[](int index) {
    if (index >= length) throw new IndexError(index, this);
    return _list[index];
  }

  void operator[]=(int index, value) {
    if (index >= length) throw new IndexError(index, this);
    return _list[index] = value;
  }

  void set length(int newLength) {
    if (newLength > _list.length) {
      _grow(newLength);
    }
    _length = newLength;
  }

  void add(E value) {
    if (_length >= _list.length) {
      _grow(_length + 1);
    }
    _list[_length++] = value;
  }

  void addAll(Iterable<E> iterable) {
    iterable.forEach((E each) {
      add(each);
    });
  }

  Iterable<E> get reversed {
    throw new UnimplementedError("_GrowableList.reversed");
  }

  void sort([int compare(E a, E b)]) {
    throw new UnimplementedError("_GrowableList.sort");
  }

  void shuffle([Random random]) {
    throw new UnimplementedError("_GrowableList.shuffle");
  }

  int indexOf(E element, [int start = 0]) {
    if (start >= length) return -1;
    if (start < 0) start = 0;
    for (int i = start; i < length; i++) {
      if (this[i] == element) return i;
    }
    return -1;
  }

  int lastIndexOf(E element, [int start]) {
    if (start == null) start = length - 1;
    if (start < 0) return -1;
    if (start >= length) start = length - 1;
    for (int i = start; i >= 0; i--) {
      if (this[i] == element) {
        return i;
      }
    }
    return -1;
  }

  void clear() {
    if (_length != 0) {
      _length = 0;
      _list = new _FixedList(4);
    }
  }

  void insert(int index, E element) {
    throw new UnimplementedError("_GrowableList.insert");
  }

  void insertAll(int index, Iterable<E> iterable) {
    throw new UnimplementedError("_GrowableList.insertAll");
  }

  void setAll(int index, Iterable<E> iterable) {
    throw new UnimplementedError("_GrowableList.setAll");
  }

  bool remove(Object value) {
    for (int i = 0; i < length; ++i) {
      if (_list[i] == value) {
        _shiftDown(i);
        return true;
      }
    }
    return false;
  }

  E removeAt(int index) {
    if (index >= length) throw new IndexError(index, this);
    E result = _list[index];
    _shiftDown(index);
    return result;

  }

  E removeLast() {
    if (length == 0) throw new IndexError(length - 1, this);
    --length;
    E result = _list[length];
    _list[length] = null;
    return result;
  }

  void removeWhere(bool test(E element)) {
    for (int i = 0; i < _length; i++) {
      if (test(this[i])) removeAt(i);
    }
  }

  void retainWhere(bool test(E element)) {
    for (int i = 0; i < _length; i++) {
      if (!test(this[i])) removeAt(i);
    }
  }

  List<E> sublist(int start, [int end]) {
    var result = new List();
    for (int i = start; i < end; i++) {
      result.add(this[i]);
    }
    return result;
  }

  Iterable<E> getRange(int start, int end) {
    throw new UnimplementedError("_GrowableList.getRange");
  }

  void setRange(int start, int end, Iterable<E> iterable, [int skipCount = 0]) {
    throw new UnimplementedError("_GrowableList.setRange");
  }

  void removeRange(int start, int end) {
    RangeError.checkValidRange(start, end, length);
    for (int i = 0; i < length - end; i++) {
      this[start + i] = this[end + i];
    }
    length -= (end - start);
  }

  void fillRange(int start, int end, [E fillValue]) {
    RangeError.checkValidRange(start, end, length);
    for (int i = start; i < end; i++) {
      this[i] = fillValue;
    }
  }

  void replaceRange(int start, int end, Iterable<E> replacement) {
    throw new UnimplementedError("_GrowableList.replaceRange");
  }

  Map<int, E> asMap() {
    throw new UnimplementedError("_GrowableList.asMap");
  }

  String toString() => "List";

  void _grow(minSize) {
    // TODO(ager): play with heuristics here.
    var newList = new _FixedList(minSize + (minSize >> 2));
    for (int i = 0; i < _list.length; ++i) {
      newList[i] = _list[i];
    }
    _list = newList;
  }

  void _shiftDown(int i) {
    for (int j = i + 1; j < length; ++j, ++i) {
      _list[i] = _list[j];
    }
    --length;
    _list[length] = null;
  }
}

class _ListIterator<E> implements Iterator<E> {
  final List _list;

  int _index = -1;
  E _current;

  _ListIterator(this._list);

  E get current => _current;

  bool moveNext() {
    if (++_index < _list.length) {
      _current = _list[_index];
      return true;
    }
    _current = null;
    return false;
  }
}
