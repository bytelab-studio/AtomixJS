#ifndef LOADER_H
#define LOADER_H

#include "module.h"

#define MAGIC0 0x2E
#define MAGIC1 0x41
#define MAGIC2 0x78
#define VERSION 2

JSModule module_load_from_file(const char* filename);

JSModule module_load_from_buffer(uint8_t* buff);

void module_free(JSModule module);

#endif //LOADER_H
