#ifndef SCOPE_H
#define SCOPE_H

#include "value.h"

typedef struct Scope Scope;

Scope* scope_create_scope(Scope* parent);

int scope_set(const Scope* scope, char* key, JSValue value);

void scope_declare(const Scope* scope, char* key, JSValue value);

JSValue scope_get(const Scope* scope, char* key);

int scope_contains(const Scope* scope, char* key, int parent_scopes);

int scope_delete(const Scope* scope, char* key);

#endif //SCOPE_H
