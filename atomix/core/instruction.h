#ifndef OPCODE_H
#define OPCODE_H

#define OPCODE_OF(ptr) ptr == NULL ? OP_NOP : *((Opcode*)ptr)

typedef enum Opcode Opcode;

#define OPCODE_LENGTH 55

typedef struct Inst Inst;
typedef struct InstInt32 InstInt32;
typedef struct InstDouble InstDouble;
typedef struct InstUInt16 InstUInt16;
typedef struct Inst2UInt16 Inst2UInt16;

#endif //OPCODE_H
