//===--- OpenBSD.h - ToolChain Implementations ------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_7DE60350_2C3B_4BA5_A783_4501700DF9DA
#define CLANG_7DE60350_2C3B_4BA5_A783_4501700DF9DA

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

#include "GenericELF.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY OpenBSD : public Generic_ELF {
/// {{{
public:
  OpenBSD(const HostInfo &Host, const llvm::Triple& Triple);

  virtual Tool &SelectTool(const Compilation &C, const JobAction &JA,
                           const ActionList &Inputs) const;
// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

