cmake_minimum_required(VERSION 3.20)
project(rtype LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ====================
# Dossiers
# ====================
set(CLIENT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# ====================
# SFML (obligatoire pour client + réseau serveur)
# ====================
if(UNIX AND NOT APPLE)
    include(CheckIncludeFile)
    check_include_file("X11/Xcursor/Xcursor.h"      HAVE_XCURSOR_H)
    check_include_file("X11/extensions/Xrandr.h"    HAVE_XRANDR_H)
    check_include_file("AL/al.h"                    HAVE_OPENAL_H)
    if(NOT HAVE_XCURSOR_H OR NOT HAVE_XRANDR_H OR NOT HAVE_OPENAL_H)
        message(STATUS "Installation automatique des dépendances SFML (libxcursor-dev, etc.)...")
        execute_process(
            COMMAND bash -c "
                sudo apt-get update -qq &&
                sudo apt-get install -y -qq libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev libudev-dev libopenal-dev libgl1-mesa-dev libfreetype6-dev libvorbis-dev libflac-dev
            "
            RESULT_VARIABLE APT_RESULT
        )
        if(NOT APT_RESULT EQUAL 0)
            message(FATAL_ERROR "Impossible d'installer les dépendances automatiquement.")
        endif()
        execute_process(COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        return()
    endif()
endif()

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML)

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
    ${CLIENT_SRC_DIR}/MovementSystem.cpp

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


add_custom_target(fclean
    COMMAND ${CMAKE_MAKE_PROGRAM} clean-client
    COMMENT "Nettoyage complet du projet"
)

add_custom_target(re
    COMMAND ${CMAKE_MAKE_PROGRAM} fclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    COMMENT "Recompile tout (client + serveur)"
)