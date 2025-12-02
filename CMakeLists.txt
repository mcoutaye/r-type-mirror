cmake_minimum_required(VERSION 3.16)
project(RType LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.6.1 COMPONENTS window graphics REQUIRED)
add_executable(r-type_client
    src/client/main.cpp
    src/engine/core/GameEngine.cpp
    src/engine/systems/InputSystem.cpp
)

target_include_directories(r-type_client PUBLIC src src/engine)

target_link_libraries(r-type_client PRIVATE sfml-graphics sfml-window)

add_executable(r-type_server
src/server/main.cpp
)

target_include_directories(r-type_server PUBLIC src)

message(STATUS "Configuration de R-Type terminée !")
message(STATUS "Exécutables : r-type_client et r-type_server")