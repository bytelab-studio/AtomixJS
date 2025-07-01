#ifndef VALUE_H
#define VALUE_H

#include "vm.h"

typedef enum JSValueType JSValueType;

typedef struct JSValue JSValue;

typedef struct JSString JSString;

int value_is_NaN(JSValue value);

/**
 * [ECMAScript® 2026 Language Specification §7.1.2](https://tc39.es/ecma262/#sec-toboolean)
 */
JSValue value_to_boolean(JSValue value);

/**
 * [ECMAScript® 2026 Language Specification §7.1.17](https://tc39.es/ecma262/#sec-tostring)
 */
JSValue value_to_string(VM* vm, JSValue value);

int value_is_array(JSValue* value);

/**
 * Returns false for `JS_OBJECT` and `JS_FUNC` otherwise true
 */
inline int value_is_primitive(JSValueType type);

/**
 * [ECMAScript® 2026 Language Specification §7.1.17](https://tc39.es/ecma262/#sec-tostring)
 */
JSValue value_to_object(JSValue input);

/**
 * Concats two strings without modyfing the left and right
 */
JSValue value_concat_string(JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §7.1.1](https://tc39.es/ecma262/#sec-toprimitive)
 */
JSValue value_to_primitive(VM* vm, JSValue input, JSValueType prefered_type);

/**
 * [ECMAScript® 2026 Language Specification §7.1.1.1](https://tc39.es/ecma262/#sec-ordinarytoprimitive)
 */
JSValue value_ordinary_to_primitive(VM* vm, JSValue input, JSValueType prefered_type);

/**
 * [ECMAScript® 2026 Language Specification §7.1.3](https://tc39.es/ecma262/#sec-tonumeric)
 */
JSValue value_to_numeric(VM* vm, JSValue input);

/**
 * [ECMAScript® 2026 Language Specification §7.1.4](https://tc39.es/ecma262/#sec-tonumber)
 */
JSValue value_to_number(VM* vm, JSValue input);

/**
 * [ECMAScript® 2026 Language Specification §7.1.4.1.1](https://tc39.es/ecma262/#sec-stringtonumber)
 */
JSValue value_string_to_number(VM* vm, JSValue input);

/**
 * [ECMAScript® 2026 Language Specification §7.2.13](https://tc39.es/ecma262/#sec-islooselyequal)
 */
JSValue value_is_loosely_equal(VM* vm, JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §7.2.14](https://tc39.es/ecma262/#sec-isstrictlyequal)
 */
JSValue value_is_strictly_equal(JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §7.2.11](https://tc39.es/ecma262/#sec-samevaluenonnumber)
 */
JSValue value_same_value_non_number(JSValue left, JSValue right);

/**
 * [ECMAScript® 2026 Language Specification §7.2.12](https://tc39.es/ecma262/#sec-islessthan)
 */
JSValue value_is_less_than(VM* vm, JSValue left, JSValue right, int left_first);

#endif //VALUE_H
