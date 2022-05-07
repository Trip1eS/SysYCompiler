include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(
    ${CMAKE_PROJECT_NAME}_test
    main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME}_test ${CMAKE_PROJECT_NAME}_lib)