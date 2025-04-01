#ifndef OPCODE_H
#define OPCODE_H

#include <inttypes.h>

#define OPCODE_OF(ptr) ptr == NULL ? OP_NOP : *((Opcode*)ptr)

typedef enum
{
    OP_NOP,
    OP_LD_INT,
    OP_LD_DOUBLE,
    OP_LD_STRING,
    OP_LD_UNDF,
    OP_LD_NULL,
    OP_LD_TRUE,
    OP_LD_FALSE,
    OP_ADD,
    OP_MINUS,
    OP_BINARY_AND,
    OP_BINARY_OR,
    OP_BINARY_XOR,
    OP_BINARY_LSHFT,
    OP_BINARY_RSHFT,
    OP_BINARY_ZRSHFT,
    OP_TEQ,
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
    OP_OBJ_ALLOC,
    OP_OBJ_STORE,
    OP_OBJ_LOAD,
    OP_RETURN,
    OP_PUSH_SCOPE,
    OP_POP_SCOPE,
    OP_JMP,
    OP_JMP_F,
    OP_JMP_T,
    OP_EXPORT
} Opcode;

#define OPCODE_LENGTH 37

typedef struct
{
    Opcode opcode;
    int32_t operand;
} InstInt32;

typedef struct
{
    Opcode opcode;
    double operand;
} InstDouble;

typedef struct
{
    Opcode opcode;
} Inst;

typedef struct
{
    Opcode opcode;
    uint16_t operand;
} InstUInt16;

typedef struct
{
    Opcode opcode;
    uint16_t operand;
    uint16_t operand2;
} Inst2UInt16;

#endif //OPCODE_H
