//===--- GenericGCC.h - ToolChain Implementations ---------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_B4C414A3_6FDC_4091_9CA9_5DF10D8FCFFD
#define CLANG_B4C414A3_6FDC_4091_9CA9_5DF10D8FCFFD

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

namespace clang {
namespace driver {
namespace toolchains {

/// Generic_GCC - A tool chain using the 'gcc' command to perform
/// all subcommands; this relies on gcc translating the majority of
/// command line options.
class LLVM_LIBRARY_VISIBILITY Generic_GCC : public ToolChain {
/// {{{
protected:
  mutable llvm::DenseMap<unsigned, Tool*> Tools;

public:
  Generic_GCC(const HostInfo &Host, const llvm::Triple& Triple);
  ~Generic_GCC();

  virtual Tool &SelectTool(const Compilation &C, const JobAction &JA,
                           const ActionList &Inputs) const;

  virtual bool IsUnwindTablesDefault() const;
  virtual const char *GetDefaultRelocationModel() const;
  virtual const char *GetForcedPicModel() const;
/// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

