#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_
#include <atomic>
#include <chrono>
#include <memory>

#include <random>
#include <string>
#include <thread>
#include <vector>
#include "proto/spider.pb.h"
class TaskInfo;
class TaskManager;
class FetchClient;
class FetcherManager;
class Dispatcher {
public:
    Dispatcher(const std::shared_ptr<TaskManager>& task_manager,
               const std::shared_ptr<FetcherManager>& fetch_manager);
    ~Dispatcher();
    spiderproto::CrawlingTask CombineCrawlingTask(
        const std::shared_ptr<TaskInfo>& taskinfo,
        const std::string& fetcher_name,
        std::vector<spiderproto::CrawlUrl>& urls);
    void Start();
    void Join();
    void Stop();
    std::vector<std::vector<spiderproto::CrawlUrl>> SpilitTask(
        const std::shared_ptr<TaskInfo>& taskinfo, int client_count,
        int url_count);

    bool Distribute(const std::shared_ptr<TaskInfo>& taskinfo);

    bool HaveSurviveFetcher();
    void ControlSpeed(int* send_url_count,
                      const std::shared_ptr<TaskInfo>& taskinfo);
    bool ShouldDispatch();

    void MaxSpeed(int* send_url_count, int);

private:
    void DispatchInternal();

private:
    const std::shared_ptr<TaskManager>& m_task_manager;
    const std::shared_ptr<FetcherManager>& m_fetcher_manager;
    std::unique_ptr<std::thread> m_dispatch_thd;
    std::atomic<bool> m_stop;
    uint64_t m_seq;
    std::mt19937 m_g;  // shuffle device
};

#endif  // _DISPATCHER_H_
