#ifndef SCHEDULER_CLIENT_H_
#define SCHEDULER_CLIENT_H_

#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>
#include "proto/spider.grpc.pb.h"
#include "proto/spider.pb.h"
class SchedulerClient {
public:
    SchedulerClient(std::shared_ptr<grpc::Channel> channel)
        : m_stub(spiderproto::Schedule::NewStub(channel)) {}

    std::string add_task(const spiderproto::BasicTask& btask) {
        spiderproto::BasicTask task;
        task.CopyFrom(btask);

        spiderproto::TaskResponse response;

        grpc::ClientContext context;

        grpc::Status status = m_stub->add_task(&context, task, &response);

        if (status.ok()) {
            std::cout << "Rpc success :" << std::endl;
            return response.taskid();

        } else {
            std::cout << status.error_code() << ":" << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<spiderproto::Schedule::Stub> m_stub;
};

#endif  // SCHEDULER_CLIENT_H_
