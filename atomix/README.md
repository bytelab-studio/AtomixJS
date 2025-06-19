# AtomixJS VM

## Project structure

```text
atomix/
├── bdwgc/               # Implementation of Boehm-Demers-Weiser conservative garbage collector
├── core/                # Core SourceCode actual VM definition
├── modules/             # Domain specific modules
│   └── core/            # Core module for exposing VM functionalities
├── debug/               # Code for the debug executable (Includes all modules)
└── release/             # Code for the production executable (WIP)
```

## Building

### Install zig

It is required to have the zig toolchain installed. You can either install it globally and add it to the global enviromnent or install it locally into the `/zig` folder of the project root.

### Compile the compiler

The next step is to compile typescript project with tsc. Therfore go into the `/atomixc` folder and run:
 
```sh
npm install
npm install -g typescript # (when not installed)
tsc
```

### Activate the environment

Last thing to do is to activate the project environment by import the init script into your session. On Windows PowerShell is required for the usage.


```sh
. ./env/init.sh # or init.ps1 on windows
```

If everything is sucesfully you should see the `(atomixc)` prefix in you CLI prompt.

Following things happend when init file is imported:
1. A atomixc alias is created that targets the JavaScript output file atomixc.js in the `/atomixc/dist/bin` folder.
2. If a local `/zig` folder exist it will be added to PATH variable

> **NOTE:** All changes are only affected for the CLI-session re-open the session will reset all things.

### Enable IntelliSense

AtomixJS uses it own build system. To still support the major range of editors. `atomixc` can generated a compilation database format file (cdf file) which most editors support.

The file can be generated with the following command:
```sh
atomixc engine cdf -o <dst_path>
```

Most editors require additonal configuration like custom compiler definiton.

### Build development suit

To build the project enter the follwoing commands in any directory. (I prefer the `/atomix/` folder)

```sh
atomixc engine init -p <platform> -a <arch>
```

replace `<platform>` with `linux` or `ẁindows` and `<arch>` with `x86_64` or `arm64` according to your target system. After the build a `.atomix` folder appears with the current build state.

The executable can be found under `./.atomix/bin/Debug/<platform>-<arch>/runner(.exe)`

### Build production suit (Currently not possible)

With the current build-system not possible

### Compiling behavior

The folder `core`, `modules/**` are compiled into a static archive (`.a`).
Then based on the configuration one of the loader (`debug` or `release`) is also compiled into a static archive. Additonally a module information file that is dynamicly generated will be compiled into an object.

Last but not least everything is linked into a executable

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
- The module will automatically be loaded during compilation, and if the appropriate option is set for the production executable.

### 3. Create a `mod.json` in the Module Folder

- Inside the newly created module folder, you must create a `mod.json` file for configure your module.

Example `mod.json` for the new module:

```json
{
    "loader": ["my_loader"]
}
```

The new module will be automatically linked with the static core library during the build process.

### 5. Define an Entry Point

We can now define our entry point of the module it is required that all loader items defined in `mod.json` are defined and external accessible in the module source code

```c++
#include "api.h"

// Declaration of the initialization function
void my_loader(VM* vm, Scope* scope) {
    // Add native functions or manipulate VM infrastructure here
}

```
