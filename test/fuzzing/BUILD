cc_test(
    name = "expression_fuzzing_test",
    size = "small",
    srcs = ["expression_fuzzing_test.cc"],
    timeout = "long",
    deps = [
        "@googletest//:gtest",
        "//src/ast:ast",
        "//src/codegen:codegen",
        "//src/lexer:lexer",
        "//src/parser:parser",
    ],
    args = [
        "--seed=42",
        "--iterations=1000",
        "--max-height=30"
    ],
)
