cc_binary(
    name = "compiler",
    srcs = ["compiler.cc"],
    deps = [
        "//src/lexer:lexer",
        "//src/parser:parser",
        "//src/ast:ast",
        "//src/graphviz",
        "//src/codegen:codegen",
        "//src/semantic:semantic",
    ],
    visibility = ["//visibility:public"],
)