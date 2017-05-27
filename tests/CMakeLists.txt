include_directories(${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()
set(TEST_SOURCES
    hsm_test.cpp
#    examplest.cpp
)

set(UT_DRIVER hsm_UT)
add_executable(${UT_DRIVER} ${TEST_SOURCES} ${MOCK_LIST})
target_link_libraries(${UT_DRIVER} hsm gtest_main gmock)

if(BUILD_COVERAGE_UNIT_TESTS)
    set_target_properties(${UT_DRIVER} PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS_COVERAGE})
    target_link_libraries(${UT_DRIVER} ${CMAKE_CXX_COVERAGE_LIBRARY})
    setup_target_for_coverage(${UT_DRIVER}_cov ${UT_DRIVER} ${COVERAGE_REPORT_LOCATION})
endif()
