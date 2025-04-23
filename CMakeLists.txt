cmake_minimum_required(VERSION 3.30)

project(chess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

set(IMGUI_DIR vendor/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp
)

add_executable(
    chess

    src/Game/Piece/BishopPiece.cpp
    src/Game/Piece/KingPiece.cpp
    src/Game/Piece/KnightPiece.cpp
    src/Game/Piece/PawnPiece.cpp
    src/Game/Piece/Piece.cpp
    src/Game/Piece/QueenPiece.cpp
    src/Game/Piece/RookPiece.cpp

    src/Game/Common.cpp
    src/Game/Game.cpp
    src/Game/Move.cpp

    src/GUI/ChessGUI.cpp

    src/main.cpp

    ${IMGUI_SOURCES}
)

target_include_directories(chess PRIVATE
    vendor/stb
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${IMGUI_DIR}/misc/cpp
)

target_compile_options(chess PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(chess PRIVATE
    glfw
    OpenGL::GL
)

# Copy the resources directory to the build output directory
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
