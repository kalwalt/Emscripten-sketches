project(stb_image_example)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXECUTABLE_SUFFIX ".js")

add_executable(stb_example simple_stb_example.cpp)

get_filename_component(PARENT_DIR ./ ABSOLUTE)
message( "parent dir ${PARENT_DIR}")
target_include_directories(stb_example PRIVATE "${PARENT_DIR}/stb_image")

set_target_properties(stb_example PROPERTIES
    LINK_FLAGS " -I./stb_image -s WASM=1 --preload-file ${PARENT_DIR}/chalk.jpg@build --bind "
)
