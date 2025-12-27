load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "codegen",
    srcs = [
        "c_ast_to_asm/c_ast_to_asm_visitor.cc",
        "c_ast_to_tacky/c_ast_to_tacky_visitor.cc",
        "tacky_to_asm/tacky_to_asm_visitor.cc"
    ],
    hdrs = [
        "c_ast_to_asm/c_ast_to_asm.h",
        "c_ast_to_tacky/c_ast_to_tacky.h",
        "tacky_to_asm/tacky_to_asm.h"
    ],
    deps = [
        "//src/ast:ast",
        "//src/tacky:tacky",
        "//src/asm:asm",
    ],
    visibility = ["//visibility:public"],
)