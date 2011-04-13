//===--- GenericGCC.cpp - ToolChain Implementations -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "GenericGCC.h"

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

/// Generic_GCC - A tool chain using the 'gcc' command to perform
/// all subcommands; this relies on gcc translating the majority of
/// command line options.

Generic_GCC::Generic_GCC(const HostInfo &Host, const llvm::Triple& Triple)
  : ToolChain(Host, Triple) {
  getProgramPaths().push_back(getDriver().getInstalledDir());
  if (getDriver().getInstalledDir() != getDriver().Dir)
    getProgramPaths().push_back(getDriver().Dir);
}

Generic_GCC::~Generic_GCC() {
  // Free tool implementations.
  for (llvm::DenseMap<unsigned, Tool*>::iterator
         it = Tools.begin(), ie = Tools.end(); it != ie; ++it)
    delete it->second;
}

Tool &Generic_GCC::SelectTool(const Compilation &C,
                              const JobAction &JA,
                              const ActionList &Inputs) const {
  Action::ActionClass Key;
  if (getDriver().ShouldUseClangCompiler(C, JA, getTriple()))
    Key = Action::AnalyzeJobClass;
  else
    Key = JA.getKind();

  Tool *&T = Tools[Key];
  if (!T) {
    switch (Key) {
    case Action::InputClass:
    case Action::BindArchClass:
      assert(0 && "Invalid tool kind.");
    case Action::PreprocessJobClass:
      T = new tools::gcc::Preprocess(*this); break;
    case Action::PrecompileJobClass:
      T = new tools::gcc::Precompile(*this); break;
    case Action::AnalyzeJobClass:
      T = new tools::Clang(*this); break;
    case Action::CompileJobClass:
      T = new tools::gcc::Compile(*this); break;
    case Action::AssembleJobClass:
      T = new tools::gcc::Assemble(*this); break;
    case Action::LinkJobClass:
      T = new tools::gcc::Link(*this); break;

      // This is a bit ungeneric, but the only platform using a driver
      // driver is Darwin.
    case Action::LipoJobClass:
      T = new tools::darwin::Lipo(*this); break;
    case Action::DsymutilJobClass:
      T = new tools::darwin::Dsymutil(*this); break;
    }
  }

  return *T;
}

bool Generic_GCC::IsUnwindTablesDefault() const {
  // FIXME: Gross; we should probably have some separate target
  // definition, possibly even reusing the one in clang.
  return getArchName() == "x86_64";
}

const char *Generic_GCC::GetDefaultRelocationModel() const {
  return "static";
}

const char *Generic_GCC::GetForcedPicModel() const {
  return 0;
}

