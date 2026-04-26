cmake_minimum_required(VERSION 3.10)
project(AuraRetailOS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include_directories(include .)

file(GLOB SOURCES "src/*.cpp")

add_executable(AuraRetailOS ${SOURCES})
target_link_libraries(AuraRetailOS Threads::Threads)

if(WIN32)
    target_link_libraries(AuraRetailOS ws2_32 crypt32)
endif()
