add_library(core STATIC
        instruction.c
        instruction.h
        loader.c
        loader.h
        module.c
        module.h
        panic.h
        vm.h
        object.c
        object.h
        dict.c
        dict.h
        scope.c
        scope.h
        value.c
        value.h
        function.c
        function.h
        api.c
        api.h
        allocator.c
        allocator.h
        execution.c
        execution.h
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})