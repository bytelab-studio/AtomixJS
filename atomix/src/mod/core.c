#include <stdio.h>

#include "../api.h"
#include "../function.h"

JSValue print(JSValue* args, size_t argc)
{
    if (argc == 0)
    {
        printf("\n");
        return JS_VALUE_UNDEFINED;
    }
    for (size_t i = 0; i < argc; i++)
    {
        JSValue value = args[i];
        switch (value.type)
        {
        case JS_INTEGER:
            printf("%i\n", value.value.as_int);
            break;
        case JS_DOUBLE:
            if (value_is_NaN(&value))
            {
                printf("NaN\n");
                break;
            }
            printf("%f\n", value.value.as_double);
            break;
        case JS_STRING:
            printf("%s\n", (char*)value.value.as_pointer);
            break;
        case JS_OBJECT:
            printf("[Object]\n");
            break;
        case JS_FUNC:
            printf("[Function]\n");
            break;
        case JS_UNDEFINED:
            printf("undefined\n");
            break;
        case JS_NULL:
            printf("null\n");
            break;
        case JS_BOOLEAN:
            printf("%s\n", value.value.as_int ? "true" : "false");
            break;
        }
    }

    return JS_VALUE_UNDEFINED;
}

void core_init(Scope* scope)
{
    JSFunction* _print = function_create_native_function(print);
    scope_declare(scope, "print", JS_VALUE_FUNCTION(_print));
}

MODULE_INIT(core_init);
