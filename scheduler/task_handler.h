#ifndef TASK_HANDLER_H
#define TASK_HANDLER_H
#include <memory>
#include <string>
#include "proto/spider.pb.h"
class TaskManager;
class MySqlpp;
class TaskHandler {
public:
    TaskHandler(const std::shared_ptr<TaskManager>& task_manager,
                const std::shared_ptr<MySqlpp>& mysqlpp);
    ~TaskHandler();
    bool AddTask(const spiderproto::BasicTask& task);
    bool Exsit(const std::string& taskid);
    std::string UpdateTask(const spiderproto::BasicTask& task);

private:
    std::shared_ptr<TaskManager> m_task_manager;
    std::shared_ptr<MySqlpp> m_mysqlpp;
};

#endif  // _TASK_HANDLER_H
