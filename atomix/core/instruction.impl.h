#ifndef INSTRUCTION_IMPL_H
#define INSTRUCTION_IMPL_H

#include "instruction.h"

#include <inttypes.h>

enum Opcode
{
    OP_NOP,
    OP_LD_INT,
    OP_LD_DOUBLE,
    OP_LD_STRING,
    OP_LD_UNDF,
    OP_LD_NULL,
    OP_LD_TRUE,
    OP_LD_FALSE,
    OP_LD_THIS,
    OP_ADD,
    OP_MINUS,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_BINARY_AND,
    OP_BINARY_OR,
    OP_BINARY_XOR,
    OP_BINARY_LSHFT,
    OP_BINARY_RSHFT,
    OP_BINARY_ZRSHFT,
    OP_BINARY_NOT,
    OP_NOT,
    OP_NEGATE,
    OP_TYPEOF,
    OP_EQ,
    OP_NEQ,
    OP_TEQ,
    OP_NTEQ,
    OP_GT,
    OP_GEQ,
    OP_LT,
    OP_LEQ,
    OP_POP,
    OP_DUP,
    OP_SWAP,
    OP_ALLOC_LOCAL,
    OP_STORE_LOCAL,
    OP_LOAD_LOCAL,
    OP_LOAD_ARG,
    OP_FUNC_DECL,
    OP_FUNC_DECL_E,
    OP_CALL,
    OP_ARR_ALLOC,
    OP_OBJ_ALLOC,
    OP_OBJ_STORE,
    OP_OBJ_LOAD,
    OP_OBJ_CLOAD,
    OP_OBJ_CSTORE,
    OP_RETURN,
    OP_PUSH_SCOPE,
    OP_POP_SCOPE,
    OP_JMP,
    OP_JMP_F,
    OP_JMP_T,
    OP_EXPORT
};

struct InstInt32
{
    Opcode opcode;
    int32_t operand;
};

struct InstDouble
{
    Opcode opcode;
    double operand;
};

struct Inst
{
    Opcode opcode;
};

struct InstUInt16
{
    Opcode opcode;
    uint16_t operand;
};

struct Inst2UInt16
{
    Opcode opcode;
    uint16_t operand;
    uint16_t operand2;
};

#endif //INSTRUCTION_IMPL_H