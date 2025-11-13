cmake_minimum_required(VERSION 3.15)

project(HFT-LOB VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "03 -DNDEBUG")

include_directories(include)

set(SOURCES
    src/order_book.cpp
    src/main.cpp
)

add_executable(orderbook ${SOURCES})

install(TARGETS orderbook DESTINATION bin)

