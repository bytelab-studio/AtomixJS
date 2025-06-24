#ifndef VALUE_H
#define VALUE_H

typedef enum JSValueType JSValueType;

typedef struct JSValue JSValue;

int value_is_falsy(JSValue* value);

static inline int value_is_truthy(JSValue* value)
{
    return !value_is_falsy(value);
}

int value_is_NaN(JSValue* value);

char* value_to_string(JSValue* value);

int value_is_array(JSValue* value);

#endif //VALUE_H
