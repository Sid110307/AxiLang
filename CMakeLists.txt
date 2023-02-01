cmake_minimum_required(VERSION 3.2)
project(AxiLang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unknown-pragmas")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

add_executable(axilang
        ${PROJECT_SOURCE_DIR}/main.cpp
        ${PROJECT_SOURCE_DIR}/api.cpp
        ${PROJECT_SOURCE_DIR}/lexer.cpp
        ${PROJECT_SOURCE_DIR}/include/api.h
        ${PROJECT_SOURCE_DIR}/include/lexer.h)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
target_include_directories(axilang PUBLIC ${Python3_INCLUDE_DIRS})
target_link_libraries(axilang PUBLIC Python3::Python Python3::Module)

find_package(Boost REQUIRED COMPONENTS python program_options filesystem system)

if (Boost_FOUND)
    target_include_directories(axilang PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(axilang PUBLIC ${Boost_LIBRARIES})
endif ()
