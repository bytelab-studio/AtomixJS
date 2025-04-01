add_executable(atomix main.c)

target_link_libraries(atomix PRIVATE -Wl,--whole-archive core -Wl,--no-whole-archive)

foreach (MODULE ${DEBUG_MODULE_TO_LINK})
    target_link_libraries(atomix PRIVATE -Wl,--whole-archive ${MODULE} -Wl,--no-whole-archive)
endforeach ()

target_include_directories(atomix PRIVATE ${CMAKE_SOURCE_DIR}/core)