#---------------------------------------------------------------------------
#CMake minimum required version
cmake_minimum_required (VERSION 2.8)
#---------------------------------------------------------------------------
#look up dir's file, and add to var
aux_source_directory(. DIR_TESTS)
#---------------------------------------------------------------------------
#add include path
#include_directories(${LIBRARY_OUTPUT_PATH_INC})
#---------------------------------------------------------------------------
#add lib path
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../depend/base/lib)
#---------------------------------------------------------------------------
#generate test
set(TESTNAME net_test)
add_executable(${TESTNAME} ${DIR_TESTS})
#---------------------------------------------------------------------------
#add link lib
set(LIBNAME net)
target_link_libraries(${TESTNAME} ${LIBNAME} base pthread)
#---------------------------------------------------------------------------
#install target 
#install(TARGETS json_test DESTINATION ${EXECUTABLE_OUTPUT_PATH})
#---------------------------------------------------------------------------
#add test
add_test(test_run ${EXECUTABLE_OUTPUT_PATH}/${TESTNAME})
#---------------------------------------------------------------------------
