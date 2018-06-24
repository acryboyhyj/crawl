#ifndef _SCHEDULER_H_
#define _SCHEDULET_H_

#include <memory>

class MySqlpp;
class TaskManager;
class ScheduleServer;
class Dispatcher;
class FetcherManager;
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
    std::unique_ptr<MySqlpp> m_mysqlpp;
    std::unique_ptr<ScheduleServer> m_schedule_server;
    std::unique_ptr<Dispatcher> m_dispatcher;
};

#endif  //_SCHEDLER_H_
