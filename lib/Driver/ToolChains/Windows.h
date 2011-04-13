//===--- Windows.h - ToolChain Implementations ------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_4AC458A3_CCF5_44EA_8232_9DED45C51BDC
#define CLANG_4AC458A3_CCF5_44EA_8232_9DED45C51BDC

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY Windows : public ToolChain {
/// {{{
  mutable llvm::DenseMap<unsigned, Tool*> Tools;

public:
  Windows(const HostInfo &Host, const llvm::Triple& Triple);

  virtual Tool &SelectTool(const Compilation &C, const JobAction &JA,
                           const ActionList &Inputs) const;

  virtual bool IsIntegratedAssemblerDefault() const;
  virtual bool IsUnwindTablesDefault() const;
  virtual const char *GetDefaultRelocationModel() const;
  virtual const char *GetForcedPicModel() const;
/// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

