#ifndef SYMBOL_H
#define SYMBOL_H

#include "value.h"
#include "vm.h"

JSValue symbol_to_primitive(VM* vm);

JSValue symbol_to_string_tag(VM* vm);

#endif //SYMBOL_H