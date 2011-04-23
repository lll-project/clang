# TODO: Set the install directory.

set(known_subdirs
  "compiler-rt"
  "libcxx"
  )

if (CLANG_BOOTSTRAPPING)
  set(CLANG_BOOTSTRAP_CXX_DRIVER "${LLVM_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/clang++" CACHE INTERNAL "Bootstrap C++ driver")
  set(CLANG_BOOTSTRAP_C_DRIVER "${LLVM_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/clang" CACHE INTERNAL "Bootstrap C driver")
  set(CLANG_BOOTSTRAP_BARRIER "clang-${CLANG_VERSION}" CACHE INTERNAL "Target to depend on for the bootstrap driver")
endif()

foreach (dir ${known_subdirs})
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/CMakeLists.txt)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${dir})
  endif()
endforeach()

