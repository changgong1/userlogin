package(default_visibility = ["//visibility:public"])

# load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
# load("//cpp:cpp_grpc_library.bzl", "cpp_grpc_library")
load("@build_stack_rules_proto//cpp:cpp_grpc_library.bzl", "cpp_grpc_library")


cpp_grpc_library(
    name = "cc_hello_proto",
    deps = ["//proto:hello_proto"],
)

cc_library(
    name = "greet",
    srcs = ["greet.cc"],
    hdrs = ["greet.h"],
)

cc_library(
    name = "helloworld_proto",
    srcs = ["//proto:helloworld.grpc.pb.h"],
    hdrs = ["//proto:helloworld.grpc.pb.h"],
)

cc_binary(
    name = "helloworld",
    srcs = ["helloworld.cc"],
    deps = [
        ":greet",
    ],
)


cc_binary(
    name = "helloworld_service",
    srcs = ["helloworld_service.cc"],
    deps = [
        ":cc_hello_proto"
    ],
)

cc_binary(
    name = "helloworld_client",
    srcs = ["helloworld_client.cc"],
    deps = [
        ":cc_hello_proto"
    ],
)