add_library(network STATIC
    UdpSender.cpp
    UdpReceiver.cpp
)

target_include_directories(network
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(network
    PRIVATE
        sfml-network
        sfml-system
)

add_custom_target(network_clean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/network.dir/*.o
    COMMENT "Supprime les fichiers objets de bibliothèque network"
)

add_custom_target(network_fclean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/network.dir/*.o
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/libnetwork.a
    COMMENT "Supprime les fichiers objets et la bibliothèque du network"
)
