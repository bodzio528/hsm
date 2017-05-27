# Copyright Bogumił Chojnowski 2017
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.1)

project(HSM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -pedantic)

add_custom_target(hsm_HEADERS SOURCES
    include/hsm/hsm.hpp
    include/hsm/hsm_exceptions.hpp
)

add_library(hsm INTERFACE)
add_dependencies(hsm hsm_HEADERS)
target_include_directories(hsm INTERFACE include)

# coverage (GCC)
option(BUILD_COVERAGE_UNIT_TESTS "Decide whether generate coverage report for unit tests" OFF)
if(BUILD_COVERAGE_UNIT_TESTS)
    set(COVERAGE_REPORT_LOCATION "CoverageReport" CACHE STRING "Location for HTML coverage report." FORCE)

    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
    include(CodeCoverage)
endif()

# test targets
add_subdirectory(googletest-master)
add_subdirectory(tests)

