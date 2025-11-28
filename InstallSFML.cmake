if(UNIX AND NOT APPLE)
    find_program(APT apt)
    if(APT)
        execute_process(
            COMMAND sudo apt update
            COMMAND sudo apt install -y libsfml-dev
            RESULT_VARIABLE apt_result
        )
        if(NOT apt_result EQUAL 0)
            message(FATAL_ERROR "Échec de l'installation de SFML avec apt. Veuillez l'installer manuellement.")
        endif()
    else()
        message(FATAL_ERROR "APT non trouvé. Veuillez installer SFML manuellement.")
    endif()
elseif(WIN32)
    find_program(VCPKG vcpkg)
    if(VCPKG)
        execute_process(
            COMMAND vcpkg install sfml
            RESULT_VARIABLE vcpkg_result
        )
        if(NOT vcpkg_result EQUAL 0)
            message(FATAL_ERROR "Échec de l'installation de SFML avec vcpkg. Veuillez l'installer manuellement.")
        endif()
    else()
        message(WARNING "vcpkg non trouvé. SFML doit être installé manuellement.")
        set(SFML_ROOT "C:/SFML" CACHE PATH "Chemin vers SFML sur Windows")
        find_package(SFML 2.5 COMPONENTS system network window REQUIRED)
    endif()
else()
    message(FATAL_ERROR "Système non supporté. Veuillez installer SFML manuellement.")
endif()

find_package(SFML 2.5 COMPONENTS system network window REQUIRED)
