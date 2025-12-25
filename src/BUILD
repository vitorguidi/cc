cc_binary(
    name = "compiler",
    srcs = ["compiler.cc"],
    deps = [
        "//src/lexer:lexer",
        "//src/parser:parser",
        "//src/ast:ast",
        "//src/codegen:codegen"
    ],
    visibility = ["//visibility:public"],
)