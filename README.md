How to use this (for now):

bazel build //src:compiler

./bazel-bin/src/compiler examples/return_2.c return_2.s

gcc return_2.s -o return_2

./return_2

echo $?

expected:

vitor@vitor-ubuntu:~/projects/cc$ echo $?

2