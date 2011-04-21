//===--- Linux.cpp - ToolChain Implementations ----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Linux.h"

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

/// Linux toolchain (very bare-bones at the moment).

enum LinuxDistro {
  ArchLinux,
  DebianLenny,
  DebianSqueeze,
  Exherbo,
  RHEL6,
  RHEL5,
  RHEL4,
  Fedora13,
  Fedora14,
  Fedora15,
  FedoraRawhide,
  OpenSuse11_3,
  UbuntuHardy,
  UbuntuIntrepid,
  UbuntuJaunty,
  UbuntuKarmic,
  UbuntuLucid,
  UbuntuMaverick,
  UbuntuNatty,
  UnknownDistro
};

static bool IsRedHat(enum LinuxDistro Distro) {
  return Distro == Fedora13 || Distro == Fedora14 ||
         Distro == Fedora15 || Distro == FedoraRawhide;
}

static bool IsOpenSuse(enum LinuxDistro Distro) {
  return Distro == OpenSuse11_3;
}

static bool IsDebian(enum LinuxDistro Distro) {
  return Distro == DebianLenny || Distro == DebianSqueeze;
}

static bool IsUbuntu(enum LinuxDistro Distro) {
  return Distro == UbuntuHardy  || Distro == UbuntuIntrepid ||
         Distro == UbuntuLucid  || Distro == UbuntuMaverick || 
         Distro == UbuntuJaunty || Distro == UbuntuKarmic ||
         Distro == UbuntuNatty;
}

static bool IsDebianBased(enum LinuxDistro Distro) {
  return IsDebian(Distro) || IsUbuntu(Distro);
}

static bool HasMultilib(llvm::Triple::ArchType Arch, enum LinuxDistro Distro) {
  if (Arch == llvm::Triple::x86_64) {
    bool Exists;
    if (Distro == Exherbo &&
        (llvm::sys::fs::exists("/usr/lib32/libc.so", Exists) || !Exists))
      return false;

    return true;
  }
  if (Arch == llvm::Triple::ppc64)
    return true;
  if ((Arch == llvm::Triple::x86 || Arch == llvm::Triple::ppc) && IsDebianBased(Distro))
    return true;
  return false;
}

static LinuxDistro DetectLinuxDistro(llvm::Triple::ArchType Arch) {
  llvm::OwningPtr<llvm::MemoryBuffer> File;
  if (!llvm::MemoryBuffer::getFile("/etc/lsb-release", File)) {
    llvm::StringRef Data = File.get()->getBuffer();
    llvm::SmallVector<llvm::StringRef, 8> Lines;
    Data.split(Lines, "\n");
    for (unsigned int i = 0, s = Lines.size(); i < s; ++ i) {
      if (Lines[i] == "DISTRIB_CODENAME=hardy")
        return UbuntuHardy;
      else if (Lines[i] == "DISTRIB_CODENAME=intrepid")
        return UbuntuIntrepid;
      else if (Lines[i] == "DISTRIB_CODENAME=jaunty")
        return UbuntuJaunty;
      else if (Lines[i] == "DISTRIB_CODENAME=karmic")
        return UbuntuKarmic;
      else if (Lines[i] == "DISTRIB_CODENAME=lucid")
        return UbuntuLucid;
      else if (Lines[i] == "DISTRIB_CODENAME=maverick")
        return UbuntuMaverick;
      else if (Lines[i] == "DISTRIB_CODENAME=natty")
        return UbuntuNatty;
    }
    return UnknownDistro;
  }

  if (!llvm::MemoryBuffer::getFile("/etc/redhat-release", File)) {
    llvm::StringRef Data = File.get()->getBuffer();
    if (Data.startswith("Fedora release 15"))
      return Fedora15;
    else if (Data.startswith("Fedora release 14"))
      return Fedora14;
    else if (Data.startswith("Fedora release 13"))
      return Fedora13;
    else if (Data.startswith("Fedora release") &&
             Data.find("Rawhide") != llvm::StringRef::npos)
      return FedoraRawhide;
    else if (Data.startswith("Red Hat Enterprise Linux") &&
             Data.find("release 6") != llvm::StringRef::npos)
      return RHEL6;
    else if (Data.startswith("Red Hat Enterprise Linux") &&
             Data.find("release 5") != llvm::StringRef::npos)
      return RHEL6;
    else if (Data.startswith("Red Hat Enterprise Linux") &&
             Data.find("release 4") != llvm::StringRef::npos)
      return RHEL6;
    return UnknownDistro;
  }

  if (!llvm::MemoryBuffer::getFile("/etc/debian_version", File)) {
    llvm::StringRef Data = File.get()->getBuffer();
    if (Data[0] == '5')
      return DebianLenny;
    else if (Data.startswith("squeeze/sid"))
      return DebianSqueeze;
    return UnknownDistro;
  }

  if (!llvm::MemoryBuffer::getFile("/etc/SuSE-release", File)) {
    llvm::StringRef Data = File.get()->getBuffer();
    if (Data.startswith("openSUSE 11.3"))
      return OpenSuse11_3;
    return UnknownDistro;
  }

  bool Exists;
  if (!llvm::sys::fs::exists("/etc/exherbo-release", Exists) && Exists)
    return Exherbo;

  if (!llvm::sys::fs::exists("/etc/arch-release", Exists) && Exists)
    return ArchLinux;

  return UnknownDistro;
}

Linux::Linux(const HostInfo &Host, const llvm::Triple &Triple)
  : Generic_ELF(Host, Triple) {
  llvm::Triple::ArchType Arch =
    llvm::Triple(getDriver().DefaultHostTriple).getArch();

  std::string Suffix32  = "";
  if (Arch == llvm::Triple::x86_64)
    Suffix32 = "/32";

  std::string Suffix64  = "";
  if (Arch == llvm::Triple::x86 || Arch == llvm::Triple::ppc)
    Suffix64 = "/64";

  std::string Lib32 = "lib";

  bool Exists;
  if (!llvm::sys::fs::exists("/lib32", Exists) && Exists)
    Lib32 = "lib32";

  std::string Lib64 = "lib";
  bool Symlink;
  if (!llvm::sys::fs::exists("/lib64", Exists) && Exists &&
      (llvm::sys::fs::is_symlink("/lib64", Symlink) || !Symlink))
    Lib64 = "lib64";

  std::string GccTriple = "";
  if (Arch == llvm::Triple::arm || Arch == llvm::Triple::thumb) {
    if (!llvm::sys::fs::exists("/usr/lib/gcc/arm-linux-gnueabi", Exists) &&
        Exists)
      GccTriple = "arm-linux-gnueabi";
  } else if (Arch == llvm::Triple::x86_64) {
    if (!llvm::sys::fs::exists("/usr/lib/gcc/x86_64-linux-gnu", Exists) &&
        Exists)
      GccTriple = "x86_64-linux-gnu";
    else if (!llvm::sys::fs::exists("/usr/lib/gcc/x86_64-unknown-linux-gnu",
             Exists) && Exists)
      GccTriple = "x86_64-unknown-linux-gnu";
    else if (!llvm::sys::fs::exists("/usr/lib/gcc/x86_64-pc-linux-gnu",
             Exists) && Exists)
      GccTriple = "x86_64-pc-linux-gnu";
    else if (!llvm::sys::fs::exists("/usr/lib/gcc/x86_64-redhat-linux",
             Exists) && Exists)
      GccTriple = "x86_64-redhat-linux";
    else if (!llvm::sys::fs::exists("/usr/lib64/gcc/x86_64-suse-linux",
             Exists) && Exists)
      GccTriple = "x86_64-suse-linux";
    else if (!llvm::sys::fs::exists("/usr/lib/gcc/x86_64-manbo-linux-gnu",
             Exists) && Exists)
      GccTriple = "x86_64-manbo-linux-gnu";
  } else if (Arch == llvm::Triple::x86) {
    if (!llvm::sys::fs::exists("/usr/lib/gcc/i686-linux-gnu", Exists) && Exists)
      GccTriple = "i686-linux-gnu";
    else if (!llvm::sys::fs::exists("/usr/lib/gcc/i686-pc-linux-gnu", Exists) &&
             Exists)
      GccTriple = "i686-pc-linux-gnu";
    else if (!llvm::sys::fs::exists("/usr/lib/gcc/i486-linux-gnu", Exists) &&
             Exists)
      GccTriple = "i486-linux-gnu";
    else if (!llvm::sys::fs::exists("/usr/lib/gcc/i686-redhat-linux", Exists) &&
             Exists)
      GccTriple = "i686-redhat-linux";
    else if (!llvm::sys::fs::exists("/usr/lib/gcc/i586-suse-linux", Exists) &&
             Exists)
      GccTriple = "i586-suse-linux";
    else if (!llvm::sys::fs::exists("/usr/lib/gcc/i486-slackware-linux", Exists) &&
             Exists)
      GccTriple = "i486-suse-linux";
  } else if (Arch == llvm::Triple::ppc) {
    if (!llvm::sys::fs::exists("/usr/lib/powerpc-linux-gnu", Exists) && Exists)
      GccTriple = "powerpc-linux-gnu";
    else if (!llvm::sys::fs::exists("/usr/lib/gcc/powerpc-unknown-linux-gnu", Exists) && Exists)
      GccTriple = "powerpc-unknown-linux-gnu";
  } else if (Arch == llvm::Triple::ppc64) {
    if (!llvm::sys::fs::exists("/usr/lib/gcc/powerpc64-unknown-linux-gnu", Exists) && Exists)
      GccTriple = "powerpc64-unknown-linux-gnu";
    else if (!llvm::sys::fs::exists("/usr/lib64/gcc/powerpc64-unknown-linux-gnu", Exists) && Exists)
      GccTriple = "powerpc64-unknown-linux-gnu";
  }

  const char* GccVersions[] = {"4.6.0",
                               "4.5.2", "4.5.1", "4.5",
                               "4.4.5", "4.4.4", "4.4.3", "4.4",
                               "4.3.4", "4.3.3", "4.3.2", "4.3",
                               "4.2.4", "4.2.3", "4.2.2", "4.2.1", "4.2"};
  std::string Base = "";
  for (unsigned i = 0; i < sizeof(GccVersions)/sizeof(char*); ++i) {
    std::string Suffix = GccTriple + "/" + GccVersions[i];
    std::string t1 = "/usr/lib/gcc/" + Suffix;
    if (!llvm::sys::fs::exists(t1 + "/crtbegin.o", Exists) && Exists) {
      Base = t1;
      break;
    }
    std::string t2 = "/usr/lib64/gcc/" + Suffix;
    if (!llvm::sys::fs::exists(t2 + "/crtbegin.o", Exists) && Exists) {
      Base = t2;
      break;
    }
    std::string t3 = "/usr/lib/" + GccTriple + "/gcc/" + Suffix;
    if (!llvm::sys::fs::exists(t3 + "/crtbegin.o", Exists) && Exists) {
      Base = t3;
      break;
    }
  }

  path_list &Paths = getFilePaths();
  bool Is32Bits = (getArch() == llvm::Triple::x86 || getArch() == llvm::Triple::ppc);

  std::string Suffix;
  std::string Lib;

  if (Is32Bits) {
    Suffix = Suffix32;
    Lib = Lib32;
  } else {
    Suffix = Suffix64;
    Lib = Lib64;
  }

  llvm::sys::Path LinkerPath(Base + "/../../../../" + GccTriple + "/bin/ld");
  if (!llvm::sys::fs::exists(LinkerPath.str(), Exists) && Exists)
    Linker = LinkerPath.str();
  else
    Linker = GetProgramPath("ld");

  LinuxDistro Distro = DetectLinuxDistro(Arch);

  if (IsUbuntu(Distro)) {
    ExtraOpts.push_back("-z");
    ExtraOpts.push_back("relro");
  }

  if (Arch == llvm::Triple::arm || Arch == llvm::Triple::thumb)
    ExtraOpts.push_back("-X");

  if (IsRedHat(Distro) || Distro == UbuntuMaverick)
    ExtraOpts.push_back("--hash-style=gnu");

  if (IsDebian(Distro) || Distro == UbuntuLucid || Distro == UbuntuJaunty ||
      Distro == UbuntuKarmic)
    ExtraOpts.push_back("--hash-style=both");

  if (IsRedHat(Distro))
    ExtraOpts.push_back("--no-add-needed");

  if (Distro == DebianSqueeze || IsOpenSuse(Distro) ||
      IsRedHat(Distro) || Distro == UbuntuLucid || Distro == UbuntuMaverick ||
      Distro == UbuntuKarmic)
    ExtraOpts.push_back("--build-id");

  if (Distro == ArchLinux)
    Lib = "lib";

  Paths.push_back(Base + Suffix);
  if (HasMultilib(Arch, Distro)) {
    if (IsOpenSuse(Distro) && Is32Bits)
      Paths.push_back(Base + "/../../../../" + GccTriple + "/lib/../lib");
    Paths.push_back(Base + "/../../../../" + Lib);
    Paths.push_back("/lib/../" + Lib);
    Paths.push_back("/usr/lib/../" + Lib);
  }
  if (!Suffix.empty())
    Paths.push_back(Base);
  if (IsOpenSuse(Distro))
    Paths.push_back(Base + "/../../../../" + GccTriple + "/lib");
  Paths.push_back(Base + "/../../..");
  if (Arch == getArch() && IsUbuntu(Distro))
    Paths.push_back("/usr/lib/" + GccTriple);
}

bool Linux::HasNativeLLVMSupport() const {
  return true;
}

Tool &Linux::SelectTool(const Compilation &C, const JobAction &JA,
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
        T = new tools::linuxtools::Assemble(*this);
      break;
    case Action::LinkJobClass:
      T = new tools::linuxtools::Link(*this); break;
    default:
      T = &Generic_GCC::SelectTool(C, JA, Inputs);
    }
  }

  return *T;
}

