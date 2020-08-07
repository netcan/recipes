find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(
    llvm_libs
    Support
    Core
    Option
    FrontendOpenMP)

# add_executable(
    # RefactorDecl
    # RefactorDecl.cpp)

add_library(
    RefactorDecl
    SHARED
    RefactorDecl.cpp)

target_link_libraries(RefactorDecl
    PRIVATE
    clangASTMatchers
    clangAST
    clangBasic
    clangLex
    clangRewrite
    clangSerialization
    clangSema
    clangAnalysis
    clangDriver
    clangEdit
    clangFrontend
    clangParse
    # clangTooling
    # clangFormat
    # clangToolingInclusions
    # clangToolingCore
    ${llvm_libs})
