#include "symbol.impl.h"

#include "object.h"
#include "api.h"

#include "value.impl.h"

static JSObject* createSymbol(VM* vm, char* description)
{
    JSObject* symbol = object_create_object(object_get_symbol_prototype());
    object_set_property(vm, symbol, init_string((char*)"description"), init_string_value(description));
    return symbol;
}

static JSObject* _symbol_to_primitive = NULL;

JSValue symbol_to_primitive(VM* vm)
{
    if (!_symbol_to_primitive)
    {
        _symbol_to_primitive = createSymbol(vm, (char*)"Symbol.toPrimitive");
    }

    return JS_VALUE_SYMBOL(_symbol_to_primitive);
}

static JSObject* _symbol_to_string_tag = NULL;

JSValue symbol_to_string_tag(VM* vm)
{
    if (!_symbol_to_string_tag)
    {
        _symbol_to_string_tag = createSymbol(vm, (char*)"Symbol.toStringTag");
    }

    return JS_VALUE_SYMBOL(_symbol_to_string_tag);
}