cmake_minimum_required(VERSION 3.20)
project(rtype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT MSVC)
    add_compile_options(-Wall -Wextra -Werror)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(SFML 2.5 QUIET COMPONENTS system window graphics network)

if(SFML_FOUND)
    message(STATUS "SFML trouvé sur le système (version ${SFML_VERSION})")
else()
    message(STATUS "SFML non trouvé → compilation automatique depuis external/")

    add_subdirectory(external)

    list(APPEND CMAKE_PREFIX_PATH "${SFML_INSTALL_DIR}")

    find_package(SFML 2.5 REQUIRED COMPONENTS system window graphics network)
endif()

message(STATUS "SFML include dir : ${SFML_INCLUDE_DIR}")
message(STATUS "SFML libraries   : ${SFML_LIBRARIES}")

add_subdirectory(src/protocol)
add_subdirectory(src/network)
add_subdirectory(src/args)
add_subdirectory(src/game)
add_subdirectory(src/packet)
add_subdirectory(src/server)
add_subdirectory(src/client)

add_custom_target(myclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target protocol_clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target network_clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target args_clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target game_clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target packet_clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target server_clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target client_clean
    COMMENT "Nettoie les fichiers objets"
)

add_custom_target(myfclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target protocol_fclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target network_fclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target args_fclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target game_fclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target packet_fclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target server_fclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target client_fclean
    COMMENT "Nettoie les fichiers objets et les exécutables"
)

add_custom_target(myre
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target myfclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    COMMENT "Nettoie et recompile tout"
)
