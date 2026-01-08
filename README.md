# Development Guide

## 🛠 Compiling a C Program

This compiler will perform the following steps:

* C AST generation
* Semantic analysis
* 3 address code (Tacky) conversion
* ASM generation
* Pseudoregister replacement
* Instruction fix up
* Assembly code text dump

To compile a source file into assembly using the custom compiler, follow these steps:

### 1. Build and Run the Compiler
First, ensure the binary is built, create the output directory, and run the compilation:

```bash
# Build the compiler target
bazel build //src/compiler:compiler

# Ensure the output directory exists
mkdir -p asm_output

# Compile the source to assembly
./bazel-bin/src/compiler \
    $PWD/examples/complex_expression.c \
    $PWD/asm_output/complex_expression.S
```

2.  **Assemble and Link:**
    ```bash
    cd asm_output
    gcc -o complex_expression complex_expression.S
    ./complex_expression
    ```

3. **AST Visualization:**
The compiler will automatically dump Graphviz plots for all stages of the AST into the `/asm_output` directory.

4. **Stress testing:**

    For stress testing of expression evaluation:
    ```bash
    bazel test \
        --test_arg=--seed=400 \
        --test_arg=--iterations=5000 \
        --test_arg=--max_height=40 \
        //test/fuzzing:expression_fuzzing_test
    ```

4. **Features implemented so far:**

- [x] Chapter 1: A Minimal Compiler
- [x] Chapter 2: Unary Operators
- [x] Chapter 3: Binary Operators
- [x] Chapter 4: Logical and Relational Operators
- [x] Chapter 5: Local Variables
- [x] Chapter 6: if Statements and Conditional Expressions
- [ ] Chapter 7: Compound Statements
- [x] Chapter 8: Loops
- [ ] Chapter 9: Functions
- [ ] Chapter 10: File Scope Variable Declarations and Storage-Class Specifiers
- [ ] Chapter 11: Long Integers
- [ ] Chapter 12: Unsigned Integers
- [ ] Chapter 13: Floating-Point Numbers
- [ ] Chapter 14: Pointers
- [ ] Chapter 15: Arrays and Pointer Arithmetic
- [ ] Chapter 16: Characters and Strings
- [ ] Chapter 17: Supporting Dynamic Memory Allocation
- [ ] Chapter 18: Structures
- [ ] Chapter 19: Optimizing TACKY Programs
- [ ] Chapter 20: Register Allocation