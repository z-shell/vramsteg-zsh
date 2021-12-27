cmake_minimum_required (VERSION 2.8)
include_directories (${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR})
set (vramsteg_SRCS vramsteg.cpp
                   Progress.cpp Progress.h)
add_executable (vramsteg ${vramsteg_SRCS})
install (TARGETS vramsteg DESTINATION bin)

#set (CMAKE_BUILD_TYPE debug)
#set (CMAKE_C_FLAGS_DEBUG "-g")
#set (CMAKE_C_FLAGS_RELEASE "-O3")

