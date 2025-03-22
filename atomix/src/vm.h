#ifndef VM_H
#define VM_H

#include "value.h"
#include "instruction.h"
#include "module.h"
#include "scope.h"
#include "function.h"

#define STACK_SIZE 254

typedef struct
{
    size_t instruction_counter;
    size_t stack_counter;
    size_t stack_start;
    JSValue stack[STACK_SIZE];
} VMStats;

typedef struct VM
{
    JSModule module;
    Scope* globalScope;
    Scope* scope;
    VMStats stats;
    void (*inst_set[OPCODE_LENGTH])(struct VM*, void*);
} VM;

VM vm_init(JSModule module);

void vm_exec(VM* vm);

JSValue vm_exec_function(VM* vm, JSFunction* function);

#endif //VM_H
