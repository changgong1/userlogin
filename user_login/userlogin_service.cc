#include <iostream>
#include <memory>
#include <string>
#include "mysqlpool.h"
#include "redispool.h"
 
#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "proto/user_login.grpc.pb.h"
#else
#include "proto/user_login.grpc.pb.h"
#endif

#define  SERVER_ADDRESS "0.0.0.0:8080"

#define  MYSQL_ADDRESS "127.0.0.1"
#define  MYSQL_USRNAME "root"
#define  MYSQL_PORT 3306
#define  MYSQL_USRPASSWORD "123456"
#define  MYSQL_USEDB "user_login"
#define  MYSQL_MAX_CONNECTCOUNT 30

#define  REDIS_ADDRESS "127.0.0.1"
#define  REDIS_PORT 6379
#define  REDIS_MAX_CONNECTCOUNT 500
 
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using userservice::LoginRequest;
using userservice::TokenReply;
using userservice::TokenCheckRequest;
using userservice::TokenCheckReply;
using userservice::UserLoginService;

// Logic and data behind the server's behavior.
class UserLoginServiceImpl final : public UserLoginService::Service {
    Status UserRegister(ServerContext* context, const LoginRequest* request,
                    TokenReply* reply) override {
        std::cout << "Received:" << request->userid() << std::endl;
        std::vector<stUserLogin> p_userInfo;
        bool flag = mysql->GetUserInfoByUserId(request->userid().c_str(), p_userInfo);
        if (!flag) {
            return Status::CANCELLED;
        }
        reply->set_token(request->userid());
        return Status::OK;
    }

public:
    void initData(MysqlPool* m,RedisPool* r){
        mysql = m;
        redis = r;
    }
private:
    MysqlPool* mysql;
    RedisPool* redis;
};

void RunServer(MysqlPool* mysql,RedisPool* redis) {
    std::string server_address("0.0.0.0:50051");
    UserLoginServiceImpl service;
    service.initData(mysql, redis);

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
} 

int main(int argc, char** argv) {
    //初始化数据库
    MysqlPool *mysql = MysqlPool::getMysqlPoolObject();
    mysql->setParameter(MYSQL_ADDRESS,MYSQL_USRNAME,MYSQL_USRPASSWORD,MYSQL_USEDB,MYSQL_PORT,NULL,0,MYSQL_MAX_CONNECTCOUNT);

    RedisPool *redis = RedisPool::getRedisPoolObject();
    redis->setParameter(REDIS_ADDRESS,REDIS_PORT,NULL,0,REDIS_MAX_CONNECTCOUNT);
    RunServer(mysql, redis);

    delete mysql;
    delete redis;

    return 0;
}