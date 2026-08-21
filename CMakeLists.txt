cmake_minimum_required(VERSION 3.10)
project(DSA_NeetCode)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

# Add CONFIGURE_DEPENDS to automatically detect new/deleted files
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*/*_test.cpp")

foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_target ${test_source} NAME_WE)
    
    get_filename_component(test_dir ${test_source} DIRECTORY)
    string(REGEX REPLACE "_test$" "" impl_target "${test_target}")
    
    # Gather all potential source files for this test
    set(target_sources ${test_source})
    
    set(impl_source "${test_dir}/${impl_target}.cpp")
    if(EXISTS "${impl_source}")
        list(APPEND target_sources ${impl_source})
    endif()
    
    set(utils_source "${test_dir}/test_utils.cpp")
    if(EXISTS "${utils_source}")
        list(APPEND target_sources ${utils_source})
    endif()

    # Create executable with test, implementation, and utils combined
    add_executable(${test_target} ${target_sources})

    target_link_libraries(${test_target} GTest::GTest GTest::Main pthread)
    
    gtest_discover_tests(${test_target})
endforeach()