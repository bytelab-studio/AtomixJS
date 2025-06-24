#include <stdio.h>
#include <gc.h>

#include "AtomixJS.h"

#include "loader.impl.h"
#include "format.impl.h"
#include "vm.impl.h"

static JSModule _module;
static JSBundle _bundle;

static JSModule* module = &_module;
static JSBundle* bundle = &_bundle;

extern uint8_t __BYTECODE__[];
extern const size_t __BYTECODE_SIZE__;

int main(int argc, const char** argv)
{
    GC_init();

    LoadResult result = unknown_load_from_buffer(__BYTECODE__, module, bundle);
    
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
