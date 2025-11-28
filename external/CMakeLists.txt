if(WIN32)
    include(ExternalProject)

    ExternalProject_Add(
        sfml
        URL https://www.sfml-dev.org/files/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit.zip
        URL_MD5 1234567890abcdef1234567890abcdef
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(sfml SOURCE_DIR)
    set(SFML_INCLUDE_DIR ${SOURCE_DIR}/include)
    set(SFML_LIBRARY_DIR ${SOURCE_DIR}/lib)
else()
    include(ExternalProject)

    ExternalProject_Add(
        sfml
        URL https://www.sfml-dev.org/files/SFML-2.5.1-sources.zip
        URL_MD5 7e396d95b12568f2b590eaf6f7257e82
        CMAKE_ARGS
            -DSFML_BUILD_EXAMPLES=OFF
            -DSFML_BUILD_DOC=OFF
            -DSFML_BUILD_TESTS=OFF
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(sfml SOURCE_DIR BINARY_DIR)
    set(SFML_INCLUDE_DIR ${SOURCE_DIR}/include)
    set(SFML_LIBRARY_DIR ${BINARY_DIR}/lib)
endif()

set(SFML_SYSTEM_LIBRARY "${SFML_LIBRARY_DIR}/libsfml-system.a")
set(SFML_WINDOW_LIBRARY "${SFML_LIBRARY_DIR}/libsfml-window.a")
set(SFML_GRAPHICS_LIBRARY "${SFML_LIBRARY_DIR}/libsfml-graphics.a")
set(SFML_NETWORK_LIBRARY "${SFML_LIBRARY_DIR}/libsfml-network.a")

add_library(sfml-system INTERFACE IMPORTED)
target_include_directories(sfml-system INTERFACE ${SFML_INCLUDE_DIR})
target_link_libraries(sfml-system INTERFACE "${SFML_LIBRARY_DIR}/libsfml-system.a")

add_library(sfml-window INTERFACE IMPORTED)
target_include_directories(sfml-window INTERFACE ${SFML_INCLUDE_DIR})
target_link_libraries(sfml-window INTERFACE "${SFML_LIBRARY_DIR}/libsfml-window.a")

add_library(sfml-graphics INTERFACE IMPORTED)
target_include_directories(sfml-graphics INTERFACE ${SFML_INCLUDE_DIR})
target_link_libraries(sfml-graphics INTERFACE "${SFML_LIBRARY_DIR}/libsfml-graphics.a")

add_library(sfml-network INTERFACE IMPORTED)
target_include_directories(sfml-network INTERFACE ${SFML_INCLUDE_DIR})
target_link_libraries(sfml-network INTERFACE "${SFML_LIBRARY_DIR}/libsfml-network.a")

set(SFML_FOUND TRUE)
set(SFML_VERSION 2.5)
