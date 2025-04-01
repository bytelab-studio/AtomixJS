add_library(mod_core STATIC
        core.c
)

target_link_libraries(mod_core PRIVATE core)