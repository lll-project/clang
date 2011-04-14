set(LLVM_LINK_COMPONENTS mc)

add_clang_library(clangBasic
  Builtins.cpp
  ConvertUTF.c
  Diagnostic.cpp
  DiagnosticIDs.cpp
  FileManager.cpp
  FileSystemStatCache.cpp
  IdentifierTable.cpp
  SourceLocation.cpp
  SourceManager.cpp
  TargetInfo.cpp
  Targets.cpp
  TokenKinds.cpp
  Version.cpp
  VersionTuple.cpp
  )

execute_process(COMMAND ${LLVM_MAIN_SRC_DIR}/utils/GetSourceVersion
                        ${LLVM_MAIN_SRC_DIR}
                TIMEOUT 5
                OUTPUT_VARIABLE SVN_REVISION)

execute_process(COMMAND ${LLVM_MAIN_SRC_DIR}/utils/GetRepositoryPath
                        ${LLVM_MAIN_SRC_DIR}
                TIMEOUT 5
                OUTPUT_VARIABLE SVN_REPOSITORY)

add_definitions("-DSVN_REVISION=\"${SVN_REVISION}\"")
add_definitions("-DSVN_REPOSITORY=\"${SVN_REPOSITORY}\"")

add_dependencies(clangBasic 
                 ClangARMNeon
                 ClangAttrList
                 ClangDiagnosticAnalysis
                 ClangDiagnosticAST
                 ClangDiagnosticCommon
                 ClangDiagnosticDriver
                 ClangDiagnosticFrontend
                 ClangDiagnosticGroups
                 ClangDiagnosticLex
                 ClangDiagnosticParse
                 ClangDiagnosticSema)

