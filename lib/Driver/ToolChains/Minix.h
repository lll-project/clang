//===--- Minix.h - ToolChain Implementations --------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_CE0F9B78_EBCA_4A9E_875F_47D8AB3A8FBF
#define CLANG_CE0F9B78_EBCA_4A9E_875F_47D8AB3A8FBF

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

#include "GenericGCC.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY Minix : public Generic_GCC {
/// {{{
public:
  Minix(const HostInfo &Host, const llvm::Triple& Triple);

  virtual Tool &SelectTool(const Compilation &C, const JobAction &JA,
                           const ActionList &Inputs) const;
/// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

