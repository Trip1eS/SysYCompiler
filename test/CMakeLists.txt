include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${LLVM_INCLUDE_DIRS})
add_executable(
        compiler-test
        main.cpp)
target_link_libraries(compiler-test compiler-lib)