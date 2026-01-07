load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "graphviz",
    srcs = ["c.cc", "common.cc", "tacky.cc", "x86_64_asm.cc"],
    hdrs = ["graphviz.h"],
    deps = [
        "//src/ast:ast",
    ],
    visibility = ["//visibility:public"],
)