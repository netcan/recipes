find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(
    RefactorDecl
    SHARED
    RefactorDecl.cpp)

target_link_libraries(RefactorDecl
    "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")

add_executable(
    RefactorDeclMain
    RefactorDeclMain.cpp)

target_link_libraries(RefactorDeclMain
    PRIVATE
    clangTooling)
