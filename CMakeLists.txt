cmake_minimum_required(VERSION 3.20)
project(rtype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ====================
# Dossiers
# ====================
set(CLIENT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SERVER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/server")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# ====================
# SFML (obligatoire pour client + réseau serveur)
# ====================
include(FetchContent)
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG        2.6.x
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(SFML)

find_package(SFML 2.6 REQUIRED COMPONENTS graphics window system network)

# ====================
# CLIENT : rtype_client (la game engine SFML + ECS)
# ====================
set(CLIENT_SOURCES
    ${CLIENT_SRC_DIR}/main.cpp
    ${CLIENT_SRC_DIR}/RenderSystem.cpp
    ${CLIENT_SRC_DIR}/InputSystem.cpp
    ${CLIENT_SRC_DIR}/CollisionSystem.cpp
    ${CLIENT_SRC_DIR}/WaveSystem.cpp
    ${CLIENT_SRC_DIR}/ResourceManager.cpp
    # Ajoute ici d'autres .cpp du client si tu en crées plus tard
)

add_executable(rtype_client ${CLIENT_SOURCES})

target_include_directories(rtype_client PRIVATE
    ${INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}  # au cas où tu aies des includes relatifs
)

target_link_libraries(rtype_client PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

# Copie le dossier assets à côté du binaire (INDISPENSABLE pour que ça marche en debug)
add_custom_command(TARGET rtype_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:rtype_client>/assets
    COMMENT "Copie du dossier assets pour rtype_client"
)

# ====================
# SERVEUR : rtype_server (ton code réseau actuel)
# ====================
set(SERVER_SOURCES
    ${SERVER_SRC_DIR}/main.cpp
    ${SERVER_SRC_DIR}/Args.cpp
    ${SERVER_SRC_DIR}/NetworkManager.cpp
    ${SERVER_SRC_DIR}/UdpReceiver.cpp
    ${SERVER_SRC_DIR}/UdpSender.cpp
    ${SERVER_SRC_DIR}/GameLoop.cpp
    ${SERVER_SRC_DIR}/PacketInterpreter.cpp
)

add_executable(rtype_server ${SERVER_SOURCES})

target_include_directories(rtype_server PRIVATE
    ${INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rtype_server PRIVATE
    pthread
    sfml-network
    sfml-system
)

# ====================
# Commandes make clean / fclean / re personnalisées
# ====================
add_custom_target(clean-client
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/rtype_client
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/rtype_client.dir
    COMMENT "Nettoyage complet du client"
)

add_custom_target(clean-server
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/rtype_server
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/rtype_server.dir
    COMMENT "Nettoyage du serveur"
)

add_custom_target(fclean
    COMMAND ${CMAKE_MAKE_PROGRAM} clean-client
    COMMAND ${CMAKE_MAKE_PROGRAM} clean-server
    COMMENT "Nettoyage complet du projet"
)

add_custom_target(re
    COMMAND ${CMAKE_MAKE_PROGRAM} fclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    COMMENT "Recompile tout (client + serveur)"
)