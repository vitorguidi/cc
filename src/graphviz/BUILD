load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "graphviz",
    srcs = ["c_ast.cc", "common.cc", "tacky.cc", "asm.cc"],
    hdrs = ["graphviz.h"],
    deps = [
        "//src/ast:ast",
        "//src/tacky:tacky",
        "//src/asm:asm"
    ],
    visibility = ["//visibility:public"],
)