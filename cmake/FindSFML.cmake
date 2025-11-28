find_path(SFML_INCLUDE_DIR
    NAMES SFML/Config.hpp
    PATHS
    ${SFML_ROOT}/include
    /usr/local/include
    /usr/include
    /opt/local/include
    /sw/include
)

find_library(SFML_SYSTEM_LIBRARY
    NAMES sfml-system
    PATHS
    ${SFML_ROOT}/lib
    /usr/local/lib
    /usr/lib
    /opt/local/lib
    /sw/lib
)

find_library(SFML_NETWORK_LIBRARY
    NAMES sfml-network
    PATHS
    ${SFML_ROOT}/lib
    /usr/local/lib
    /usr/lib
    /opt/local/lib
    /sw/lib
)

find_library(SFML_WINDOW_LIBRARY
    NAMES sfml-window
    PATHS
    ${SFML_ROOT}/lib
    /usr/local/lib
    /usr/lib
    /opt/local/lib
    /sw/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SFML
    REQUIRED_VARS SFML_INCLUDE_DIR SFML_SYSTEM_LIBRARY SFML_NETWORK_LIBRARY
    VERSION_VAR SFML_VERSION
)

if(SFML_FOUND)
    file(READ "${SFML_INCLUDE_DIR}/SFML/Config.hpp" SFML_CONFIG)
    string(REGEX MATCH "define SFML_VERSION_MAJOR [0-9]+" _ ${SFML_CONFIG})
    set(SFML_VERSION_MAJOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "define SFML_VERSION_MINOR [0-9]+" _ ${SFML_CONFIG})
    set(SFML_VERSION_MINOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "define SFML_VERSION_PATCH [0-9]+" _ ${SFML_CONFIG})
    set(SFML_VERSION_PATCH ${CMAKE_MATCH_1})
    set(SFML_VERSION ${SFML_VERSION_MAJOR}.${SFML_VERSION_MINOR}.${SFML_VERSION_PATCH})
    set(SFML_INCLUDE_DIRS ${SFML_INCLUDE_DIR})
    set(SFML_LIBRARIES
        ${SFML_SYSTEM_LIBRARY}
        ${SFML_NETWORK_LIBRARY}
        ${SFML_WINDOW_LIBRARY}
    )
endif()

mark_as_advanced(
    SFML_INCLUDE_DIR
    SFML_SYSTEM_LIBRARY
    SFML_NETWORK_LIBRARY
    SFML_WINDOW_LIBRARY
)
