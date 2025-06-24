#ifndef API_IMPL_H
#define API_IMPL_H

#include "api.h"

typedef struct NativeModuleList {
    struct NativeModuleList* next;
    JSModule* module;
} NativeModuleList;

extern NativeModuleList* native_modules;

typedef void (*module_init)(Scope*);

#endif //API_IMPL_H