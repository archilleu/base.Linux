#---------------------------------------------------------------------------
#CMake minimum required version
cmake_minimum_required (VERSION 2.8)
#---------------------------------------------------------------------------
#look up dir's file, and add to var
aux_source_directory(. DIR_TESTS)
#---------------------------------------------------------------------------
#add header file
include_directories(../thirdpart)
#---------------------------------------------------------------------------
#add lib path
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdpart/base/lib)
#---------------------------------------------------------------------------
#generate test
set(TEST_event_loop test_event_loop)
add_executable(${TEST_event_loop} test_event_loop.cc)

set(TEST_acceptor test_acceptor)
add_executable(${TEST_acceptor} test_acceptor.cc)

set(TEST_buffer test_buffer)
add_executable(${TEST_buffer} test_buffer.cc)

set(TEST_event_loop_thread test_event_loop_thread)
add_executable(${TEST_event_loop_thread} test_event_loop_thread.cc)

set(TEST_event_loop_thread_pool test_event_loop_thread_pool)
add_executable(${TEST_event_loop_thread_pool} test_event_loop_thread_pool.cc)

set(TEST_inet_address test_inet_address)
add_executable(${TEST_inet_address} test_inet_address.cc)

set(TEST_socket test_socket)
add_executable(${TEST_socket} test_socket.cc)

set(TEST_tcp_connection test_tcp_connection)
add_executable(${TEST_tcp_connection} test_tcp_connection.cc)

set(TEST_tcp_server test_tcp_server)
add_executable(${TEST_tcp_server} test_tcp_server.cc)

set(TEST_tcp_connector test_tcp_connector)
add_executable(${TEST_tcp_connector} test_connector.cc)

set(TEST_datagram_packet test_datagram_packet)
add_executable(${TEST_datagram_packet} test_datagram_packet.cc)

set(TEST_datagram_socket test_datagram_socket)
add_executable(${TEST_datagram_socket} test_datagram_socket.cc)

set(TEST_tcp_client test_tcp_client)
add_executable(${TEST_tcp_client} test_tcp_client.cc)

set(TEST_udp_receiver test_udp_receiver)
add_executable(${TEST_udp_receiver} test_udp_receiver.cc)

set(TEST_udp_server test_udp_server)
add_executable(${TEST_udp_server} test_udp_server.cc)

#add include path
#include_directories(${LIBRARY_OUTPUT_PATH_INC})
#---------------------------------------------------------------------------
#add lib path
#link_directories(${CMAKE_INSTALL_PREFIX}/src})
#---------------------------------------------------------------------------
#add link lib
set(LIBNAME net)

target_link_libraries(${TEST_event_loop} ${LIBNAME} base pthread)

target_link_libraries(${TEST_acceptor} ${LIBNAME} base pthread)

target_link_libraries(${TEST_buffer} ${LIBNAME} base pthread)

target_link_libraries(${TEST_event_loop_thread} ${LIBNAME} base pthread)

target_link_libraries(${TEST_event_loop_thread_pool} ${LIBNAME} base pthread)

target_link_libraries(${TEST_inet_address} ${LIBNAME} base pthread)

target_link_libraries(${TEST_socket} ${LIBNAME} base pthread)

target_link_libraries(${TEST_tcp_connection} ${LIBNAME} base pthread)

target_link_libraries(${TEST_tcp_server} ${LIBNAME} base pthread)

target_link_libraries(${TEST_tcp_connector} ${LIBNAME} base pthread)

target_link_libraries(${TEST_datagram_packet} ${LIBNAME} base pthread)

target_link_libraries(${TEST_datagram_socket} ${LIBNAME} base pthread)

target_link_libraries(${TEST_tcp_client} ${LIBNAME} base pthread)

target_link_libraries(${TEST_udp_receiver} ${LIBNAME} base pthread)

target_link_libraries(${TEST_udp_server} ${LIBNAME} base pthread)
#---------------------------------------------------------------------------

#add test
add_test(${TEST_event_loop} ${EXECUTABLE_OUTPUT_PATH}/${TEST_event_loop})

add_test(${TEST_acceptor} ${EXECUTABLE_OUTPUT_PATH}/${TEST_acceptor})

add_test(${TEST_buffer} ${EXECUTABLE_OUTPUT_PATH}/${TEST_buffer}
    WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

add_test(${TEST_event_loop_thread} ${EXECUTABLE_OUTPUT_PATH}/${TEST_event_loop_thread})

add_test(${TEST_event_loop_thread_pool}
    ${EXECUTABLE_OUTPUT_PATH}/${TEST_event_loop_thread_pool})

add_test(${TEST_inet_address} ${EXECUTABLE_OUTPUT_PATH}/${TEST_inet_address})

add_test(${TEST_socket} ${EXECUTABLE_OUTPUT_PATH}/${TEST_socket})

add_test(${TEST_tcp_connection} ${EXECUTABLE_OUTPUT_PATH}/${TEST_tcp_connection})

add_test(${TEST_tcp_server} ${EXECUTABLE_OUTPUT_PATH}/${TEST_tcp_server})

add_test(${TEST_tcp_connector} ${EXECUTABLE_OUTPUT_PATH}/${TEST_tcp_connector})

add_test(${TEST_datagram_packet} ${EXECUTABLE_OUTPUT_PATH}/${TEST_datagram_packet})

add_test(${TEST_datagram_socket} ${EXECUTABLE_OUTPUT_PATH}/${TEST_datagram_socket})

add_test(${TEST_tcp_client} ${EXECUTABLE_OUTPUT_PATH}/${TEST_tcp_client})

add_test(${TEST_udp_receiver} ${EXECUTABLE_OUTPUT_PATH}/${TEST_udp_receiver})

add_test(${TEST_udp_server} ${EXECUTABLE_OUTPUT_PATH}/${TEST_udp_server})
#---------------------------------------------------------------------------
