#ifndef EXECUTION_H
#define EXECUTION_H

#include "value.h"
#include "module.h"
#include "vm.h"
#include "function.h"

VM vm_init(JSModule module);

void vm_exec(VM* vm);

JSValue vm_exec_function(VM* vm, JSFunction* function);

void vm_free(VM vm);

#endif //EXECUTION_H
