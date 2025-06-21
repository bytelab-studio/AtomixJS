## VM Opcodes

This document documents the instruction set of the AtomixJS virtual machine.

### Table of Contents

- [NOP (0x00)](#nop-0x00)
- [LD_INT (0x01)](#ld_int-0x01)
- [LD_DOUBLE (0x02)](#ld_double-0x02)
- [LD_STRING (0x03)](#ld_string-0x03)
- [LD_UNDF (0x04)](#ld_undf-0x04)
- [LD_NULL (0x05)](#ld_null-0x05)
- [LD_TRUE (0x06)](#ld_true-0x06)
- [LD_FALSE (0x07)](#ld_false-0x07)
- [LD_THIS (0x08)](#ld_this-0x08)
- [ADD (0x09)](#add-0x09)
- [MINUS (0x0A)](#minus-0xa)
- [MUL (0x0B)](#mul-0xb)
- [DIV (0x0C)](#div-0xc)
- [MOD (0x0D)](#mod-0xd)
- [BINARY_AND (0x0E)](#binary_and-0x0e)
- [BINARY_OR (0x0F)](#binary_or-0x0f)
- [BINARY_XOR (0x10)](#binary_xor-0x10)
- [BINARY_LSHFT (0x11)](#binary_lshft-0x11)
- [BINARY_RSHFT (0x12)](#binary_rshft-0x12)
- [BINARY_ZRSHFT (0x13)](#binary_zrshft-0x13)
- [BINARY_NOT (0x14)](#binary_not-0x14)
- [NOT (0x15)](#not-0x15)
- [NEGATE (0x16)](#negate-0x16)
- [TYPEOF (0x17)](#typeof-0x17)
- [TEQ (0x18)](#teq-0x18)
- [NTEQ (0x19)](#nteq-0x19)
- [GT (0x1A)](#gt-0x1a)
- [GEQ (0x1B)](#geq-0x1b)
- [LT (0x1C)](#lt-0x1c)
- [LEQ (0x1D)](#leq-0x1d)
- [POP (0x1E)](#pop-0x1e)
- [DUP (0x1F)](#dup-0x1f)
- [SWAP (0x20)](#swap-0x20)
- [ALLOC_LOCAL (0x21)](#alloc_local-0x21)
- [STORE_LOCAL (0x22)](#store_local-0x22)
- [LOAD_LOCAL (0x23)](#load_local-0x23)
- [LOAD_ARG (0x24)](#load_arg-0x24)
- [FUNC_DECL (0x25)](#func_decl-0x25)
- [FUNC_DECL_E (0x26)](#func_decl_e-0x26)
- [CALL (0x27)](#call-0x27)
- [ARR_ALLOC (0x28)](#arr_alloc-0x28)
- [OBJ_ALLOC (0x29)](#obj_alloc-0x29)
- [OBJ_STORE (0x2A)](#obj_store-0x2a)
- [OBJ_LOAD (0x2B)](#obj_load-0x2b)
- [OBJ_CLOAD (0x2C)](#obj_cload-0x2c)
- [OBJ_CSTORE (0x2D)](#obj_cstore-0x2d)
- [RETURN (0x2E)](#return-0x2e)
- [PUSH_SCOPE (0x2F)](#push_scope-0x2f)
- [PSCOPE (0x30)](#pscope-0x30)
- [JMP (0x31)](#jmp-0x31)
- [JMP_F (0x32)](#jmp_f-0x32)
- [JMP_T (0x33)](#jmp_t-0x33)
- [EXPORT (0x34)](#export-0x34)

---

### NOP (0x00)

**Description:**  
Performs no operation. This instruction is typically used as a placeholder and has no effect on the stack or program state.

**Stack Effect:**  
_None_

**Use Cases:**  
- Acts as a placeholder for jump targets or instructions that are not yet known at code generation time.

---

### LD_INT (0x01)

**Description:**  
Loads a constant 32-bit signed integer value onto the stack. The integer is encoded as an immediate operand following the instruction.

**Stack Effect:**  
- Pushes a 32-bit signed integer onto the stack.

**Use Cases:**  
- Push literal integer values for arithmetic or logical operations.
- Initialize variables or function arguments with constant integers.

---

### LD_DOUBLE (0x02)

**Description:**  
Loads a constant 64-bit double-precision floating-point value onto the stack. The value is encoded as an immediate operand following the instruction.

**Stack Effect:**  
- Pushes a 64-bit floating-point number onto the stack.

**Use Cases:**  
- Push literal floating-point values for mathematical operations.
- Initialize variables or function arguments with constant doubles.

---

### LD_STRING (0x03)

**Description:**  
Loads a constant string onto the stack. The string is referenced by an index into the string table or constant pool.

**Stack Effect:**  
- Pushes a string value onto the stack.

**Use Cases:**  
- Push string literals for concatenation, comparison, or printing.
- Initialize variables or pass arguments that require text values.

---

### LD_UNDF (0x04)

**Description:**  
Pushes the special `undefined` value onto the stack. This represents an uninitialized or absent value in the VM.

**Stack Effect:**  
- Pushes the `undefined` value onto the stack.

**Use Cases:**  
- Represent uninitialized variables.

---

### LD_NULL (0x05)

**Description:**  
Pushes the special `null` value onto the stack, representing a null reference or absence of an object.

**Stack Effect:**  
- Pushes the `null` value onto the stack.

**Use Cases:**  
- Represent null references.

---

### LD_TRUE (0x06)

**Description:**  
Pushes the boolean value `true` onto the stack.

**Stack Effect:**  
- Pushes the boolean `true` onto the stack.

**Use Cases:**  
- Represent boolean literals in expressions.
- Initialize variables or function arguments with a true value.

---

### LD_FALSE (0x07)

**Description:**  
Pushes the boolean value `false` onto the stack.

**Stack Effect:**  
- Pushes the boolean `false` onto the stack.

**Use Cases:**  
- Represent boolean literals in expressions.
- Initialize variables or function arguments with a false value.

---

### LD_THIS (0x08)

**Description:**  
Pushes the current context object (`this`) onto the stack, typically referring to the object instance in method calls.  
If the `this` context does not exist in the current scope, the `undefined` value is pushed instead.

**Stack Effect:**  
- Pushes the `this` reference or `undefined` onto the stack.

**Use Cases:**  
- Access the current object inside instance methods.  
- Pass the current context to functions or expressions.  
- Handle cases where `this` is not defined in the current scope.

---

### ADD (0x09)

**Description:**  
Performs addition on the top two values on the stack. The operation supports numeric addition or string concatenation depending on operand types.

**Stack Effect:**  
- Pops two values from the stack, pushes their sum or concatenation result back.

**Use Cases:**  
- Add integers or floating-point numbers.
- Concatenate strings.
- Implement arithmetic or string operations in expressions.

---

### MINUS (0x0A)

**Description:**  
Subtracts the top value on the stack (`right`) from the second top value (`left`) following JavaScript operator coercion rules. Operands are converted as needed to perform numeric subtraction.

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value  
- `right` = top value  
Pushes the result of `left - right` onto the stack.

**Use Cases:**  
- Perform subtraction following JavaScript semantics, including type coercion.  
- Support arithmetic operations within the VM.

---

### MUL (0x0B)

**Description:**  
Multiplies the top two values on the stack following JavaScript operator coercion rules. Operands are converted as needed to perform numeric multiplication.

**Stack Effect:**  
- Pops two values from the stack (`left`, `right`) and pushes the result of `left * right`.

**Use Cases:**  
- Perform multiplication following JavaScript semantics, including type coercion.  
- Support arithmetic operations within the VM.

---

### DIV (0x0C)

**Description:**  
Divides the second top value on the stack (`left`) by the top value (`right`), following JavaScript operator coercion rules. Operands are converted as needed to perform numeric division.

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value  
- `right` = top value  
Pushes the result of `left / right` onto the stack.

**Use Cases:**  
- Perform division following JavaScript semantics, including type coercion.  
- Support arithmetic operations within the VM.

---

### MOD (0x0D)

**Description:**  
Calculates the remainder of dividing the second top value on the stack (`left`) by the top value (`right`), following JavaScript operator coercion rules. Operands are converted as needed to perform numeric modulo operation.

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value  
- `right` = top value  
Pushes the result of `left % right` onto the stack.

**Use Cases:**  
- Perform modulo operation following JavaScript semantics, including type coercion.  
- Support arithmetic operations within the VM.

---

### BINARY_AND (0x0E)

**Description:**  
Performs a bitwise AND operation on the top two values on the stack, following JavaScript operator coercion rules. Operands are converted to 32-bit integers before the operation.

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value  
- `right` = top value  
Pushes the result of `left & right` onto the stack.

**Use Cases:**  
- Perform bitwise AND operations in expressions.  
- Implement low-level binary logic within the VM.

---

### BINARY_OR (0x0F)

**Description:**  
Performs a bitwise OR operation on the top two values on the stack, following JavaScript operator coercion rules. Operands are converted to 32-bit integers before the operation.

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value  
- `right` = top value  
Pushes the result of `left | right` onto the stack.

**Use Cases:**  
- Perform bitwise OR operations in expressions.  
- Implement low-level binary logic within the VM.

---

### BINARY_XOR (0x10)

**Description:**  
Performs a bitwise XOR (exclusive OR) operation on the top two values on the stack, following JavaScript operator coercion rules. Operands are converted to 32-bit integers before the operation.

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value  
- `right` = top value  
Pushes the result of `left ^ right` onto the stack.

**Use Cases:**  
- Perform bitwise XOR operations in expressions.  
- Implement low-level binary logic within the VM.

---

### BINARY_LSHFT (0x11)

**Description:**  
Performs a bitwise left shift operation on the top two values on the stack, following JavaScript operator coercion rules. The left operand is converted to a 32-bit integer and shifted left by the number of bits specified by the right operand (also converted to 32-bit integer).

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value (value to shift)  
- `right` = top value (number of bits to shift)  
Pushes the result of `left << right` onto the stack.

**Use Cases:**  
- Perform bitwise left shift operations in expressions.  
- Implement low-level binary logic and bit manipulation within the VM.

---

### BINARY_RSHFT (0x12)

**Description:**  
Performs a bitwise signed right shift operation on the top two values on the stack, following JavaScript operator coercion rules. The left operand is converted to a 32-bit integer and shifted right (sign-propagating) by the number of bits specified by the right operand (also converted to 32-bit integer).

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value (value to shift)  
- `right` = top value (number of bits to shift)  
Pushes the result of `left >> right` onto the stack.

**Use Cases:**  
- Perform bitwise signed right shift operations in expressions.  
- Implement low-level binary logic and bit manipulation within the VM.

---

### BINARY_RSHFT (0x12)

**Description:**  
Performs a bitwise signed right shift operation on the top two values on the stack, following JavaScript operator coercion rules. The left operand is converted to a 32-bit integer and shifted right (sign-propagating) by the number of bits specified by the right operand (also converted to 32-bit integer).

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value (value to shift)  
- `right` = top value (number of bits to shift)  
Pushes the result of `left >> right` onto the stack.

**Use Cases:**  
- Perform bitwise signed right shift operations in expressions.  
- Implement low-level binary logic and bit manipulation within the VM.

---

### BINARY_ZRSHFT (0x13)

**Description:**  
Performs a bitwise unsigned right shift operation on the top two values on the stack, following JavaScript operator coercion rules. The left operand is converted to a 32-bit integer and shifted right (unsigned) by the number of bits specified by the right operand (also converted to 32-bit integer).

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value (value to shift)  
- `right` = top value (number of bits to shift)  
Pushes the result of `left >>> right` onto the stack.

**Use Cases:**  
- Perform bitwise unsigned right shift operations in expressions.  
- Implement low-level binary logic and bit manipulation within the VM.

---

### BINARY_NOT (0x14)

**Description:**  
Performs a bitwise NOT (complement) operation on the top value of the stack, following JavaScript operator coercion rules. The operand is converted to a 32-bit integer before the operation.

**Stack Effect:**  
Pops one value from the stack and pushes the bitwise complement (~operand) result back onto the stack.

**Use Cases:**  
- Perform bitwise NOT operations in expressions.  
- Implement low-level binary logic within the VM.

---

### NOT (0x15)

**Description:**  
Performs the logical NOT operation on the top value of the stack, following JavaScript boolean coercion rules. Converts the operand to a boolean and pushes the negated boolean value.

**Stack Effect:**  
Pops one value from the stack and pushes the logical negation result back onto the stack.

**Use Cases:**  
- Implement logical NOT in expressions.  
- Support boolean logic operations within the VM.

---

### NEGATE (0x16)

**Description:**  
Performs numeric negation on the top value of the stack, following JavaScript operator coercion rules. Converts the operand to a number and pushes the negated value.

**Stack Effect:**  
Pops one value from the stack and pushes the negated numeric result back onto the stack.

**Use Cases:**  
- Implement unary minus operation in expressions.  
- Support numeric negation within the VM.

---

### TYPEOF (0x17)

**Description:**  
Evaluates the type of the top value on the stack following JavaScript's `typeof` operator rules and pushes a string representing the type.

**Stack Effect:**  
Pops one value from the stack and pushes a string describing its type (e.g., "number", "string", "undefined", "object", etc.).

**Use Cases:**  
- Implement the `typeof` operator within the VM.  
- Allow runtime type inspection in scripts.

---

### TEQ (0x18)

**Description:**  
Performs a strict equality comparison (`===`) between the second top and the top values on the stack, following JavaScript strict equality rules without type coercion.

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value  
- `right` = top value  
Pushes `true` if `left === right`, otherwise `false`.

**Use Cases:**  
- Implement strict equality checks in expressions.  
- Support comparison operations within the VM.

---

### NTEQ (0x19)

**Description:**  
Performs a strict inequality comparison (`!==`) between the second top and the top values on the stack, following JavaScript strict inequality rules without type coercion.

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value  
- `right` = top value  
Pushes `true` if `left !== right`, otherwise `false`.

**Use Cases:**  
- Implement strict inequality checks in expressions.  
- Support comparison operations within the VM.

---

### GT (0x1A)

**Description:**  
Performs a greater-than comparison (`>`) between the second top and the top values on the stack, following JavaScript relational comparison rules.

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value  
- `right` = top value  
Pushes `true` if `left > right`, otherwise `false`.

**Use Cases:**  
- Implement greater-than comparisons in expressions.  
- Support relational operations within the VM.

---

### GEQ (0x1B)

**Description:**  
Performs a greater-than-or-equal-to comparison (`>=`) between the second top and the top values on the stack, following JavaScript relational comparison rules.

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value  
- `right` = top value  
Pushes `true` if `left >= right`, otherwise `false`.

**Use Cases:**  
- Implement greater-than-or-equal-to comparisons in expressions.  
- Support relational operations within the VM.

---

### LT (0x1C)

**Description:**  
Performs a less-than comparison (`<`) between the second top and the top values on the stack, following JavaScript relational comparison rules.

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value  
- `right` = top value  
Pushes `true` if `left < right`, otherwise `false`.

**Use Cases:**  
- Implement less-than comparisons in expressions.  
- Support relational operations within the VM.

---

### LEQ (0x1D)

**Description:**  
Performs a less-than-or-equal-to comparison (`<=`) between the second top and the top values on the stack, following JavaScript relational comparison rules.

**Stack Effect:**  
Pops two values from the stack in this order:  
- `left` = second top value  
- `right` = top value  
Pushes `true` if `left <= right`, otherwise `false`.

**Use Cases:**  
- Implement less-than-or-equal-to comparisons in expressions.  
- Support relational operations within the VM.

---

### POP (0x1E)

**Description:**  
Removes the top value from the stack without using it.

**Stack Effect:**  
Pops one value from the top of the stack.

**Use Cases:**  
- Discard values that are no longer needed.  
- Clean up the stack after computations or side effects.

---

### DUP (0x1F)

**Description:**  
Duplicates the top value on the stack by pushing a copy of it onto the stack.  
**Note:** If the top value is an object, only the reference is duplicated, not the object itself.

**Stack Effect:**  
Reads the top value of the stack and pushes a copy of it onto the stack.

**Use Cases:**  
- Duplicate values for multiple uses without popping.  
- Prepare operands for instructions requiring the same value twice.

---

### SWAP (0x20)

**Description:**  
Swaps the top two values on the stack.

**Stack Effect:**  
Pops the top two values from the stack, then pushes them back in reverse order.

**Use Cases:**  
- Reorder values on the stack for subsequent operations.  
- Adjust operand positions without additional data movement.

---

### ALLOC_LOCAL (0x21)

**Description:**  
Pops a value off the stack and stores it as a new variable in the current scope.  
Requires an operand: an index into the string table representing the variable name.

**Stack Effect:**  
Pops one value from the stack and associates it with the given variable name in the current local scope.

**Use Cases:**  
- Declare and initialize local variables during function execution.  
- Manage variable bindings within the current scope.

---

### STORE_LOCAL (0x22)

**Description:**  
Pops a value off the stack and stores it in an existing variable in the current scope.  
Requires an operand: an index into the string table representing the variable name.

**Stack Effect:**  
- Pops one value from the stack and assigns it to the specified variable.

**Use Cases:**  
- Update the value of a variable during execution.  
- Support variable assignment within the scope stack.

---

### STORE_LOCAL (0x22)

**Description:**  
Pops a value off the stack and stores it in an existing variable in the current scope or one of its parent scopes.  
Requires an operand: an index into the string table representing the variable name.  
The VM panics if the variable is not found in any accessible scope.

**Stack Effect:**  
Pops one value from the stack and assigns it to the specified variable.

**Use Cases:**  
- Update variable values respecting scope chain lookup.  
- Enforce variable existence during assignment.

---

### LOAD_LOCAL (0x23)

**Description:**  
Loads the value of a variable from the current scope or one of its parent scopes onto the stack.  
Requires an operand: an index into the string table representing the variable name.  
The VM panics if the variable is not found in any accessible scope.

**Stack Effect:**  
Pushes the value of the specified variable onto the stack.

**Use Cases:**  
- Retrieve variable values for use in expressions or operations.  
- Support variable access respecting scope chain.

---

### LOAD_ARG (0x24)

**Description:**  
Loads a function argument value onto the stack.  
Requires an operand: the zero-based index of the argument to load.  
If the argument index does not exist, `undefined` is pushed.

**Stack Effect:**  
Pushes the value of the specified argument onto the stack, or `undefined` if out of bounds.

**Use Cases:**  
- Access function arguments inside the function body.  
- Support parameter passing and optional/missing arguments.

---

### FUNC_DECL (0x25)

**Description:**  
Declares a function in the current scope, similar to how `ALLOC_LOCAL` declares variables.  
Requires two operands:  
- The first operand is an index into the string table representing the function name.  
- The second operand is the count of instructions representing the function body length.  

The function object is pushed onto the stack. After declaration, the VM jumps over the function body by the given instruction count.

**Stack Effect:**  
Pushes the newly declared function object onto the stack.

**Use Cases:**  
- Declare and initialize functions in the current scope.

---

### FUNC_DECL_E (0x26)

**Description:**  
Declares an anonymous function (not bound to any name in the current scope).  
Requires one operand: the count of instructions representing the function body length.  

The function object is pushed onto the stack. After declaration, the VM jumps over the function body by the given instruction count.

**Stack Effect:**  
Pushes the newly declared anonymous function object onto the stack.

**Use Cases:**  
- Create anonymous functions for immediate use or assignment.

---

### CALL (0x27)

**Description:**  
Calls a function on the stack.  
Requires one operand: the number of arguments (`N`) to pass to the function.  

The stack before the call must have, from top to bottom:  
- `argN` (last argument)  
- ...  
- `arg2`  
- `arg1` (first argument)  
- `this` context (or `undefined` if none)  
- the function to call  

The arguments must be pushed in **reverse order** (last argument first).  
After the call, all arguments, the `this` context, and the function are popped from the stack.  
The function’s return value is pushed onto the stack; if the function returns nothing, `undefined` is pushed.

**Stack Effect:**  
Pops the function, `this` context, and all arguments; pushes the return value or `undefined`.

**Use Cases:**  
- Support calling functions with arbitrary argument counts and contexts.

---

### ARR_ALLOC (0x28)

**Description:**  
Allocates a new array object with the array prototype.  

**Stack Effect:**  
Pushes the newly allocated array object onto the stack.

**Use Cases:**  
- Create new array instances during execution.  

---

### OBJ_ALLOC (0x29)

**Description:**  
Allocates a new generic object with the object prototype.  

**Stack Effect:**  
Pushes the newly allocated object onto the stack.

**Use Cases:**  
- Create new object instances dynamically.  

---

### OBJ_STORE (0x2A)

**Description:**  
Stores a value into an object property.  
Requires one operand: an index into the string table representing the property name.  

The value is pushed first, followed by the object.  
This instruction pops the object and value from the stack, and assigns the value to the specified property on the object.

**Stack Effect:**  
Pops the object and the value.  
No values are pushed onto the stack.

**Use Cases:**  
- Assign object properties.

---

### OBJ_LOAD (0x2B)

**Description:**  
Loads a value from an object property.  
Requires one operand: an index into the string table representing the property name.  

Pops the object from the stack, then pushes the value of the specified property.  
If the property does not exist, `undefined` is pushed.

**Stack Effect:**  
Pops the object.  
Pushes the property value (or `undefined` if not found).

**Use Cases:**  
- Retrieve property values from objects.  

---

### OBJ_CLOAD (0x2C)

**Description:**  
Loads a value from an object using a dynamic property key.  
This instruction pops the key (as a value) and the object from the stack, in that order.  
If the property does not exist, `undefined` is pushed.

**Stack Effect:**  
Pops the key and the object.  
Pushes the corresponding property value or `undefined`.

**Use Cases:**  
- Access properties using computed keys (e.g., `obj[key]`).

---

### OBJ_CSTORE (0x2D)

**Description:**  
Stores a value into an object using a dynamic property key.  
The required stack order is:
- First, push the value to be assigned.
- Then, push the object to modify.
- Finally, push the key to use as the property name.

This instruction pops the value, object, and key (in that order) from the stack and performs the assignment:  
`object[key] = value`.

**Stack Effect:**  
Pops value, object, and key.  
No values are pushed.

**Use Cases:**  
- Dynamically assign properties using runtime-evaluated keys.

---

### RETURN (0x2E)

**Description:**  
Special opcode that exits the current function execution.  
If the stack contains any values, the top value is popped and used as the return value. Otherwise, `undefined` is returned.

**Stack Effect:**  
- If the stack > 0, pops the top value and returns it.  
- Otherwise, returns `undefined`.

**Use Cases:**  
- Exit functions where a value may or may not have been pushed before return.  

---

### PUSH_SCOPE (0x2F)

**Description:**  
Creates and pushes a new scope onto the VM's internal scope stack.  
This new scope is used to isolate variable declarations and lookups from outer scopes.  
All variables declared after this instruction are contained within the new scope until it is popped.

**Stack Effect:**  
No changes to the value stack.  
Modifies the internal scope stack.

**Use Cases:**  
- Entering a new block scope (e.g., inside functions or control structures).  
- Prevents variable leakage between different execution contexts.

---

### PUSH_SCOPE (0x2F)

**Description:**  
Creates and pushes a new scope onto the VM's internal scope stack.  
This new scope inherits from the **current scope**, which becomes its **parent scope**.  
All variable declarations and lookups within this new scope will fall back to the parent scope if not found locally.

**Stack Effect:**  
No changes to the value stack.  
Modifies the internal scope stack.

**Use Cases:**  
- Entering a new block scope (e.g., inside functions or control structures).

---

### PSCOPE (0x30)

**Description:**  
Removes the current (top-most) scope from the VM's internal scope stack, returning control to the parent scope.  
This effectively discards all variables declared in the popped scope.

**Scope Lifetime:**  
The scope is only **freed** if it is **not referenced** by any active or declared function—either directly or through one of its child scopes.  

**Stack Effect:**  
No changes to the value stack.  
Modifies the internal scope stack.

**Use Cases:**  
- Exiting a block scope.

---

### JMP (0x31)

**Description:**  
Performs an unconditional jump to an absolute instruction index in the current module.  
This directly sets the instruction pointer to the given offset, bypassing any intermediate code.

**Stack Effect:**  
No change to the value stack.

**Operand:**  
- Absolute jump offset (unsigned integer).  
  Refers to an instruction index in the current module.

**Behavior:**  
If the target offset is out of bounds (i.e., not a valid instruction index), module execution stops  
as if there are no more instructions left to execute.

**Use Cases:**  
- Handling custom flow control or intermediate compilation steps.

---

### JMP_F (0x32)

**Description:**  
Performs a conditional jump to an absolute instruction index if the top value on the stack is falsy.
The top value is popped from the stack before the jump decision.

**Stack Effect:**  
- Pops one value from the stack.

**Operand:**  
- Absolute jump offset (unsigned short).  
  Refers to an instruction index in the current module.

**Behavior:**  
If the target offset is out of bounds, module execution stops as if no further instructions exist.

**Use Cases:**  
- Control flow for if-statements, loops, and short-circuit evaluation.

---

### JMP_T (0x33)

**Description:**  
Performs a conditional jump to an absolute instruction index if the top value on the stack is truthy.
The top value is popped from the stack before the jump decision.

**Stack Effect:**  
- Pops one value from the stack.

**Operand:**  
- Absolute jump offset (unsigned short).  
  Refers to an instruction index in the current module.

**Behavior:**  
If the target offset is out of bounds, module execution stops as if no further instructions exist.

**Use Cases:**
- Control flow for if-statements, loops, and short-circuit evaluation.

---

### EXPORT (0x34)

**Description:**  
Pops a value from the stack and writes it into the module's internal export object, using the operand as the property name.  
The operand is an index into the string table representing the export property name.

**Important Note:**  
The exported property is **not bound** to any original source or variable.  
Subsequent changes to the original source do not affect the exported value.

**Stack Effect:**  
- Pops one value from the stack.

**Operand:**  
- Index to the string table (name of the export property).

**Use Cases:**  
- Exporting values from a module for external use.