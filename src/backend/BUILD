load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "backend",
    srcs = [
        "x86_64_asm/tacky_to_asm_visitor.cc",
        "x86_64_asm/pseudo_replacer_visitor.cc",
        "x86_64_asm/instruction_fixup_visitor.cc",
        "x86_64_asm/asm_rewrite_visitor.cc",
        "x86_64_asm/asm_dump.cc",
    ],
    hdrs = [
        "x86_64_asm/tacky_to_asm.h",
        "x86_64_asm/asm_dump.h",
    ],
    deps = [
        "//src/ast:ast",
    ],
    visibility = ["//visibility:public"],
)