//===--- TCE.h - ToolChain Implementations ----------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_717B0BB8_4F2F_4028_8B1D_68054A1512E7
#define CLANG_717B0BB8_4F2F_4028_8B1D_68054A1512E7

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

namespace clang {
namespace driver {
namespace toolchains {

/// TCE - A tool chain using the llvm bitcode tools to perform
/// all subcommands. See http://tce.cs.tut.fi for our peculiar target.
class LLVM_LIBRARY_VISIBILITY TCE : public ToolChain {
/// {{{
public:
  TCE(const HostInfo &Host, const llvm::Triple& Triple);
  ~TCE();

  virtual Tool &SelectTool(const Compilation &C, const JobAction &JA,
                           const ActionList &Inputs) const;
  bool IsMathErrnoDefault() const;
  bool IsUnwindTablesDefault() const;
  const char* GetDefaultRelocationModel() const;
  const char* GetForcedPicModel() const;

private:
  mutable llvm::DenseMap<unsigned, Tool*> Tools;
/// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

