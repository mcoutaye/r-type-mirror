set(SRC_FILES
    main.cpp
    UdpClient.cpp
)

add_executable(r-type_client ${SRC_FILES})

target_link_libraries(r-type_client
    PRIVATE
        sfml-network
        sfml-window
        sfml-system
        sfml-graphics
        sfml-audio
)

target_include_directories(r-type_client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# === FIX WINDOWS: Copie les assets à côté de l'exécutable ===
# Sur Windows, CMake place les exécutables dans build/Debug/ ou build/Release/
# On doit copier les assets pour que les chemins relatifs fonctionnent
add_custom_command(TARGET r-type_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:r-type_client>/assets
    COMMENT "Copie du dossier assets à côté de l'exécutable (fix Windows)"
)

# Copie l'exécutable à la racine du build/ pour faciliter l'accès
add_custom_command(TARGET r-type_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:r-type_client>
        ${CMAKE_BINARY_DIR}/r-type_client$<TARGET_FILE_SUFFIX:r-type_client>
    COMMENT "Copie de r-type_client à build/ (racine)"
)

# Copie aussi les assets à la racine du build/
add_custom_command(TARGET r-type_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copie des assets à build/assets/"
)

# Sur Windows, désactive la console pour une application graphique
if(WIN32)
    set_target_properties(r-type_client PROPERTIES
        WIN32_EXECUTABLE TRUE  # Pas de console cmd.exe au lancement
    )
endif()

add_custom_target(client_clean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/r-type_client.dir/*.o
    COMMENT "Supprime les fichiers objets du client"
)

add_custom_target(client_fclean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/r-type_client.dir/*.o
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/r-type_client
    COMMENT "Supprime les fichiers objets et l'exécutable du client"
)
