#include "value.impl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <gc.h>

#include "panic.h"
#include "api.h"
#include "symbol.h"
#include "number.h"

#include "object.impl.h"
#include "function.impl.h"

int value_is_NaN(JSValue value)
{
    if (value.type != JS_NUMBER)
    {
        return 0;
    }

    uint64_t bits = *((uint64_t*)&value.value.as_number);
    return (bits & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL &&
        (bits & 0x7FF0000000000000ULL) != 0;
}

JSValue value_to_boolean(JSValue value)
{
    if (value.type == JS_BOOLEAN)
    {
        return value;
    }

    if (value.type == JS_UNDEFINED || value.type == JS_NULL)
    {
        return JS_VALUE_BOOL(0);
    }
    
    // TODO test for BigInt
    if (value.type == JS_NUMBER)
    {
        return JS_VALUE_BOOL(
            !(value.value.as_number == 0.0 || value.value.as_number == -0.0 || value_is_NaN(value))
        );
    }

    if (value.type == JS_STRING)
    {
        JS_VALUE_BOOL(((JSString*)value.value.as_pointer)->length > 0);
    }

    return JS_VALUE_BOOL(1);
}

JSValue value_to_string(VM* vm, JSValue value)
{
    switch(value.type)
    {
    case JS_STRING:
        return value;
    case JS_SYMBOL:
        // TODO throw TypeError
        return JS_VALUE_UNDEFINED;
    case JS_UNDEFINED:
        return init_string_value("undefined");
    case JS_NULL:
        return init_string_value("null");
    case JS_BOOLEAN:
        return init_string_value(value.value.as_boolean ? "true" : "false");
    case JS_NUMBER:
        return init_string_value("TODO");
        // TODO respect BigInt
    default:
        value = value_to_primitive(vm, value, JS_STRING);
        return value_to_string(vm, value);
    }
}

int value_is_array(JSValue* value)
{
    if (value->type != JS_OBJECT)
    {
        return 0;
    }

    JSObject* object_prototype = object_get_object_prototype();
    JSObject* array_prototype = object_get_array_prototype();

    JSObject* object = value->value.as_pointer;
    JSObject* prototype = object->prototype;
    while (prototype != object_prototype)
    {
        if (prototype == array_prototype)
        {
            return 1;
        }
        prototype = prototype->prototype;
    }

    return 0;
}

inline int value_is_primitive(JSValueType type)
{
    return type != JS_OBJECT && type != JS_FUNC;
}

JSValue value_to_object(JSValue value)
{
    switch(value.type)
    {
    case JS_NUMBER:
        // TODO
        PANIC("Not implemented");
        break;
    case JS_STRING:
        // TODO
        PANIC("Not implemented");
        break;
    case JS_OBJECT:
        return JS_VALUE_OBJECT(value.value.as_pointer);
    case JS_FUNC:
        return JS_VALUE_OBJECT(((JSFunction*)value.value.as_pointer)->base);
    case JS_UNDEFINED:
    case JS_NULL:
        // TODO throw TypeError
        PANIC("Not implemented");
    case JS_BOOLEAN:
        // TODO
        PANIC("Not implemented");
    case JS_SYMBOL:
        return JS_VALUE_OBJECT(value.value.as_pointer);
    default:
        PANIC("Cannot box value");
    }
}

JSValue value_concat_string(JSValue left, JSValue right)
{
    if (left.type != JS_STRING || right.type != JS_STRING)
    {
        PANIC("Faulty values in function");
    }

    JSString* l = left.value.as_pointer;
    JSString* r = right.value.as_pointer;
    char* buff = GC_malloc_atomic(l->length + r->length + 1);
    memcpy(buff, l->buff, l->length);
    memcpy(buff + l->length, r->buff, r->length);
    buff[l->length + r->length] = '\0';
    
    JSString* string = GC_MALLOC(sizeof(JSString));
    string->buff = buff;
    string->length = l->length + r->length;

    return JS_VALUE_STRING(string);
}

JSValue value_to_primitive(VM* vm, JSValue input, JSValueType prefered_type)
{
    if (input.type != JS_OBJECT)
    {
        return input;
    }

    JSValue out;
    if (object_try_get_property_by_symbol(vm, input.value.as_pointer, symbol_to_primitive(vm).value.as_pointer, &out))
    {
        JSValue hint;
        switch(prefered_type)
        {
        case JS_UNDEFINED:
            hint = init_string_value("default");
            break;
        case JS_NUMBER:
            hint = init_string_value("number");
            break;
        case JS_STRING:
            hint = init_string_value("string");
            break;
        default:
            PANIC("Unknown prefered_type");
            break;
        }

        if (out.type != JS_FUNC)
        {
            // TODO throw TypeError
            PANIC("Symbol.toPrimitive holds not a function");
        }

        JSValue result = api_call_function(vm, out.value.as_pointer, input, &hint, 1);
        if (result.type == JS_OBJECT)
        {
            // TODO throw TypeError
            PANIC("Return value from Symbol.toPrimitive is an object");
        }
        return result;
    }

    if (prefered_type == JS_UNDEFINED)
    {
        prefered_type = JS_NUMBER;
    }

    return value_ordinary_to_primitive(vm, input, prefered_type);
}

JSValue value_ordinary_to_primitive(VM* vm, JSValue input, JSValueType prefered_type)
{
    char* methods[2];

    if (prefered_type == JS_STRING)
    {
        methods[0] = "toString";
        methods[1] = "valueOf";
    }
    else
    {
        methods[0] = "valueOf";
        methods[1] = "toString";
    }

    JSValue obj = value_to_object(input);

    for (int i = 0; i < 2; i++)
    {
        JSValue out;
        if (!object_try_get_property(vm, obj.value.as_pointer, init_string(methods[i]), &out) || out.type != JS_FUNC)
        {
            continue;
        }
        out = api_call_function(vm, out.value.as_pointer, input, NULL, 0);
        if (out.type != JS_OBJECT)
        {
            return out;
        }
    }

    // TODO throw TypeError
    PANIC("Could not convert to primitive");
}

JSValue value_to_numeric(VM* vm, JSValue input)
{
    input = value_to_primitive(vm, input, JS_NUMBER);
    // TOOD If primValue is a BigInt, return primValue.
    return value_to_number(vm, input);
}

JSValue value_to_number(VM* vm, JSValue input)
{
    if (input.type == JS_NUMBER)
    {
        return input;
    }
    // TODO check for BigInt
    if (input.type == JS_SYMBOL)
    {
        // TODO throw TypeError
        PANIC("Ccannot convert to number");
    }
    if (input.type == JS_UNDEFINED)
    {
        return JS_VALUE_NUMBER(JS_NaN);
    }
    if (input.type == JS_NULL || (input.type == JS_BOOLEAN && input.value.as_boolean == 0))
    {
        return JS_VALUE_NUMBER(0.0);
    }
    if (input.type == JS_BOOLEAN)
    {
        return JS_VALUE_NUMBER(1.0);
    }
    if (input.type == JS_STRING)
    {
        return value_string_to_number(vm, input);
    }
    input = value_to_primitive(vm, input, JS_NUMBER);
    return value_to_number(vm, input);
}

JSValue value_string_to_number(VM* vm, JSValue input)
{
    int is_invalid = 0;
    int is_double = 0;
    char* str = input.value.as_pointer;
    char* pos = str;
    while(pos++) {
        char c = *pos;
        if (!((c >= '0' && c <= '9') || c == '.'))
        {
            is_invalid = 1;
            break;
        }
        if (c == '.')
        {
            is_double = 1;
        }
    }

    if (is_invalid)
    {
        return JS_VALUE_NUMBER(JS_NaN);
    }

    if (is_double)
    {
        char* end;
        double val = strtod(str, &end);
        if (str == end)
        {
            return JS_VALUE_NUMBER(JS_NaN);
        }

        return JS_VALUE_NUMBER(val);
    }
    
    errno = 0;
    pos = str;
    int64_t val = strtol(str, &pos, 10);

    if ((errno == ERANGE && (val == INT64_MAX || val == INT64_MIN)) || (errno != 0 && val == 0))
    {
        return JS_VALUE_NUMBER(JS_NaN);
    }
    if (val < INT32_MIN || val > INT32_MAX)
    {
        return JS_VALUE_NUMBER(JS_NaN);
    }

    return JS_VALUE_NUMBER((double)(int)val);

}

JSValue value_is_loosely_equal(VM* vm, JSValue left, JSValue right)
{
    if (left.type == right.type)
    {
        return value_is_strictly_equal(left, right);
    }

    if ((left.type == JS_UNDEFINED && right.type == JS_NULL) ||
        (left.type == JS_NULL && right.type == JS_UNDEFINED))
    {
        return JS_VALUE_BOOL(1);
    }

    if (left.type == JS_NUMBER && right.type == JS_STRING)
    {
        return value_is_loosely_equal(vm, left, value_to_number(vm, right));
    }
    if (left.type == JS_STRING && right.type == JS_NUMBER)
    {
        return value_is_loosely_equal(vm, value_to_number(vm, left), right);
    }

    // TODO Implement step 7. If x is a BigInt and y is a String
    // TODO Implement step 8. If x is a String and y is a BigInt

    if (left.type == JS_BOOLEAN)
    {
        return value_is_loosely_equal(vm, value_to_number(vm, left), right);
    }
    if (right.type == JS_BOOLEAN)
    {
        return value_is_loosely_equal(vm, left, value_to_number(vm, right));
    }

    // TODO add left == BigInt?
    if (right.type == JS_OBJECT && (left.type == JS_STRING || left.type == JS_NUMBER || left.type == JS_SYMBOL))
    {
        return value_is_loosely_equal(vm, left, value_to_primitive(vm, right, JS_UNDEFINED));
    }
    // TODO add right == BigInt?
    if (left.type == JS_OBJECT && (right.type == JS_STRING || right.type == JS_NUMBER || right.type == JS_SYMBOL))
    {
        return value_is_loosely_equal(vm, value_to_primitive(vm, left, JS_UNDEFINED), right);
    }

    // TODO implement step 13. If x is a BigInt and y is a Number, or if x is a Number and y is a BigInt

    return JS_VALUE_BOOL(0);
}

JSValue value_is_strictly_equal(JSValue left, JSValue right)
{
    if (left.type != right.type)
    {
        return JS_VALUE_BOOL(0);
    }

    if (left.type == JS_NUMBER)
    {
        return number_equal(left, right);
    }

    return value_same_value_non_number(left, right);
}

JSValue value_same_value_non_number(JSValue left, JSValue right)
{
    if (left.type == JS_UNDEFINED || right.type == JS_UNDEFINED)
    {
        return JS_VALUE_BOOL(1);
    }
    // TODO test for BigInt
    if (left.type == JS_STRING)
    {
        JSString* left_string = left.value.as_pointer;
        JSString* right_string = right.value.as_pointer;

        return JS_VALUE_BOOL(
            left_string->length == right_string->length &&
            strcmp(left_string->buff, right_string->buff) == 0
        );
    }
    if (left.type == JS_BOOLEAN)
    {
        return JS_VALUE_BOOL(left.value.as_boolean == right.value.as_boolean);
    }

    return JS_VALUE_BOOL(left.value.as_pointer == right.value.as_pointer);
}

JSValue value_is_less_than(VM* vm, JSValue left, JSValue right, int left_first)
{
    JSValue p_left;
    JSValue p_right;

    if (left_first)
    {
        p_left = value_to_primitive(vm, left, JS_NUMBER);
        p_right = value_to_primitive(vm, right, JS_NUMBER);
    }
    else
    {
        p_right = value_to_primitive(vm, right, JS_NUMBER);
        p_left = value_to_primitive(vm, left, JS_NUMBER);
    }

    if (p_right.type == JS_STRING && p_left.type == JS_STRING)
    {
        return JS_VALUE_BOOL(strcmp(((JSString*)p_left.value.as_pointer)->buff, ((JSString*)p_right.value.as_pointer)->buff) < 0);
    }

    // TODO respect BigInt
    p_left = value_to_numeric(vm, p_left);
    p_right = value_to_numeric(vm, p_right);
    
    if (p_left.type == p_right.type)
    {
        if (p_left.type == JS_NUMBER)
        {
            return number_less_than(p_left, p_right);
        }
        
        // TODO respect BigInts
        PANIC("BigInt are not implemented");
    }
    
    if (value_is_NaN(p_left) || value_is_NaN(p_right))
    {
        return JS_VALUE_UNDEFINED;
    }
    
    // TODO respect BigInts
    if (p_left.value.as_number == JS_NEG_INFINITY || p_right.value.as_number == JS_POS_INFINITY)
    {
        return JS_VALUE_BOOL(1);
    }
    if(p_left.value.as_number == JS_POS_INFINITY || p_right.value.as_number == JS_NEG_INFINITY)
    {
        return JS_VALUE_BOOL(0);
    }

    return JS_VALUE_BOOL(p_left.value.as_number < p_right.value.as_number);
}