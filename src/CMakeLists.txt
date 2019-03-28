#---------------------------------------------------------------------------
#CMake minimum required version
cmake_minimum_required (VERSION 2.8)
#---------------------------------------------------------------------------
#look up dir's file, and add to var
#aux_source_directory(<dir> <variable>)
aux_source_directory(. DIR_SRCS)
#---------------------------------------------------------------------------
#generate static library
#add_library(libname ${<dir>})
set(LIBNAME net)
add_library(${LIBNAME} ${DIR_SRCS})
#---------------------------------------------------------------------------
#install target 
install(TARGETS ${LIBNAME} DESTINATION ${LIBRARY_OUTPUT_PATH_LIB})

set(HEADERS
	buffer.h
    callback.h
    event_loop.h
    inet_address.h
    socket.h
    tcp_client.h
    tcp_server.h
    tcp_connection.h
    timer.h
    timer_id.h
    timer_queue.h

    udp_server.h
    datagram_packet.h
    datagram_socket.h
    )
install(FILES ${HEADERS} DESTINATION ${LIBRARY_OUTPUT_PATH_INC})
#---------------------------------------------------------------------------
