#include <stdio.h>

#include "src/loader.h"
#include "src/vm.h"

#define TESTING

void print_data(void** instructions, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        printf("%zu: %i\n", i, OPCODE_OF(instructions[i]));
    }
}

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        printf("Usage: ./atomix <filename>\n");
        return 1;
    }
    const char* bin_file = argv[1];

    JSModule module = module_load_from_file(bin_file);
#ifndef TESTING
    print_data(module.data_section.instructions, module.data_section.count);
#endif
    VM vm = vm_init(module);
    while (vm.stats.instruction_counter < vm.module.data_section.count)
    {
        vm_exec(&vm);
    }
    return 0;
}
