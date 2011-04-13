//===--- AuroraUX.cpp - ToolChain Implementations -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "AuroraUX.h"

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

/// AuroraUX - AuroraUX tool chain which can call as(1) and ld(1) directly.

AuroraUX::AuroraUX(const HostInfo &Host, const llvm::Triple& Triple)
  : Generic_GCC(Host, Triple) {

  getProgramPaths().push_back(getDriver().getInstalledDir());
  if (getDriver().getInstalledDir() != getDriver().Dir)
    getProgramPaths().push_back(getDriver().Dir);

  getFilePaths().push_back(getDriver().Dir + "/../lib");
  getFilePaths().push_back("/usr/lib");
  getFilePaths().push_back("/usr/sfw/lib");
  getFilePaths().push_back("/opt/gcc4/lib");
  getFilePaths().push_back("/opt/gcc4/lib/gcc/i386-pc-solaris2.11/4.2.4");

}

Tool &AuroraUX::SelectTool(const Compilation &C, const JobAction &JA,
                           const ActionList &Inputs) const {
  Action::ActionClass Key;
  if (getDriver().ShouldUseClangCompiler(C, JA, getTriple()))
    Key = Action::AnalyzeJobClass;
  else
    Key = JA.getKind();

  Tool *&T = Tools[Key];
  if (!T) {
    switch (Key) {
    case Action::AssembleJobClass:
      T = new tools::auroraux::Assemble(*this); break;
    case Action::LinkJobClass:
      T = new tools::auroraux::Link(*this); break;
    default:
      T = &Generic_GCC::SelectTool(C, JA, Inputs);
    }
  }

  return *T;
}

