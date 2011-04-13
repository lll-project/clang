//===--- FreeBSD.h - ToolChain Implementations ------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_4695A8EB_9878_45FC_A9D3_05B8B92B047C
#define CLANG_4695A8EB_9878_45FC_A9D3_05B8B92B047C

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

#include "GenericELF.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY FreeBSD : public Generic_ELF {
/// {{{
public:
  FreeBSD(const HostInfo &Host, const llvm::Triple& Triple);

  virtual Tool &SelectTool(const Compilation &C, const JobAction &JA,
                           const ActionList &Inputs) const;
/// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

