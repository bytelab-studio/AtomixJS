#ifndef PANIC_H
#define PANIC_H

#include <stdio.h>
#include <stdlib.h>

#define PANIC(message) printf("PANIC: %s :: %s:%i\n", message, __FILE__, __LINE__); exit(1)

#endif //PANIC_H
