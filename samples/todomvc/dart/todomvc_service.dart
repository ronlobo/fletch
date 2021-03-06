// Copyright (c) 2015, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

// Generated file. Do not edit.

library todomvc_service;

import "dart:ffi";
import "dart:service" as service;
import "struct.dart";

final Channel _channel = new Channel();
final Port _port = new Port(_channel);
final Foreign _postResult = Foreign.lookup("PostResultToService");

bool _terminated = false;
TodoMVCService _impl;

abstract class TodoMVCService {
  void createItem(BoxedString title);
  void deleteItem(int id);
  void completeItem(int id);
  void uncompleteItem(int id);
  void clearItems();
  void dispatch(int id);
  void sync(PatchSetBuilder result);
  void reset();

  static void initialize(TodoMVCService impl) {
    if (_impl != null) {
      throw new UnsupportedError("Cannot re-initialize");
    }
    _impl = impl;
    _terminated = false;
    service.register("TodoMVCService", _port);
  }

  static bool hasNextEvent() {
    return !_terminated;
  }

  static void handleNextEvent() {
    var request = _channel.receive();
    switch (request.getInt32(0)) {
      case _TERMINATE_METHOD_ID:
        _terminated = true;
        _postResult.icall$1(request);
        break;
      case _CREATE_ITEM_METHOD_ID:
        _impl.createItem(getRoot(new BoxedString(), request));
        _postResult.icall$1(request);
        break;
      case _DELETE_ITEM_METHOD_ID:
        _impl.deleteItem(request.getInt32(48));
        _postResult.icall$1(request);
        break;
      case _COMPLETE_ITEM_METHOD_ID:
        _impl.completeItem(request.getInt32(48));
        _postResult.icall$1(request);
        break;
      case _UNCOMPLETE_ITEM_METHOD_ID:
        _impl.uncompleteItem(request.getInt32(48));
        _postResult.icall$1(request);
        break;
      case _CLEAR_ITEMS_METHOD_ID:
        _impl.clearItems();
        _postResult.icall$1(request);
        break;
      case _DISPATCH_METHOD_ID:
        _impl.dispatch(request.getUint16(48));
        _postResult.icall$1(request);
        break;
      case _SYNC_METHOD_ID:
        MessageBuilder mb = new MessageBuilder(16);
        PatchSetBuilder builder = mb.initRoot(new PatchSetBuilder(), 8);
        _impl.sync(builder);
        var result = getResultMessage(builder);
        request.setInt64(48, result);
        _postResult.icall$1(request);
        break;
      case _RESET_METHOD_ID:
        _impl.reset();
        _postResult.icall$1(request);
        break;
      default:
        throw new UnsupportedError("Unknown method");
    }
  }

  static const int _TERMINATE_METHOD_ID = 0;
  static const int _CREATE_ITEM_METHOD_ID = 1;
  static const int _DELETE_ITEM_METHOD_ID = 2;
  static const int _COMPLETE_ITEM_METHOD_ID = 3;
  static const int _UNCOMPLETE_ITEM_METHOD_ID = 4;
  static const int _CLEAR_ITEMS_METHOD_ID = 5;
  static const int _DISPATCH_METHOD_ID = 6;
  static const int _SYNC_METHOD_ID = 7;
  static const int _RESET_METHOD_ID = 8;
}

class Node extends Reader {
  bool get isNil => 1 == this.tag;
  bool get isNum => 2 == this.tag;
  int get num => segment.memory.getInt32(offset + 0);
  bool get isTruth => 3 == this.tag;
  bool get truth => segment.memory.getUint8(offset + 0) != 0;
  bool get isStr => 4 == this.tag;
  String get str => readString(new _uint16List(), 0);
  List<int> get strData => readList(new _uint16List(), 0);
  bool get isCons => 5 == this.tag;
  Cons get cons => new Cons()
      ..segment = segment
      ..offset = offset + 0;
  int get tag => segment.memory.getUint16(offset + 22);
}

class NodeBuilder extends Builder {
  void setNil() {
    tag = 1;
  }
  void set num(int value) {
    tag = 2;
    segment.memory.setInt32(offset + 0, value);
  }
  void set truth(bool value) {
    tag = 3;
    segment.memory.setUint8(offset + 0, value ? 1 : 0);
  }
  void set str(String value) {
    tag = 4;

    NewString(new _uint16BuilderList(), 0, value);
  }
  List<int> initStrData(int length) {
    tag = 4;

    return NewList(new _uint16BuilderList(), 0, length, 2);
  }
  ConsBuilder initCons() {
    tag = 5;
    return new ConsBuilder()
        ..segment = segment
        ..offset = offset + 0;
  }
  void set tag(int value) {
    segment.memory.setUint16(offset + 22, value);
  }
}

class Cons extends Reader {
  Node get fst => readStruct(new Node(), 0);
  Node get snd => readStruct(new Node(), 8);
  int get deleteEvent => segment.memory.getUint16(offset + 16);
  int get completeEvent => segment.memory.getUint16(offset + 18);
  int get uncompleteEvent => segment.memory.getUint16(offset + 20);
}

class ConsBuilder extends Builder {
  NodeBuilder initFst() {
    return NewStruct(new NodeBuilder(), 0, 24);
  }
  NodeBuilder initSnd() {
    return NewStruct(new NodeBuilder(), 8, 24);
  }
  void set deleteEvent(int value) {
    segment.memory.setUint16(offset + 16, value);
  }
  void set completeEvent(int value) {
    segment.memory.setUint16(offset + 18, value);
  }
  void set uncompleteEvent(int value) {
    segment.memory.setUint16(offset + 20, value);
  }
}

class Patch extends Reader {
  Node get content => new Node()
      ..segment = segment
      ..offset = offset + 0;
  List<int> get path => readList(new _uint8List(), 24);
}

class PatchBuilder extends Builder {
  NodeBuilder initContent() {
    return new NodeBuilder()
        ..segment = segment
        ..offset = offset + 0;
  }
  List<int> initPath(int length) {
    return NewList(new _uint8BuilderList(), 24, length, 1);
  }
}

class PatchSet extends Reader {
  List<Patch> get patches => readList(new _PatchList(), 0);
}

class PatchSetBuilder extends Builder {
  List<PatchBuilder> initPatches(int length) {
    return NewList(new _PatchBuilderList(), 0, length, 32);
  }
}

class BoxedString extends Reader {
  String get str => readString(new _uint16List(), 0);
  List<int> get strData => readList(new _uint16List(), 0);
}

class BoxedStringBuilder extends Builder {
  void set str(String value) {

    NewString(new _uint16BuilderList(), 0, value);
  }
  List<int> initStrData(int length) {

    return NewList(new _uint16BuilderList(), 0, length, 2);
  }
}

class _uint16List extends ListReader implements List<int> {
  int operator[](int index) => segment.memory.getUint16(offset + index * 2);
}

class _uint16BuilderList extends ListBuilder implements List<int> {
  int operator[](int index) => segment.memory.getUint16(offset + index * 2);
  void operator[]=(int index, int value) => segment.memory.setUint16(offset + index * 2, value);
}

class _uint8List extends ListReader implements List<int> {
  int operator[](int index) => segment.memory.getUint8(offset + index * 1);
}

class _uint8BuilderList extends ListBuilder implements List<int> {
  int operator[](int index) => segment.memory.getUint8(offset + index * 1);
  void operator[]=(int index, int value) => segment.memory.setUint8(offset + index * 1, value);
}

class _PatchList extends ListReader implements List<Patch> {
  Patch operator[](int index) => readListElement(new Patch(), index, 32);
}

class _PatchBuilderList extends ListBuilder implements List<PatchBuilder> {
  PatchBuilder operator[](int index) => readListElement(new PatchBuilder(), index, 32);
}
