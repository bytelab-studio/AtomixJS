#include "value.h"

#include <stdio.h>
#include <stdlib.h>

#include "panic.h"

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
