# AtomixJS VM

## Project structure

```text
atomix/
├── CMakeLists.txt       # Root CMake file to handle the build process
├── core/                # Core SourceCode actual VM definition
├── modules/             # Domain specific modules
│   └── core/            # Core module for exposing VM functionalities
├── debug/               # Code for the debug executable (Includes all modules)
└── release/             # Code for the production executable (WIP)
```

## Building

### Build development suit

This configuration links all available modules automatically:

```shell
mkdir ./cmake-build-debug
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug --config Debug --target atomix -j 14
```

### Build production suit

Specify which modules to include in the build process:

```shell
mkdir ./cmake-build-debug
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug --config Debug --target prod -DMOD_CORE=ON
```

### Linking behavior

Each module is compiled into a static archive (`.a`). To ensure that all object files from the specified modules are
included in the final executable, we use the `--whole-archive` linker option:

```cmake
target_link_options(atomix PRIVATE -Wl,--whole-archive mod_core -Wl,--no-whole-archive)
```

This ensures that symbols from all object files in `mod_core` are preserved and linked, even if they are not directly
referenced in the code.

## Adding a New Module

To add a new module to the project, follow these steps:

### 1. Create a New Folder in the `modules/` Directory

- Simply create a new folder inside the `modules/` directory. The folder name will be used to identify the module, so it
  must be **unique** and **case-sensitive**. For example, `my_module`, `My_module`, and `MY_MODULE` would be considered
  the same name because they would all be converted to the same uppercase string (`MY_MODULE`), which is not unique.
- Example: If you want to create a module called `my_module`, create a folder named `my_module` under `modules/`.

### 2. Module Initialization

- By creating this new folder, a new module is automatically initialized by the build system. The module will be
  included in the compilation process depending on the build configuration.
- The module will automatically be loaded during compilation, and if the appropriate option is set for production (`MOD_
  {MODULE_NAME}`), it will be linked to the executable.

### 3. Create a `CMakeLists.txt` in the Module Folder

- Inside the newly created module folder, you must create a `CMakeLists.txt` file to define the module as a static
  library.
- The static library should be named `mod_{MODULE_NAME}`, where `{MODULE_NAME}` is the name of your folder in
  lowercase (e.g., `mod_my_module`).

Example `CMakeLists.txt` for the new module:

```cmake
# modules/my_module/CMakeLists.txt
add_library(mod_my_module STATIC my_mod.c)

# Link the core static library
target_link_libraries(mod_my_module PRIVATE core)
```

### 4. Linking the Module

- The new module will be automatically linked with the core library during both the debug and production builds,
  provided the corresponding option (`MOD_{MODULE_NAME}`) is set during CMake configuration.
- The flag used to enable a module for production builds must be in **uppercase**.

For example, to enable the `my_module` module in the production build, you would pass the following option when running
CMake:

cmake .. -DMOD_MY_MODULE=ON --target prod

### 5. Define an Entry Point with `MODULE_INIT`

- Somewhere in the code of your module, you need to use the `MODULE_INIT` macro to define an entry point for the module.
  This entry point is where you can add native functions or manipulate the VM infrastructure.
- The `MODULE_INIT` macro allows the initialization function to be placed in a special section, ensuring it is properly
  included during compilation.

Example of using the `MODULE_INIT` macro:

```c++
#include "api.h"  // Include core module

// Declaration of the initialization function
void my_module_init(VM* vm, Scope* scope) {
    // Add native functions or manipulate VM infrastructure here
}

// Register the initialization function in the module section
MODULE_INIT(my_module_init);
```