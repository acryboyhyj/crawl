#ifndef _SCHEDULE_SEVER_H_
#define _SCHEDULE_SEVER_H_

#include <grpcpp/grpcpp.h>
#include <atomic>
#include <memory>
#include <thread>
#include "bloomfilter.h"
#include "proto/spider.grpc.pb.h"
#include "proto/spider.pb.h"
class TaskManager;
class FetcherManager;
class ScheduleServer final : public spiderproto::Schedule::Service {
public:
    ScheduleServer(const std::shared_ptr<TaskManager>& task_manager,
                   const std::shared_ptr<FetcherManager>& fetcher_manager);
    ~ScheduleServer();
    grpc::Status add_task(grpc::ServerContext* context,
                          const spiderproto::BasicTask* task,
                          spiderproto::TaskResponse* response) override;

    grpc::Status add_fetcher(grpc::ServerContext* context,
                             const spiderproto::Fetcher* fetcher,
                             spiderproto::TaskResponse* response);
    grpc::Status add_crawledtask(grpc::ServerContext* context,
                                 const spiderproto::CrawledTask* task,
                                 spiderproto::TaskResponse* response);

    void Start();
    void Stop();
    void Join();

    void RunServer();

private:
    std::shared_ptr<TaskManager> m_task_manager;
    std::shared_ptr<FetcherManager> m_fetcher_manager;
    std::unique_ptr<std::thread> m_rpc_thd;
    std::atomic<bool> m_stop;
    std::unique_ptr<BloomFilter> m_bloom_filter;
};

#endif  // SCHEDULE_SERVER_H
