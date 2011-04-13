//===--- DragonFly.h - ToolChain Implementations ----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_A311E3AA_6FDC_4C8E_91D7_F30DDEADB04A
#define CLANG_A311E3AA_6FDC_4C8E_91D7_F30DDEADB04A

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

#include "DragonFly.h"

#include "GenericELF.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY DragonFly : public Generic_ELF {
/// {{{
public:
  DragonFly(const HostInfo &Host, const llvm::Triple& Triple);

  virtual Tool &SelectTool(const Compilation &C, const JobAction &JA,
                           const ActionList &Inputs) const;
/// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

