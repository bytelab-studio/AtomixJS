#ifndef LOADER_H
#define LOADER_H

#include "module.h"
#include "bundle.h"

typedef enum {
    LOAD_BUNDLE,
    LOAD_MODULE
} LoadResult;

JSModule module_load_from_file(const char* filename);

JSModule module_load_from_buffer(uint8_t* buff);

JSBundle bundle_load_from_file(const char* filename);

JSBundle bundle_load_from_buffer(uint8_t* buff);

LoadResult unknown_load_from_file(const char* filename, JSModule* module, JSBundle* bundle);

LoadResult unknown_load_from_buffer(uint8_t* buff, JSModule* module, JSBundle* bundle);

void module_free(JSModule module);

void bundle_free(JSBundle bundle);

#endif //LOADER_H
