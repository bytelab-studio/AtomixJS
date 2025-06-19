#include <stdio.h>
#include <gc.h>

#include "loader.h"
#include "format.h"
#include "execution.h"

static JSModule _module;
static JSBundle _bundle;

static JSModule* module = &_module;
static JSBundle* bundle = &_bundle;

int main(int argc, const char** argv)
{
    GC_init();
    if (argc < 2)
    {
        printf("Usage: ./atomix <filename>\n");
        return 1;
    }
    const char* bin_file = argv[1];

    LoadResult result = unknown_load_from_file(bin_file, module, bundle);
    
    if (result == LOAD_BUNDLE) {
        if (!bundle->entryPoint)
        {
            return 0;
        }

        module = bundle_get_module(bundle, bundle->entryPoint);
    }

    VM vm = vm_init(module);
    vm_exec_module(&vm, module);    

    return 0;
}
