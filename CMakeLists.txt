cmake_minimum_required(VERSION 3.20)
project(rtype_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -I.")

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/server")

set(SRC_FILES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/Args.cpp
    ${SRC_DIR}/NetworkManager.cpp
    ${SRC_DIR}/UdpReceiver.cpp
    ${SRC_DIR}/UdpSender.cpp
    ${SRC_DIR}/GameLoop.cpp
    ${SRC_DIR}/PacketInterpreter.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        pthread
        sfml-network
        sfml-system
)

add_custom_target(myclean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/*.o
    COMMENT "Supprime les fichiers objets"
)

add_custom_target(myfclean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/*.o
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    COMMENT "Supprime les fichiers objets et l'exécutable"
)

add_custom_target(myre
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target myfclean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    COMMENT "Nettoie et recompile tout"
)
