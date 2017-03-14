cmake_minimum_required(VERSION 3.0)
project(zlib_stream)

include(deps.cmake)

add_library(zlib_stream
    include/zlib/gzip_filter.hpp
    src/gzip_filter.cpp)

target_include_directories(zlib_stream PUBLIC ${CMAKE_BINARY_DIR}/${dep_zlib} ${CMAKE_SOURCE_DIR}/${dep_zlib})
target_include_directories(zlib_stream PUBLIC include)
target_link_libraries(zlib_stream zlib)
target_compile_features(zlib_stream PUBLIC cxx_final)
