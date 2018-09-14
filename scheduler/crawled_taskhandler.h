#ifndef CRAWLED_TASKHANDLER_H
#define CRAWLED_TASKHANDLER_H
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include "bloomfilter.h"
#include "concurrent_queue.h"
#include "proto/spider.pb.h"
class TaskManager;
class MySqlpp;
class CrawledtaskHandler {
public:
    CrawledtaskHandler(
        const std::shared_ptr<TaskManager>& task_manager,
        const std::shared_ptr<MySqlpp> mysqlpp,
        const std::shared_ptr<ConcurrentQueue<spiderproto::CrawledTask>>
            concurrent_queue,
        const std::shared_ptr<BloomFilter<std::string>> bf);

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

    std::shared_ptr<BloomFilter<std::string>> m_bf;
    std::unique_ptr<std::thread> m_thread;
    std::atomic<bool> m_stop;
};

#endif  // _CRAWLER_TASKHANDLER_H_
