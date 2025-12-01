add_library(game STATIC
    GameLoop.cpp
    GamePlayLoop.cpp
    GuiLoop.cpp
)

target_include_directories(game
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(game
    PRIVATE
        network
        protocol
        sfml-system
)

add_custom_target(game_clean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/game.dir/*.o
    COMMENT "Supprime les fichiers objets de bibliothèque game"
)

add_custom_target(game_fclean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/game.dir/*.o
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/libgame.a
    COMMENT "Supprime les fichiers objets et la bibliothèque du game"
)
