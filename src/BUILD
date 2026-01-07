cc_binary(
    name = "compiler",
    srcs = ["compiler.cc"],
    deps = [
        "//src/lexer:lexer",
        "//src/parser:parser",
        "//src/ast:ast",
        "//src/graphviz",
        "//src/ir:ir",
        "//src/backend:backend",
        "//src/semantic:semantic",
    ],
    visibility = ["//visibility:public"],
)