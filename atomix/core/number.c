#include "number.impl.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "api.h"

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

    if (right.value.as_number == JS_POS_INFINITY)
    {
        return JS_VALUE_NUMBER(
            left.value.as_number >= 0.0
                ? JS_POS_INFINITY
                : JS_NEG_INFINITY
        );
    }

    if (right.value.as_number == JS_NEG_INFINITY)
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

static void find_n_k_s(double x, int radix, int* n, int* k, int* s)
{
    double logx = log(x) / log((double)radix);
    *n = (int)floor(logx) + 1;

    int max_digits = 20;
    for (int digits = 1; digits <= max_digits; digits++)
    {
        double power = pow((double)radix, digits - (*n));
        double scaled = x * power;

        double scaled_int = round(scaled);
    
        if (fabs(scaled - scaled_int) < 1e-12)
        {
            double lower_bound = pow((double)radix, digits - 1);
            double upper_bound = pow((double)radix, digits);
        
            if (scaled_int >= lower_bound && scaled_int < upper_bound)
            {
                *k = digits;
                *s = (int)scaled_int;
                return;
            }
        }
    }

    // Fallback: if no exact integer found, just use rounded value with max digits
    double power = pow((double)radix, max_digits - (*n));
    double scaled = x * power;
    double scaled_int = round(scaled);
    *k = max_digits;
    *s = (int)scaled_int;
}

static void to_radix_string(int number, int radix, int min_digits, char* output)
{
    char temp[64];
    int index = 0;

    do {
        int digit = number % radix;
        temp[index++] = "0123456789abcdefghijklmnopqrstuvwxyz"[digit];
        number /= radix;
    }
    while(number > 0);

    while (index < min_digits)
    {
        temp[index++] = '0';
    }

    for (int i = 0; i < index; i++)
    {
        output[i] = temp[index - i - 1];
    }

    output[index] = '\0';
}

JSValue number_to_string(JSValue input, int radix)
{
    if (value_is_NaN(input))
    {
        return init_string_value("NaN");
    }

    if (input.value.as_number == 0.0 || input.value.as_number == -0.0)
    {
        return init_string_value("0");
    }

    if (input.value.as_number < 0)
    {
        input.value.as_number = -input.value.as_number;
        input = number_to_string(input, radix);
        return value_concat_string(init_string_value("-"), input);
    }

    if (input.value.as_number == JS_POS_INFINITY)
    {
        return init_string_value("Infinity");
    }

    static char buffer[128];
    char temp[64];
    int i, len, n, k, s;
    find_n_k_s(input.value.as_number, radix, &n, &k, &s);

    if (radix != 10 || (n >= -5 && n <= 21))
    {
        to_radix_string(s, radix, k, temp);
    
        if (n >= k)
        {
            strcpy(buffer, temp);
            for (i = 0; i < n - k; i++)
            {
                strcat(buffer, "0");
            }
            return init_string_value(buffer);
        }
        if (n > 0)
        {
            strncpy(buffer, temp, n);
            buffer[n] = '\0';
            strcat(buffer, ".");
            strcat(buffer, temp + n);
            return init_string_value(buffer);
        }

        strcpy(buffer, "0.");
        for (i = 0; i < -n; i++)
        {
            strcat(buffer, "0");
        }
        strcat(buffer, temp);
        return init_string_value(buffer);
    }

    char exponent_sign = n < 0 
        ? '-' 
        : '+';

    if (k == 1)
    {
        to_radix_string(s, radix, k, temp);
        snprintf(buffer, sizeof(buffer), "%se%c%d", temp, exponent_sign, abs(n - 1));
        return init_string_value(buffer);
    }

    to_radix_string(s, radix, k, temp);
    buffer[0] = temp[0];
    buffer[1] = '.';
    strcpy(buffer + 2, temp + 1);
    len = strlen(buffer);
    snprintf(buffer + len, sizeof(buffer) - len, "e%c%d", exponent_sign, abs(n - 1));
    return init_string_value(buffer);
}