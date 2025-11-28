cmake_minimum_required(VERSION 3.20)
project(rtype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT MSVC)
    add_compile_options(-Wall -Wextra -Werror)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(SFML 2.5 COMPONENTS system window graphics network QUIET)

if(NOT SFML_FOUND)
    message(STATUS "SFML non trouvé sur le système → téléchargement/compilation automatique")
    add_subdirectory(external)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupSFML.cmake)
else()
    message(STATUS "SFML trouvé sur le système (version ${SFML_VERSION})")
endif()

include_directories(${SFML_INCLUDE_DIR})

add_subdirectory(src/protocol)
add_subdirectory(src/network)
add_subdirectory(src/args)
add_subdirectory(src/game)
add_subdirectory(src/packet)
add_subdirectory(src/server)
add_subdirectory(src/client)

include(GNUInstallDirs)

install(TARGETS rtype_client rtype_server
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(TARGET sfml-system)
    install(DIRECTORY "${SFML_INSTALL_DIR}/"
            DESTINATION .
            USE_SOURCE_PERMISSIONS
            PATTERN "*.git" EXCLUDE)
endif()

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
