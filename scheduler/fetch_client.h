#ifndef _FETCH_CLIENT_H_
#define _FETCH_CLIENT_H_
#include <grpcpp/grpcpp.h>
#include <atomic>
#include <iostream>
#include <memory>
#include <string>
#include "proto/spider.grpc.pb.h"
#include "proto/spider.pb.h"

class FetchClient {
public:
    FetchClient(const std::string& name, const std::string& addr,
                std::shared_ptr<grpc::Channel> channel);

    std::string add_crawlingtask(
        const spiderproto::CrawlingTask& crawling_task);

    std::string GetName();
    std::string GetAddr();

private:
    const std::string m_name;
    const std::string m_addr;
    std::unique_ptr<spiderproto::Fetch::Stub> m_stub;
};

#endif  // _FETCH_CLIENT_H_
