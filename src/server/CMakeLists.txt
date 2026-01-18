set(SRC_FILES
    main.cpp
    UdpServer.cpp
)

add_executable(r-type_server ${SRC_FILES})

target_link_libraries(r-type_server
    PRIVATE
        sfml-network
        sfml-system
        sfml-window
        sfml-graphics
)

target_include_directories(r-type_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# === FIX WINDOWS: Copie les assets si nécessaire ===
add_custom_command(TARGET r-type_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:r-type_server>/assets
    COMMENT "Copie du dossier assets à côté de l'exécutable serveur (fix Windows)"
)

# Copie l'exécutable à la racine du build/
add_custom_command(TARGET r-type_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:r-type_server>
        ${CMAKE_BINARY_DIR}/r-type_server$<TARGET_FILE_SUFFIX:r-type_server>
    COMMENT "Copie de r-type_server à build/ (racine)"
)

add_custom_target(server_clean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/r-type_server.dir/*.o
    COMMENT "Supprime les fichiers objets du serveur"
)

add_custom_target(server_fclean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/r-type_server.dir/*.o
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/r-type_server
    COMMENT "Supprime les fichiers objets et l'exécutable du serveur"
)
