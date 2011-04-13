//===--- DarwinGenericGCC.h - ToolChain Implementations ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_F1D6C9A4_EB69_4A28_A948_4AD775B28070
#define CLANG_F1D6C9A4_EB69_4A28_A948_4AD775B28070

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

#include "GenericGCC.h"

namespace clang {
namespace driver {
namespace toolchains {

/// Darwin_Generic_GCC - Generic Darwin tool chain using gcc.
class LLVM_LIBRARY_VISIBILITY Darwin_Generic_GCC : public Generic_GCC {
/// {{{
public:
  Darwin_Generic_GCC(const HostInfo &Host, const llvm::Triple& Triple)
    : Generic_GCC(Host, Triple) {}

  std::string ComputeEffectiveClangTriple(const ArgList &Args) const;

  virtual const char *GetDefaultRelocationModel() const { return "pic"; }
/// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

