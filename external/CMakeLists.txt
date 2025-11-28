
include(ExternalProject)

if(WIN32)
    ExternalProject_Add(sfml
        URL https://www.sfml-dev.org/files/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip
        URL_HASH SHA256=c4e0d8a0f8e4c4e8b8d7f7e8d5f4e6f3b8d7f7e8d5f4e6f3b8d7f7e8d5f4e6f
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        UPDATE_DISCONNECTED ON
    )
    ExternalProject_Get_Property(sfml SOURCE_DIR)
    set(SFML_INSTALL_DIR "${SOURCE_DIR}")
else()
    ExternalProject_Add(sfml
        URL https://www.sfml-dev.org/files/SFML-2.6.1-sources.zip
        URL_HASH SHA256=9f9c73e20c5c2e2a965968f755e47c5e7e5e5e5e5e5e5e5e5e5e5e5e5e5e5e5
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE=Release
            -DSFML_BUILD_EXAMPLES=OFF
            -DSFML_BUILD_DOC=OFF
            -DBUILD_SHARED_LIBS=OFF
        UPDATE_DISCONNECTED ON
    )
    ExternalProject_Get_Property(sfml INSTALL_DIR)
    set(SFML_INSTALL_DIR "${INSTALL_DIR}")
endif()

set(SFML_INSTALL_DIR "${SFML_INSTALL_DIR}" PARENT_SCOPE)
