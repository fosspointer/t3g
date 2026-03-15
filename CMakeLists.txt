cmake_minimum_required(VERSION 3.16)
project(t3game VERSION 1.0 DESCRIPTION "Customizable tic-tac-toe game implementation" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB T3_GAME_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(game ${T3_GAME_SOURCES})

target_compile_definitions(
    game PUBLIC
    $<$<CONFIG:Release>:T3G_RELEASE>
    $<$<CONFIG:MinSizeRel>:T3G_RELEASE>
    $<$<CONFIG:Debug>:T3G_DEBUG>
    $<$<CONFIG:RelWithDebInfo>:T3G_DEBUG>
    T3G_VERSION=\"${PROJECT_VERSION}\"
)
