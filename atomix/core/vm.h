#ifndef VM_H
#define VM_H

#include "format.h"
#include "instruction.h"
#include "scope.h"

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
    JSModule* module;
    Scope* globalScope;
    Scope* scope;
    VMStats stats;
    void (*inst_set[OPCODE_LENGTH])(struct VM*, void*);
} VM;

#endif //VM_H
