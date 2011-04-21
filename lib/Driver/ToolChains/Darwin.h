//===--- Darwin.h - ToolChain Implementations -------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_BBA13606_B261_4203_8DB8_A65BB70BE023
#define CLANG_BBA13606_B261_4203_8DB8_A65BB70BE023

#include "clang/Driver/Action.h"
#include "clang/Driver/ToolChain.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Compiler.h"

#include "../Tools.h"

namespace clang {
namespace driver {
namespace toolchains {

/// Darwin - The base Darwin tool chain.
class LLVM_LIBRARY_VISIBILITY Darwin : public ToolChain {
// {{{
public:
  /// The host version.
  unsigned DarwinVersion[3];

private:
  mutable llvm::DenseMap<unsigned, Tool*> Tools;

  /// Whether the information on the target has been initialized.
  //
  // FIXME: This should be eliminated. What we want to do is make this part of
  // the "default target for arguments" selection process, once we get out of
  // the argument translation business.
  mutable bool TargetInitialized;

  /// Whether we are targeting iPhoneOS target.
  mutable bool TargetIsIPhoneOS;

  /// The OS version we are targeting.
  mutable unsigned TargetVersion[3];

  /// The default macosx-version-min of this tool chain; empty until
  /// initialized.
  std::string MacosxVersionMin;

private:
  void AddDeploymentTarget(DerivedArgList &Args) const;

public:
  Darwin(const HostInfo &Host, const llvm::Triple& Triple);
  ~Darwin();

  std::string ComputeEffectiveClangTriple(const ArgList &Args) const;

  /// @name Darwin Specific Toolchain API
  /// {

  // FIXME: Eliminate these ...Target functions and derive separate tool chains
  // for these targets and put version in constructor.
  void setTarget(bool isIPhoneOS, unsigned Major, unsigned Minor,
                 unsigned Micro) const {
    // FIXME: For now, allow reinitialization as long as values don't
    // change. This will go away when we move away from argument translation.
    if (TargetInitialized && TargetIsIPhoneOS == isIPhoneOS &&
        TargetVersion[0] == Major && TargetVersion[1] == Minor &&
        TargetVersion[2] == Micro)
      return;

    assert(!TargetInitialized && "Target already initialized!");
    TargetInitialized = true;
    TargetIsIPhoneOS = isIPhoneOS;
    TargetVersion[0] = Major;
    TargetVersion[1] = Minor;
    TargetVersion[2] = Micro;
  }

  bool isTargetIPhoneOS() const {
    assert(TargetInitialized && "Target not initialized!");
    return TargetIsIPhoneOS;
  }

  bool isTargetIOSSimulator() const {
    // In GCC, the simulator historically was treated as being OS X in some
    // contexts, like determining the link logic, despite generally being called
    // with an iOS deployment target. For compatibility, we detect the
    // simulator is iOS + x86, and treat it differently in a few contexts.
    return isTargetIPhoneOS() && 
      (getTriple().getArch() == llvm::Triple::x86 ||
       getTriple().getArch() == llvm::Triple::x86_64);
  }

  bool isTargetInitialized() const { return TargetInitialized; }

  void getTargetVersion(unsigned (&Res)[3]) const {
    assert(TargetInitialized && "Target not initialized!");
    Res[0] = TargetVersion[0];
    Res[1] = TargetVersion[1];
    Res[2] = TargetVersion[2];
  }

  /// getDarwinArchName - Get the "Darwin" arch name for a particular compiler
  /// invocation. For example, Darwin treats different ARM variations as
  /// distinct architectures.
  llvm::StringRef getDarwinArchName(const ArgList &Args) const;

  static bool isVersionLT(unsigned (&A)[3], unsigned (&B)[3]) {
    for (unsigned i=0; i < 3; ++i) {
      if (A[i] > B[i]) return false;
      if (A[i] < B[i]) return true;
    }
    return false;
  }

  bool isIPhoneOSVersionLT(unsigned V0, unsigned V1=0, unsigned V2=0) const {
    assert(isTargetIPhoneOS() && "Unexpected call for OS X target!");
    unsigned B[3] = { V0, V1, V2 };
    return isVersionLT(TargetVersion, B);
  }

  bool isMacosxVersionLT(unsigned V0, unsigned V1=0, unsigned V2=0) const {
    assert(!isTargetIPhoneOS() && "Unexpected call for iPhoneOS target!");
    unsigned B[3] = { V0, V1, V2 };
    return isVersionLT(TargetVersion, B);
  }

  /// AddLinkSearchPathArgs - Add the linker search paths to \arg CmdArgs.
  ///
  /// \param Args - The input argument list.
  /// \param CmdArgs [out] - The command argument list to append the paths
  /// (prefixed by -L) to.
  virtual void AddLinkSearchPathArgs(const ArgList &Args,
                                     ArgStringList &CmdArgs) const = 0;

  /// AddLinkRuntimeLibArgs - Add the linker arguments to link the compiler
  /// runtime library.
  virtual void AddLinkRuntimeLibArgs(const ArgList &Args,
                                     ArgStringList &CmdArgs) const = 0;

  /// }
  /// @name ToolChain Implementation
  /// {

  virtual types::ID LookupTypeForExtension(const char *Ext) const;

  virtual bool HasNativeLLVMSupport() const;

  virtual DerivedArgList *TranslateArgs(const DerivedArgList &Args,
                                        const char *BoundArch) const;

  virtual Tool &SelectTool(const Compilation &C, const JobAction &JA,
                           const ActionList &Inputs) const;

  virtual bool IsBlocksDefault() const {
    // Always allow blocks on Darwin; users interested in versioning are
    // expected to use /usr/include/Blocks.h.
    return true;
  }
  virtual bool IsIntegratedAssemblerDefault() const {
#ifdef DISABLE_DEFAULT_INTEGRATED_ASSEMBLER
    return false;
#else
    // Default integrated assembler to on for x86.
    return (getTriple().getArch() == llvm::Triple::x86 ||
            getTriple().getArch() == llvm::Triple::x86_64);
#endif
  }
  virtual bool IsStrictAliasingDefault() const {
#ifdef DISABLE_DEFAULT_STRICT_ALIASING
    return false;
#else
    return ToolChain::IsStrictAliasingDefault();
#endif
  }
  
  virtual bool IsObjCDefaultSynthPropertiesDefault() const {
    return false;
  }

  virtual bool IsObjCNonFragileABIDefault() const {
    // Non-fragile ABI is default for everything but i386.
    return getTriple().getArch() != llvm::Triple::x86;
  }
  virtual bool IsObjCLegacyDispatchDefault() const {
    // This is only used with the non-fragile ABI.

    // Legacy dispatch is used everywhere except on x86_64.
    return getTriple().getArch() != llvm::Triple::x86_64;
  }
  virtual bool UseObjCMixedDispatch() const {
    // This is only used with the non-fragile ABI and non-legacy dispatch.

    // Mixed dispatch is used everywhere except OS X before 10.6.
    return !(!isTargetIPhoneOS() && isMacosxVersionLT(10, 6));
  }
  virtual bool IsUnwindTablesDefault() const;
  virtual unsigned GetDefaultStackProtectorLevel() const {
    // Stack protectors default to on for 10.6 and beyond.
    return !isTargetIPhoneOS() && !isMacosxVersionLT(10, 6);
  }
  virtual const char *GetDefaultRelocationModel() const;
  virtual const char *GetForcedPicModel() const;

  virtual bool SupportsProfiling() const;

  virtual bool SupportsObjCGC() const;

  virtual bool UseDwarfDebugFlags() const;

  virtual bool UseSjLjExceptions() const;

  /// }
/// }}}
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif

