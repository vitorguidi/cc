load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "codegen",
    srcs = [
        "ast_to_asm_visitor.cc",
        "ast_to_tacky_visitor.cc",
        "tacky_to_asm_visitor.cc"
    ],
    hdrs = [
        "ast_to_asm_visitor.h",
        "ast_to_tacky_visitor.h",
        "tacky_to_asm_visitor.h"
    ],
    deps = [
        "//src/ast:ast",
        "//src/tacky:tacky",
        "//src/asm:asm",
    ],
    visibility = ["//visibility:public"],
)