//===--- DarwinClang.cpp - ToolChain Implementations ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "DarwinClang.h"

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

DarwinClang::DarwinClang(const HostInfo &Host, const llvm::Triple& Triple)
  : Darwin(Host, Triple)
{
  std::string UsrPrefix = "llvm-gcc-4.2/";

  getProgramPaths().push_back(getDriver().getInstalledDir());
  if (getDriver().getInstalledDir() != getDriver().Dir)
    getProgramPaths().push_back(getDriver().Dir);

  // We expect 'as', 'ld', etc. to be adjacent to our install dir.
  getProgramPaths().push_back(getDriver().getInstalledDir());
  if (getDriver().getInstalledDir() != getDriver().Dir)
    getProgramPaths().push_back(getDriver().Dir);

  // For fallback, we need to know how to find the GCC cc1 executables, so we
  // also add the GCC libexec paths. This is legacy code that can be removed
  // once fallback is no longer useful.
  std::string ToolChainDir = "i686-apple-darwin";
  ToolChainDir += llvm::utostr(DarwinVersion[0]);
  ToolChainDir += "/4.2.1";

  std::string Path = getDriver().Dir;
  Path += "/../" + UsrPrefix + "libexec/gcc/";
  Path += ToolChainDir;
  getProgramPaths().push_back(Path);

  Path = "/usr/" + UsrPrefix + "libexec/gcc/";
  Path += ToolChainDir;
  getProgramPaths().push_back(Path);
}

void DarwinClang::AddLinkSearchPathArgs(const ArgList &Args,
                                       ArgStringList &CmdArgs) const {
  // The Clang toolchain uses explicit paths for internal libraries.

  // Unfortunately, we still might depend on a few of the libraries that are
  // only available in the gcc library directory (in particular
  // libstdc++.dylib). For now, hardcode the path to the known install location.
  llvm::sys::Path P(getDriver().Dir);
  P.eraseComponent(); // .../usr/bin -> ../usr
  P.appendComponent("lib");
  P.appendComponent("gcc");
  switch (getTriple().getArch()) {
  default:
    assert(0 && "Invalid Darwin arch!");
  case llvm::Triple::x86:
  case llvm::Triple::x86_64:
    P.appendComponent("i686-apple-darwin10");
    break;
  case llvm::Triple::arm:
  case llvm::Triple::thumb:
    P.appendComponent("arm-apple-darwin10");
    break;
  case llvm::Triple::ppc:
  case llvm::Triple::ppc64:
    P.appendComponent("powerpc-apple-darwin10");
    break;
  }
  P.appendComponent("4.2.1");

  // Determine the arch specific GCC subdirectory.
  const char *ArchSpecificDir = 0;
  switch (getTriple().getArch()) {
  default:
    break;
  case llvm::Triple::arm:
  case llvm::Triple::thumb: {
    std::string Triple = ComputeLLVMTriple(Args);
    llvm::StringRef TripleStr = Triple;
    if (TripleStr.startswith("armv5") || TripleStr.startswith("thumbv5"))
      ArchSpecificDir = "v5";
    else if (TripleStr.startswith("armv6") || TripleStr.startswith("thumbv6"))
      ArchSpecificDir = "v6";
    else if (TripleStr.startswith("armv7") || TripleStr.startswith("thumbv7"))
      ArchSpecificDir = "v7";
    break;
  }
  case llvm::Triple::ppc64:
    ArchSpecificDir = "ppc64";
    break;
  case llvm::Triple::x86_64:
    ArchSpecificDir = "x86_64";
    break;
  }

  if (ArchSpecificDir) {
    P.appendComponent(ArchSpecificDir);
    bool Exists;
    if (!llvm::sys::fs::exists(P.str(), Exists) && Exists)
      CmdArgs.push_back(Args.MakeArgString("-L" + P.str()));
    P.eraseComponent();
  }

  bool Exists;
  if (!llvm::sys::fs::exists(P.str(), Exists) && Exists)
    CmdArgs.push_back(Args.MakeArgString("-L" + P.str()));
}

void DarwinClang::AddLinkRuntimeLibArgs(const ArgList &Args,
                                        ArgStringList &CmdArgs) const {
  // Darwin doesn't support real static executables, don't link any runtime
  // libraries with -static.
  if (Args.hasArg(options::OPT_static))
    return;

  // Reject -static-libgcc for now, we can deal with this when and if someone
  // cares. This is useful in situations where someone wants to statically link
  // something like libstdc++, and needs its runtime support routines.
  if (const Arg *A = Args.getLastArg(options::OPT_static_libgcc)) {
    getDriver().Diag(clang::diag::err_drv_unsupported_opt)
      << A->getAsString(Args);
    return;
  }

  // Otherwise link libSystem, then the dynamic runtime library, and finally any
  // target specific static runtime library.
  CmdArgs.push_back("-lSystem");

  // Select the dynamic runtime library and the target specific static library.
  const char *DarwinStaticLib = 0;
  if (isTargetIPhoneOS()) {
    CmdArgs.push_back("-lgcc_s.1");

    // We may need some static functions for armv6/thumb which are required to
    // be in the same linkage unit as their caller.
    if (getDarwinArchName(Args) == "armv6")
      DarwinStaticLib = "libclang_rt.armv6.a";
  } else {
    // The dynamic runtime library was merged with libSystem for 10.6 and
    // beyond; only 10.4 and 10.5 need an additional runtime library.
    if (isMacosxVersionLT(10, 5))
      CmdArgs.push_back("-lgcc_s.10.4");
    else if (isMacosxVersionLT(10, 6))
      CmdArgs.push_back("-lgcc_s.10.5");

    // For OS X, we thought we would only need a static runtime library when
    // targetting 10.4, to provide versions of the static functions which were
    // omitted from 10.4.dylib.
    //
    // Unfortunately, that turned out to not be true, because Darwin system
    // headers can still use eprintf on i386, and it is not exported from
    // libSystem. Therefore, we still must provide a runtime library just for
    // the tiny tiny handful of projects that *might* use that symbol.
    if (isMacosxVersionLT(10, 5)) {
      DarwinStaticLib = "libclang_rt.10.4.a";
    } else {
      if (getTriple().getArch() == llvm::Triple::x86)
        DarwinStaticLib = "libclang_rt.eprintf.a";
    }
  }

  /// Add the target specific static library, if needed.
  if (DarwinStaticLib) {
    llvm::sys::Path P(getDriver().ResourceDir);
    P.appendComponent("lib");
    P.appendComponent("darwin");
    P.appendComponent(DarwinStaticLib);

    // For now, allow missing resource libraries to support developers who may
    // not have compiler-rt checked out or integrated into their build.
    bool Exists;
    if (!llvm::sys::fs::exists(P.str(), Exists) && Exists)
      CmdArgs.push_back(Args.MakeArgString(P.str()));
  }
}

void DarwinClang::AddCXXStdlibLibArgs(const ArgList &Args,
                                      ArgStringList &CmdArgs) const {
  CXXStdlibType Type = GetCXXStdlibType(Args);

  switch (Type) {
  case ToolChain::CST_Libcxx:
    CmdArgs.push_back("-lc++");
    break;

  case ToolChain::CST_Libstdcxx: {
    // Unfortunately, -lstdc++ doesn't always exist in the standard search path;
    // it was previously found in the gcc lib dir. However, for all the Darwin
    // platforms we care about it was -lstdc++.6, so we search for that
    // explicitly if we can't see an obvious -lstdc++ candidate.

    // Check in the sysroot first.
    bool Exists;
    if (const Arg *A = Args.getLastArg(options::OPT_isysroot)) {
      llvm::sys::Path P(A->getValue(Args));
      P.appendComponent("usr");
      P.appendComponent("lib");
      P.appendComponent("libstdc++.dylib");

      if (llvm::sys::fs::exists(P.str(), Exists) || !Exists) {
        P.eraseComponent();
        P.appendComponent("libstdc++.6.dylib");
        if (!llvm::sys::fs::exists(P.str(), Exists) && Exists) {
          CmdArgs.push_back(Args.MakeArgString(P.str()));
          return;
        }
      }
    }

    // Otherwise, look in the root.
    if ((llvm::sys::fs::exists("/usr/lib/libstdc++.dylib", Exists) || !Exists)&&
      (!llvm::sys::fs::exists("/usr/lib/libstdc++.6.dylib", Exists) && Exists)){
      CmdArgs.push_back("/usr/lib/libstdc++.6.dylib");
      return;
    }

    // Otherwise, let the linker search.
    CmdArgs.push_back("-lstdc++");
    break;
  }
  }
}

void DarwinClang::AddCCKextLibArgs(const ArgList &Args,
                                   ArgStringList &CmdArgs) const {
  // For Darwin platforms, use the compiler-rt-based support library
  // instead of the gcc-provided one (which is also incidentally
  // only present in the gcc lib dir, which makes it hard to find).

  llvm::sys::Path P(getDriver().ResourceDir);
  P.appendComponent("lib");
  P.appendComponent("darwin");
  P.appendComponent("libclang_rt.cc_kext.a");
  
  // For now, allow missing resource libraries to support developers who may
  // not have compiler-rt checked out or integrated into their build.
  bool Exists;
  if (!llvm::sys::fs::exists(P.str(), Exists) && Exists)
    CmdArgs.push_back(Args.MakeArgString(P.str()));
}

