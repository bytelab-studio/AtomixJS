#ifndef LOADER_H
#define LOADER_H

#include <inttypes.h>

#include "format.h"

typedef enum LoadResult LoadResult;

void module_load_from_file(const char* filename, JSModule* module);

void module_load_from_buffer(uint8_t* buff, JSModule* module);

void bundle_load_from_file(const char* filename, JSBundle* bundle);

void bundle_load_from_buffer(uint8_t* buff, JSBundle* bundle);

LoadResult unknown_load_from_file(const char* filename, JSModule* module, JSBundle* bundle);

LoadResult unknown_load_from_buffer(uint8_t* buff, JSModule* module, JSBundle* bundle);

#endif //LOADER_H
