add_executable(r-type_server
    main.cpp
)

target_include_directories(r-type_server PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(r-type_server PRIVATE
    sfml-network
    sfml-system
)

add_custom_target(server_clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles/r-type_server.dir
    COMMENT "Nettoyage des fichiers objets du serveur"
)

add_custom_target(server_fclean
    COMMAND ${CMAKE_COMMAND} -E remove -f $<TARGET_FILE:r-type_server>
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles/r-type_server.dir
    COMMENT "Nettoyage complet du serveur"
)
