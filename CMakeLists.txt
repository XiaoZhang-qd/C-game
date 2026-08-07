cmake_minimum_required(VERSION 3.15)


project(
    PXPT_Racer
    VERSION 1.0
    LANGUAGES C
)



set(CMAKE_C_STANDARD 99)

set(
CMAKE_C_STANDARD_REQUIRED
ON
)



# =====================
# Include directories
# =====================


include_directories(

    ${CMAKE_SOURCE_DIR}/common

    ${CMAKE_SOURCE_DIR}/client

    ${CMAKE_SOURCE_DIR}/server

    ${CMAKE_SOURCE_DIR}/raylib/src

    ${CMAKE_SOURCE_DIR}/raygui-5.0/src

)



# =====================
# Raylib static library
# =====================


set(
RAYLIB_LIB

${CMAKE_SOURCE_DIR}/raylib/src/libraylib.a

)




# =====================
# Common
# =====================


set(
COMMON_SRC

common/socket.c

)



# =====================
# Server
# =====================


add_executable(

pxpt-server


server/main.c

server/server.c


${COMMON_SRC}

)



# =====================
# Client
# =====================


add_executable(

pxpt-client


client/main.c

client/gui.c

client/game.c

client/network.c

client/raygui_impl.c


${COMMON_SRC}

)




# =====================
# Client link raylib
# =====================


target_link_libraries(

pxpt-client

${RAYLIB_LIB}

)



# =====================
# Platform
# =====================


if(WIN32)


message(
STATUS
"Windows"
)


target_link_libraries(

pxpt-server

ws2_32

)


target_link_libraries(

pxpt-client

ws2_32

)



elseif(APPLE)


message(
STATUS
"Apple"
)



target_link_libraries(

pxpt-client

"-framework Cocoa"

"-framework IOKit"

"-framework CoreVideo"

"-framework OpenGL"

)



elseif(ANDROID)


message(
STATUS
"Android"
)



elseif(UNIX)


message(
STATUS
"Linux/BSD"
)



target_link_libraries(

pxpt-server

pthread

)


target_link_libraries(

pxpt-client

pthread

)



endif()



# =====================
# Math library
# =====================


if(NOT WIN32)


target_link_libraries(

pxpt-client

m

)


target_link_libraries(

pxpt-server

m

)


endif()