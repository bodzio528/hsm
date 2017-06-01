# Copyright Bogumił Chojnowski 2017
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.1)

project(HSM LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
find_package(Hana REQUIRED)
if(NOT Hana_FOUND)
    message(FATAL_ERROR "Hana library not found in the system. Aborting.")
endif()

set(TARGET_NAME hsm)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_compile_options(-Wall -pedantic -std=c++14)

set(TARGET_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/hsm/hsm.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/hsm/hsm_exceptions.hpp
)

add_library(${TARGET_NAME} INTERFACE)
target_sources(${TARGET_NAME} INTERFACE ${TARGET_HEADERS})

target_include_directories(${TARGET_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            ${Hana_INCLUDE_DIRS}
)

target_include_directories(${TARGET_NAME} INTERFACE include)

# coverage (GCC)
option(BUILD_COVERAGE_UNIT_TESTS "Decide whether generate coverage report for unit tests" OFF)
if(BUILD_COVERAGE_UNIT_TESTS)
    set(COVERAGE_REPORT_LOCATION "CoverageReport" CACHE STRING "Location for HTML coverage report." FORCE)
    include(CodeCoverage)
endif()

# test library
add_subdirectory(googletest-master)

# test targets
enable_testing()
add_subdirectory(tests)
