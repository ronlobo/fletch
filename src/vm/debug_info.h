// Copyright (c) 2015, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#ifndef SRC_VM_DEBUG_INFO_H_
#define SRC_VM_DEBUG_INFO_H_

#include <unordered_map>

#include "src/vm/object.h"

namespace fletch {

class Breakpoint {
 public:
  Breakpoint(Function* function, int bytecode_index, int id, bool is_one_shot);
  Breakpoint(Function* function,
             int bytecode_index,
             int id,
             bool is_one_shot,
             Coroutine* coroutine,
             int stack_height);

  Function* function() const { return function_; }
  int bytecode_index() const { return bytecode_index_; }
  int id() const { return id_; }
  bool is_one_shot() const { return is_one_shot_; }
  Stack* stack() const {
    if (coroutine_ == NULL) return NULL;
    return coroutine_->stack();
  }
  int stack_height() const { return stack_height_; }

  // GC support for process GCs.
  void VisitPointers(PointerVisitor* visitor);

  // GC support for program GCs.
  void VisitProgramPointers(PointerVisitor* visitor);

 private:
  Function* function_;
  int bytecode_index_;
  int id_;
  bool is_one_shot_;
  Coroutine* coroutine_;
  int stack_height_;
};

class DebugInfo {
 public:
  DebugInfo();

  bool ShouldBreak(uint8_t* bcp, Object** sp);
  int SetBreakpoint(Function* function, int bytecode_index);
  int SetStepOverBreakpoint(Function* function,
                            int bytecode_index,
                            Coroutine* coroutine,
                            int stack_height);
  bool DeleteBreakpoint(int id);
  bool is_stepping() const { return is_stepping_; }
  void set_is_stepping(bool value) { is_stepping_ = value; }
  bool is_at_breakpoint() const { return is_at_breakpoint_; }
  void set_is_at_breakpoint(bool value) { is_at_breakpoint_ = value; }

  // GC support for process GCs.
  void VisitPointers(PointerVisitor* visitor);

  // GC support for program GCs.
  void VisitProgramPointers(PointerVisitor* visitor);
  void UpdateBreakpoints();

 private:
  int next_breakpoint_id() { return next_breakpoint_id_++; }

  bool is_stepping_;
  bool is_at_breakpoint_;

  typedef std::unordered_map<uint8_t*, Breakpoint> BreakpointMap;
  BreakpointMap breakpoints_;
  int next_breakpoint_id_;
};

}  // namespace fletch

#endif  // SRC_VM_DEBUG_INFO_H_
