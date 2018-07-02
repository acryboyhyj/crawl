#include "task_handler.h"
#include <mysql++/mysql++.h>
#include "mysqlpp.h"
#include "task_manager.h"
TaskHandler::TaskHandler(const std::shared_ptr<TaskManager>& task_manager,
                         const std::shared_ptr<MySqlpp>& mysqlpp)
    : m_task_manager(task_manager), m_mysqlpp(mysqlpp) {}

TaskHandler::~TaskHandler() {}

bool TaskHandler::AddTask(const spiderproto::BasicTask& btask) {
    if (!m_task_manager->AddTask(btask)) return false;
    if (!m_mysqlpp->InsertTask(btask)) return false;

    return true;
}

bool TaskHandler::FindTask(const std::string& taskid) {
    if (m_task_manager->FindTaskInfo(taskid))
        return true;
    else
        return false;
}

std::string TaskHandler::UpdateTask(const spiderproto::BasicTask& btask) {
    return m_task_manager->UpdateTask(btask);
}
