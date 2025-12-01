set(SRC_FILES
    main.cpp
)

add_executable(rtype_server ${SRC_FILES})

target_link_libraries(rtype_server
    PRIVATE
        network
        protocol
        args
        game
        packet
        sfml-network
        sfml-system
)

target_include_directories(rtype_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(server_clean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/rtype_server.dir/*.o
    COMMENT "Supprime les fichiers objets du serveur"
)

add_custom_target(server_fclean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/rtype_server.dir/*.o
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/rtype_server
    COMMENT "Supprime les fichiers objets et l'exécutable du serveur"
)
