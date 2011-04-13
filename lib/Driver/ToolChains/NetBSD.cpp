//===--- NetBSD.cpp - ToolChain Implementations ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "NetBSD.h"

#include "clang/Driver/Arg.h"
#include "clang/Driver/ArgList.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/HostInfo.h"
#include "clang/Driver/OptTable.h"
#include "clang/Driver/Option.h"
#include "clang/Driver/Options.h"
#include "clang/Basic/Version.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/system_error.h"

#include <cstdlib> // ::getenv

using namespace clang::driver;
using namespace clang::driver::toolchains;

/// NetBSD - NetBSD tool chain which can call as(1) and ld(1) directly.

NetBSD::NetBSD(const HostInfo &Host, const llvm::Triple& Triple)
  : Generic_ELF(Host, Triple) {

  // Determine if we are compiling 32-bit code on an x86_64 platform.
  bool Lib32 = false;
  if (Triple.getArch() == llvm::Triple::x86 &&
      llvm::Triple(getDriver().DefaultHostTriple).getArch() ==
        llvm::Triple::x86_64)
    Lib32 = true;

  if (getDriver().UseStdLib) {
    if (Lib32)
      getFilePaths().push_back("=/usr/lib/i386");
    else
      getFilePaths().push_back("=/usr/lib");
  }
}

Tool &NetBSD::SelectTool(const Compilation &C, const JobAction &JA,
                         const ActionList &Inputs) const {
  Action::ActionClass Key;
  if (getDriver().ShouldUseClangCompiler(C, JA, getTriple()))
    Key = Action::AnalyzeJobClass;
  else
    Key = JA.getKind();

  bool UseIntegratedAs = C.getArgs().hasFlag(options::OPT_integrated_as,
                                             options::OPT_no_integrated_as,
                                             IsIntegratedAssemblerDefault());

  Tool *&T = Tools[Key];
  if (!T) {
    switch (Key) {
    case Action::AssembleJobClass:
      if (UseIntegratedAs)
        T = new tools::ClangAs(*this);
      else
        T = new tools::netbsd::Assemble(*this);
      break;
    case Action::LinkJobClass:
      T = new tools::netbsd::Link(*this); break;
    default:
      T = &Generic_GCC::SelectTool(C, JA, Inputs);
    }
  }

  return *T;
}

