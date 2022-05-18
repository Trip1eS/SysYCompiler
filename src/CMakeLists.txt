set(RUN_FILES main.cpp)

file(GLOB_RECURSE SOURCE_FILES "*.cpp")
list(REMOVE_ITEM SOURCE_FILES RUN_FILES)

foreach (target ${LLVM_TARGETS_TO_BUILD})
    list(APPEND targets "LLVM${target}CodeGen")
endforeach ()

add_library(compiler-lib ${SOURCE_FILES})
target_link_libraries(compiler-lib ${llvm_libs} ${targets})
set_target_properties(compiler-lib PROPERTIES PREFIX "")

add_executable(compiler ${RUN_FILES})
target_link_libraries(compiler compiler-lib)