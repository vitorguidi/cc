load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "tacky",
    srcs = ["tacky.cc"],
    hdrs = ["tacky.h"],
    deps = [
        "//src/ast:ast",
    ],
    visibility = ["//visibility:public"],
)