//===--- NetBSD.h - ToolChain Implementations -------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_4068CFB9_8EDF_4C12_BAC7_85223AA3FE8F
#define CLANG_4068CFB9_8EDF_4C12_BAC7_85223AA3FE8F

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

#include "GenericELF.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY NetBSD : public Generic_ELF {
/// {{{
public:
  NetBSD(const HostInfo &Host, const llvm::Triple& Triple);

  virtual Tool &SelectTool(const Compilation &C, const JobAction &JA,
                           const ActionList &Inputs) const;
/// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

