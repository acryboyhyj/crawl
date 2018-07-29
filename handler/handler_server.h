#ifndef _HANDLER_SEVER_H
#define _HANDLER_SEVER_H

#include <grpcpp/grpcpp.h>
#include <atomic>
#include <memory>
#include <thread>
#include "concurrent_queue.h"
#include "proto/spider.grpc.pb.h"
#include "proto/spider.pb.h"
class HandleServer final : public spiderproto::Handle::Service {
public:
    explicit HandleServer(
        const std::shared_ptr<ConcurrentQueue<spiderproto::CrawlDoc>>&
            concurrent_queue);
    ~HandleServer();

    grpc::Status add_crawldoc(grpc::ServerContext* context,
                              const spiderproto::CrawlDoc* crawldoc,
                              spiderproto::TaskResponse* response) override;

    void RunServer();
    void Start();
    void Join();
    void Stop();

private:
    const std::shared_ptr<ConcurrentQueue<spiderproto::CrawlDoc>>
        m_concurrent_queue;
    std::unique_ptr<std::thread> m_rpc_thd;
    std::atomic<bool> m_stop;
};

#endif  // _HANDLER_SEVER_H
