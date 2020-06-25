
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "build_stack_rules_proto",
    urls = ["https://github.com/stackb/rules_proto/archive/4c2226458203a9653ae722245cc27e8b07c383f7.tar.gz"],
    strip_prefix = "rules_proto-4c2226458203a9653ae722245cc27e8b07c383f7",
)

load("@build_stack_rules_proto//cpp:deps.bzl", "cpp_grpc_compile")

cpp_grpc_compile()

http_archive(
    name = "com_github_grpc_grpc",
    urls = [
        "https://github.com/grpc/grpc/archive/512ab8679bf60f321ce133db3323307f7b11504c.tar.gz",
    ],
    strip_prefix = "grpc-512ab8679bf60f321ce133db3323307f7b11504c",
)

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()
