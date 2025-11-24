cmake_minimum_required(VERSION 3.20)
project(RTypeTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB HEADER_FILES CONFIGURE_DEPENDS "include/*.hpp")

add_executable(RTypeTest
    ${SRC_FILES}
    ${HEADER_FILES}
)

target_include_directories(RTypeTest
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
