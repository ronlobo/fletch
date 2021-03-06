// Copyright (c) 2015, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

library fletchc.function_codegen;

import 'package:compiler/src/constants/expressions.dart' show
    ConstantExpression;

import 'package:compiler/src/dart2jslib.dart' show
    MessageKind,
    Registry;

import 'package:compiler/src/elements/elements.dart';
import 'package:compiler/src/resolution/resolution.dart';
import 'package:compiler/src/tree/tree.dart';
import 'package:compiler/src/universe/universe.dart';
import 'package:compiler/src/util/util.dart' show Spannable;
import 'package:compiler/src/dart_types.dart';

import 'fletch_context.dart';

import 'fletch_backend.dart';

import 'fletch_constants.dart' show
    CompiledFunctionConstant,
    FletchClassConstant;

import '../bytecodes.dart' show
    Bytecode;

import 'compiled_function.dart' show
    CompiledFunction;

import 'closure_environment.dart';

import 'codegen_visitor.dart';

class FunctionCodegen extends CodegenVisitor {

  int setterResultSlot;

  FunctionCodegen(CompiledFunction compiledFunction,
                  FletchContext context,
                  TreeElements elements,
                  Registry registry,
                  ClosureEnvironment closureEnvironment,
                  FunctionElement function)
      : super(compiledFunction, context, elements, registry,
              closureEnvironment, function);

  FunctionElement get function => element;

  void compile() {
    ClassElement enclosing = function.enclosingClass;
    // Generate implicit 'null' check for '==' functions, except for Null.
    if (enclosing != null &&
        enclosing.declaration != context.compiler.nullClass &&
        function.name == '==') {
      BytecodeLabel notNull = new BytecodeLabel();
      builder.loadParameter(1);
      builder.loadLiteralNull();
      builder.identicalNonNumeric();
      builder.branchIfFalse(notNull);
      builder.loadLiteralFalse();
      builder.ret();
      builder.bind(notNull);
    }

    FunctionSignature functionSignature = function.functionSignature;
    int parameterCount = functionSignature.parameterCount;

    // If the function is a setter, push the argument to later be returned.
    // TODO(ajohnsen): If the argument is semantically final, we don't have to
    // do this.
    if (function.isSetter) {
      setterResultSlot = - parameterCount - 1;
      builder.loadSlot(setterResultSlot);
    }

    int i = 0;
    functionSignature.orderedForEachParameter((ParameterElement parameter) {
      int slot = i++ - parameterCount - 1;
      scope[parameter] = createLocalValueForParameter(parameter, slot);
    });

    ClosureInfo info = closureEnvironment.closures[function];
    if (info != null) {
      int index = 0;
      if (info.isThisFree) {
        thisValue = new UnboxedLocalValue(builder.stackSize, null);
        builder.loadParameter(0);
        builder.loadField(index++);
      }
      for (LocalElement local in info.free) {
        scope[local] = createLocalValueFor(local);
        // TODO(ajohnsen): Use a specialized helper for loading the closure.
        builder.loadParameter(0);
        builder.loadField(index++);
      }
    }

    FunctionExpression node = function.node;
    if (node != null) {
      node.body.accept(this);
    }

    // Emit implicit 'return null' if no terminator is present.
    if (!builder.endsWithTerminator) {
      if (function.isSetter) {
        builder.loadSlot(setterResultSlot);
      } else {
        builder.loadLiteralNull();
      }
      builder.ret();
    }

    builder.methodEnd();
  }

  void optionalReplaceResultValue() {
    if (function.isSetter) {
      builder.pop();
      builder.loadSlot(setterResultSlot);
    }
  }
}
