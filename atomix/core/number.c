#include "number.impl.h"

#include <math.h>

#include "value.impl.h"

int32_t number_to_int32(VM* vm, JSValue value)
{
    value = value_to_number(vm, value);
    if (value_is_NaN(value) || 
        value.value.as_number == JS_POS_INFINITY || 
        value.value.as_number == JS_NEG_INFINITY ||
        value.value.as_number == 0.0 ||
        value.value.as_number == -0.0)
    {
        return 0;
    }

    double int_parts = trunc(value.value.as_number);
    uint32_t int32_bits = ((uint32_t)((int64_t)int_parts % ((int64_t)1 << 32)));

    if (int32_bits >= (1U << 31))
    {
        return (int32_t)(int32_bits - (1U << 32));
    }

    return (int32_t)int32_bits;
}

uint32_t number_to_uint32(VM* vm, JSValue value)
{
    value = value_to_number(vm, value);
    if (value_is_NaN(value) || 
        value.value.as_number == JS_POS_INFINITY || 
        value.value.as_number == JS_NEG_INFINITY ||
        value.value.as_number == 0.0 ||
        value.value.as_number == -0.0)
    {
        return 0;
    }

    double int_parts = trunc(value.value.as_number);
    return (uint32_t)((int64_t)int_parts % ((int64_t)1 << 32));
}

JSValue number_unary_minus(JSValue right)
{
    if (value_is_NaN(right))
    {
        return right;
    }

    return JS_VALUE_NUMBER(-right.value.as_number);
}

JSValue number_bitwise_not(VM* vm, JSValue right)
{
    int32_t value = number_to_int32(vm, right);

    return JS_VALUE_NUMBER((double)(~value));
}

JSValue number_multiply(JSValue left, JSValue right)
{
    if (value_is_NaN(left) || value_is_NaN(right))
    {
        return JS_VALUE_NUMBER(JS_NaN);
    }

    if (left.value.as_number == JS_POS_INFINITY || left.value.as_number == JS_NEG_INFINITY)
    {
        if (right.value.as_number == JS_POS_INFINITY || right.value.as_number == JS_NEG_INFINITY)
        {
            return JS_VALUE_NUMBER(JS_NaN);    
        }

        if (right.value.as_number > 0.0)
        {
            return right;
        }
        
        return JS_VALUE_NUMBER(-right.value.as_number);
    }

    if (right.value.as_number == JS_POS_INFINITY || right.value.as_number == JS_NEG_INFINITY)
    {
        if (left.value.as_number == JS_POS_INFINITY || left.value.as_number == JS_NEG_INFINITY)
        {
            return JS_VALUE_NUMBER(JS_NaN);    
        }

        if (left.value.as_number > 0.0)
        {
            return left;
        }
        
        return JS_VALUE_NUMBER(-left.value.as_number);
    }

    if (left.value.as_number == -0.0)
    {
        return JS_VALUE_NUMBER(
            right.value.as_number <= -0.0
                ? 0.0
                : -0.0
        );
    }

    if (right.value.as_number == -0.0)
    {
        return JS_VALUE_NUMBER(
            left.value.as_number <= -0.0
                ? 0.0
                : -0.0
        );
    }

    return JS_VALUE_NUMBER(left.value.as_number * right.value.as_number);
}

JSValue number_divide(JSValue left, JSValue right)
{
    if (value_is_NaN(left) || value_is_NaN(right))
    {
        return JS_VALUE_NUMBER(JS_NaN);
    }

    if (left.value.as_number == JS_POS_INFINITY || left.value.as_number == JS_NEG_INFINITY)
    {
        if (right.value.as_number == JS_POS_INFINITY || right.value.as_number == JS_NEG_INFINITY)
        {
            return JS_VALUE_NUMBER(JS_NaN);
        }

        if (right.value.as_number >= 0.0)
        {
            return left;
        }

        return JS_VALUE_NUMBER(-left.value.as_number);
    }

    if (right.value.as_number == 0.0)
    {
        return JS_VALUE_NUMBER(
            left.value.as_number >= 0.0
                ? JS_POS_INFINITY
                : JS_NEG_INFINITY
        );
    }

    if (right.value.as_number == -0.0)
    {
        return JS_VALUE_NUMBER(
            left.value.as_number >= 0.0
                ? JS_NEG_INFINITY
                : JS_POS_INFINITY
        );
    }

    return JS_VALUE_NUMBER(left.value.as_number / right.value.as_number);
}

JSValue number_remainder(JSValue left, JSValue right)
{
    if (value_is_NaN(left) || value_is_NaN(right) ||
        left.value.as_number == JS_POS_INFINITY || left.value.as_number == JS_NEG_INFINITY)
    {
        return JS_VALUE_NUMBER(JS_NaN);
    }

    if (right.value.as_number == JS_POS_INFINITY || right.value.as_number == JS_NEG_INFINITY)
    {
        return left;
    }

    if (right.value.as_number == 0.0 || right.value.as_number == -0.0)
    {
        return JS_VALUE_NUMBER(JS_NaN);
    }

    if (left.value.as_number == 0.0 || left.value.as_number == -0.0)
    {
        return left;
    }

    double quotient = left.value.as_number / right.value.as_number;
    double q = trunc(quotient);
    double r = left.value.as_number - (right.value.as_number * q);

    if (r == 0.0 && left.value.as_number < -0.0)
    {
        return JS_VALUE_NUMBER(-0.0);
    }

    return JS_VALUE_NUMBER(r);
}

JSValue number_add(JSValue left, JSValue right)
{
    if (value_is_NaN(left) || value_is_NaN(right) ||
        (left.value.as_number == JS_POS_INFINITY && right.value.as_number == JS_NEG_INFINITY) ||
        (left.value.as_number == JS_NEG_INFINITY && right.value.as_number == JS_POS_INFINITY))
    {
        return JS_VALUE_NUMBER(JS_NaN); 
    }

    if (left.value.as_number == JS_POS_INFINITY || left.value.as_number == JS_NEG_INFINITY)
    {
        return left;
    }
    
    if (right.value.as_number == JS_POS_INFINITY || right.value.as_number == JS_NEG_INFINITY ||
        (left.value.as_number == -0.0 && right.value.as_number == -0.0))
    {
        return right;
    }

    return JS_VALUE_NUMBER(left.value.as_number + right.value.as_number);
}

JSValue number_subtract(JSValue left, JSValue right)
{
    return number_add(left, number_unary_minus(right));
}

JSValue number_left_shift(VM* vm, JSValue left, JSValue right)
{
    int32_t left_value = number_to_int32(vm, left);
    uint32_t right_value = number_to_uint32(vm, right);

    return JS_VALUE_NUMBER(left_value << (right_value % 32));
}

JSValue number_signed_right_shift(VM* vm, JSValue left, JSValue right)
{
    int32_t left_value = number_to_int32(vm, left);
    uint32_t right_value = number_to_uint32(vm, right);

    return JS_VALUE_NUMBER(left_value >> (right_value % 32));
}

JSValue number_unsigned_right_shift(VM* vm, JSValue left, JSValue right)
{
    uint32_t left_value = number_to_uint32(vm, left);
    uint32_t right_value = number_to_uint32(vm, right);

    return JS_VALUE_NUMBER(left_value >> (right_value % 32));   
}

JSValue number_less_than(JSValue left, JSValue right)
{
    if (value_is_NaN(left) || value_is_NaN(right))
    {
        return JS_VALUE_UNDEFINED;
    }

    if (left.value.as_number == right.value.as_number ||
        (left.value.as_number == 0.0 && right.value.as_number == -0.0) ||
        (left.value.as_number == -0.0 && right.value.as_number == 0.0))
    {
        return JS_VALUE_BOOL(0);
    }

    if (left.value.as_number == JS_POS_INFINITY)
    {
        return JS_VALUE_BOOL(0);
    }
    if (right.value.as_number == JS_POS_INFINITY)
    {
        return JS_VALUE_BOOL(1);
    }
    if (right.value.as_number == JS_NEG_INFINITY)
    {
        return JS_VALUE_BOOL(0);
    }
    if (left.value.as_number == JS_NEG_INFINITY)
    {
        return JS_VALUE_BOOL(1);
    }

    return JS_VALUE_BOOL(left.value.as_number < right.value.as_number);
}

JSValue number_equal(JSValue left, JSValue right)
{
    if (value_is_NaN(left) || value_is_NaN(right))
    {
        return JS_VALUE_BOOL(0);
    }

    return JS_VALUE_BOOL(
        left.value.as_number == right.value.as_number ||
        left.value.as_number == 0.0 && right.value.as_number == -0.0 ||
        left.value.as_number == -0.0 && right.value.as_number == 0.0
    );
}

JSValue number_bitwise_and(VM* vm, JSValue left, JSValue right)
{
    int32_t left_value = number_to_int32(vm, left);
    int32_t right_value = number_to_int32(vm, right);

    return JS_VALUE_NUMBER(left_value & right_value);
}

JSValue number_bitwise_xor(VM* vm, JSValue left, JSValue right)
{
    int32_t left_value = number_to_int32(vm, left);
    int32_t right_value = number_to_int32(vm, right);

    return JS_VALUE_NUMBER(left_value ^ right_value);
}

JSValue number_bitwise_or(VM* vm, JSValue left, JSValue right)
{
    int32_t left_value = number_to_int32(vm, left);
    int32_t right_value = number_to_int32(vm, right);

    return JS_VALUE_NUMBER(left_value | right_value);
}