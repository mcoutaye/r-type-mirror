add_executable(r-type_client
    main.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/core/GameEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/systems/InputSystem.cpp
)

target_include_directories(r-type_client PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/engine
)

target_link_libraries(r-type_client PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

add_custom_target(client_clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles/r-type_client.dir
    COMMENT "Nettoyage des fichiers objets du client"
)

add_custom_target(client_fclean
    COMMAND ${CMAKE_COMMAND} -E remove -f $<TARGET_FILE:r-type_client>
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles/r-type_client.dir
    COMMENT "Nettoyage complet du client"
)
