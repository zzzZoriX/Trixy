cmake_minimum_required(VERSION 3.31)
project(trixy)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NPCAP_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external_libs/npcap-sdk")

add_executable(trixy src/core/impl/trixy_core.cpp src/core/impl/trixy_console.cpp src/network/ping/impl/trixy-ping.cpp)

target_include_directories(trixy PRIVATE "${NPCAP_SDK_DIR}/Include")

find_package(Boost REQUIRED CONFIG)

target_link_libraries(trixy PRIVATE Boost::boost ws2_32 mswsock)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})