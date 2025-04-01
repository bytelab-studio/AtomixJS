add_executable(prod main.c)

target_link_libraries(prod PRIVATE -Wl,--whole-archive core -Wl,--no-whole-archive)

foreach (MODULE ${DEBUG_MODULE_TO_LINK})
    target_link_libraries(prod PRIVATE -Wl,--whole-archive ${MODULE} -Wl,--no-whole-archive)
endforeach ()

target_include_directories(prod PRIVATE ${CMAKE_SOURCE_DIR}/core)