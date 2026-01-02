load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "codegen",
    srcs = [
        "c_ast_to_tacky/c_ast_to_tacky_visitor.cc",
        "tacky_to_asm/tacky_to_asm_visitor.cc",
        "tacky_to_asm/pseudo_replacer_visitor.cc",
        "tacky_to_asm/instruction_fixup_visitor.cc",
        "tacky_to_asm/asm_rewrite_visitor.cc",
        "asm_dump/asm_dump.cc",
    ],
    hdrs = [
        "c_ast_to_tacky/c_ast_to_tacky.h",
        "tacky_to_asm/tacky_to_asm.h",
        "asm_dump/asm_dump.h",
    ],
    deps = [
        "//src/ast:ast",
        "//src/tacky:tacky",
        "//src/asm:asm",
    ],
    visibility = ["//visibility:public"],
)