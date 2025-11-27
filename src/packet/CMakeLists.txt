add_library(packet STATIC
    PacketInterpreter.cpp
    ServerPacketInterpreter.cpp
    ClientPacketInterpreter.cpp
)

target_include_directories(packet
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(packet
    PRIVATE
        network
        protocol
        game
)

add_custom_target(packet_clean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/packet.dir/*.o
    COMMENT "Supprime les fichiers objets de la bibliothèque packet"
)

add_custom_target(packet_fclean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/packet.dir/*.o
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/libpacket.a
    COMMENT "Supprime les fichiers objets et la bibliothèque packet"
)
