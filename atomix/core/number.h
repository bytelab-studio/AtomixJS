#ifndef NUMBER_H
#define NUMBER_H

#include <inttypes.h>

#include "vm.h"
#include "value.h"

/**
 * [ECMAScript® 2026 Language Specification §7.1.6](https://tc39.es/ecma262/#sec-toint32)
 */
int32_t number_to_int32(VM* vm, JSValue value);

/**
 * [ECMAScript® 2026 Language Specification §7.1.6](https://tc39.es/ecma262/#sec-touint32)
 */
uint32_t number_to_uint32(VM* vm, JSValue value);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.1](https://tc39.es/ecma262/#sec-numeric-types-number-unaryMinus)
 */
JSValue number_unary_minus(JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.2](https://tc39.es/ecma262/#sec-numeric-types-number-unaryMinus)
 */
JSValue number_bitwise_not(VM* vm, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.4](https://tc39.es/ecma262/#sec-numeric-types-number-multiply)
 */
JSValue number_multiply(JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.5](https://tc39.es/ecma262/#sec-numeric-types-number-divide)
 */
JSValue number_divide(JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.6](https://tc39.es/ecma262/#sec-numeric-types-number-remainder)
 */
JSValue number_remainder(JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.7](https://tc39.es/ecma262/#sec-numeric-types-number-add)
 */
JSValue number_add(JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.8](https://tc39.es/ecma262/#sec-numeric-types-number-subtract)
 */
JSValue number_subtract(JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.9](https://tc39.es/ecma262/#sec-numeric-types-number-leftShift)
 */
JSValue number_left_shift(VM* vm, JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.10](https://tc39.es/ecma262/#sec-numeric-types-number-signedRightShift)
 */
JSValue number_signed_right_shift(VM* vm, JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.11](https://tc39.es/ecma262/#sec-numeric-types-number-unsignedRightShift)
 */
JSValue number_unsigned_right_shift(VM* vm, JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.12](https://tc39.es/ecma262/#sec-numeric-types-number-lessThan)
 */
JSValue number_less_than(JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.13](https://tc39.es/ecma262/#sec-numeric-types-number-equal)
 */
JSValue number_equal(JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.17](https://tc39.es/ecma262/#sec-numeric-types-number-bitwiseAND) includes [§6.1.6.1.16](https://tc39.es/ecma262/#sec-numberbitwiseop)
 */
JSValue number_bitwise_and(VM* vm, JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.18](https://tc39.es/ecma262/#sec-numeric-types-number-bitwiseXOR) includes [§6.1.6.1.16](https://tc39.es/ecma262/#sec-numberbitwiseop)
 */
JSValue number_bitwise_xor(VM* vm, JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.19](https://tc39.es/ecma262/#sec-numeric-types-number-bitwiseOR) includes [§6.1.6.1.16](https://tc39.es/ecma262/#sec-numberbitwiseop)
 */
JSValue number_bitwise_or(VM* vm, JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §6.1.6.1.19](https://tc39.es/ecma262/#sec-numeric-types-number-bitwiseOR) includes [§6.1.6.1.16](https://tc39.es/ecma262/#sec-numberbitwiseop)
 */
JSValue number_to_string(JSValue input, int radix);

#endif //NUMBER_H