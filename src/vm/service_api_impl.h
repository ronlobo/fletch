// Copyright (c) 2015, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#ifndef SRC_VM_SERVICE_API_IMPL_H_
#define SRC_VM_SERVICE_API_IMPL_H_

#include "include/service_api.h"

namespace fletch {

class Monitor;
class Port;
struct ServiceRequest;

// TODO(ager): Instead of making this accessible, we should
// probably post a callback into dart? Fix the service param;
// for now it is a pointer to a pointer so we can post something
// into dart that dart can free.
extern "C" void PostResultToService(ServiceRequest* request);

class Service {
 public:
  Service(char* name, Port* port);
  ~Service();
  ServiceApiValueType Invoke(int id, ServiceApiValueType argument);
  void InvokeAsync(int id,
                   ServiceApiValueType argument,
                   ServiceApiCallback callback,
                   void* data);
  char* name() const { return name_; }

 private:
  friend void PostResultToService(ServiceRequest* request);

  void PostResult(ServiceApiValueType result);
  ServiceApiValueType WaitForResult();

  Monitor* const result_monitor_;
  bool has_result_;
  ServiceApiValueType result_;

  char* const name_;
  Port* const port_;
};

}  // namespace fletch

#endif  // SRC_VM_SERVICE_API_IMPL_H_
