cmake_minimum_required(VERSION 3.5)

project(toyjson VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 14)

## Setup gtest
FUNCTION(set_gtest)
    execute_process(
            COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download
    )
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()

    execute_process(
            COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download
    )
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    add_subdirectory(
            ${CMAKE_CURRENT_BINARY_DIR}/googletest-src
            ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
            EXCLUDE_FROM_ALL
    )
ENDFUNCTION()

configure_file(cmake/get_gtest.cmake.in googletest-download/CMakeLists.txt)
set_gtest()

## Major project
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

set(header ${CMAKE_CURRENT_LIST_DIR}/include/toy_json.hpp
           ${CMAKE_CURRENT_LIST_DIR}/include/utils/parsers.hpp)

set(src ${CMAKE_CURRENT_LIST_DIR}/src/toy_json.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/utils/parsers.cpp)

add_library(${PROJECT_NAME} SHARED ${src} ${header})

## Test project
set(test_name test_${PROJECT_NAME})
set(test_src ${CMAKE_CURRENT_LIST_DIR}/test/test_utils_parsers.cpp
             ${CMAKE_CURRENT_LIST_DIR}/test/test_json_parse.cpp)

add_executable(${test_name} ${test_src})
target_link_libraries(${test_name} ${PROJECT_NAME} gtest_main)
