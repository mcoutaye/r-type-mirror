add_executable(r-type_server
    main.cpp
    Args.cpp
    NetworkManager.cpp
    UdpReceiver.cpp
    UdpSender.cpp
    GameLoop.cpp
    PacketInterpreter.cpp
)

target_include_directories(r-type_server PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(r-type_server PRIVATE
    sfml-network
    sfml-system
    pthread
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
