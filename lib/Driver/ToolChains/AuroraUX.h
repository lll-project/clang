//===--- AuroraUX.h - ToolChain Implementations -----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_102D4E36_81D7_414A_BA94_CD9F46F879AF
#define CLANG_102D4E36_81D7_414A_BA94_CD9F46F879AF

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

#include "GenericGCC.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY AuroraUX : public Generic_GCC {
/// {{{
public:
  AuroraUX(const HostInfo &Host, const llvm::Triple& Triple);

  virtual Tool &SelectTool(const Compilation &C, const JobAction &JA,
                           const ActionList &Inputs) const;
/// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

