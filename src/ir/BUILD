load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "ir",
    srcs = [
        "tacky/c_to_tacky.cc",
    ],
    hdrs = [
        "tacky/c_to_tacky.h",
    ],
    deps = [
        "//src/ast:ast",
    ],
    visibility = ["//visibility:public"],
)