cmake_minimum_required(VERSION 3.29)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

project(AtomicRefCounted LANGUAGES C CXX)

add_library(arc ./src/Arc.cpp
        test/src/WeakSpec.cpp)

target_include_directories(arc PUBLIC
        $<BUILD_INTERFACE:${arc_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/include>
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

enable_testing()

add_subdirectory(test)
