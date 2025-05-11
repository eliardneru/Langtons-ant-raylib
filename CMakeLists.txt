cmake_minimum_required(VERSION 3.10)
project(AntGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set Compiler as llvm, you can change this to use another, but i personally think that msvc sucks
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Set path
set(RAY_DIR "${CMAKE_SOURCE_DIR}/lib/RAYLIB")

include_directories(

    ${RAY_DIR}/include
)

link_directories(

    ${RAY_DIR}/lib
)

add_executable(AntGame
    src/game.cpp
)


# Copy assets
add_custom_command(TARGET AntGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:AntGame>/assets"
)

# copy sfml stuff

add_custom_command(TARGET AntGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${RAY_DIR}/lib/raylib.dll"
        "$<TARGET_FILE_DIR:AntGame>"
)




target_link_libraries(AntGame
    raylib
    winmm
    gdi32
    opengl32
    shell32
)
