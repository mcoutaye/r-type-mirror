cmake_minimum_required(VERSION 3.20)
project(rtype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

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
