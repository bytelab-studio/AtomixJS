#include "loader.impl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gc.h>

#include "panic.h"
#include "scope.h"

#include "format.impl.h"
#include "instruction.impl.h"

#define EXPORT_BUCKET_SIZE 16

static uint8_t* loader_read_file(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file) {
        PANIC("Could not open file");
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t* buffer = GC_malloc_uncollectable(size);
    if (!buffer) {
        PANIC("Could not allocate memory");
    }
    fread(buffer, size, 1, file);
    fclose(file);
    return buffer;
}

void module_load_from_file(const char* filename, JSModule* module)
{
    uint8_t* buffer = loader_read_file(filename);
    module_load_from_buffer(buffer, module);
    GC_free(buffer);
}

void bundle_load_from_file(const char* filename, JSBundle* bundle)
{
    uint8_t* buffer = loader_read_file(filename);
    bundle_load_from_buffer(buffer, bundle);
    GC_free(buffer);
}

LoadResult unknown_load_from_file(const char* filename, JSModule* module, JSBundle* bundle)
{
    uint8_t* buffer = loader_read_file(filename);
    LoadResult result = unknown_load_from_buffer(buffer, module, bundle);
    GC_free(buffer);
    return result;
}

#define READ_BLOCK(buff, position, offset, shift) (((uint32_t)(uint8_t)buff[position - offset]) << shift)
#define READ_BIG_BLOCK(buff, position, offset, shift) (((uint64_t)(uint8_t)buff[position - offset]) << shift)
#define READ_U16(buff, position) (position += 2, (uint16_t)(READ_BLOCK(buff, position, 1, 8) | READ_BLOCK(buff, position, 2, 0)))
#define READ_U32(buff, position) (position += 4, (uint32_t)(READ_BLOCK(buff, position, 1, 24) | READ_BLOCK(buff, position, 2, 16) | READ_BLOCK(buff, position, 3, 8) | READ_BLOCK(buff, position, 4, 0)))
#define READ_I32(buff, position) (position += 4, (int32_t)(READ_BLOCK(buff, position, 1, 24) | READ_BLOCK(buff, position, 2, 16) | READ_BLOCK(buff, position, 3, 8) | READ_BLOCK(buff, position, 4, 0)))
#define READ_U64(buff, position) (position += 8, (uint64_t)(READ_BIG_BLOCK(buff, position, 1, 56) | READ_BIG_BLOCK(buff, position, 2, 48) | READ_BIG_BLOCK(buff, position, 3, 40) | READ_BIG_BLOCK(buff, position, 4, 32) | \
                                                            READ_BIG_BLOCK(buff, position, 5, 24) | READ_BIG_BLOCK(buff, position, 6, 16) | READ_BIG_BLOCK(buff, position, 7, 8) | READ_BIG_BLOCK(buff, position, 8, 0)))

static double READ_DOUBLE(const uint8_t* buff, size_t* position)
{
    double _val;
    memcpy(&_val, buff + *position, 8);
    *position += 8;
    return _val;
}

static StringTable load_string_table(const uint8_t* buff)
{
    size_t position = 0;
    StringTable string_table;

    string_table.length = READ_U32(buff, position);
    string_table.count = READ_U32(buff, position);
    string_table.offsets = GC_malloc(string_table.count * sizeof(uint32_t));
    if (!string_table.offsets)
    {
        PANIC("Could not allocate memory");
    }
    memcpy(string_table.offsets, buff + position, string_table.count * sizeof(uint32_t));
    size_t str_buff_length = string_table.length - string_table.count * sizeof(uint32_t) - 2 * sizeof(uint32_t);
    string_table.strings = GC_malloc(str_buff_length);
    memcpy(string_table.strings, buff + position + string_table.count * sizeof(uint32_t), str_buff_length);

    return string_table;
}

static void* load_instruction(const uint8_t* buff, size_t* start_position)
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
            InstInt32* x = GC_malloc_atomic(sizeof(InstInt32));
            x->opcode = opcode;
            x->operand = READ_I32(buff, position);
            inst = x;
            break;
        }
    case OP_LD_DOUBLE:
        {
            InstDouble* x = GC_malloc_atomic(sizeof(InstDouble));
            x->opcode = opcode;
            x->operand = READ_DOUBLE(buff, &position);
            inst = x;
            break;
        }
    case OP_LD_THIS:
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
            Inst* x = GC_malloc_atomic(sizeof(Inst));
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
            InstUInt16* x = GC_malloc_atomic(sizeof(InstUInt16));
            x->opcode = opcode;
            x->operand = READ_U16(buff, position);
            inst = x;
            break;
        }
    case OP_FUNC_DECL:
        {
            Inst2UInt16* x = GC_malloc_atomic(sizeof(Inst2UInt16));
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

static DataSection load_data_section(const uint8_t* buff)
{
    size_t position = 0;
    DataSection data_section;

    data_section.length = READ_U32(buff, position);
    data_section.count = READ_U32(buff, position);
    data_section.instructions = GC_malloc(data_section.count * sizeof(void*));
    for (size_t i = 0; i < data_section.count; i++)
    {
        data_section.instructions[i] = load_instruction(buff, &position);
    }

    return data_section;
}

static void module_load_from_buffer_offset(uint8_t* buff, JSModule* module, size_t* pos)
{
    module->bundle = NULL;

    size_t position = *pos;
    module->header.magic[0] = buff[position++];
    module->header.magic[1] = buff[position++];
    module->header.magic[2] = buff[position++];
    module->header.magic[3] = buff[position++];

    if (module->header.magic[0] != MODULE_MAGIC0 ||
        module->header.magic[1] != MODULE_MAGIC1 ||
        module->header.magic[2] != MODULE_MAGIC2 ||
        module->header.magic[3] != MODULE_MAGIC3)
    {
        PANIC("Invalid magic number");
    }

    module->header.version = READ_U16(buff, position);
    if (module->header.version != MODULE_VERSION)
    {
        PANIC("Invalid VM Version");
    }

    module->header.hash = READ_U64(buff, position);
    module->header.string_table = READ_U32(buff, position);
    module->header.data_section = READ_U32(buff, position);

    module->string_table = load_string_table(buff + module->header.string_table + *pos);
    module->data_section = load_data_section(buff + module->header.data_section + *pos);
    module->initialized = 0;
    module->exports = object_create_object(object_get_object_prototype());
    module->scope = scope_create_scope(NULL);
    *pos = position;
}

void module_load_from_buffer(uint8_t* buff, JSModule* module)
{
    size_t pos = 0;
    return module_load_from_buffer_offset(buff, module, &pos);
}

void bundle_load_from_buffer(uint8_t* buff, JSBundle* bundle)
{
    size_t position = 0;
    bundle->magic[0] = buff[position++];
    bundle->magic[1] = buff[position++];
    bundle->magic[2] = buff[position++];
    bundle->magic[3] = buff[position++];

    if (bundle->magic[0] != BUNDLE_MAGIC0 ||
        bundle->magic[1] != BUNDLE_MAGIC1 ||
        bundle->magic[2] != BUNDLE_MAGIC2 ||
        bundle->magic[3] != BUNDLE_MAGIC3)
    {
        PANIC("Invalid magic number");
    }

    bundle->version = READ_U16(buff, position);
    if (bundle->version != BUNDLE_VERSION)
    {
        PANIC("Invalid VM Version");
    }

    bundle->entryPoint = READ_U64(buff, position);
    bundle->moduleCount = READ_U16(buff, position);
    bundle->modules = GC_malloc(bundle->moduleCount * sizeof(JSModule));
    if (!bundle->modules)
    {
        PANIC("Could not allocate memory");
    }

    for (uint16_t i = 0; i < bundle->moduleCount; i++)
    {
        module_load_from_buffer_offset(buff, &bundle->modules[i], &position);
        bundle->modules[i].bundle = bundle;
    }
}

LoadResult unknown_load_from_buffer(uint8_t* buff, JSModule* module, JSBundle* bundle)
{
    if (buff[0] == MODULE_MAGIC0 &&
        buff[1] == MODULE_MAGIC1 &&
        buff[2] == MODULE_MAGIC2 &&
        buff[3] == MODULE_MAGIC3)
    {
        module_load_from_buffer(buff, module);
        return LOAD_MODULE;
    }

    bundle_load_from_buffer(buff, bundle);
    return LOAD_BUNDLE;
}
