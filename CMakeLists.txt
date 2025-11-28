cmake_minimum_required(VERSION 3.20)
project(r-type LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB HEADER_FILES CONFIGURE_DEPENDS "include/*.hpp")

add_executable(r-type
    ${SRC_FILES}
    ${HEADER_FILES}
)

target_include_directories(r-type
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(WIN32)
    target_link_libraries(r-type PRIVATE ws2_32)
endif()
