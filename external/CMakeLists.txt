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

ExternalProject_Get_Property(sfml SOURCE_DIR BINARY_DIR)

set(SFML_SOURCE_DIR ${SOURCE_DIR})
set(SFML_BINARY_DIR ${BINARY_DIR})

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SetupSFML.cmake)
