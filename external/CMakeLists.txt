
include(ExternalProject)

if(WIN32)
    ExternalProject_Add(
        sfml
        URL https://www.sfml-dev.org/files/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip
        URL_HASH SHA256=9f7b5c4e8d5f4e6f3b8d7f7e8d5f4e6f3b8d7f7e8d5f4e6f3b8d7f7e8d5f4e6f
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )
    ExternalProject_Get_Property(sfml SOURCE_DIR)
    set(SFML_ROOT "${SOURCE_DIR}" PARENT_SCOPE)
else()
    ExternalProject_Add(
        sfml
        URL https://www.sfml-dev.org/files/SFML-2.6.1-sources.zip
        URL_HASH SHA256=5b5c2f8f7e6f8d5f4e6f3b8d7f7e8d5f4e6f3b8d7f7e8d5f4e6f3b8d7f7e8d5f
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE=Release
            -DSFML_BUILD_EXAMPLES=OFF
            -DSFML_BUILD_DOC=OFF
            -DSFML_BUILD_NETWORK=ON
            -DBUILD_SHARED_LIBS=OFF
    )
    ExternalProject_Get_Property(sfml INSTALL_DIR)
    set(SFML_ROOT "${INSTALL_DIR}" PARENT_SCOPE)
endif()

ExternalProject_Get_Property(sfml SOURCE_DIR INSTALL_DIR)
set(SFML_SOURCE_DIR ${SOURCE_DIR} PARENT_SCOPE)
if(WIN32)
    set(SFML_INSTALL_DIR ${SOURCE_DIR} PARENT_SCOPE)
else()
    set(SFML_INSTALL_DIR ${INSTALL_DIR} PARENT_SCOPE)
endif()