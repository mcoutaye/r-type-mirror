cmake_minimum_required(VERSION 3.20)
project(rtype LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)
set(SFML_BUILD_WINDOW ON CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(UNIX AND NOT APPLE)
    include(CheckIncludeFile)
    check_include_file("X11/Xcursor/Xcursor.h" HAVE_XCURSOR_H)
    check_include_file("X11/extensions/Xrandr.h" HAVE_XRANDR_H)
    check_include_file("AL/al.h" HAVE_OPENAL_H)
    check_include_file("lua5.4/lua.h" HAVE_LUA_H)  # Corrigé pour le chemin réel sur Ubuntu

    set(MISSING_SFML_DEPS FALSE)
    if(NOT HAVE_XCURSOR_H OR NOT HAVE_XRANDR_H OR NOT HAVE_OPENAL_H)
        set(MISSING_SFML_DEPS TRUE)
    endif()

    if(MISSING_SFML_DEPS)
        message(STATUS "Installation automatique des dépendances SFML...")
        execute_process(
            COMMAND bash -c "
                sudo apt-get update -qq || true &&
                sudo apt-get clean &&
                sudo apt-get install -y -qq --reinstall libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev libudev-dev libopenal-dev libgl1-mesa-dev libfreetype6-dev libvorbis-dev libflac-dev
            "
            RESULT_VARIABLE APT_RESULT
        )
        if(NOT APT_RESULT EQUAL 0)
            message(FATAL_ERROR "Impossible d'installer les dépendances SFML automatiquement. Lance manuellement 'sudo apt-get update && sudo apt-get install libxcursor-dev ...' (liste complète ci-dessus).")
        endif()
        execute_process(COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        return()
    endif()
endif()

find_package(SFML 2.6 COMPONENTS graphics window system network QUIET)

if(SFML_FOUND)
    message(STATUS "SFML trouvée sur le système (version ${SFML_VERSION}). Utilisation de cette version.")
else()
    message(STATUS "SFML non trouvée sur le système. Téléchargement et compilation via FetchContent...")

    include(FetchContent)
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        2.6.1
    )
    FetchContent_MakeAvailable(SFML)
endif()

# Gestion de Lua avec anti-boucle
find_package(Lua 5.4 QUIET)

if(LUA_FOUND)
    message(STATUS "Lua trouvée sur le système (version ${LUA_VERSION_STRING}). Utilisation de cette version.")
else()
    message(STATUS "Lua non trouvée sur le système.")
    if(UNIX AND NOT APPLE)
        if(NOT LUA_INSTALL_ATTEMPTED)
            set(LUA_INSTALL_ATTEMPTED TRUE CACHE BOOL "Tentative d'installation Lua effectuée")
            message(STATUS "Installation automatique de Lua via apt...")
            execute_process(
                COMMAND bash -c "
                    sudo apt-get update -qq || true &&
                    sudo apt-get clean &&
                    sudo apt-get install -y -qq --reinstall lua5.4 liblua5.4-dev
                "
                RESULT_VARIABLE APT_RESULT
            )
            if(NOT APT_RESULT EQUAL 0)
                message(FATAL_ERROR "Impossible d'installer Lua automatiquement. Lance manuellement 'sudo apt-get update && sudo apt-get install lua5.4 liblua5.4-dev'.")
            endif()
            execute_process(COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}
                            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
            return()
        else()
            message(FATAL_ERROR "Lua toujours non détectée après tentative d'installation. Vérifie manuellement : installe 'lua5.4 liblua5.4-dev' via apt, puis relance CMake dans un nouveau build dir. Vérifie aussi 'find /usr/include -name lua.h' (devrait être en /usr/include/lua5.4/lua.h).")
        endif()
    else()
        message(FATAL_ERROR "Lua non trouvée et installation automatique non supportée sur cette plateforme. Veuillez installer Lua manuellement.")
    endif()
endif()

# sol2 est header-only, donc on l'intègre toujours via FetchContent
include(FetchContent)
FetchContent_Declare(
    sol2
    GIT_REPOSITORY https://github.com/ThePhD/sol2.git
    GIT_TAG v3.3.1
)
FetchContent_MakeAvailable(sol2)

add_subdirectory(src/server)
add_subdirectory(src/client)

add_custom_target(myclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target server_clean
    COMMENT "Nettoie les fichiers objets"
)

add_custom_target(myfclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target server_fclean
    COMMENT "Nettoie les fichiers objets et les exécutables"
)

add_custom_target(myre
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target myfclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    COMMENT "Nettoie et recompile tout"
)