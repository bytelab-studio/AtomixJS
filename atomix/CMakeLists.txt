cmake_minimum_required(VERSION 3.30)
project(atomix C)

set(CMAKE_C_STANDARD 99)

add_executable(atomix main.c
        src/instruction.c
        src/instruction.h
        src/loader.c
        src/loader.h
        src/module.c
        src/module.h
        src/panic.h
        src/vm.c
        src/vm.h
        src/object.c
        src/object.h
        src/dict.c
        src/dict.h
        src/scope.c
        src/scope.h
        src/value.h
        src/function.c
        src/function.h
        src/api.c
        src/api.h
        src/mod/core.c
        src/value.c
        src/allocator.c
        src/allocator.h
)