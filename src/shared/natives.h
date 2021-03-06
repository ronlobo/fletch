// Copyright (c) 2014, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#ifndef SRC_SHARED_NATIVES_H_
#define SRC_SHARED_NATIVES_H_

namespace fletch {

#define NATIVES_DO(N)                                                    \
  N(PrintString,                 "<none>", "_printString")               \
  N(Halt,                        "<none>", "_halt")                      \
  N(ExposeGC,                    "<none>", "_exposeGC")                  \
  N(GC,                          "<none>", "_gc")                        \
                                                                         \
  N(IntParse,                    "int",    "_parse")                     \
                                                                         \
  N(SmiToDouble,                 "_Smi",   "toDouble")                   \
  N(SmiToString,                 "_Smi",   "toString")                   \
  N(SmiToMint,                   "_Smi",   "_toMint")                    \
                                                                         \
  N(SmiNegate,                   "_Smi",   "unary-")                     \
                                                                         \
  N(SmiAdd,                      "_Smi",   "+")                          \
  N(SmiSub,                      "_Smi",   "-")                          \
  N(SmiMul,                      "_Smi",   "*")                          \
  N(SmiMod,                      "_Smi",   "%")                          \
  N(SmiDiv,                      "_Smi",   "/")                          \
  N(SmiTruncDiv,                 "_Smi",   "~/")                         \
                                                                         \
  N(SmiBitNot,                   "_Smi",   "~")                          \
  N(SmiBitAnd,                   "_Smi",   "&")                          \
  N(SmiBitOr,                    "_Smi",   "|")                          \
  N(SmiBitXor,                   "_Smi",   "^")                          \
  N(SmiBitShr,                   "_Smi",   ">>")                         \
  N(SmiBitShl,                   "_Smi",   "<<")                         \
                                                                         \
  N(SmiEqual,                    "_Smi",   "==")                         \
  N(SmiLess,                     "_Smi",   "<")                          \
  N(SmiLessEqual,                "_Smi",   "<=")                         \
  N(SmiGreater,                  "_Smi",   ">")                          \
  N(SmiGreaterEqual,             "_Smi",   ">=")                         \
                                                                         \
  N(MintToDouble,                "_Mint",  "toDouble")                   \
  N(MintToString,                "_Mint",  "toString")                   \
                                                                         \
  N(MintNegate,                  "_Mint",  "unary-")                     \
                                                                         \
  N(MintAdd,                     "_Mint",  "+")                          \
  N(MintSub,                     "_Mint",  "-")                          \
  N(MintMul,                     "_Mint",  "*")                          \
  N(MintMod,                     "_Mint",  "%")                          \
  N(MintDiv,                     "_Mint",  "/")                          \
  N(MintTruncDiv,                "_Mint",  "~/")                         \
                                                                         \
  N(MintBitNot,                  "_Mint",  "~")                          \
  N(MintBitAnd,                  "_Mint",  "&")                          \
  N(MintBitOr,                   "_Mint",  "|")                          \
  N(MintBitXor,                  "_Mint",  "^")                          \
  N(MintBitShr,                  "_Mint",  ">>")                         \
  N(MintBitShl,                  "_Mint",  "<<")                         \
                                                                         \
  N(MintEqual,                   "_Mint",  "==")                         \
  N(MintLess,                    "_Mint",  "<")                          \
  N(MintLessEqual,               "_Mint",  "<=")                         \
  N(MintGreater,                 "_Mint",  ">")                          \
  N(MintGreaterEqual,            "_Mint",  ">=")                         \
                                                                         \
  N(DoubleNegate,                "double", "unary-")                     \
                                                                         \
  N(DoubleAdd,                   "double", "+")                          \
  N(DoubleSub,                   "double", "-")                          \
  N(DoubleMul,                   "double", "*")                          \
  N(DoubleMod,                   "double", "%")                          \
  N(DoubleDiv,                   "double", "/")                          \
  N(DoubleTruncDiv,              "double", "~/")                         \
                                                                         \
  N(DoubleEqual,                 "double", "==")                         \
  N(DoubleLess,                  "double", "<")                          \
  N(DoubleLessEqual,             "double", "<=")                         \
  N(DoubleGreater,               "double", ">")                          \
  N(DoubleGreaterEqual,          "double", ">=")                         \
                                                                         \
  N(DoubleIsNaN,                 "double", "isNaN")                      \
  N(DoubleIsNegative,            "double", "isNegative")                 \
                                                                         \
  N(DoubleCeil,                  "double", "ceil")                       \
  N(DoubleCeilToDouble,          "double", "ceilToDouble")               \
  N(DoubleRound,                 "double", "round")                      \
  N(DoubleRoundToDouble,         "double", "roundToDouble")              \
  N(DoubleFloor,                 "double", "floor")                      \
  N(DoubleFloorToDouble,         "double", "floorToDouble")              \
  N(DoubleTruncate,              "double", "truncate")                   \
  N(DoubleTruncateToDouble,      "double", "truncateToDouble")           \
  N(DoubleRemainder,             "double", "remainder")                  \
  N(DoubleToInt,                 "double", "toInt")                      \
  N(DoubleToString,              "double", "toString")                   \
  N(DoubleToStringAsExponential, "double", "_toStringAsExponential")     \
  N(DoubleToStringAsFixed,       "double", "_toStringAsFixed")           \
  N(DoubleToStringAsPrecision,   "double", "_toStringAsPrecision")       \
                                                                         \
  N(ListNew,                     "_ConstantList", "_new")                \
  N(ListLength,                  "_ConstantList", "length")              \
  N(ListIndexGet,                "_ConstantList", "[]")                  \
                                                                         \
  N(ListIndexSet,                "_FixedList", "[]=")                    \
                                                                         \
  N(ProcessSpawn,                "Process", "_spawn")                    \
  N(ProcessQueueGetMessage,      "Process", "_queueGetMessage")          \
  N(ProcessQueueGetChannel,      "Process", "_queueGetChannel")          \
                                                                         \
  N(CoroutineCurrent,            "Coroutine", "_coroutineCurrent")       \
  N(CoroutineNewStack,           "Coroutine", "_coroutineNewStack")      \
                                                                         \
  N(StopwatchFrequency,          "Stopwatch", "_frequency")              \
  N(StopwatchNow,                "Stopwatch", "_now")                    \
                                                                         \
  N(ForeignLookup,               "Foreign", "_lookup")                   \
  N(ForeignAllocate,             "Foreign", "_allocate")                 \
  N(ForeignFree,                 "Foreign", "_free")                     \
  N(ForeignMarkForFinalization,  "Foreign", "_markForFinalization")      \
  N(ForeignBitsPerWord,          "Foreign", "_bitsPerMachineWord")       \
  N(ForeignErrno,                "Foreign", "_errno")                    \
  N(ForeignPlatform,             "Foreign", "_platform")                 \
  N(ForeignArchitecture,         "Foreign", "_architecture")             \
  N(ForeignConvertPort,          "Foreign", "_convertPort")              \
                                                                         \
  N(ForeignCall0,                "Foreign", "_call$0")                   \
  N(ForeignCall1,                "Foreign", "_call$1")                   \
  N(ForeignCall2,                "Foreign", "_call$2")                   \
  N(ForeignCall3,                "Foreign", "_call$3")                   \
  N(ForeignCall4,                "Foreign", "_call$4")                   \
  N(ForeignCall5,                "Foreign", "_call$5")                   \
  N(ForeignCall6,                "Foreign", "_call$6")                   \
                                                                         \
  N(ForeignLCallwLw,             "Foreign", "_Lcall$wLw")                \
                                                                         \
  N(ForeignGetInt8,              "Foreign", "_getInt8")                  \
  N(ForeignGetInt16,             "Foreign", "_getInt16")                 \
  N(ForeignGetInt32,             "Foreign", "_getInt32")                 \
  N(ForeignGetInt64,             "Foreign", "_getInt64")                 \
                                                                         \
  N(ForeignSetInt8,              "Foreign", "_setInt8")                  \
  N(ForeignSetInt16,             "Foreign", "_setInt16")                 \
  N(ForeignSetInt32,             "Foreign", "_setInt32")                 \
  N(ForeignSetInt64,             "Foreign", "_setInt64")                 \
                                                                         \
  N(ForeignGetUint8,             "Foreign", "_getUint8")                 \
  N(ForeignGetUint16,            "Foreign", "_getUint16")                \
  N(ForeignGetUint32,            "Foreign", "_getUint32")                \
  N(ForeignGetUint64,            "Foreign", "_getUint64")                \
                                                                         \
  N(ForeignSetUint8,             "Foreign", "_setUint8")                 \
  N(ForeignSetUint16,            "Foreign", "_setUint16")                \
  N(ForeignSetUint32,            "Foreign", "_setUint32")                \
  N(ForeignSetUint64,            "Foreign", "_setUint64")                \
                                                                         \
  N(ForeignGetFloat32,            "Foreign", "_getFloat32")              \
  N(ForeignGetFloat64,            "Foreign", "_getFloat64")              \
                                                                         \
  N(ForeignSetFloat32,            "Foreign", "_setFloat32")              \
  N(ForeignSetFloat64,            "Foreign", "_setFloat64")              \
                                                                         \
  N(StringAdd,                   "String", "+")                          \
  N(StringCodeUnitAt,            "String", "codeUnitAt")                 \
  N(StringCreate,                "String", "_create")                    \
  N(StringEqual,                 "String", "==")                         \
  N(StringLength,                "String", "length")                     \
  N(StringSetCodeUnitAt,         "String", "_setCodeUnitAt")             \
  N(StringSubstring,             "String", "_substring")                 \
                                                                         \
  N(PortCreate,                  "Port", "_create")                      \
  N(PortClose,                   "Port", "_close")                       \
  N(PortSend,                    "Port", "send")                         \
  N(PortIncrementRef,            "Port", "_incrementRef")                \
                                                                         \
  N(SystemGetEventHandler,       "System", "_getEventHandler")           \
                                                                         \
  N(ServiceRegister,             "<none>", "register")                   \

enum Native {
#define N(e, c, n) k##e,
NATIVES_DO(N)
#undef N
  kNumberOfNatives
};

}  // namespace fletch

#endif  // SRC_SHARED_NATIVES_H_
