#include <stdio.h>

#include "src/loader.h"
#include "src/execution.h"
#include "src/allocator.h"

// #define PRINT_MEMORY_USAGE

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        printf("Usage: ./atomix <filename>\n");
        return 1;
    }
    const char* bin_file = argv[1];

    JSModule module = module_load_from_file(bin_file);
    VM vm = vm_init(module);
    while (vm.stats.instruction_counter < vm.module.data_section.count)
    {
        vm_exec(&vm);
    }
    module_free(module);
    vm_free(vm);

#ifdef PRINT_MEMORY_USAGE
    printf("Memory usage:\nAllocated: %zu\nFreed: %zu\n", allocated_memory, freed_memory);
#endif


    return 0;
}
