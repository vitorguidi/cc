load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "codegen",
    srcs = [
        "tacky/c_to_tacky_visitor.cc",
        "x86_64_asm/tacky_to_asm_visitor.cc",
        "x86_64_asm/pseudo_replacer_visitor.cc",
        "x86_64_asm/instruction_fixup_visitor.cc",
        "x86_64_asm/asm_rewrite_visitor.cc",
        "x86_64_asm/asm_dump.cc",
    ],
    hdrs = [
        "tacky/c_to_tacky.h",
        "x86_64_asm/tacky_to_asm.h",
        "x86_64_asm/asm_dump.h",
    ],
    deps = [
        "//src/ast:ast",
    ],
    visibility = ["//visibility:public"],
)