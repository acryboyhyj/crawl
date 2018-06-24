#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_
#include <atomic>
#include <memory>
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
               const std::shared_ptr<FetcherManager> fetch_manager);
    ~Dispatcher();
    spiderproto::CrawlingTask CombineCrawlingTask(
        const TaskInfo& taskinfo, const std::string fetcher_name,
        std::vector<spiderproto::CrawlUrl> urls);
    void Start();
    void Join();
    void Stop();

private:
    void DispatchInternal();

private:
    const std::shared_ptr<TaskManager> m_task_manager;
    const std::shared_ptr<FetcherManager> m_fetcher_manager;
    std::unique_ptr<std::thread> m_dispatch_thd;
    std::atomic<bool> m_stop;
};

#endif  // _DISPATCHER_H_
