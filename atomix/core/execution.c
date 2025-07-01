#include "execution.impl.h"

#include <stdint.h>
#include <math.h>

#include "panic.h"
#include "api.h"
#include "number.h"

#include "instruction.impl.h"
#include "vm.impl.h"
#include "format.impl.h"
#include "function.impl.h"
#include "scope.impl.h"

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
        .type = JS_NUMBER,
        .value.as_number = (double)inst->operand
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
        .type = JS_NUMBER,
        .value.as_number = inst->operand
    });
}

static void inst_ld_string(VM* vm, void* ptr)
{
    InstUInt16* inst = ptr;
    if (vm->stats.stack_counter >= STACK_SIZE)
    {
        PANIC("Stack overflow");
    }

    JSValue value = string_table_load_str_value(&vm->module->string_table, inst->operand);
    vm->stats.stack[vm->stats.stack_counter++] = value;
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

/**
 * [ECMAScript® 2026 Language Specification §13.15.13](https://tc39.es/ecma262/#sec-applystringornumericbinaryoperator)
 */
static void inst_add(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_to_primitive(vm, left, JS_UNDEFINED);
    right = value_to_primitive(vm, right, JS_UNDEFINED);

    if (left.type == JS_STRING || right.type == JS_STRING)
    {
        if (left.type != JS_STRING)
        {
            left = value_to_string(vm, left);
        } 
        else if (right.type != JS_STRING)
        {
            right = value_to_string(vm, right);
        }

        vm->stats.stack[vm->stats.stack_counter - 1] = value_concat_string(left, right);
    }

    left = value_to_numeric(vm, left);
    right = value_to_numeric(vm, right);
    if (left.type != right.type)
    {
        // TODO throw TypeError
        return;
    }

    // TODO perform bigint operation
    vm->stats.stack_counter[vm->stats.stack - 1] = number_add(left, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.15.13](https://tc39.es/ecma262/#sec-applystringornumericbinaryoperator)
 */
static void inst_minus(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_to_numeric(vm, left);
    right = value_to_numeric(vm, right);

    // TODO perform bigint operation
    vm->stats.stack_counter[vm->stats.stack - 1] = number_subtract(left, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.15.13](https://tc39.es/ecma262/#sec-applystringornumericbinaryoperator)
 */
static void inst_mul(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_to_numeric(vm, left);
    right = value_to_numeric(vm, right);

    // TODO perform bigint operation
    vm->stats.stack_counter[vm->stats.stack - 1] = number_multiply(left, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.15.13](https://tc39.es/ecma262/#sec-applystringornumericbinaryoperator)
 */
static void inst_div(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_to_numeric(vm, left);
    right = value_to_numeric(vm, right);

    // TODO perform bigint operation
    vm->stats.stack_counter[vm->stats.stack - 1] = number_divide(left, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.15.13](https://tc39.es/ecma262/#sec-applystringornumericbinaryoperator)
 */
static void inst_mod(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_to_numeric(vm, left);
    right = value_to_numeric(vm, right);

    // TODO perform bigint operation
    vm->stats.stack_counter[vm->stats.stack - 1] = number_remainder(left, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.15.13](https://tc39.es/ecma262/#sec-applystringornumericbinaryoperator)
 */
static void inst_binary_and(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_to_numeric(vm, left);
    right = value_to_numeric(vm, right);

    // TODO perform bigint operation
    vm->stats.stack_counter[vm->stats.stack - 1] = number_bitwise_and(vm, left, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.15.13](https://tc39.es/ecma262/#sec-applystringornumericbinaryoperator)
 */
static void inst_binary_or(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_to_numeric(vm, left);
    right = value_to_numeric(vm, right);

    // TODO perform bigint operation
    vm->stats.stack_counter[vm->stats.stack - 1] = number_bitwise_or(vm, left, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.15.13](https://tc39.es/ecma262/#sec-applystringornumericbinaryoperator)
 */
static void inst_binary_xor(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_to_numeric(vm, left);
    right = value_to_numeric(vm, right);

    // TODO perform bigint operation
    vm->stats.stack_counter[vm->stats.stack - 1] = number_bitwise_xor(vm, left, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.15.13](https://tc39.es/ecma262/#sec-applystringornumericbinaryoperator)
 */
static void inst_binary_lshft(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_to_numeric(vm, left);
    right = value_to_numeric(vm, right);

    // TODO perform bigint operation
    vm->stats.stack_counter[vm->stats.stack - 1] = number_left_shift(vm, left, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.15.13](https://tc39.es/ecma262/#sec-applystringornumericbinaryoperator)
 */
static void inst_binary_rshft(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_to_numeric(vm, left);
    right = value_to_numeric(vm, right);

    // TODO perform bigint operation
    vm->stats.stack_counter[vm->stats.stack - 1] = number_signed_right_shift(vm, left, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.15.13](https://tc39.es/ecma262/#sec-applystringornumericbinaryoperator)
 */
static void inst_binary_zrshft(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_to_numeric(vm, left);
    right = value_to_numeric(vm, right);

    // TODO perform bigint operation
    vm->stats.stack_counter[vm->stats.stack - 1] = number_unsigned_right_shift(vm, left, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.5.6.1](https://tc39.es/ecma262/#sec-bitwise-not-operator-runtime-semantics-evaluation)
 */
static void inst_binary_not(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 1)
    {
        PANIC("Stack underflow");
    }

    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    right = value_to_numeric(vm, right);

    // TODO perform bigint operation
    vm->stats.stack_counter[vm->stats.stack - 1] = number_bitwise_not(vm, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.5.7.1](https://tc39.es/ecma262/#sec-logical-not-operator-runtime-semantics-evaluation)
 */
static void inst_not(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 1)
    {
        PANIC("Stack underflow");
    }

    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];
    vm->stats.stack[vm->stats.stack_counter - 1] = JS_VALUE_BOOL(!value_to_boolean(right).value.as_boolean);
}

/**
 * [ECMAScript® 2026 Language Specification §13.5.5.1](https://tc39.es/ecma262/#sec-unary-minus-operator-runtime-semantics-evaluation)
 */
static void inst_negate(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 1)
    {
        PANIC("Stack underflow");
    }

    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    right = value_to_numeric(vm, right);

    // TODO perform bigint operation
    vm->stats.stack_counter[vm->stats.stack - 1] = number_unary_minus(right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.5.3.1](https://tc39.es/ecma262/#sec-typeof-operator-runtime-semantics-evaluation)
 */
static void inst_typeof(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 1)
    {
        PANIC("Stack underflow");
    }

    switch (vm->stats.stack[vm->stats.stack_counter - 1].type)
    {
    case JS_NUMBER:
        vm->stats.stack[vm->stats.stack_counter - 1] = init_string_value("number");
        return;
    case JS_STRING:
        vm->stats.stack[vm->stats.stack_counter - 1] = init_string_value("string");
        return;
    case JS_OBJECT:
    case JS_NULL:
        vm->stats.stack[vm->stats.stack_counter - 1] = init_string_value("object");
        return;
    case JS_FUNC:
        vm->stats.stack[vm->stats.stack_counter - 1] = init_string_value("function");
        return;
    case JS_UNDEFINED:
        vm->stats.stack[vm->stats.stack_counter - 1] = init_string_value("undefined");
        return;
    case JS_BOOLEAN:
        vm->stats.stack[vm->stats.stack_counter - 1] = init_string_value("boolean");
        return;
    case JS_SYMBOL:
        vm->stats.stack[vm->stats.stack_counter - 1] = init_string_value("symbol");
    default:
        PANIC("Unknown operand type");
    }
}

/**
 * [ECMAScript® 2026 Language Specification §13.11.1](https://tc39.es/ecma262/#sec-equality-operators-runtime-semantics-evaluation)
 */
static void inst_eq(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;
    vm->stats.stack_counter[vm->stats.stack - 1] = value_is_loosely_equal(vm, left, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.11.1](https://tc39.es/ecma262/#sec-equality-operators-runtime-semantics-evaluation)
 */
static void inst_neq(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;
    vm->stats.stack_counter[vm->stats.stack - 1] = JS_VALUE_BOOL(!value_is_loosely_equal(vm, left, right).value.as_boolean);
}

/**
 * [ECMAScript® 2026 Language Specification §13.11.1](https://tc39.es/ecma262/#sec-equality-operators-runtime-semantics-evaluation)
 */
static void inst_teq(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;
    vm->stats.stack_counter[vm->stats.stack - 1] = value_is_strictly_equal(left, right);
}

/**
 * [ECMAScript® 2026 Language Specification §13.11.1](https://tc39.es/ecma262/#sec-equality-operators-runtime-semantics-evaluation)
 */
static void inst_nteq(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;
    vm->stats.stack_counter[vm->stats.stack - 1] = JS_VALUE_BOOL(!value_is_strictly_equal(left, right).value.as_boolean);
}

/**
 * [ECMAScript® 2026 Language Specification §13.10.1](https://tc39.es/ecma262/#sec-relational-operators-runtime-semantics-evaluation)
 */
static void inst_gt(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_is_less_than(vm, right, left, 0);
    vm->stats.stack[vm->stats.stack_counter - 1] = left.type == JS_UNDEFINED
        ? JS_VALUE_BOOL(0)
        : left;
}

/**
 * [ECMAScript® 2026 Language Specification §13.10.1](https://tc39.es/ecma262/#sec-relational-operators-runtime-semantics-evaluation)
 */
static void inst_geq(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_is_less_than(vm, left, right, 1);
    vm->stats.stack[vm->stats.stack_counter - 1] = left.type == JS_UNDEFINED || left.value.as_boolean
        ? JS_VALUE_BOOL(0)
        : left;
}

/**
 * [ECMAScript® 2026 Language Specification §13.10.1](https://tc39.es/ecma262/#sec-relational-operators-runtime-semantics-evaluation)
 */
static void inst_lt(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_is_less_than(vm, left, right, 1);
    vm->stats.stack[vm->stats.stack_counter - 1] = left.type == JS_UNDEFINED
        ? JS_VALUE_BOOL(0)
        : left;
}

/**
 * [ECMAScript® 2026 Language Specification §13.10.1](https://tc39.es/ecma262/#sec-relational-operators-runtime-semantics-evaluation)
 */
static void inst_leq(VM* vm, void* ptr)
{
    if (vm->stats.stack_counter < 2)
    {
        PANIC("Stack underflow");
    }
    JSValue left = vm->stats.stack[vm->stats.stack_counter - 2];
    JSValue right = vm->stats.stack[vm->stats.stack_counter - 1];

    vm->stats.stack_counter--;

    left = value_is_less_than(vm, right, left, 0);
    vm->stats.stack[vm->stats.stack_counter - 1] = left.type == JS_UNDEFINED || left.value.as_boolean
        ? JS_VALUE_BOOL(0)
        : left;
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

    JSValue key = value_to_string(vm, computed);
    vm->stats.stack[vm->stats.stack_counter - 1] = object_get_property(vm, obj_ptr, ((JSString*)key.value.as_pointer)->buff);
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
    JSValue key = value_to_string(vm, computed);
    object_set_property(vm, obj_ptr, ((JSString*)key.value.as_pointer)->buff, value);
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
    if (!value_to_boolean(test).value.as_boolean)
    {
        vm->stats.instruction_counter = inst->operand;
    }
}

static void inst_jmp_t(VM* vm, void* ptr)
{
    InstUInt16* inst = ptr;
    JSValue test = vm->stats.stack[--vm->stats.stack_counter];
    if (value_to_boolean(test).value.as_boolean)
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
    vm.inst_set[OP_EQ] = inst_eq;
    vm.inst_set[OP_NEQ] = inst_neq;
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
