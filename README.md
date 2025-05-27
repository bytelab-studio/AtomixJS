# AtomixJS

[![CI](https://github.com/bytelab-studio/AtomixJS/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/bytelab-studio/AtomixJS/actions/workflows/ci.yml)

AtomixJS is a lightweight JavaScript runtime designed specifically for PC-like systems (Windows, Linux, MacOS) running
on 64-bit architectures. By compiling JavaScript into ByteCode rather than using dynamic interpretation methods like
`eval`, AtomixJS emphasizes efficiency, predictability, and a clear separation between browser JavaScript and desktop
environments.

## Overview

AtomixJS is split into two core components:

- **Compiler**: Written in TypeScript, the compiler leverages [acornJS](https://github.com/acornjs/acorn) for parsing
  and [estraverse](https://github.com/estools/estraverse) for AST manipulation. Its primary function is to translate
  JavaScript code into ByteCode.
- **Virtual Machine (VM)**: Implemented in C99, the VM is built with minimal dependencies (relying only on `libc`) to
  provide a lightweight and portable runtime environment for executing the compiled ByteCode.

## What AtomixJS Is and Is Not

### AtomixJS Is:

- **A ByteCode Compiler:** It translates JavaScript code into ByteCode rather than interpreting code dynamically.

- **Optimized for 64-bit PC Systems:** Specifically designed for Windows, Linux, and MacOS on 64-bit architectures.

- **Modular:** Divided into a compiler (TypeScript) and a virtual machine (C99) for a clear separation of concerns.

- **Efficient:** Emphasizes minimal dependency by relying solely on `libc` in the VM, ensuring a lean runtime.

- **Focused:** Aims to provide a solid foundational structure rather than full ECMA compliance, simplifying built-in
  modules (e.g., using a unified `crypto` module).

### AtomixJS Is Not:

- **A Dynamic Interpreter:** It does not support functions like `eval` or dynamic interpretation of JavaScript at
  runtime.
- **A Full ECMA Environment:** It does not aim to implement every ECMA built-in object or function, especially those
  that are redundant or not applicable to a desktop context.
- **A Browser Integration Framework:** Unlike NodeJS or Bun, AtomixJS does not tightly integrate browser-specific APIs
  with the desktop environment.
- **A One-Size-Fits-All Runtime:** The initial build is intentionally minimal, focusing on core functionality rather
  than broad feature coverage.
- **A Fully Dynamic Module Loader:** It supports only the ECMA Module import syntax in the first build and does not
  include dynamic runtime loading of modules beyond loading external ByteCode.

## Key Features

- **ByteCode Compilation:** AtomixJS compiles JavaScript into ByteCode to avoid runtime evaluation.
- **Streamlined Environment:** It avoids unnecessary browser-specific integrations, favoring efficiency and clarity.
- **Simplified Module System:** Supports only the ECMA Module import syntax in the initial release.
- **Dual Build Modes:**
    - **Debug Mode:** Includes all native modules and loads ByteCode from external files for testing.
    - **Production Mode:** Embeds ByteCode directly into the executable, including only essential modules for a leaner
      deployment.
- **Focused Core Functionality:** Prioritizes a robust and efficient system foundation over comprehensive ECMA standard
  coverage.

## Project Structure

The project is organized into two primary directories:

- `/atomixc` \
  Contains the TypeScript source code for the compiler. It utilizes:
    - [acornJS](https://github.com/acornjs/acorn): For parsing JavaScript
    - [estraverse](https://github.com/estools/estraverse): For traversing and manipulating the AST.
- `/atomix` \
  Contains the C99 source code for the Virtual Machine. The VM is designed to be minimalistic by relying solely on the C
  standard library (`libc`), ensuring a lean and efficient runtime.

## Contributing

Contributions to AtomixJS are welcome!

## Licensing
AtomixJS is released under the [MIT License](./LICENSE).
