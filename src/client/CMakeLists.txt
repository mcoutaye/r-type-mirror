set(SRC_FILES
    main.cpp
    UdpClient.cpp
)

add_executable(rtype_client ${SRC_FILES})

target_link_libraries(rtype_client
    PRIVATE
        sfml-network
        sfml-window
        sfml-system
        sfml-graphics
)

target_include_directories(rtype_client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(client_clean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/rtype_client.dir/*.o
    COMMENT "Supprime les fichiers objets du client"
)

add_custom_target(client_fclean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/rtype_client.dir/*.o
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/rtype_client
    COMMENT "Supprime les fichiers objets et l'exécutable du client"
)
