#ifndef SCOPE_IMPL_H
#define SCOPE_IMPL_H

#include "scope.h"

#include "dict.h"

struct Scope
{
    struct Scope* parent;
    JSDict* symbols;
};

#endif //SCOPE_IMPL_H