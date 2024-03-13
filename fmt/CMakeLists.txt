cmake_minimum_required(VERSION 3.20)

project(fmt_example)

add_subdirectory(src/fmt)

set(FMT_EXAMPLE fmt_example)
add_executable(${FMT_EXAMPLE} src/fmt_example.cpp)

set_property(TARGET ${FMT_EXAMPLE} PROPERTY CXX_STANDARD 17)
target_link_libraries(${FMT_EXAMPLE} fmt::fmt)