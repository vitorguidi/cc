# Development Guide

## 🛠 Compiling a C Program

This compiler will perform the following steps:

* C AST generation
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