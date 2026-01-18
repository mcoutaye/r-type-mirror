cmake_minimum_required(VERSION 3.21)

# On définit le dossier de sortie pour l'exécutable et les DLL
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SRC_FILES
    main.cpp
    UdpClient.cpp
)

add_executable(r-type_client ${SRC_FILES})

# Sur Windows avec MinGW/MSYS2, liaison statique des bibliothèques système
# Cela permet d'éviter d'avoir besoin de libgcc_s_seh-1.dll, libstdc++-6.dll, etc.
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
# Note: Puisque vous utilisez SFML en statique, assurez-vous que SFML_STATIC est défini
# si vous incluez les headers SFML, bien que CMake le gère souvent seul.
target_compile_definitions(r-type_client PRIVATE SFML_STATIC)

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

    # Téléchargement d'OpenAL (Souvent la seule dépendance manquante en statique)
    set(OPENAL_URL "https://www.openal.org/downloads/OpenAL32.dll")
    set(OPENAL_DLL_DEST "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/openal32.dll")
    
    # On télécharge OpenAL uniquement s'il n'est pas déjà là
    if(NOT EXISTS "${OPENAL_DLL_DEST}")
        # Création du dossier bin s'il n'existe pas encore
        file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        message(STATUS "Téléchargement de OpenAL32.dll vers ${OPENAL_DLL_DEST}...")
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