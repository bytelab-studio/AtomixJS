#ifndef BUNDLE_H
#define BUNDLE_H

#include "module.h"

#define BUNDLE_MAGIC0 0x2E
#define BUNDLE_MAGIC1 0x41
#define BUNDLE_MAGIC2 0x78
#define BUNDLE_MAGIC3 0x42

#define BUNDLE_VERSION 1


typedef struct {
    char magic[4];
    uint16_t version;
    uint64_t entryPoint;
    uint16_t moduleCount;
    JSModule* modules;
} JSBundle;

JSModule bundle_get_module(JSBundle bundle, uint64_t hash);

#endif //BUNDLE_H