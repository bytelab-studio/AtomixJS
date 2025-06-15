#ifndef EXECUTION_H
#define EXECUTION_H

#include "value.h"
#include "format.h"
#include "vm.h"
#include "function.h"

VM vm_init(JSModule* module);

void vm_exec_module(VM* vm, JSModule* module);

JSValue vm_exec_function(VM* vm, JSFunction* function);

#endif //EXECUTION_H
