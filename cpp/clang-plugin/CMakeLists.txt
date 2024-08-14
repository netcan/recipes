find_package(LLVM CONFIG)
find_package(Clang CONFIG)

if (LLVM_FOUND)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

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
endif()
