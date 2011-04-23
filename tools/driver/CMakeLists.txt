set( LLVM_USED_LIBS
  clangAST
  clangAnalysis
  clangBasic
  clangCodeGen
  clangDriver
  clangFrontend
  clangFrontendTool
  clangIndex
  clangLex
  clangParse
  clangRewrite
  clangSema
  clangSerialization
  clangStaticAnalyzerFrontend
  clangStaticAnalyzerCheckers
  clangStaticAnalyzerCore
  )

set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  asmparser
  bitreader
  bitwriter
  codegen
  ipo
  selectiondag
  )

add_clang_executable(clang-${CLANG_VERSION}
  driver.cpp
  cc1_main.cpp
  cc1as_main.cpp
  )

if(UNIX)
  set(CLANGXX_LINK_OR_COPY create_symlink)
else()
  set(CLANGXX_LINK_OR_COPY copy)
endif()

# Create the clang++-M.M, clang and clang++ symlinks in the build directory.
add_custom_target(clang++-${CLANG_VERSION}-symlink ALL
  ${CMAKE_COMMAND} -E ${CLANGXX_LINK_OR_COPY}
    "${LLVM_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/clang-${CLANG_VERSION}"
    "${LLVM_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/clang++-${CLANG_VERSION}"
  DEPENDS clang-${CLANG_VERSION})

add_custom_target(clang++-symlink ALL
  ${CMAKE_COMMAND} -E ${CLANGXX_LINK_OR_COPY}
    "${LLVM_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/clang-${CLANG_VERSION}"
    "${LLVM_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/clang++"
  DEPENDS clang-${CLANG_VERSION})

add_custom_target(clang-symlink ALL
  ${CMAKE_COMMAND} -E ${CLANGXX_LINK_OR_COPY}
    "${LLVM_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/clang-${CLANG_VERSION}"
    "${LLVM_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/clang"
  DEPENDS clang-${CLANG_VERSION})

set_target_properties(clang++-${CLANG_VERSION}-symlink PROPERTIES FOLDER "Clang executables")
set_target_properties(clang++-symlink PROPERTIES FOLDER "Clang executables")
set_target_properties(clang-symlink PROPERTIES FOLDER "Clang executables")

set_property(DIRECTORY APPEND
  PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
  "${LLVM_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/clang++-${CLANG_VERSION}"
  "${LLVM_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/clang++"
  "${LLVM_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/clang")

install(TARGETS clang-${CLANG_VERSION}
  RUNTIME DESTINATION bin)

# Create the clang++ symlink at installation time.
install(CODE
  "set(bindir \"${CMAKE_INSTALL_PREFIX}/bin/\")
   execute_process(
     COMMAND \"\${CMAKE_COMMAND}\" -E ${CLANGXX_LINK_OR_COPY}
             \"clang-${CLANG_VERSION}\" \"clang++\"
     WORKING_DIRECTORY \"\${bindir}\")
   execute_process(
     COMMAND \"\${CMAKE_COMMAND}\" -E ${CLANGXX_LINK_OR_COPY}
             \"clang-${CLANG_VERSION}\" \"clang++-${CLANG_VERSION}\"
     WORKING_DIRECTORY \"\${bindir}\")")

