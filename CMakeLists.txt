cmake_minimum_required(VERSION 3.31)
project(trixy)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NPCAP_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external_libs/npcap-sdk")

add_executable(trixy src/core/impl/trixy_core.cpp src/core/impl/trixy_console.cpp)

target_include_directories(trixy PRIVATE "${NPCAP_SDK_DIR}/Include")

if(MINGW)
    target_link_libraries(trixy PRIVATE
            "${NPCAP_SDK_DIR}/Lib/x64/libwpcap.a"
            ws2_32
    )
else()
    target_link_libraries(trixy PRIVATE
            "${NPCAP_SDK_DIR}/Lib/x64/wpcap.lib"
            ws2_32
    )
endif()