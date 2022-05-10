find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

include_directories(${CMAKE_SOURCE_DIR}/include)
set(RUN_FILES main.cpp)

file(GLOB_RECURSE SOURCE_FILES "*.cpp")
list(REMOVE_ITEM SOURCE_FILES RUN_FILES)

add_library(${CMAKE_PROJECT_NAME}_lib ${SOURCE_FILES})

add_executable(${CMAKE_PROJECT_NAME}_run ${RUN_FILES})

llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(${CMAKE_PROJECT_NAME}_run ${CMAKE_PROJECT_NAME}_lib ${llvm_libs})