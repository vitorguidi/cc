To compile a C program:


./bazel-bin/src/compiler $PWD/examples/complex_expression.c $PWD/asm_output/complex_expression.S

cd asm_output

gcc -o complex_expression complex_expression.S

graphviz plots for all stages of AST will be dumped into /asm_output (provided you have dot installed)


For end to end tests of the expression logic:

bazel test --test_arg=--seed=400 --test_arg=--iterations=5000 --test_arg=--max_height=40 //test/fuzzing:expression_fuzzing_test