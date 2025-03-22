#include "scope.h"

#include <stdlib.h>

#define SCOPE_BUCKET_SIZE 32

Scope* scope_create_scope(Scope* parent)
{
    Scope* scope = malloc(sizeof(Scope));
    scope->parent = parent;
    scope->symbols = dict_create_dict(SCOPE_BUCKET_SIZE);
    return scope;
}

int scope_set(const Scope* scope, char* key, JSValue value)
{
    if (dict_set(scope->symbols, key, value, 1))
    {
        return 1;
    }

    if (scope->parent)
    {
        return scope_set(scope->parent, key, value);
    }

    return 0;
}

void scope_declare(const Scope* scope, char* key, JSValue value)
{
    dict_set(scope->symbols, key, value, 0);
}

JSValue scope_get(const Scope* scope, char* key)
{
    JSValue* value = dict_get(scope->symbols, key);
    if (value)
    {
        return *value;
    }
    if (scope->parent)
    {
        return scope_get(scope->parent, key);
    }
    return JS_VALUE_UNDEFINED;
}

int scope_contains(const Scope* scope, char* key, int parent_scopes)
{
    JSValue* value = dict_get(scope->symbols, key);
    if (value)
    {
        return 1;
    }
    if (parent_scopes && scope->parent)
    {
        return scope_contains(scope->parent, key, 1);
    }

    return 0;
}

int scope_delete(const Scope* scope, char* key)
{
    return dict_delete(scope->symbols, key);
}

void scope_free(Scope* scope)
{
    dict_free(scope->symbols);
    free(scope);
}
