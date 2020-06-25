#include <iostream>
#include <memory>
#include <string>
 
#include <grpcpp/grpcpp.h>
 
#ifdef BAZEL_BUILD
#include "proto/helloworld.grpc.pb.h"
#else
#include "proto/helloworld.grpc.pb.h"
#endif
 
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;
 
class GreeterClient {
public:
    GreeterClient(std::shared_ptr<Channel> channel)
    : stub_(Greeter::NewStub(channel)) {}
 
    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);
 
    // Container for the data we expect from the server.
    HelloReply reply;
 
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;
 
    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);
 
    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
    }
 
private:
    std::unique_ptr<Greeter::Stub> stub_;
};
 
int main(int argc, char** argv) {
    GreeterClient greeter(grpc::CreateChannel(
        "127.0.0.1:50051", grpc::InsecureChannelCredentials()));
    std::string user("world");
    std::string reply = greeter.SayHello(user);
    std::cout << "Greeter received: " << reply << std::endl;

    return 0;
}
