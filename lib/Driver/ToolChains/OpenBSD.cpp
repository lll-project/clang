//===--- OpenBSD.cpp - ToolChain Implementations --------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "OpenBSD.h"

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

/// OpenBSD - OpenBSD tool chain which can call as(1) and ld(1) directly.

OpenBSD::OpenBSD(const HostInfo &Host, const llvm::Triple& Triple)
  : Generic_ELF(Host, Triple) {
  getFilePaths().push_back(getDriver().Dir + "/../lib");
  getFilePaths().push_back("/usr/lib");
}

Tool &OpenBSD::SelectTool(const Compilation &C, const JobAction &JA,
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
    case Action::AssembleJobClass: {
      if (UseIntegratedAs)
        T = new tools::ClangAs(*this);
      else
        T = new tools::openbsd::Assemble(*this);
      break;
    }
    case Action::LinkJobClass:
      T = new tools::openbsd::Link(*this); break;
    default:
      T = &Generic_GCC::SelectTool(C, JA, Inputs);
    }
  }

  return *T;
}

