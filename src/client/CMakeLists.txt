cmake_minimum_required(VERSION 3.21)

# On définit le dossier de sortie pour l'exécutable et les DLL
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SRC_FILES
    main.cpp
    UdpClient.cpp
)

add_executable(r-type_client ${SRC_FILES})

# Sur Windows avec MinGW/MSYS2, liaison statique des bibliothèques système
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
target_compile_definitions(r-type_client PRIVATE SFML_STATIC)
target_link_libraries(r-type_client
    PRIVATE
        sfml-network
        sfml-window
        sfml-system
        sfml-graphics
        sfml-audio
)

if(WIN32)
    target_link_libraries(r-type_client PRIVATE sfml-main)

    # === CORRECTION CRITIQUE ===
    # Au lieu de télécharger une version potentiellement 32-bits incompatible,
    # on récupère la DLL OpenAL fournie dans les sources de la SFML (FetchContent)
    # qui correspond à l'architecture (x86 ou x64) compilée.
    
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SFML_ARCH "x64")
    else()
        set(SFML_ARCH "x86")
    endif()

    # Chemin probable dans le dossier de build (basé sur vos logs _deps/sfml-src/...)
    set(SFML_OPENAL_PATH "${CMAKE_BINARY_DIR}/_deps/sfml-src/extlibs/bin/${SFML_ARCH}/openal32.dll")

    add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Tentative de copie de OpenAL pour ${SFML_ARCH}..."
        # On essaie de copier depuis les sources SFML
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_OPENAL_PATH}"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/openal32.dll"
        COMMENT "Copie de openal32.dll (version compatible ${SFML_ARCH})"
    )
endif()

# Copie du dossier assets
add_custom_command(TARGET r-type_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
    COMMENT "Synchronisation du dossier assets"
)

# Nettoyage
add_custom_target(client_clean
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/CMakeFiles/r-type_client.dir
    COMMENT "Supprime les fichiers objets du client"
)

add_custom_target(client_fclean
    DEPENDS client_clean
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Supprime le dossier bin"
)