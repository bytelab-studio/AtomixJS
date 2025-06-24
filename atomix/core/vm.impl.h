#ifndef VM_IMPL_H
#define VM_IMPL_H

#include "vm.h"

#include "format.h"
#include "scope.h"
#include "instruction.impl.h"

#include "value.impl.h"

#define STACK_SIZE 254

struct VMStats
{
    size_t instruction_counter;
    size_t stack_counter;
    size_t stack_start;
    JSValue stack[STACK_SIZE];
};

struct VM
{
    JSModule* module;
    Scope* globalScope;
    Scope* scope;
    VMStats stats;
    void (*inst_set[OPCODE_LENGTH])(struct VM*, void*);
};

#endif //VM_IMPL_H