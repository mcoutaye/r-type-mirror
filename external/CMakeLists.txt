if(WIN32)
    include(ExternalProject)

    ExternalProject_Add(
        sfml
        URL https://www.sfml-dev.org/files/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit.zip
        URL_MD5 1234567890abcdef1234567890abcdef  # Remplace par le vrai MD5
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(sfml SOURCE_DIR)
    set(SFML_INCLUDE_DIR ${SOURCE_DIR}/include)
    set(SFML_LIBRARY_DIR ${SOURCE_DIR}/lib)

    set(SFML_FOUND TRUE)
    set(SFML_VERSION 2.5)
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

    set(SFML_FOUND TRUE)
    set(SFML_VERSION 2.5)
    list(APPEND CMAKE_PREFIX_PATH ${SOURCE_DIR}/cmake/Modules)
endif()
