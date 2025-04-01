cmake_minimum_required(VERSION 3.30)
project(atomix C)

set(CMAKE_C_STANDARD 99)

# Module List
option(MOD_CORE, "Includes core module" OFF)

file(GLOB MODULE_DIRS RELATIVE ${CMAKE_SOURCE_DIR}/modules ${CMAKE_SOURCE_DIR}/modules/*)

set(DEBUG_MODULE_TO_LINK, "")
set(PROD_MODULE_TO_LINK, "")

foreach (MODULE_DIR ${MODULE_DIRS})
    if (NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/modules/${MODULE_DIR})
        continue()
    endif ()

    string(TOUPPER ${MODULE_DIR} MODULE_DIR_UPPER)
    set(MODULE_OPTION_NAME "MOD_${MODULE_DIR_UPPER}")
    string(TOLOWER ${MODULE_OPTION_NAME} MODULE_OPTION_NAME_LOWER)
    option(${MODULE_OPTION_NAME} "Includes the ${MODULE_DIR} module" OFF)
    add_subdirectory(modules/${MODULE_DIR})

    list(APPEND DEBUG_MODULE_TO_LINK ${MODULE_OPTION_NAME_LOWER})

    if (${MODULE_OPTION_NAME})
        list(APPEND PROD_MODULE_TO_LINK ${MODULE_OPTION_NAME_LOWER})
    endif ()
endforeach ()

add_subdirectory(core)

# Configurations
add_subdirectory(debug)
add_subdirectory(prod)