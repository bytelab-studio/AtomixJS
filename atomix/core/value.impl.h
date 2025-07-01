#ifndef VALUE_IMPL_H
#define VALUE_IMPL_H

#include "value.h"

#include <stdint.h>

#define JS_NaN 0.0 / 0.0
#define JS_POS_INFINITY 1 / 0.0
#define JS_NEG_INFINITY -1 / 0.0

#define JS_VALUE_NUMBER(x) ((JSValue){.type = JS_NUMBER, .value.as_number = x})
#define JS_VALUE_STRING(x) ((JSValue){.type = JS_STRING, .value.as_pointer = x})
#define JS_VALUE_UNDEFINED ((JSValue){.type = JS_UNDEFINED, .value.as_boolean = 0})
#define JS_VALUE_NULL ((JSValue){.type = JS_NULL, .value.as_boolean = 0})
#define JS_VALUE_FUNCTION(func) ((JSValue){.type = JS_FUNC, .value.as_pointer = func})
#define JS_VALUE_OBJECT(obj) ((JSValue){.type = JS_OBJECT, .value.as_pointer = obj})
#define JS_VALUE_BOOL(state) ((JSValue){.type = JS_BOOLEAN, .value.as_boolean = state})
#define JS_VALUE_SYMBOL(symbol) ((JSValue){.type = JS_SYMBOL, .value.as_pointer = symbol})

enum JSValueType
{
    JS_NUMBER,
    JS_STRING,
    JS_OBJECT,
    JS_FUNC,
    JS_UNDEFINED,
    JS_NULL,
    JS_BOOLEAN,
    JS_SYMBOL,
    JS_GS_BOX
};

struct JSValue
{
    JSValueType type;

    union
    {
        int as_boolean;
        double as_number;
        void* as_pointer;
    } value;
};

struct JSString {
    uint32_t length;
    char* buff;
};

#endif //VALUE_IMPL_H