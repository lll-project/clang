//===--- GenericELF.h - ToolChain Implementations ---------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_B095DFA6_4BD3_4970_9379_52119102E1FB
#define CLANG_B095DFA6_4BD3_4970_9379_52119102E1FB

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

#include "GenericGCC.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY Generic_ELF : public Generic_GCC {
/// {{{
 public:
  Generic_ELF(const HostInfo &Host, const llvm::Triple& Triple)
    : Generic_GCC(Host, Triple) {}

  virtual bool IsIntegratedAssemblerDefault() const {
    // Default integrated assembler to on for x86.
    return (getTriple().getArch() == llvm::Triple::x86 ||
            getTriple().getArch() == llvm::Triple::x86_64);
  }
/// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

