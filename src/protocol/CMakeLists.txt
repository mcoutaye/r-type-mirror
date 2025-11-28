add_library(protocol STATIC Protocol.cpp)

target_include_directories(protocol
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(protocol
    PRIVATE
        sfml-network
        sfml-system
)

add_custom_target(protocol_clean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/protocol.dir/*.o
    COMMENT "Supprime les fichiers objets du protocole"
)

add_custom_target(protocol_fclean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/protocol.dir/*.o
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/libprotocol.a
    COMMENT "Supprime les fichiers objets et la bibliothèque du protocole"
)
