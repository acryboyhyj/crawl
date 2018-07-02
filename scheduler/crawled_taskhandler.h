#ifndef CRAWLED_TASKHANDLER_H
#define CRAWLED_TASKHANDLER_H
#include <atomic>
#include <memory>
#include <thread>
#include "concurrent_queue.h"
#include "proto/spider.pb.h"
class TaskManager;
class MySqlpp;
class BloomFilter;
class CrawledtaskHandler {
public:
    CrawledtaskHandler(
        const std::shared_ptr<TaskManager>& task_manager,
        const std::shared_ptr<MySqlpp> mysqlpp,
        const std::shared_ptr<ConcurrentQueue<spiderproto::CrawledTask>>
            concurrent_queue);

    ~CrawledtaskHandler();

    void AddCrawledTask();
    void Start();
    void Join();
    void Stop();

private:
    std::shared_ptr<TaskManager> m_task_manager;
    std::shared_ptr<MySqlpp> m_mysqlpp;
    std::shared_ptr<ConcurrentQueue<spiderproto::CrawledTask>>
        m_concurrent_queue;
    std::unique_ptr<std::thread> m_thread;
    std::atomic<bool> m_stop;
    std::unique_ptr<BloomFilter> m_bf;
};

#endif  // _CRAWLER_TASKHANDLER_H_
