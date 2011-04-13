//===--- DarwinClang.h - ToolChain Implementations --------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_3F1D0B77_215C_48F0_842C_E31D55A450A8
#define CLANG_3F1D0B77_215C_48F0_842C_E31D55A450A8

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

#include "Darwin.h"

namespace clang {
namespace driver {
namespace toolchains {

/// DarwinClang - The Darwin toolchain used by Clang.
class LLVM_LIBRARY_VISIBILITY DarwinClang : public Darwin {
/// {{{
public:
  DarwinClang(const HostInfo &Host, const llvm::Triple& Triple);

  /// @name Darwin ToolChain Implementation
  /// {

  virtual void AddLinkSearchPathArgs(const ArgList &Args,
                                    ArgStringList &CmdArgs) const;

  virtual void AddLinkRuntimeLibArgs(const ArgList &Args,
                                     ArgStringList &CmdArgs) const;

  virtual void AddCXXStdlibLibArgs(const ArgList &Args,
                                   ArgStringList &CmdArgs) const;

  virtual void AddCCKextLibArgs(const ArgList &Args,
                                ArgStringList &CmdArgs) const;

  /// }
/// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

