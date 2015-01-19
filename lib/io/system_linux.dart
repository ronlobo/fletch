// Copyright (c) 2014, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

part of system;

const int EPOLLIN       = 0x1;
const int EPOLLOUT      = 0x4;
const int EPOLLERR      = 0x8;
const int EPOLLHUP      = 0x10;
const int EPOLLRDHUP    = 0x2000;
const int EPOLLONESHOT  = 0x40000000;

const int EPOLL_CTL_ADD = 1;
const int EPOLL_CTL_DEL = 2;
const int EPOLL_CTL_MOD = 3;

class LinuxAddrInfo extends AddrInfo {
  LinuxAddrInfo() : super._();
  LinuxAddrInfo.fromAddress(int address) : super._fromAddress(address);

  Foreign get ai_addr {
    int offset = _addrlenOffset + wordSize;
    return new Foreign.fromAddress(getWord(offset), ai_addrlen);
  }

  get ai_canonname {
    int offset = _addrlenOffset + wordSize * 2;
    return getWord(offset);
  }

  AddrInfo get ai_next {
    int offset = _addrlenOffset + wordSize * 3;
    return new LinuxAddrInfo.fromAddress(getWord(offset));
  }
}

class EpollEvent extends Struct {
  EpollEvent() : super.finalize(2);

  int get events => getWord(0);
  void set events(int value) { setWord(0, value); }

  int get data => getWord(4);
  void set data(int value) { setWord(4, value); }
}

class LinuxSystem extends PosixSystem {
  static final Foreign _epollCtl = Foreign.lookup("epoll_ctl");
  static final Foreign _epollWait = Foreign.lookup("epoll_wait");
  final EpollEvent _epollEvent = new EpollEvent();

  int get FIONREAD => 0x541B;

  int addToEventHandler(int fd) {
    _epollEvent.events = 0;
    _epollEvent.data = 0;
    int eh = System.eventHandler;
    return _retry(() => _epollCtl.icall$4(eh, EPOLL_CTL_ADD, fd, _epollEvent));
  }

  int removeFromEventHandler(int fd) {
    int eh = System.eventHandler;
    return _retry(() => _epollCtl.icall$4(eh, EPOLL_CTL_DEL, fd, Foreign.NULL));
  }

  int setPortForNextEvent(int fd, Port port, int mask) {
    int events = EPOLLRDHUP | EPOLLHUP | EPOLLONESHOT;
    if ((mask & READ_EVENT) != 0) events |= EPOLLIN;
    if ((mask & WRITE_EVENT) != 0) events |= EPOLLOUT;
    _epollEvent.events = events;
    int rawPort = port._port;
    Port._incrementRef(rawPort);
    _epollEvent.data = rawPort;
    int eh = System.eventHandler;
    return _retry(() => _epollCtl.icall$4(eh, EPOLL_CTL_MOD, fd, _epollEvent));
  }
}