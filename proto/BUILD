load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "proto_helloworld",
    srcs = ["helloworld.grpc.pb.cc"],
    hdrs = ["helloworld.grpc.pb.h"],
    visibility = ["//user_login:__pkg__"],
)

#proto_library(
#    name = "helloworld_proto",
#    srcs = [
#        "helloworld.proto",
#    ],
#    visibility = [
#        "//visibility:public",
#    ],
#)