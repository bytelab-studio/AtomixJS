#include "execution.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "panic.h"
#include "api.h"

static void inst_nop(VM* vm, void* ptr)
{
}

static void inst_ld_int(VM* vm, void* ptr)
{
    InstInt32* inst = ptr;
    if (vm->stats.stack_counter >= STACK_SIZE)
    {
        PANIC("Stack overflow");
    }
    vm->stats.stack[vm->stats.stack_counter++] = ((JSValue){
        .type = JS_INTEGER,
        .value.as_int = inst->operand
    });
}

static void inst_ld_double(VM* vm, void* ptr)
{
    InstDouble* inst = ptr;
    if (vm->stats.stack_counter >= STACK_SIZE)
    {
        PANIC("Stack overflow");
    }
    vm->stats.stack[vm->stats.stack_counter++] = ((JSValue){
        .type = JS_DOUBLE,
        .value.as_double = inst->operand
    });
}

static void inst_ld_string(VM* vm, void* ptr)
{
    InstUInt16* inst = ptr;
    if (vm->stats.stack_counter >= STACK_SIZE)
    {
        PANIC("Stack overflow");
    }

    char* str = string_table_load_str(&vm->module->string_table, inst->operand);
    vm->stats.stack[vm->stats.stack_counter++] = JS_VALUE_STRING(str);
}

static void inst_ld_undf(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter >= STACK_SIZE)
    {
        PANIC("Stack overflow");
    }
    vm->stats.stack[vm->stats.stack_counter++] = JS_VALUE_UNDEFINED;
}

static void inst_ld_null(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter >= STACK_SIZE)
    {
        PANIC("Stack overflow");
    }
    vm->stats.stack[vm->stats.stack_counter++] = JS_VALUE_NULL;
}

static void inst_ld_boolean(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter >= STACK_SIZE)
    {
        PANIC("Stack overflow");
    }
    vm->stats.stack[vm->stats.stack_counter++] = JS_VALUE_BOOL(OPCODE_OF(ptr) == OP_LD_TRUE);
}

static void inst_ld_this(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter >= STACK_SIZE)
    {
        PANIC("Stack overflow");
    }
    vm->stats.stack[vm->stats.stack_counter++] = scope_get(vm->scope, "this");
}

static void inst_add(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    // undefined + anything => NaN
    if (left.type == JS_UNDEFINED || right.type == JS_UNDEFINED)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_DOUBLE(JS_NaN);
        return;
    }

    // type boolean || type null => type number
    if (left.type == JS_BOOLEAN || left.type == JS_NULL)
    {
        left.type = JS_INTEGER;
    }
    if (right.type == JS_BOOLEAN || right.type == JS_NULL)
    {
        right.type = JS_INTEGER;
    }

    vm->stats.stack_counter--;


    if (left.type == JS_INTEGER && right.type == JS_INTEGER)
    {
        vm->stats.stack[vm->stats.stack_counter - 1].type = JS_INTEGER;
        vm->stats.stack[vm->stats.stack_counter - 1].value.as_int = left.value.as_int + right.value.as_int;
    }
    else if (left.type == JS_DOUBLE || right.type == JS_DOUBLE)
    {
        double l = left.type == JS_DOUBLE ? left.value.as_double : (double)left.value.as_int;
        double r = right.type == JS_DOUBLE ? right.value.as_double : (double)right.value.as_int;

        vm->stats.stack[vm->stats.stack_counter - 1].value.as_double = l + r;
        vm->stats.stack[vm->stats.stack_counter - 1].type = JS_DOUBLE;
    }
    else
    {
        // TODO think about strings
        PANIC("Unsupported operation");
    }
}

static void inst_minus(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    // undefined or object or function - anything => NaN
    if (left.type == JS_UNDEFINED || left.type == JS_OBJECT || left.type == JS_FUNC ||
        right.type == JS_UNDEFINED || right.type == JS_OBJECT || right.type == JS_FUNC)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_DOUBLE(JS_NaN);
        return;
    }

    // type boolean || type null => type number
    if (left.type == JS_BOOLEAN || left.type == JS_NULL)
    {
        left.type = JS_INTEGER;
    }
    if (right.type == JS_BOOLEAN || right.type == JS_NULL)
    {
        right.type = JS_INTEGER;
    }

    vm->stats.stack_counter--;

    if (left.type == JS_INTEGER && right.type == JS_INTEGER)
    {
        vm->stats.stack[vm->stats.stack_counter - 1].type = JS_INTEGER;
        vm->stats.stack[vm->stats.stack_counter - 1].value.as_int = left.value.as_int - right.value.as_int;
    }
    else if (left.type == JS_DOUBLE || right.type == JS_DOUBLE)
    {
        double l = left.type == JS_DOUBLE ? left.value.as_double : (double)left.value.as_int;
        double r = right.type == JS_DOUBLE ? right.value.as_double : (double)right.value.as_int;

        vm->stats.stack[vm->stats.stack_counter - 1].value.as_double = l - r;
        vm->stats.stack[vm->stats.stack_counter - 1].type = JS_DOUBLE;
    }
    else
    {
        PANIC("Unsupported operation");
    }
}

static void inst_mul(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    // undefined or object or function * anything => NaN
    if (left.type == JS_UNDEFINED || left.type == JS_OBJECT || left.type == JS_FUNC ||
        right.type == JS_UNDEFINED || right.type == JS_OBJECT || right.type == JS_FUNC)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_DOUBLE(JS_NaN);
        return;
    }

    // type boolean || type null => type number
    if (left.type == JS_BOOLEAN || left.type == JS_NULL)
    {
        left.type = JS_INTEGER;
    }
    if (right.type == JS_BOOLEAN || right.type == JS_NULL)
    {
        right.type = JS_INTEGER;
    }

    vm->stats.stack_counter--;

    if (left.type == JS_INTEGER && right.type == JS_INTEGER)
    {
        vm->stats.stack[vm->stats.stack_counter - 1].type = JS_INTEGER;
        vm->stats.stack[vm->stats.stack_counter - 1].value.as_int = left.value.as_int * right.value.as_int;
    }
    else if (left.type == JS_DOUBLE || right.type == JS_DOUBLE)
    {
        double l = left.type == JS_DOUBLE ? left.value.as_double : (double)left.value.as_int;
        double r = right.type == JS_DOUBLE ? right.value.as_double : (double)right.value.as_int;

        r = l * r;
        vm->stats.stack[vm->stats.stack_counter - 1] = r == (int)r
                                                           ? JS_VALUE_INT((int)r)
                                                           : JS_VALUE_DOUBLE(r);
    }
    else
    {
        PANIC("Unsupported operation");
    }
}

static void inst_div(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    // undefined or object or function / anything => NaN
    if (left.type == JS_UNDEFINED || left.type == JS_OBJECT || left.type == JS_FUNC ||
        right.type == JS_UNDEFINED || right.type == JS_OBJECT || right.type == JS_FUNC || right.type == JS_NULL)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_DOUBLE(JS_NaN);
        return;
    }

    // type boolean || type null => type number
    if (left.type == JS_NULL)
    {
        left = JS_VALUE_DOUBLE(0);
    }
    else if (left.type == JS_INTEGER || left.type == JS_BOOLEAN)
    {
        left = JS_VALUE_DOUBLE(left.value.as_int);
    }
    if (right.type == JS_INTEGER || right.type == JS_BOOLEAN)
    {
        right = JS_VALUE_DOUBLE(right.value.as_int);
    }


    vm->stats.stack_counter--;

    if (left.type == JS_DOUBLE || right.type == JS_DOUBLE)
    {
        double l = left.type == JS_DOUBLE ? left.value.as_double : (double)left.value.as_int;
        double r = right.type == JS_DOUBLE ? right.value.as_double : (double)right.value.as_int;

        if (r == 0.0 && l == 0.0)
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_DOUBLE(JS_NaN);
            return;
        }
        else if (r == 0.0)
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_DOUBLE(l > 0 ? JS_POS_INFINITY : JS_NEG_INFINITY);
            return;
        }


        r = l / r;
        vm->stats.stack[vm->stats.stack_counter - 1] = r == (int)r
                                                           ? JS_VALUE_INT((int)r)
                                                           : JS_VALUE_DOUBLE(r);
    }
    else
    {
        PANIC("Unsupported operation");
    }
}

static void inst_mod(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    // undefined or object or function % anything => NaN
    if (left.type == JS_UNDEFINED || left.type == JS_OBJECT || left.type == JS_FUNC ||
        right.type == JS_UNDEFINED || right.type == JS_OBJECT || right.type == JS_FUNC || right.type == JS_NULL)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_DOUBLE(JS_NaN);
        return;
    }

    // type boolean || type null => type number
    if (left.type == JS_BOOLEAN || left.type == JS_NULL)
    {
        left.type = JS_INTEGER;
    }
    if (right.type == JS_BOOLEAN)
    {
        right.type = JS_INTEGER;
    }

    vm->stats.stack_counter--;

    if (left.type == JS_INTEGER && right.type == JS_INTEGER)
    {
        if (right.value.as_int == 0)
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_DOUBLE(JS_NaN);
            return;
        }

        vm->stats.stack[vm->stats.stack_counter - 1].type = JS_INTEGER;
        vm->stats.stack[vm->stats.stack_counter - 1].value.as_int = left.value.as_int % right.value.as_int;
    }
    else if (left.type == JS_DOUBLE || right.type == JS_DOUBLE)
    {
        double l = left.type == JS_DOUBLE ? left.value.as_double : (double)left.value.as_int;
        double r = right.type == JS_DOUBLE ? right.value.as_double : (double)right.value.as_int;

        if (r == 0.0)
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_DOUBLE(JS_NaN);
            return;
        }

        r = fmod(l, r);
        vm->stats.stack[vm->stats.stack_counter - 1] = r == (int)r
                                                           ? JS_VALUE_INT((int)r)
                                                           : JS_VALUE_DOUBLE(r);
    }
    else
    {
        PANIC("Unsupported operation");
    }
}

static void inst_binary_and(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    if ((left.type != JS_INTEGER && left.type != JS_DOUBLE) ||
        (right.type != JS_INTEGER && right.type != JS_DOUBLE))
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(0);
        return;
    }

    int leftValue = left.type == JS_DOUBLE
                        ? (int)left.value.as_double
                        : left.value.as_int;

    int rightValue = right.type == JS_DOUBLE
                         ? (int)right.value.as_double
                         : right.value.as_int;
    vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(leftValue & rightValue);
}

static void inst_binary_or(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    if ((left.type != JS_INTEGER && left.type != JS_DOUBLE) ||
        (right.type != JS_INTEGER && right.type != JS_DOUBLE))
    {
        if (left.type == JS_INTEGER)
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = left;
        }
        else if (left.type == JS_DOUBLE)
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT((int)left.value.as_double);
        }
        else if (right.type == JS_INTEGER)
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = right;
        }
        else if (right.type == JS_DOUBLE)
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT((int)right.value.as_double);
        }
        else
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(0);
        }
        return;
    }

    int leftValue = left.type == JS_DOUBLE
                        ? (int)left.value.as_double
                        : left.value.as_int;

    int rightValue = right.type == JS_DOUBLE
                         ? (int)right.value.as_double
                         : right.value.as_int;

    vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(leftValue | rightValue);
}

static void inst_binary_xor(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    if ((left.type != JS_INTEGER && left.type != JS_DOUBLE) ||
        (right.type != JS_INTEGER && right.type != JS_DOUBLE))
    {
        if (left.type == JS_INTEGER)
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = left;
        }
        else if (left.type == JS_DOUBLE)
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT((int)left.value.as_double);
        }
        else if (right.type == JS_INTEGER)
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = right;
        }
        else if (right.type == JS_DOUBLE)
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT((int)right.value.as_double);
        }
        else
        {
            vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(0);
        }
        return;
    }

    int leftValue = left.type == JS_DOUBLE
                        ? (int)left.value.as_double
                        : left.value.as_int;

    int rightValue = right.type == JS_DOUBLE
                         ? (int)right.value.as_double
                         : right.value.as_int;

    vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(leftValue ^ rightValue);
}

static void inst_binary_lshft(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    if (left.type != JS_INTEGER && left.type != JS_DOUBLE)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(0);
        return;
    }
    if (right.type != JS_INTEGER && right.type != JS_DOUBLE)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = left.type == JS_DOUBLE
                                                           ? JS_VALUE_DOUBLE((int)left.value.as_double)
                                                           : left;
        return;
    }

    int leftValue = left.type == JS_DOUBLE
                        ? (int)left.value.as_double
                        : left.value.as_int;

    int rightValue = right.type == JS_DOUBLE
                         ? (int)right.value.as_double
                         : right.value.as_int;

    vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(leftValue << rightValue);
}

static void inst_binary_rshft(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    if (left.type != JS_INTEGER && left.type != JS_DOUBLE)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(0);
        return;
    }
    if (right.type != JS_INTEGER && right.type != JS_DOUBLE)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = left.type == JS_DOUBLE
                                                           ? JS_VALUE_DOUBLE((int)left.value.as_double)
                                                           : left;
        return;
    }

    int leftValue = left.type == JS_DOUBLE
                        ? (int)left.value.as_double
                        : left.value.as_int;

    int rightValue = right.type == JS_DOUBLE
                         ? (int)right.value.as_double
                         : right.value.as_int;

    vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(leftValue >> rightValue);
}

static void inst_binary_zrshft(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    if (left.type != JS_INTEGER && left.type != JS_DOUBLE)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(0);
        return;
    }
    if (right.type != JS_INTEGER && right.type != JS_DOUBLE)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = left.type == JS_DOUBLE
                                                           ? JS_VALUE_DOUBLE((int)left.value.as_double)
                                                           : left;
        return;
    }

    int leftValue = left.type == JS_DOUBLE
                        ? (int)left.value.as_double
                        : left.value.as_int;

    int rightValue = right.type == JS_DOUBLE
                         ? (int)right.value.as_double
                         : right.value.as_int;

    vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(
        (int)((unsigned int)leftValue >> (unsigned int)rightValue));
}

static void inst_binary_not(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 1)
    {
        PANIC("Stack underflow");
    }
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    if (right.type == JS_UNDEFINED || right.type == JS_OBJECT || right.type == JS_FUNC)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(-1);
        return;
    }

    if (right.type == JS_BOOLEAN || right.type == JS_NULL)
    {
        right.type = JS_INTEGER;
    }
    else if (right.type == JS_DOUBLE)
    {
        right = JS_VALUE_INT((int)right.value.as_double);
    }

    vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(~right.value.as_int);
}

static void inst_not(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 1)
    {
        PANIC("Stack underflow");
    }
    vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(
        value_is_falsy(&vm->stats.stack[vm->stats.stack_counter - 1]) ? 1 : 0
    );
}

static void inst_negate(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 1)
    {
        PANIC("Stack underflow");
    }

    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    if (right.type == JS_BOOLEAN || right.type == JS_NULL)
    {
        right.type = JS_INTEGER;
    }
    else if (right.type == JS_UNDEFINED || right.type == JS_OBJECT || right.type == JS_FUNC)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_DOUBLE(JS_NaN);
        return;
    }

    if (right.type == JS_INTEGER)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_INT(-right.value.as_int);
        return;
    }
    if (right.type == JS_DOUBLE)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_DOUBLE(-right.value.as_double);
        return;
    }
    PANIC("Unknown operand type");
}

static void inst_typeof(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 1)
    {
        PANIC("Stack underflow");
    }

    switch (vm->stats.stack[vm->stats.stack_counter - 1].type)
    {
    case JS_INTEGER:
    case JS_DOUBLE:
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_STRING(init_string("number"));
        return;
    case JS_STRING:
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_STRING(init_string("string"));
        return;
    case JS_OBJECT:
    case JS_NULL:
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_STRING(init_string("object"));
        return;
    case JS_FUNC:
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_STRING(init_string("function"));
        return;
    case JS_UNDEFINED:
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_STRING(init_string("undefined"));
        return;
    case JS_BOOLEAN:
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_STRING(init_string("boolean"));
        return;
    case JS_SYMBOL:
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_SYMBOL(init_string("symbol"));
    }
    PANIC("Unknown operand type");
}

static void inst_teq(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    if (left.type != right.type &&
        !((left.type == JS_DOUBLE && right.type == JS_INTEGER) ||
            (left.type == JS_INTEGER && right.type == JS_DOUBLE))
    )
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(0);
        return;
    }

    if (left.type == JS_DOUBLE && right.type == JS_INTEGER)
    {
        double rightValue = (double)right.value.as_int;
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_double == rightValue);
        return;
    }
    if (left.type == JS_INTEGER && right.type == JS_DOUBLE)
    {
        double leftValue = (double)left.value.as_int;
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(leftValue == right.value.as_double);
        return;
    }
    if (left.type == JS_INTEGER)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_int == right.value.as_int);
        return;
    }
    if (left.type == JS_DOUBLE)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_double == right.value.as_double);
        return;
    }

    if (left.type == JS_NULL || left.type == JS_UNDEFINED)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(1);
        return;
    }

    if (left.type == JS_FUNC || left.type == JS_OBJECT)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_pointer == right.value.as_pointer);
        return;
    }

    PANIC("Unknown comparison");
}

static void inst_nteq(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    if (left.type != right.type &&
        !((left.type == JS_DOUBLE && right.type == JS_INTEGER) ||
            (left.type == JS_INTEGER && right.type == JS_DOUBLE))
    )
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(1);
        return;
    }

    if (left.type == JS_DOUBLE && right.type == JS_INTEGER)
    {
        double rightValue = (double)right.value.as_int;
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_double != rightValue);
        return;
    }
    if (left.type == JS_INTEGER && right.type == JS_DOUBLE)
    {
        double leftValue = (double)left.value.as_int;
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(leftValue != right.value.as_double);
        return;
    }
    if (left.type == JS_INTEGER)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_int != right.value.as_int);
        return;
    }
    if (left.type == JS_DOUBLE)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_double != right.value.as_double);
        return;
    }

    if (left.type == JS_NULL || left.type == JS_UNDEFINED)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(0);
        return;
    }

    if (left.type == JS_FUNC || left.type == JS_OBJECT)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_pointer != right.value.as_pointer);
        return;
    }

    PANIC("Unknown comparison");
}

static void inst_gt(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    if (left.type == JS_BOOLEAN)
    {
        left.type = JS_INTEGER;
    }
    if (right.type == JS_BOOLEAN)
    {
        right.type = JS_INTEGER;
    }

    if (left.type == JS_UNDEFINED || right.type == JS_UNDEFINED)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(0);
        return;
    }
    if (left.type == JS_NULL)
    {
        left = JS_VALUE_INT(0);
    }
    if (right.type == JS_NULL)
    {
        right = JS_VALUE_INT(0);
    }

    if (left.type == JS_INTEGER && right.type == JS_INTEGER)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_int > right.value.as_int);
        return;
    }
    if (left.type == JS_DOUBLE && right.type == JS_DOUBLE)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_double > right.value.as_double);
        return;
    }
    if (left.type == JS_DOUBLE && right.type == JS_INTEGER)
    {
        double rightValue = right.value.as_int;
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_double > rightValue);
        return;
    }
    if (left.type == JS_INTEGER && right.type == JS_DOUBLE)
    {
        double leftValue = left.value.as_int;
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(leftValue > right.value.as_double);
        return;
    }
    // TODO string comparison
    PANIC("Unknown comparison");
}

static void inst_geq(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    if (left.type == JS_BOOLEAN)
    {
        left.type = JS_INTEGER;
    }
    if (right.type == JS_BOOLEAN)
    {
        right.type = JS_INTEGER;
    }

    if (left.type == JS_UNDEFINED || right.type == JS_UNDEFINED)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(0);
        return;
    }
    if (left.type == JS_NULL)
    {
        left = JS_VALUE_INT(0);
    }
    if (right.type == JS_NULL)
    {
        right = JS_VALUE_INT(0);
    }

    if (left.type == JS_INTEGER && right.type == JS_INTEGER)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_int >= right.value.as_int);
        return;
    }
    if (left.type == JS_DOUBLE && right.type == JS_DOUBLE)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_double >= right.value.as_double);
        return;
    }
    if (left.type == JS_DOUBLE && right.type == JS_INTEGER)
    {
        double rightValue = right.value.as_int;
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_double >= rightValue);
        return;
    }
    if (left.type == JS_INTEGER && right.type == JS_DOUBLE)
    {
        double leftValue = left.value.as_int;
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(leftValue >= right.value.as_double);
        return;
    }
    // TODO string comparison
    PANIC("Unknown comparison");
}

static void inst_lt(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    if (left.type == JS_BOOLEAN)
    {
        left.type = JS_INTEGER;
    }
    if (right.type == JS_BOOLEAN)
    {
        right.type = JS_INTEGER;
    }

    if (left.type == JS_UNDEFINED || right.type == JS_UNDEFINED)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(0);
        return;
    }
    if (left.type == JS_NULL)
    {
        left = JS_VALUE_INT(0);
    }
    if (right.type == JS_NULL)
    {
        right = JS_VALUE_INT(0);
    }

    if (left.type == JS_INTEGER && right.type == JS_INTEGER)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_int < right.value.as_int);
        return;
    }
    if (left.type == JS_DOUBLE && right.type == JS_DOUBLE)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_double < right.value.as_double);
        return;
    }
    if (left.type == JS_DOUBLE && right.type == JS_INTEGER)
    {
        double rightValue = right.value.as_int;
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_double < rightValue);
        return;
    }
    if (left.type == JS_INTEGER && right.type == JS_DOUBLE)
    {
        double leftValue = left.value.as_int;
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(leftValue < right.value.as_double);
        return;
    }
    // TODO string comparison
    PANIC("Unknown comparison");
}

static void inst_leq(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    if (left.type == JS_BOOLEAN)
    {
        left.type = JS_INTEGER;
    }
    if (right.type == JS_BOOLEAN)
    {
        right.type = JS_INTEGER;
    }

    if (left.type == JS_UNDEFINED || right.type == JS_UNDEFINED)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(0);
        return;
    }
    if (left.type == JS_NULL)
    {
        left = JS_VALUE_INT(0);
    }
    if (right.type == JS_NULL)
    {
        right = JS_VALUE_INT(0);
    }

    if (left.type == JS_INTEGER && right.type == JS_INTEGER)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_int <= right.value.as_int);
        return;
    }
    if (left.type == JS_DOUBLE && right.type == JS_DOUBLE)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_double <= right.value.as_double);
        return;
    }
    if (left.type == JS_DOUBLE && right.type == JS_INTEGER)
    {
        double rightValue = right.value.as_int;
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(left.value.as_double <= rightValue);
        return;
    }
    if (left.type == JS_INTEGER && right.type == JS_DOUBLE)
    {
        double leftValue = left.value.as_int;
        vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(leftValue <= right.value.as_double);
        return;
    }
    // TODO string comparison
    PANIC("Unknown comparison");
}

static void inst_pop(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter == 0)
    {
        PANIC("Stack underflow");
    }

    vm->stats.stack_counter--;
}

static void inst_dup(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter == 0)
    {
        PANIC("Stack underflow");
    }
    if (vm->stats.stack_counter >= STACK_SIZE)
    {
        PANIC("Stack overflow");
    }
    vm->stats.stack_counter++;
    vm->stats.stack[vm->stats.stack_counter - 1] = vm->stats.stack[vm->stats.stack_counter - 2];
}

static void inst_swap(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue tmp = vm->stats.stack[vm->stats.stack_counter - 2];
    vm->stats.stack[vm->stats.stack_counter - 2] = vm->stats.stack[vm->stats.stack_counter - 1];
    vm->stats.stack[vm->stats.stack_counter - 1] = tmp;
}

static void inst_alloc_store_local(VM* vm, void* ptr)
{
    InstUInt16* inst = ptr;
    int is_alloc = inst->opcode == OP_ALLOC_LOCAL;
    if (vm->stats.stack_counter == 0)
    {
        PANIC("Stack underflow");
    }
    JSValue value = vm->stats.stack[vm->stats.stack_counter - 1];
    vm->stats.stack_counter--;
    char* key = string_table_load_str(&vm->module->string_table, inst->operand);
    if (is_alloc)
    {
        scope_declare(vm->scope, key, value);
        return;
    }

    if (!scope_set(vm->scope, key, value))
    {
        PANIC("Could not update local");
    }
}

static void inst_load_local(VM* vm, void* ptr)
{
    InstUInt16* inst = ptr;
    if (vm->stats.stack_counter >= STACK_SIZE)
    {
        PANIC("Stack overflow");
    }
    char* key = string_table_load_str(&vm->module->string_table, inst->operand);
    if (!scope_contains(vm->scope, key, 1))
    {
        PANIC("Symbol not found");
    }
    vm->stats.stack[vm->stats.stack_counter++] = scope_get(vm->scope, key);
}

static void inst_load_arg(VM* vm, void* ptr)
{
    InstUInt16* inst = ptr;
    if (vm->stats.stack_counter - (vm->stats.stack_counter - vm->stats.stack_start) <= inst->operand)
    {
        vm->stats.stack[vm->stats.stack_counter++] = JS_VALUE_UNDEFINED;
        return;
    }

    vm->stats.stack[vm->stats.stack_counter++]
        = vm->stats.stack[vm->stats.stack_counter - (vm->stats.stack_counter - vm->stats.stack_start) - inst->operand - 1];
}

static void inst_func_decl(VM* vm, void* ptr)
{
    int is_function_decl = OPCODE_OF(ptr) == OP_FUNC_DECL;
    uint16_t idx;
    uint16_t size;
    if (is_function_decl)
    {
        Inst2UInt16* inst = ptr;
        idx = inst->operand;
        size = inst->operand2;
    }
    else
    {
        InstUInt16* inst = ptr;
        idx = 0;
        size = inst->operand;
    }

    if (vm->stats.stack_counter >= STACK_SIZE)
    {
        PANIC("Stack overflow");
    }
    JSFunction* function = function_create_function(
        vm->scope,
        vm->module,
        vm->stats.instruction_counter,
        vm->stats.instruction_counter + size);
    JSValue value = (JSValue){
        .type = JS_FUNC,
        .value.as_pointer = function
    };
    if (is_function_decl)
    {
        char* key = string_table_load_str(&vm->module->string_table, idx);
        scope_declare(vm->scope, key, value);
    }
    vm->stats.stack[vm->stats.stack_counter++] = value;
    vm->stats.instruction_counter += size;
}

static void inst_call(VM* vm, void* ptr)
{
    InstUInt16* inst = ptr;
    if (vm->stats.stack_counter == 0)
    {
        PANIC("Stack underflow");
    }
    JSValue value = vm->stats.stack[--vm->stats.stack_counter];
    if (value.type != JS_FUNC)
    {
        PANIC("Callee is not a function");
    }

    JSFunction* function = value.value.as_pointer;
    JSValue return_value;

    if (function->is_native)
    {
        if (!function->native_function)
        {
            PANIC("Function holds not a valid pointer");
        }
        JSValue args[inst->operand + 1];
        for (uint16_t i = 0; i <= inst->operand; i++) {
            args[i] = vm->stats.stack[vm->stats.stack_counter - i - 1];
        }
    
        JSValue this = args[0];
        return_value = function->native_function(vm, this, args + 1, inst->operand);
        vm->stats.stack_counter -= inst->operand + 1;
    }
    else
    {
        JSValue this_value = vm->stats.stack[vm->stats.stack_counter - 1];
        char* this_key = init_string("this");
        scope_declare(function->scope, this_key, this_value);
        return_value = vm_exec_function(vm, function);
        vm->stats.stack_counter -= inst->operand + 1;
    }
    vm->stats.stack[vm->stats.stack_counter++] = return_value;
}

static void inst_arr_alloc(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter >= STACK_SIZE)
    {
        PANIC("Stack overflow");
    }
    JSObject* obj = object_create_object(object_get_array_prototype());
    vm->stats.stack[vm->stats.stack_counter++] = JS_VALUE_OBJECT(obj);
}

static void inst_obj_alloc(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter >= STACK_SIZE)
    {
        PANIC("Stack overflow");
    }
    JSObject* obj = object_create_object(object_get_object_prototype());
    vm->stats.stack[vm->stats.stack_counter++] = JS_VALUE_OBJECT(obj);
}

static void inst_obj_store(VM* vm, void* ptr)
{
    InstUInt16* inst = ptr;
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue value = vm->stats.stack[--vm->stats.stack_counter];
    JSValue obj = vm->stats.stack[--vm->stats.stack_counter];
    if (obj.type != JS_OBJECT && obj.type != JS_FUNC)
    {
        PANIC("Target is not a object");
    }
    char* key = string_table_load_str(&vm->module->string_table, inst->operand);
    JSObject* obj_ptr = obj.type == JS_FUNC
        ? ((JSFunction*)obj.value.as_pointer)->base
        : (JSObject*)obj.value.as_pointer;
    object_set_property(vm, obj_ptr, key, value);
}

static void inst_obj_load(VM* vm, void* ptr)
{
    InstUInt16* inst = ptr;
    if (vm->stats.stack_counter < 1)
    {
        PANIC("Stack underflow");
    }
    JSValue obj = vm->stats.stack[vm->stats.stack_counter - 1];
    if (obj.type != JS_OBJECT && obj.type != JS_FUNC)
    {
        PANIC("Target is not a object");
    }
    char* key = string_table_load_str(&vm->module->string_table, inst->operand);
    JSObject* obj_ptr = obj.type == JS_FUNC
        ? ((JSFunction*)obj.value.as_pointer)->base
        : (JSObject*)obj.value.as_pointer;
    vm->stats.stack[vm->stats.stack_counter - 1] = object_get_property(vm, obj_ptr, key);
}

static void inst_obj_cload(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue computed = vm->stats.stack[--vm->stats.stack_counter];
    JSValue obj = vm->stats.stack[vm->stats.stack_counter - 1];
    if (obj.type != JS_OBJECT && obj.type != JS_FUNC)
    {
        PANIC("Target is not a object");
    }
    JSObject* obj_ptr = obj.type == JS_FUNC
        ? ((JSFunction*)obj.value.as_pointer)->base
        : obj.value.as_pointer;
    
    if (computed.type == JS_SYMBOL)
    {
        vm->stats.stack[vm->stats.stack_counter - 1] = object_get_property_by_symbol(vm, obj_ptr, computed.value.as_pointer);
        return;
    }

    char* key = value_to_string(&computed);
    vm->stats.stack[vm->stats.stack_counter - 1] = object_get_property(vm, obj_ptr, key);
}

static void inst_obj_cstore(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 3)
    {
        PANIC("Stack underflow");
    }
    JSValue computed = vm->stats.stack[--vm->stats.stack_counter];
    JSValue obj = vm->stats.stack[--vm->stats.stack_counter];
    JSValue value = vm->stats.stack[--vm->stats.stack_counter];
    if (obj.type != JS_OBJECT && obj.type != JS_FUNC)
    {
        PANIC("Target is not a object");
    }
    JSObject* obj_ptr = obj.type == JS_FUNC
        ? ((JSFunction*)obj.value.as_pointer)->base
        : (JSObject*)obj.value.as_pointer;
    if (computed.type == JS_SYMBOL)
    {
        object_set_property_with_symbol(vm, obj_ptr, computed.value.as_pointer, value);
        return;
    }
    char* key = value_to_string(&computed);
    object_set_property(vm, obj_ptr, key, value);
}

static void inst_push_scope(VM* vm, void* ptr)
{
    vm->scope = scope_create_scope(vm->scope);
}

static void inst_pop_scope(VM* vm, void* ptr)
{
    if (!vm->scope->parent)
    {
        PANIC("No parent scope to pop");
    }

    vm->scope = vm->scope->parent;
}

static void inst_jmp(VM* vm, void* ptr)
{
    InstUInt16* inst = ptr;
    vm->stats.instruction_counter = inst->operand;
}

static void inst_jmp_f(VM* vm, void* ptr)
{
    InstUInt16* inst = ptr;
    JSValue test = vm->stats.stack[--vm->stats.stack_counter];
    if (value_is_falsy(&test))
    {
        vm->stats.instruction_counter = inst->operand;
    }
}

static void inst_jmp_t(VM* vm, void* ptr)
{
    InstUInt16* inst = ptr;
    JSValue test = vm->stats.stack[--vm->stats.stack_counter];
    if (value_is_truthy(&test))
    {
        vm->stats.instruction_counter = inst->operand;
    }
}

static void inst_export(VM* vm, void* ptr)
{
    InstUInt16* inst = ptr;
    if (vm->stats.stack_counter < 1)
    {
        PANIC("Stack underflow");
    }

    JSValue value = vm->stats.stack[--vm->stats.stack_counter];
    char* key = string_table_load_str(&vm->module->string_table, inst->operand);
    object_set_property(vm, vm->module->exports, key, value);
}

VM vm_init(JSModule* module)
{
    VM vm;
    vm.module = module;
    vm.stats.instruction_counter = 0;
    vm.stats.stack_counter = 0;
    vm.stats.stack_start = 0;
    vm.globalScope = scope_create_scope(NULL);

    vm.inst_set[OP_NOP] = inst_nop;
    vm.inst_set[OP_LD_INT] = inst_ld_int;
    vm.inst_set[OP_LD_DOUBLE] = inst_ld_double;
    vm.inst_set[OP_LD_STRING] = inst_ld_string;
    vm.inst_set[OP_LD_UNDF] = inst_ld_undf;
    vm.inst_set[OP_LD_NULL] = inst_ld_null;
    vm.inst_set[OP_LD_TRUE] = inst_ld_boolean;
    vm.inst_set[OP_LD_FALSE] = inst_ld_boolean;
    vm.inst_set[OP_LD_THIS] = inst_ld_this;
    vm.inst_set[OP_ADD] = inst_add;
    vm.inst_set[OP_MINUS] = inst_minus;
    vm.inst_set[OP_MUL] = inst_mul;
    vm.inst_set[OP_DIV] = inst_div;
    vm.inst_set[OP_MOD] = inst_mod;
    vm.inst_set[OP_BINARY_AND] = inst_binary_and;
    vm.inst_set[OP_BINARY_OR] = inst_binary_or;
    vm.inst_set[OP_BINARY_XOR] = inst_binary_xor;
    vm.inst_set[OP_BINARY_LSHFT] = inst_binary_lshft;
    vm.inst_set[OP_BINARY_RSHFT] = inst_binary_rshft;
    vm.inst_set[OP_BINARY_ZRSHFT] = inst_binary_zrshft;
    vm.inst_set[OP_BINARY_NOT] = inst_binary_not;
    vm.inst_set[OP_NOT] = inst_not;
    vm.inst_set[OP_NEGATE] = inst_negate;
    vm.inst_set[OP_TYPEOF] = inst_typeof;
    vm.inst_set[OP_TEQ] = inst_teq;
    vm.inst_set[OP_NTEQ] = inst_nteq;
    vm.inst_set[OP_GT] = inst_gt;
    vm.inst_set[OP_GEQ] = inst_geq;
    vm.inst_set[OP_LT] = inst_lt;
    vm.inst_set[OP_LEQ] = inst_leq;
    vm.inst_set[OP_POP] = inst_pop;
    vm.inst_set[OP_DUP] = inst_dup;
    vm.inst_set[OP_SWAP] = inst_swap;
    vm.inst_set[OP_ALLOC_LOCAL] = inst_alloc_store_local;
    vm.inst_set[OP_STORE_LOCAL] = inst_alloc_store_local;
    vm.inst_set[OP_LOAD_LOCAL] = inst_load_local;
    vm.inst_set[OP_LOAD_ARG] = inst_load_arg;
    vm.inst_set[OP_FUNC_DECL] = inst_func_decl;
    vm.inst_set[OP_FUNC_DECL_E] = inst_func_decl;
    vm.inst_set[OP_CALL] = inst_call;
    vm.inst_set[OP_ARR_ALLOC] = inst_arr_alloc;
    vm.inst_set[OP_OBJ_ALLOC] = inst_obj_alloc;
    vm.inst_set[OP_OBJ_STORE] = inst_obj_store;
    vm.inst_set[OP_OBJ_LOAD] = inst_obj_load;
    vm.inst_set[OP_OBJ_CSTORE] = inst_obj_cstore;
    vm.inst_set[OP_OBJ_CLOAD] = inst_obj_cload;
    vm.inst_set[OP_RETURN] = inst_nop;
    vm.inst_set[OP_PUSH_SCOPE] = inst_push_scope;
    vm.inst_set[OP_POP_SCOPE] = inst_pop_scope;
    vm.inst_set[OP_JMP] = inst_jmp;
    vm.inst_set[OP_JMP_F] = inst_jmp_f;
    vm.inst_set[OP_JMP_T] = inst_jmp_t;
    vm.inst_set[OP_EXPORT] = inst_export;

    bind_modules(&vm, vm.globalScope);

    return vm;
}

static void vm_exec(VM* vm)
{
    if (vm->stats.instruction_counter >= vm->module->data_section.count)
    {
        PANIC("Instruction counter is out of bounds of the current module");
    }

    void* instruction = vm->module->data_section.instructions[vm->stats.instruction_counter++];
    if (instruction == NULL || OPCODE_OF(instruction) == OP_NOP)
    {
        return;
    }
    vm->inst_set[OPCODE_OF(instruction)](vm, instruction);
}

void vm_exec_module(VM* vm, JSModule* module)
{
    JSModule* current_module = vm->module;
    VMStats stats = vm->stats;
    Scope* scope = vm->scope;

    module->scope->parent = vm->globalScope;

    vm->scope = module->scope;
    vm->module = module;
    vm->stats.instruction_counter = 0;
    vm->stats.stack_counter = 0;
    vm->stats.stack_start = 0;

    while (vm->stats.instruction_counter < vm->module->data_section.count)
    {
        vm_exec(vm);
    }

    vm->module = current_module;
    vm->stats = stats;
    vm->scope = scope;
}

JSValue vm_exec_function(VM* vm, JSFunction* function)
{
    JSModule* current_module = vm->module;
    VMStats stats = vm->stats;
    Scope* scope = vm->scope;

    vm->module = function->module;
    vm->stats.instruction_counter = function->meta.instruction_start;
    vm->stats.stack_start = vm->stats.stack_counter;
    vm->scope = function->scope;

    while (vm->stats.instruction_counter < function->meta.instruction_end)
    {
        Opcode opcode = OPCODE_OF(vm->module->data_section.instructions[vm->stats.instruction_counter]);
        if (opcode == OP_RETURN)
        {
            break;
        }
        vm_exec(vm);
    }
    JSValue return_value = vm->stats.stack_counter > vm->stats.stack_start
        ? vm->stats.stack[--vm->stats.stack_counter]
        : JS_VALUE_UNDEFINED;

    vm->module = current_module;
    vm->stats = stats;
    vm->scope = scope;

    return return_value;
}
