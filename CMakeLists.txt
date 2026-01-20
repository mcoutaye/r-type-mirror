cmake_minimum_required(VERSION 3.20)
project(rtype LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO ON CACHE BOOL "" FORCE)
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)
set(SFML_BUILD_WINDOW ON CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(UNIX AND NOT APPLE)
    include(CheckIncludeFile)
    check_include_file("X11/Xcursor/Xcursor.h" HAVE_XCURSOR_H)
    check_include_file("X11/extensions/Xrandr.h" HAVE_XRANDR_H)
    check_include_file("AL/al.h" HAVE_OPENAL_H)

    if(NOT HAVE_XCURSOR_H OR NOT HAVE_XRANDR_H OR NOT HAVE_OPENAL_H)
        message(STATUS "Installation automatique des dépendances SFML...")
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

find_package(SFML 2.6 COMPONENTS graphics window system network audio QUIET)

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

# ============================================================
# Documentation targets
# ============================================================

# Trouve Doxygen
find_package(Doxygen QUIET)

if(DOXYGEN_FOUND)
    # Cible pour générer la documentation Doxygen
    add_custom_target(doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "🔨 Génération de la documentation Doxygen..."
        VERBATIM
    )
    
    # Cible pour générer toute la documentation et la servir (avec Doxygen)
    add_custom_target(docs-all
        COMMAND ${CMAKE_COMMAND} -E echo "📚 Génération de la documentation Doxygen..."
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
        COMMAND ${CMAKE_COMMAND} -E echo "🌐 Démarrage du serveur de documentation..."
        COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/serve_docs.py
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "📖 Génération et affichage de toute la documentation"
        VERBATIM
    )
    
    message(STATUS "✅ Doxygen trouvé - cibles 'doxygen' et 'docs-all' disponibles")
else()
    # Cible docs-all sans Doxygen
    add_custom_target(docs-all
        COMMAND ${CMAKE_COMMAND} -E echo "⚠️  Doxygen non disponible - génération de la doc markdown uniquement"
        COMMAND ${CMAKE_COMMAND} -E echo "    Installez Doxygen avec: sudo apt-get install doxygen"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "🌐 Démarrage du serveur de documentation..."
        COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/serve_docs.py
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "📖 Affichage de la documentation (sans Doxygen)"
        VERBATIM
    )
    
    message(STATUS "⚠️  Doxygen non trouvé - documentation API non disponible")
    message(STATUS "    Installez avec: sudo apt-get install doxygen")
endif()

# Cible pour servir la documentation en localhost
add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E echo "📚 Démarrage du serveur de documentation..."
    COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/serve_docs.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "🌐 Serveur de documentation sur http://localhost:8080"
    VERBATIM
)

# Message d'aide
add_custom_target(docs-help
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "╔════════════════════════════════════════════════╗"
    COMMAND ${CMAKE_COMMAND} -E echo "║     📚 Commandes de documentation             ║"
    COMMAND ${CMAKE_COMMAND} -E echo "╚════════════════════════════════════════════════╝"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "  make docs       → Servir la doc markdown en HTML"
    COMMAND ${CMAKE_COMMAND} -E echo "  make doxygen    → Générer la doc API Doxygen (si disponible)"
    COMMAND ${CMAKE_COMMAND} -E echo "  make docs-all   → Générer tout et servir"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "📖 Documentation disponible sur http://localhost:8080"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    VERBATIM
)
