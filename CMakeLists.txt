cmake_minimum_required(VERSION 3.20)
project(AxiLang
        VERSION 0.9.5
        LANGUAGES CXX
        HOMEPAGE_URL "https://github.com/Sid110307/AxiLang.git"
)

set(CMAKE_CXX_STANDARD 17)
if (WIN32)
    set(CMAKE_CXX_FLAGS "/W4 /WX /EHsc")
else ()
    set(CMAKE_CXX_FLAGS "-fvisibility=hidden -Wall -Wextra -Wno-unknown-pragmas")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

add_executable(axilang
        ${PROJECT_SOURCE_DIR}/main.cpp
        ${PROJECT_SOURCE_DIR}/api.cpp
        ${PROJECT_SOURCE_DIR}/lexer.cpp
        ${PROJECT_SOURCE_DIR}/parser.cpp
        ${PROJECT_SOURCE_DIR}/interpreter.cpp
        ${PROJECT_SOURCE_DIR}/include/api.h
        ${PROJECT_SOURCE_DIR}/include/lexer.h
        ${PROJECT_SOURCE_DIR}/include/parser.h
        ${PROJECT_SOURCE_DIR}/include/interpreter.h
        ${PROJECT_SOURCE_DIR}/include/utils.h
)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
if (Python3_FOUND)
    target_include_directories(axilang PUBLIC ${Python3_INCLUDE_DIRS})
    target_link_libraries(axilang PUBLIC Python3::Python Python3::Module)
endif ()

find_package(Boost REQUIRED COMPONENTS python program_options filesystem system)
if (Boost_FOUND)
    target_include_directories(axilang PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(axilang PUBLIC ${Boost_LIBRARIES})
endif ()

find_package(CURL REQUIRED)
if (CURL_FOUND)
    target_include_directories(axilang PUBLIC ${CURL_INCLUDE_DIRS})
    target_link_libraries(axilang PUBLIC ${CURL_LIBRARIES})
endif ()

target_compile_definitions(axilang PUBLIC
        PROJECT_VERSION="${PROJECT_VERSION}"
        MAX_REDIRECTS=5
        PYTHON_EXECUTABLE="${Python3_EXECUTABLE}"
)
