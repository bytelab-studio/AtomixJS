#include "value.h"

#include <stdio.h>
#include <stdlib.h>

#include "panic.h"
#include "allocator.h"
#include "api.h"

int value_is_falsy(JSValue* value)
{
    switch (value->type)
    {
    case JS_INTEGER:
    case JS_BOOLEAN:
        return value->value.as_int == 0;
    case JS_DOUBLE:
        return value->value.as_double == 0.0 || value->value.as_double == -0.0;
    case JS_STRING:
        // TODO check for empty string
        return 0;
    case JS_OBJECT:
    case JS_FUNC:
        return 0;
    case JS_UNDEFINED:
    case JS_NULL:
        return 1;
    }

    PANIC("Undefined JSValue Type");
}

int value_is_NaN(JSValue* value)
{
    if (value->type != JS_DOUBLE)
    {
        return 0;
    }

    uint64_t bits = *((uint64_t*)&value->value.as_double);
    return (bits & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL &&
        (bits & 0x7FF0000000000000ULL) != 0;
}

static char* int_to_string(int value)
{
    int size = snprintf(NULL, 0, "%d", value);
    char* str = js_malloc(size + 1);
    if (!str)
    {
        PANIC("Could not allocate memory");
    }
    snprintf(str, size + 1, "%d", value);
    return str;
}

static char* double_to_string(double value)
{
    uint64_t bits = *((uint64_t*)&value);
    if ((bits & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL &&
        (bits & 0x7FF0000000000000ULL) != 0)
    {
        return init_string("NaN");
    }

    int size = snprintf(NULL, 0, "%.7g", value);
    char* str = js_malloc(size + 1);
    if (!str)
    {
        PANIC("Could not allocate memory");
    }
    snprintf(str, size + 1, "%.7g", value);
    return str;
}

char* value_to_string(JSValue* value)
{
    switch (value->type)
    {
    case JS_INTEGER:
        return int_to_string(value->value.as_int);
    case JS_DOUBLE:
        return double_to_string(value->value.as_double);
    case JS_BOOLEAN:
        return init_string(value->value.as_int ? "true" : "false");
    case JS_NULL:
        return init_string("null");
    case JS_UNDEFINED:
        return init_string("undefined");
    case JS_OBJECT:
        return init_string("[Object]");
    case JS_FUNC:
        return init_string("[Function]");
    case JS_STRING:
        return value->value.as_pointer;
    }

    PANIC("Undefined JSValue Type");
}
