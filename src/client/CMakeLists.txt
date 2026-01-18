cmake_minimum_required(VERSION 3.21) # Version 3.21 requise pour $<TARGET_RUNTIME_DLLS>

# On définit le dossier de sortie pour l'exécutable et les DLL
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SRC_FILES
    main.cpp
    UdpClient.cpp
)

add_executable(r-type_client ${SRC_FILES})

# Sur Windows avec MinGW/MSYS2, liaison statique des bibliothèques système pour éviter les DLL manquantes
if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_options(r-type_client PRIVATE -static-libgcc -static-libstdc++ -static)
endif()

# Inclusion des répertoires
target_include_directories(r-type_client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Liaison avec SFML
target_link_libraries(r-type_client
    PRIVATE
        sfml-network
        sfml-window
        sfml-system
        sfml-graphics
        sfml-audio
)

# Spécificités Windows
if(WIN32)
    # sfml-main est nécessaire pour le point d'entrée main() standard sur Windows
    target_link_libraries(r-type_client PRIVATE sfml-main)

    # COPIE AUTOMATIQUE DES DLL (SFML, OpenAL, etc.)
    # Cette commande magique trouve toutes les DLL dont l'exe a besoin et les copie
    add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:r-type_client>
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND_EXPAND_LISTS
        COMMENT "Copie des DLL dépendantes dans le dossier bin/"
    )

    # Téléchargement d'OpenAL (sécurité si non trouvé par le système)
    set(OPENAL_URL "https://www.openal.org/downloads/OpenAL32.dll")
    set(OPENAL_DLL_DEST "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/openal32.dll")
    if(NOT EXISTS "${OPENAL_DLL_DEST}")
        message(STATUS "Téléchargement de OpenAL32.dll...")
        file(DOWNLOAD "${OPENAL_URL}" "${OPENAL_DLL_DEST}" SHOW_PROGRESS)
    endif()
endif()

# Copie du dossier assets dans le dossier de l'exécutable
add_custom_command(TARGET r-type_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
    COMMENT "Synchronisation du dossier assets dans bin/"
)

# --- Commandes de nettoyage personnalisées ---

add_custom_target(client_clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles/r-type_client.dir
    COMMENT "Supprime les fichiers objets du client"
)

add_custom_target(client_fclean
    DEPENDS client_clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Supprime le dossier bin (exécutable et DLL)"
)