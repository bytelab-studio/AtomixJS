#include "loader.h"
#include "panic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "instruction.h"

#define EXPORT_BUCKET_SIZE 16

JSModule module_load_from_file(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        PANIC("Could not open file");
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = js_malloc(size);
    if (!buffer)
    {
        PANIC("Could not allocate memory");
    }
    fread(buffer, size, 1, file);
    fclose(file);

    JSModule module = module_load_from_buffer(buffer);
    js_free(buffer);
    return module;
}

#define READ_BLOCK(buff, position, offset, shift) (((uint8_t)buff[position - offset]) << shift)
#define READ_U16(buff, position) (position += 2, (uint16_t)(READ_BLOCK(buff, position, 1, 8) | READ_BLOCK(buff, position, 2, 0)))
#define READ_U32(buff, position) (position += 4, (uint32_t)(READ_BLOCK(buff, position, 1, 24) | READ_BLOCK(buff, position, 2, 16) | READ_BLOCK(buff, position, 3, 8) | READ_BLOCK(buff, position, 4, 0)))
#define READ_I32(buff, position) (position += 4, (int32_t)(READ_BLOCK(buff, position, 1, 24) | READ_BLOCK(buff, position, 2, 16) | READ_BLOCK(buff, position, 3, 8) | READ_BLOCK(buff, position, 4, 0)))

#define READ_DOUBLE(buff, position) ({          \
    double _val;                                \
    memcpy(&_val, buff + position, 8);          \
    position += 8;                              \
    _val;                                       \
})

StringTable load_string_table(const char* buff)
{
    size_t position = 0;
    StringTable string_table;

    string_table.length = READ_U32(buff, position);
    string_table.count = READ_U32(buff, position);
    string_table.offsets = js_malloc(string_table.count * sizeof(uint32_t));
    if (!string_table.offsets)
    {
        PANIC("Could not allocate memory");
    }
    memcpy(string_table.offsets, buff + position, string_table.count * sizeof(uint32_t));
    size_t str_buff_length = string_table.length - string_table.count * sizeof(uint32_t) - 2 * sizeof(uint32_t);
    string_table.strings = js_malloc(str_buff_length);
    memcpy(string_table.strings, buff + position + string_table.count * sizeof(uint32_t), str_buff_length);

    return string_table;
}

void* load_instruction(const char* buff, size_t* start_position)
{
    size_t position = *start_position;
    Opcode opcode = (Opcode)buff[position++];
    void* inst;
    switch (opcode)
    {
    default:
    case OP_NOP:
        inst = NULL;
        break;
    case OP_LD_INT:
        {
            InstInt32* x = js_malloc(sizeof(InstInt32));
            x->opcode = opcode;
            x->operand = READ_I32(buff, position);
            inst = x;
            break;
        }
    case OP_LD_DOUBLE:
        {
            InstDouble* x = js_malloc(sizeof(InstDouble));
            x->opcode = opcode;
            x->operand = READ_DOUBLE(buff, position);
            inst = x;
            break;
        }
    case OP_ADD:
    case OP_MINUS:
    case OP_MUL:
    case OP_DIV:
    case OP_MOD:
    case OP_BINARY_AND:
    case OP_BINARY_OR:
    case OP_BINARY_XOR:
    case OP_BINARY_LSHFT:
    case OP_BINARY_RSHFT:
    case OP_BINARY_ZRSHFT:
    case OP_BINARY_NOT:
    case OP_NOT:
    case OP_NEGATE:
    case OP_TYPEOF:
    case OP_TEQ:
    case OP_NTEQ:
    case OP_GT:
    case OP_GEQ:
    case OP_LT:
    case OP_LEQ:
    case OP_POP:
    case OP_DUP:
    case OP_SWAP:
    case OP_LD_UNDF:
    case OP_LD_NULL:
    case OP_LD_TRUE:
    case OP_LD_FALSE:
    case OP_ARR_ALLOC:
    case OP_OBJ_ALLOC:
    case OP_OBJ_CLOAD:
    case OP_OBJ_CSTORE:
    case OP_RETURN:
    case OP_PUSH_SCOPE:
    case OP_POP_SCOPE:
        {
            Inst* x = js_malloc(sizeof(Inst));
            x->opcode = opcode;
            inst = x;
            break;
        }
    case OP_LD_STRING:
    case OP_ALLOC_LOCAL:
    case OP_STORE_LOCAL:
    case OP_LOAD_LOCAL:
    case OP_LOAD_ARG:
    case OP_FUNC_DECL_E:
    case OP_CALL:
    case OP_OBJ_STORE:
    case OP_OBJ_LOAD:
    case OP_JMP:
    case OP_JMP_F:
    case OP_JMP_T:
    case OP_EXPORT:
        {
            InstUInt16* x = js_malloc(sizeof(InstUInt16));
            x->opcode = opcode;
            x->operand = READ_U16(buff, position);
            inst = x;
            break;
        }
    case OP_FUNC_DECL:
        {
            Inst2UInt16* x = js_malloc(sizeof(Inst2UInt16));
            x->opcode = opcode;
            x->operand = READ_U16(buff, position);
            x->operand2 = READ_U16(buff, position);
            inst = x;
            break;
        }
    }

    *start_position = position;
    return inst;
}

DataSection load_data_section(const char* buff)
{
    size_t position = 0;
    DataSection data_section;

    data_section.length = READ_U32(buff, position);
    data_section.count = READ_U32(buff, position);
    data_section.instructions = js_malloc(data_section.count * sizeof(void*));
    for (size_t i = 0; i < data_section.count; i++)
    {
        data_section.instructions[i] = load_instruction(buff, &position);
    }

    return data_section;
}

JSModule module_load_from_buffer(char* buff)
{
    JSModule module;
    size_t position = 0;
    module.header.magic[0] = buff[position++];
    module.header.magic[1] = buff[position++];
    module.header.magic[2] = buff[position++];


    if (module.header.magic[0] != MAGIC0 ||
        module.header.magic[1] != MAGIC1 ||
        module.header.magic[2] != MAGIC2)
    {
        PANIC("Invalid magic number");
    }

    module.header.version = READ_U16(buff, position);
    if (module.header.version != VERSION)
    {
        PANIC("Invalid VM Version");
    }

    module.header.string_table = READ_U32(buff, position);
    module.header.data_section = READ_U32(buff, position);

    module.string_table = load_string_table(buff + module.header.string_table);
    module.data_section = load_data_section(buff + module.header.data_section);
    module.exports = object_create_object(object_get_object_prototype());
    return module;
}

void module_free(JSModule module)
{
    js_free(module.string_table.offsets);
    js_free(module.string_table.strings);
    for (size_t i = 0; i < module.data_section.count; i++)
    {
        if (module.data_section.instructions[i] != NULL)
        {
            js_free(module.data_section.instructions[i]);
        }
    }
    js_free(module.data_section.instructions);
    object_free(module.exports);
}
