#ifndef HANDLER_CLIENT_H_
#define HANDLER_CLIENT_H_

#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>
#include "proto/spider.grpc.pb.h"
#include "proto/spider.pb.h"
class HandlerClient {
public:
    explicit HandlerClient(std::shared_ptr<grpc::Channel> channel)
        : m_stub(spiderproto::Handle::NewStub(channel)) {}

    std::string add_crawldoc(const spiderproto::CrawlDoc& crawldoc) {
        spiderproto::CrawlDoc doc;
        doc.CopyFrom(crawldoc);

        spiderproto::TaskResponse response;

        grpc::ClientContext context;

        grpc::Status status = m_stub->add_crawldoc(&context, doc, &response);

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
    std::unique_ptr<spiderproto::Handle::Stub> m_stub;
};

#endif
