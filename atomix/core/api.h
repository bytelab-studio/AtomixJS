#ifndef API_H
#define API_H

#include "scope.h"

typedef void (*module_init)(Scope*);

void bind_modules(Scope* scope);

char* init_string(char* str);

#endif //API_H
