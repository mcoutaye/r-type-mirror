add_executable(r-type_client
    main.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/systems/InputSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/systems/MovementSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/systems/RenderSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/systems/CollisionSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/systems/WaveSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/systems/RessourceManager.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/systems/StageFactory.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/systems/EntityFactory.cpp
)

target_include_directories(r-type_client PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
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
