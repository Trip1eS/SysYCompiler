set(RUN_FILES main.cpp)

file(GLOB_RECURSE SOURCE_FILES "*.cpp")
list(REMOVE_ITEM SOURCE_FILES RUN_FILES)

foreach (target ${LLVM_TARGETS_TO_BUILD})
    list(APPEND targets "LLVM${target}CodeGen")
endforeach ()

add_library(${CMAKE_PROJECT_NAME}_lib ${SOURCE_FILES})
target_link_libraries(${CMAKE_PROJECT_NAME}_lib ${llvm_libs} ${targets})
set_target_properties(${CMAKE_PROJECT_NAME}_lib PROPERTIES PREFIX "")

add_executable(${CMAKE_PROJECT_NAME}_run ${RUN_FILES})

target_link_libraries(${CMAKE_PROJECT_NAME}_run ${CMAKE_PROJECT_NAME}_lib)