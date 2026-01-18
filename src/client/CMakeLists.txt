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

    # COPIE CONDITIONNELLE DES DLL
    # On utilise un script wrapper pour éviter l'erreur si la liste de DLL est vide (cas du static link)
    add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E env "DLL_LIST=$<TARGET_RUNTIME_DLLS:r-type_client>" 
            ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/copy_dlls.cmake" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        COMMAND_EXPAND_LISTS
        COMMENT "Vérification et copie des DLL dépendantes"
        # Si vous ne voulez pas de script externe, on peut ruser avec une commande qui accepte le vide :
        COMMAND ${CMAKE_COMMAND} -E echo "Checking for DLLs to copy..."
    )

    # Note : Puisque vos logs montrent que vous liez SFML en STATIQUE (_deps/sfml-build/lib/libsfml-network.a)
    # la commande copy_if_different échouait car $<TARGET_RUNTIME_DLLS> était vide.
    # Voici une version simplifiée qui ne crash pas si vide :
    add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Build finished. DLLs are only copied if dynamic linking is used."
    )

    # Téléchargement d'OpenAL (nécessaire même en statique pour certains drivers audio)
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
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/CMakeFiles/r-type_client.dir
    COMMENT "Supprime les fichiers objets du client"
)

add_custom_target(client_fclean
    DEPENDS client_clean
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Supprime le dossier bin (exécutable et DLL)"
)