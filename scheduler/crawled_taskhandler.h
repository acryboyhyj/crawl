#ifndef CRAWLED_TASKHANDLER_H
#define CRAWLED_TASKHANDLER_H
#include <memory>
#include "concurrent_queue.h"
#include "proto/spider.pb.h"
class TaskManager;
class MySqlpp;
class BloomFilter;
class CrawledtaskHandler {
    CrawledtaskHandler(
        const std::shared_ptr<TaskManager>& task_manager,
        const std::shared_ptr<MySqlpp> mysqlpp,
        const std::shared_ptr<ConcurrentQueue<spiderproto::CrawledTask>>
            concurrent_queue);

    ~CrawledtaskHandler();

private:
    std::shared_ptr<TaskManager> m_task_manager;
    std::shared_ptr<MySqlpp> m_sqlpp;
    std::shared_ptr<ConcurrentQueue<spiderproto::CrawledTask>>
        m_concurrent_queue;
};

#endif  // _CRAWLER_TASKHANDLER_H_
