set(SFML_INCLUDE_DIR "${SFML_SOURCE_DIR}/include" CACHE PATH "SFML include directory")
set(SFML_LIBRARY_DIR "${SFML_BINARY_DIR}/lib" CACHE PATH "SFML library directory")
set(SFML_FOUND TRUE CACHE BOOL "SFML found")
set(SFML_VERSION 2.5 CACHE STRING "SFML version")

set(SFML_SYSTEM_LIBRARY "${SFML_LIBRARY_DIR}/libsfml-system.a" CACHE FILEPATH "SFML system library")
set(SFML_WINDOW_LIBRARY "${SFML_LIBRARY_DIR}/libsfml-window.a" CACHE FILEPATH "SFML window library")
set(SFML_GRAPHICS_LIBRARY "${SFML_LIBRARY_DIR}/libsfml-graphics.a" CACHE FILEPATH "SFML graphics library")
set(SFML_NETWORK_LIBRARY "${SFML_LIBRARY_DIR}/libsfml-network.a" CACHE FILEPATH "SFML network library")

add_library(sfml-system STATIC IMPORTED)
set_target_properties(sfml-system PROPERTIES
    IMPORTED_LOCATION "${SFML_SYSTEM_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SFML_INCLUDE_DIR}"
)

add_library(sfml-window STATIC IMPORTED)
set_target_properties(sfml-window PROPERTIES
    IMPORTED_LOCATION "${SFML_WINDOW_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SFML_INCLUDE_DIR}"
)

add_library(sfml-graphics STATIC IMPORTED)
set_target_properties(sfml-graphics PROPERTIES
    IMPORTED_LOCATION "${SFML_GRAPHICS_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SFML_INCLUDE_DIR}"
)

add_library(sfml-network STATIC IMPORTED)
set_target_properties(sfml-network PROPERTIES
    IMPORTED_LOCATION "${SFML_NETWORK_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SFML_INCLUDE_DIR}"
)
