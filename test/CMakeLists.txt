#---------------------------------------------------------------------------
#CMake minimum required version
cmake_minimum_required (VERSION 2.8)
#---------------------------------------------------------------------------
#look up dir's file, and add to var
aux_source_directory(. DIR_TESTS)
#---------------------------------------------------------------------------
#add lib path
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../depend/base/lib)
#---------------------------------------------------------------------------
#generate test
set(TEST_event_loop test_event_loop)
add_executable(${TEST_event_loop} test_event_loop.cc)

#add include path
#include_directories(${LIBRARY_OUTPUT_PATH_INC})
#---------------------------------------------------------------------------
#add lib path
#link_directories(${CMAKE_INSTALL_PREFIX}/src})
#---------------------------------------------------------------------------
#add link lib
set(LIBNAME net)

target_link_libraries(${TEST_event_loop} ${LIBNAME} base pthread)
#---------------------------------------------------------------------------
#add test
add_test(${TEST_event_loop} ${EXECUTABLE_OUTPUT_PATH}/${TEST_event_loop})

#---------------------------------------------------------------------------
