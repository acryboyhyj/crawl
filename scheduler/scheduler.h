#ifndef _SCHEDULER_H_
#define _SCHEDULET_H_

#include <memory>
#include "concurrent_queue.h"
#include "proto/spider.pb.h"
class MySqlpp;
class TaskManager;
class FetcherManager;
class TaskHandler;
class ScheduleServer;
class Dispatcher;
class CrawledtaskHandler;
class Scheduler {
public:
    Scheduler();
    ~Scheduler();
    bool Start();
    bool Load();
    void Dispatch();
    void Join();

private:
    std::shared_ptr<TaskManager> m_task_manager;
    std::shared_ptr<FetcherManager> m_fetcher_manager;
    std::shared_ptr<ConcurrentQueue<spiderproto::CrawledTask>>
        m_concurrent_queue;
    std::shared_ptr<MySqlpp> m_mysqlpp;
    std::shared_ptr<TaskHandler> m_task_handler;
    std::shared_ptr<CrawledtaskHandler> m_crawledtask_handler;
    std::unique_ptr<ScheduleServer> m_schedule_server;
    std::unique_ptr<Dispatcher> m_dispatcher;
};

#endif  //_SCHEDLER_H_
