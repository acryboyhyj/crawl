#include "task_handler.h"
#include <mysql++/mysql++.h>
#include "mysqlpp.h"
#include "task_manager.h"
TaskHandler::TaskHandler(const std::shared_ptr<TaskManager>& task_manager,
                         const std::shared_ptr<MySqlpp>& mysqlpp)
    : m_task_manager(task_manager), m_mysqlpp(mysqlpp) {}

TaskHandler::~TaskHandler() {}

bool TaskHandler::AddTask(const spiderproto::BasicTask& btask) {
    const std::string newtaskid = m_task_manager->AddTask(btask);
    if (newtaskid.empty()) return false;
    if (!m_mysqlpp->InsertTask(btask, newtaskid)) return false;

    return true;
}

bool TaskHandler::Exsit(const std::string& taskid) {
    if (m_task_manager->Exist(taskid))
        return true;
    else
        return false;
}

void TaskHandler::UpdateTask(const spiderproto::BasicTask& btask) {
    m_task_manager->UpdateTask(btask);
    m_mysqlpp->UpdateTask(btask);
}
