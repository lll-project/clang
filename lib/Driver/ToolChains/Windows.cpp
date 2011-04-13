//===--- Windows.cpp - ToolChain Implementations --------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Windows.h"

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

Windows::Windows(const HostInfo &Host, const llvm::Triple& Triple)
  : ToolChain(Host, Triple) {
}

Tool &Windows::SelectTool(const Compilation &C, const JobAction &JA,
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
    case Action::LipoJobClass:
    case Action::DsymutilJobClass:
      assert(0 && "Invalid tool kind.");
    case Action::PreprocessJobClass:
    case Action::PrecompileJobClass:
    case Action::AnalyzeJobClass:
    case Action::CompileJobClass:
      T = new tools::Clang(*this); break;
    case Action::AssembleJobClass:
      T = new tools::ClangAs(*this); break;
    case Action::LinkJobClass:
      T = new tools::visualstudio::Link(*this); break;
    }
  }

  return *T;
}

bool Windows::IsIntegratedAssemblerDefault() const {
  return true;
}

bool Windows::IsUnwindTablesDefault() const {
  // FIXME: Gross; we should probably have some separate target
  // definition, possibly even reusing the one in clang.
  return getArchName() == "x86_64";
}

const char *Windows::GetDefaultRelocationModel() const {
  return "static";
}

const char *Windows::GetForcedPicModel() const {
  if (getArchName() == "x86_64")
    return "pic";
  return 0;
}
