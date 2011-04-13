//===--- Linux.h - ToolChain Implementations --------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_5A4E633D_1FEB_4759_90F7_4134120C360E
#define CLANG_5A4E633D_1FEB_4759_90F7_4134120C360E

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

#include "GenericELF.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY Linux : public Generic_ELF {
/// {{{
public:
  Linux(const HostInfo &Host, const llvm::Triple& Triple);

  virtual bool HasNativeLLVMSupport() const;

  virtual Tool &SelectTool(const Compilation &C, const JobAction &JA,
                           const ActionList &Inputs) const;

  std::string Linker;
  std::vector<std::string> ExtraOpts;
/// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

