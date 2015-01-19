// Copyright (c) 2014, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#include "src/vm/program.h"

// TODO(ager): Implement a self-contained simple hash map.
#include <stdlib.h>
#include <string.h>
#include <unordered_map>

#include "src/shared/bytecodes.h"
#include "src/shared/globals.h"
#include "src/shared/utils.h"

#include "src/vm/heap.h"
#include "src/vm/object.h"
#include "src/vm/process.h"
#include "src/vm/session.h"

namespace fletch {

typedef std::unordered_map<Object*, int> ObjectIndexMap;

static List<const char> StringFromCharZ(const char* str) {
  return List<const char>(str, strlen(str));
}

Program::Program()
    : scheduler_(NULL),
      session_(NULL),
      entry_(NULL),
      classes_(NULL),
      constants_(NULL),
      static_methods_(NULL),
      static_fields_(NULL),
      is_compact_(false) {
}

Program::~Program() { }

static int AddToMap(ObjectIndexMap* map, Object* value) {
  ObjectIndexMap::const_iterator it = map->find(value);
  if (it != map->end()) {
    return it->second;
  } else {
    int index = map->size();
    map->insert({value, index});
    return index;
  }
}

static Array* MapToArray(ObjectIndexMap* map, Program* program) {
  Array* result = Array::cast(program->CreateArray(map->size()));
  ObjectIndexMap::const_iterator it = map->begin();
  ObjectIndexMap::const_iterator end = map->end();
  for (; it != end; ++it) result->set(it->second, it->first);
  return result;
}

class LiteralsRewriter {
 public:
  LiteralsRewriter(Space* space, Function* function)
      : function_(function) {
    ASSERT(space->in_no_allocation_failure_scope());
  }

  void AddLiteralAndRewrite(Array* table, uint8_t* bcp) {
    int index = Utils::ReadInt32(bcp + 1);
    Object* literal = table->get(index);
    int literal_index = AddToMap(&literals_index_map_, literal);
    Object** literal_address = function_->literal_address_for(literal_index);
    int offset = reinterpret_cast<uint8_t*>(literal_address) - bcp;
    *bcp = *bcp + 1;
    Utils::WriteInt32(bcp + 1, offset);
  }

  int NumberOfLiterals() { return literals_index_map_.size(); }

  void FillInLiterals(Function* function) {
    ObjectIndexMap::const_iterator it = literals_index_map_.begin();
    ObjectIndexMap::const_iterator end = literals_index_map_.end();
    for (; it != end; ++it) function->set_literal_at(it->second, it->first);
  }

 private:
  Function* function_;
  ObjectIndexMap literals_index_map_;
};

Object* Program::UnfoldFunction(Function* function, Space* to) {
  LiteralsRewriter rewriter(to, function);

  uint8_t* bcp = function->bytecode_address_for(0);

  while (true) {
    Opcode opcode = static_cast<Opcode>(*bcp);

    switch (opcode) {
      case kLoadConst:
        rewriter.AddLiteralAndRewrite(constants(), bcp);
        break;
      case kInvokeStatic:
      case kInvokeFactory:
        rewriter.AddLiteralAndRewrite(static_methods(), bcp);
        break;
      case kAllocate:
        rewriter.AddLiteralAndRewrite(classes(), bcp);
        break;
      case kMethodEnd: {
        ASSERT(function->literals_size() == 0);
        int number_of_literals = rewriter.NumberOfLiterals();
        Object* clone = function->UnfoldInToSpace(to, number_of_literals);
        Function* result = Function::cast(clone);
        rewriter.FillInLiterals(result);
        ASSERT(result->literals_size() == number_of_literals);
        return result;
      }
      case kLoadConstUnfold:
      case kInvokeStaticUnfold:
      case kInvokeFactoryUnfold:
      case kAllocateUnfold:
        // We should only unfold folded functions.
        UNREACHABLE();
      default:
        break;
    }

    bcp += Bytecode::Size(opcode);
  }

  UNREACHABLE();
  return NULL;
}

class UnfoldingVisitor: public PointerVisitor {
 public:
  UnfoldingVisitor(Program* program, Space* from, Space* to)
      : program_(program),
        from_(from),
        to_(to) { }

  void Visit(Object** p) { UnfoldPointer(p); }

  void VisitBlock(Object** start, Object** end) {
    // Unfold all HeapObject pointers in [start, end)
    for (Object** p = start; p < end; p++) UnfoldPointer(p);
  }

 private:
  void UnfoldPointer(Object** p) {
    Object* object = *p;
    if (!object->IsHeapObject()) return;
    if (!from_->Includes(reinterpret_cast<uword>(object))) return;
    // Check for forwarding address before checking type.
    HeapObject* f = HeapObject::cast(object)->forwarding_address();
    if (f != NULL) {
      *p = f;
    } else if (object->IsFunction()) {
      // Rewrite functions.
      *p = program_->UnfoldFunction(Function::cast(object), to_);
    } else {
      *p = reinterpret_cast<HeapObject*>(object)->CloneInToSpace(to_);
    }
  }

  Program* program_;
  Space* from_;
  Space* to_;
};

void Program::Unfold() {
  ASSERT(is_compact());
  // Unfolding operates as a scavenge copying over objects as it goes.
  Space* to = new Space();
  {
    NoAllocationFailureScope scope(to);
    UnfoldingVisitor visitor(this, heap_.space(), to);
    IterateRoots(&visitor);
    to->CompleteScavenge(&visitor);
  }
  heap_.ReplaceSpace(to);
  classes_ = NULL;
  constants_ = NULL;
  static_methods_ = NULL;
  is_compact_ = false;
}

class ProgramTableRewriter {
 public:
  void AddMethodAndRewrite(uint8_t* bcp, uint8_t* new_bcp) {
    AddAndRewrite(&static_methods_, bcp, new_bcp);
  }

  void AddConstantAndRewrite(uint8_t* bcp, uint8_t* new_bcp) {
    AddAndRewrite(&constants_, bcp, new_bcp);
  }

  void AddClassAndRewrite(uint8_t* bcp, uint8_t* new_bcp) {
    AddAndRewrite(&classes_, bcp, new_bcp);
  }

  Array* CreateClassArray(Program* program) {
    return MapToArray(&classes_, program);
  }

  Array* CreateConstantArray(Program* program) {
    return MapToArray(&constants_, program);
  }

  Array* CreateStaticMethodArray(Program* program) {
    return MapToArray(&static_methods_, program);
  }

 private:
  void AddAndRewrite(ObjectIndexMap* map, uint8_t* bcp, uint8_t* new_bcp) {
    Object* literal = Function::ConstantForBytecode(bcp);
    int new_index = AddToMap(map, literal);
    *new_bcp = *new_bcp - 1;
    Utils::WriteInt32(new_bcp + 1, new_index);
  }

  ObjectIndexMap classes_;
  ObjectIndexMap constants_;
  ObjectIndexMap static_methods_;
};

void Program::FoldFunction(Function* old_function,
                           Function* new_function,
                           ProgramTableRewriter* rewriter) {
  uint8_t* start = old_function->bytecode_address_for(0);
  uint8_t* bcp = start;
  uint8_t* new_start = new_function->bytecode_address_for(0);

  while (true) {
    Opcode opcode = static_cast<Opcode>(*bcp);
    uint8_t* new_bcp = new_start + (bcp - start);

    switch (opcode) {
      case kLoadConstUnfold:
        rewriter->AddConstantAndRewrite(bcp, new_bcp);
        break;
      case kInvokeStaticUnfold:
      case kInvokeFactoryUnfold:
        rewriter->AddMethodAndRewrite(bcp, new_bcp);
        break;
      case kAllocateUnfold:
        rewriter->AddClassAndRewrite(bcp, new_bcp);
        break;
      case kMethodEnd: {
        return;
      }
      case kLoadConst:
      case kInvokeStatic:
      case kInvokeFactory:
      case kAllocate:
        // We should only fold unfolded functions.
        UNREACHABLE();
      default:
        break;
    }

    bcp += Bytecode::Size(opcode);
  }

  UNREACHABLE();
}

class FoldingVisitor: public PointerVisitor {
 public:
  FoldingVisitor(Space* from,
                 Space* to,
                 ProgramTableRewriter* rewriter,
                 Program* program)
      : from_(from),
        to_(to),
        rewriter_(rewriter),
        program_(program) { }

  void Visit(Object** p) { UnfoldPointer(p); }

  void VisitBlock(Object** start, Object** end) {
    // Fold all HeapObject pointers in [start, end)
    for (Object** p = start; p < end; p++) UnfoldPointer(p);
  }

 private:
  void UnfoldPointer(Object** p) {
    Object* object = *p;
    if (!object->IsHeapObject()) return;
    if (!from_->Includes(reinterpret_cast<uword>(object))) return;
    // Check for forwarding address before checking type.
    HeapObject* f = HeapObject::cast(object)->forwarding_address();
    if (f != NULL) {
      *p = f;
    } else if (object->IsFunction()) {
      // Copy over function without literals and setup forwarding
      // pointer.
      Function* old_function = Function::cast(object);
      *p = old_function->FoldInToSpace(to_);

      // Copy over the literals.
      Object** first = old_function->literal_address_for(0);
      VisitBlock(first, first + old_function->literals_size());

      // Rewrite the bytecodes of the new function.
      Function* new_function = Function::cast(*p);
      program_->FoldFunction(old_function, new_function, rewriter_);
    } else {
      *p = reinterpret_cast<HeapObject*>(object)->CloneInToSpace(to_);
    }
  }

  Space* from_;
  Space* to_;
  ProgramTableRewriter* rewriter_;
  Program* program_;
};

void Program::Fold() {
  ASSERT(!is_compact());
  // Folding operates as a scavenge copying over objects as it goes.
  ProgramTableRewriter rewriter;
  Space* to = new Space();
  NoAllocationFailureScope scope(to);
  FoldingVisitor visitor(heap_.space(), to, &rewriter, this);
  IterateRoots(&visitor);
  to->CompleteScavenge(&visitor);
  heap_.ReplaceSpace(to);
  classes_ = rewriter.CreateClassArray(this);
  constants_ = rewriter.CreateConstantArray(this);
  static_methods_ = rewriter.CreateStaticMethodArray(this);
  is_compact_ = true;
}

Process* Program::SpawnProcess() {
  return new Process(this);
}

Object* Program::CreateArrayWith(int capacity, Object* initial_value) {
  Object* result = heap()->CreateArray(array_class(), capacity, initial_value);
  return result;
}

Object* Program::CreateClass(int fields) {
  InstanceFormat format = InstanceFormat::instance_format(fields);
  Object* raw_class = heap()->CreateClass(format, meta_class(), null_object());
  if (raw_class->IsFailure()) return raw_class;
  Class* klass = Class::cast(raw_class);
  ASSERT(klass->NumberOfInstanceFields() == fields);
  return klass;
}

Object* Program::CreateDouble(double value) {
  return heap()->CreateDouble(double_class(), value);
}

Object* Program::CreateFunction(int arity,
                                List<uint8> bytes,
                                int number_of_literals) {
  return heap()->CreateFunction(function_class(),
                                arity,
                                bytes,
                                number_of_literals);
}

Object* Program::CreateLargeInteger(int64 value) {
  return heap()->CreateLargeInteger(large_integer_class(), value);
}

Object* Program::CreateInteger(int64 value) {
  if ((sizeof(int64) > sizeof(word) &&
       static_cast<int64>(static_cast<word>(value)) != value) ||
      !Smi::IsValid(value)) {
    return CreateLargeInteger(value);
  }
  return Smi::FromWord(value);
}

Object* Program::CreateString(List<const char> str) {
  Object* raw_result = heap()->CreateString(string_class(), str.length());
  if (raw_result->IsFailure()) return raw_result;
  String* result = String::cast(raw_result);
  ASSERT(result->length() == str.length());
  // Set the content.
  for (int i = 0; i < str.length(); i++) {
    result->set_char(i, str[i]);
  }
  return result;
}

Object* Program::CreateInstance(Class* klass) {
  return heap()->CreateHeapObject(klass, null_object());
}

Object* Program::CreateInitializer(Function* function) {
  return heap()->CreateInitializer(initializer_class_, function);
}

class MarkProcessesFoundVisitor : public ProcessVisitor {
 public:
  virtual void VisitProcess(Process* process) {
    process->set_program_gc_state(Process::kFound);
  }
};

class CollectGarbageAndCookStacksVisitor : public ProcessVisitor {
 public:
  explicit CollectGarbageAndCookStacksVisitor(Process** list) : list_(list) { }

  virtual void VisitProcess(Process* process) {
    ASSERT(process->program_gc_state() == Process::kFound);
    process->set_program_gc_state(Process::kProcessed);
    int number_of_stacks = process->CollectGarbageAndChainStacks(list_);
    process->CookStacks(number_of_stacks);
    process->CollectProcessesInQueues(list_);
  }
 private:
  Process** list_;
};

class VisitProgramRootsVisitor : public ProcessVisitor {
 public:
  explicit VisitProgramRootsVisitor(PointerVisitor* visitor)
      : visitor_(visitor) { }

  virtual void VisitProcess(Process* process) {
    process->IterateProgramPointers(visitor_);
  }

 private:
  PointerVisitor* visitor_;
};

class UncookStacksVisitor : public ProcessVisitor {
 public:
  UncookStacksVisitor() : unlink_(false) { }

  void set_unlink(bool value) { unlink_ = value; }

  virtual void VisitProcess(Process* process) {
    process->UncookAndUnchainStacks();
    process->set_program_gc_state(Process::kUnknown);
    if (unlink_) process->set_next(NULL);
  }

 private:
  bool unlink_;
};

static void VisitProcesses(Process* processes, ProcessVisitor* visitor) {
  for (Process* current = processes;
       current != NULL;
       current = current->next()) {
    ASSERT(current->program_gc_state() == Process::kProcessed);
    visitor->VisitProcess(current);
  }
}

void Program::CollectGarbage() {
  // If there is no scheduler or we fail to stop the program we just
  // bail.
  // TODO(ager): Would it make sense to make StopProgram blocking
  // until it can stop the program?
  if (scheduler() == NULL) return;
  if (!scheduler()->StopProgram(this)) return;

  {
    MarkProcessesFoundVisitor visitor;
    scheduler()->VisitProcesses(this, &visitor);
  }

  // List of processes that are only referenced by ports floating
  // around in the system.
  Process* additional_processes = NULL;

  {
    CollectGarbageAndCookStacksVisitor visitor(&additional_processes);
    scheduler()->VisitProcesses(this, &visitor);
    while (additional_processes != NULL &&
           additional_processes->program_gc_state() == Process::kFound) {
      Process* current = additional_processes;
      while (current != NULL &&
             current->program_gc_state() == Process::kFound) {
        visitor.VisitProcess(current);
        current = current->next();
      }
    }
  }

  Space* to = new Space();
  {
    // While garbage collecting, do not fail allocations. Instead grow
    // the to-space as needed.
    NoAllocationFailureScope scope(to);
    ScavengeVisitor visitor(heap_.space(), to);
    IterateRoots(&visitor);
    if (session_ != NULL) session_->IteratePointers(&visitor);
    {
      VisitProgramRootsVisitor visit_program_roots_visitor(&visitor);
      scheduler()->VisitProcesses(this, &visit_program_roots_visitor);
      VisitProcesses(additional_processes, &visit_program_roots_visitor);
    }
    to->CompleteScavenge(&visitor);
  }
  heap_.ReplaceSpace(to);

  {
    UncookStacksVisitor uncook_visitor;
    scheduler()->VisitProcesses(this, &uncook_visitor);
    uncook_visitor.set_unlink(true);
    VisitProcesses(additional_processes, &uncook_visitor);
  }
  scheduler()->ResumeProgram(this);
}

void Program::Initialize() {
  // Create root set for the Program. During setup, do not fail
  // allocations, instead allocate new chunks.
  NoAllocationFailureScope scope(heap_.space());

  // Create null as the first object other allocated objects can use
  // null_object for initial values.
  InstanceFormat null_format =
      InstanceFormat::instance_format(0, InstanceFormat::NULL_MARKER);
  null_object_ = HeapObject::cast(heap()->Allocate(null_format.fixed_size()));

  meta_class_ = Class::cast(heap()->CreateMetaClass());

  {
    InstanceFormat format = InstanceFormat::array_format();
    array_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
  }

  empty_array_ = Array::cast(CreateArray(0));

  {
    InstanceFormat format = InstanceFormat::instance_format(0);
    object_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
  }

  {
    InstanceFormat format = InstanceFormat::num_format();
    num_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
    num_class_->set_super_class(object_class_);
  }

  {
    InstanceFormat format = InstanceFormat::num_format();
    int_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
    int_class_->set_super_class(num_class_);
  }

  {
    InstanceFormat format = InstanceFormat::smi_format();
    smi_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
    smi_class_->set_super_class(int_class_);
  }

  {
    InstanceFormat format = InstanceFormat::heap_integer_format();
    large_integer_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
    large_integer_class_->set_super_class(int_class_);
  }

  {
    InstanceFormat format = InstanceFormat::double_format();
    double_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
    double_class_->set_super_class(num_class_);
  }

  {
    InstanceFormat format = InstanceFormat::boxed_format();
    boxed_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
  }

  {
    InstanceFormat format = InstanceFormat::stack_format();
    stack_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
  }

  {
    InstanceFormat format =
        InstanceFormat::instance_format(2, InstanceFormat::COROUTINE_MARKER);
    coroutine_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
  }

  {
    InstanceFormat format =
        InstanceFormat::instance_format(1, InstanceFormat::PORT_MARKER);
    port_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
  }

  {
    InstanceFormat format =
        InstanceFormat::instance_format(2, InstanceFormat::FOREIGN_MARKER);
    foreign_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
  }

  {
    InstanceFormat format = InstanceFormat::initializer_format();
    initializer_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
  }

  {
    InstanceFormat format = InstanceFormat::instance_format(1);
    constant_list_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
  }

  {
    InstanceFormat format = InstanceFormat::instance_format(2);
    constant_map_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
  }

  {
    InstanceFormat format = InstanceFormat::string_format();
    string_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
    string_class_->set_super_class(object_class_);
  }

  empty_string_ = String::cast(heap()->CreateString(string_class(), 0));

  {
    InstanceFormat format = InstanceFormat::function_format();
    function_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
  }

  {
    InstanceFormat format = InstanceFormat::byte_array_format();
    byte_array_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
  }

  Class* null_class;
  { // Create null class and singleton.
    null_class =
        Class::cast(heap()->CreateClass(null_format, meta_class_,
                                        null_object_));
    null_class->set_super_class(object_class_);
    null_object_->set_class(null_class);
    null_object_->Initialize(null_format.fixed_size(),
                             null_object_);
  }

  { // Create the bool class.
    InstanceFormat format = InstanceFormat::instance_format(0);
    bool_class_ =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
    bool_class_->set_super_class(object_class_);
  }

  { // Create False class and the false object.
    InstanceFormat format =
        InstanceFormat::instance_format(0, InstanceFormat::FALSE_MARKER);
    Class* false_class =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
    false_class->set_super_class(bool_class_);
    false_class->set_methods(empty_array_);
    false_object_ =
        HeapObject::cast(heap()->CreateHeapObject(false_class, null_object()));
  }

  { // Create True class and the true object.
    InstanceFormat format =
        InstanceFormat::instance_format(0, InstanceFormat::TRUE_MARKER);
    Class* true_class =
        Class::cast(heap()->CreateClass(format, meta_class_, null_object_));
    true_class->set_super_class(bool_class_);
    true_class->set_methods(empty_array_);
    true_object_ =
        HeapObject::cast(heap()->CreateHeapObject(true_class, null_object()));
  }

  // Create the retry after gc failure object payload.
  raw_retry_after_gc_ =
      String::cast(CreateString(StringFromCharZ("Retry after GC.")));

  // Create the wrong argument type failure object payload.
  raw_wrong_argument_type_ =
      String::cast(CreateString(StringFromCharZ("Wrong argument type.")));

  raw_index_out_of_bounds_ =
      String::cast(CreateString(StringFromCharZ("Index out of bounds.")));

  raw_illegal_state_ =
      String::cast(CreateString(StringFromCharZ("Illegal state.")));

  raw_should_preempt_ =
      String::cast(CreateString(StringFromCharZ("Should preempt.")));

  native_failure_result_ = null_object_;
}

bool Program::RunMainInNewProcess() {
  // TODO(ager): GC for testing only.
  CollectGarbage();

  Scheduler scheduler;
  scheduler.ScheduleProgram(this);

  Process* process = SpawnProcess();
  Function* entry = process->entry();
  int main_arity = process->main_arity();
  process->SetupExecutionStack();
  Stack* stack = process->stack();
  uint8_t* bcp = entry->bytecode_address_for(0);
  stack->set(0, Smi::FromWord(main_arity));
  stack->set(1, NULL);
  stack->set(2, reinterpret_cast<Object*>(bcp));
  stack->set_top(2);
  scheduler.EnqueueProcess(process);
  return scheduler.Run();
}

void Program::IterateRoots(PointerVisitor* visitor) {
  visitor->VisitBlock(first_root_address(), last_root_address() + 1);
  visitor->Visit(reinterpret_cast<Object**>(&entry_));
  visitor->Visit(reinterpret_cast<Object**>(&classes_));
  visitor->Visit(reinterpret_cast<Object**>(&constants_));
  visitor->Visit(reinterpret_cast<Object**>(&static_methods_));
  visitor->Visit(reinterpret_cast<Object**>(&static_fields_));
}

}  // namespace fletch