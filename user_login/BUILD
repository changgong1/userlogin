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
    srcs = ["//proto:helloworld.grpc.pb.cc"],
    hdrs = ["//proto:elloworld.grpc.pb.h"],
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
        "//proto:proto_helloworld"
    ],
)

cc_binary(
    name = "helloworld_client",
    srcs = ["helloworld_client.cc"],
    deps = [
        "//proto:proto_helloworld"
    ],
)

# userService

cc_library(
    name = "mysqlDriver",
     srcs = ["libmysqlclient.so","libmysqlclient_r.so"],
    hdrs = glob(["include/**/*.h"]),
)

cc_library(
    name = "mysqlpool",
    srcs = ["mysqlpool.cc"],
    hdrs = ["mysqlpool.h"],
    deps = [
        ":mysqlDriver",
    ],
    copts = ["-Isrc/include"],
)


cc_library(
    name = "hiredisDriver",
    srcs = ["libhiredis.so","libhiredis.a"],
    hdrs = glob(["include/**/*.h"]),
)

cc_library(
    name = "redispool",
    srcs = ["redispool.cc"],
    hdrs = ["redispool.h"],
    deps = [
        ":hiredisDriver",
    ],
    #copts = ["-Isrc/include"],
)

cc_binary(
    name = "userlogin_service",
    srcs = ["userlogin_service.cc"],
    deps = [
        ":mysqlpool",
        "redispool",
        "//proto:proto_user_login",
    ],
)