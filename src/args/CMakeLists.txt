add_library(args STATIC Args.cpp)

target_include_directories(args
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

add_custom_target(args_clean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/args.dir/*.o
    COMMENT "Supprime les fichiers objets de la bibliothèque args"
)

add_custom_target(args_fclean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeFiles/args.dir/*.o
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/libargs.a
    COMMENT "Supprime les fichiers objets et la bibliothèque args"
)
