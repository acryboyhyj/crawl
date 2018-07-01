#include "task_manager.h"
#include <glog/logging.h>
#include <ctime>
#include <iostream>
#include "taskinfo.h"
TaskManager::TaskManager() {}

TaskManager::~TaskManager() {}

bool TaskManager::AddTask(const std::vector<spiderproto::BasicTask>& btasks) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& btask : btasks) {
            std::shared_ptr<TaskInfo> temp(new TaskInfo(btask));
            m_taskinfos[btask.taskid()] = temp;
        }
    }
    std::cout << "addtask from db" << std::endl;
    LOG(INFO) << "add task from db success" << std::endl;
    return true;
}

bool TaskManager::AddTask(const spiderproto::BasicTask& btask) {
    std::lock_guard<std::mutex> lock(m_mutex);

    spiderproto::BasicTask temp_task;
    temp_task.CopyFrom(btask);

    temp_task.set_taskid(GenerateTaskid());

    std::shared_ptr<TaskInfo> p(new TaskInfo(temp_task));
    m_taskinfos[temp_task.taskid()] = p;
    LOG(INFO) << "addtask sucess, the taskid is" << temp_task.taskid();

    return true;
}

void TaskManager::ShowTasks() const {
    for (const auto& task : m_taskinfos) {
        LOG(INFO) << "taskid : " << task.first << std::endl << " taskinfo : ";
        task.second->ShowTaskInfo();
        std::cout << std::endl;
    }
}

std::string TaskManager::GenerateTaskid() {
    std::time_t now_time = std::time(nullptr);
    char timestr[100];
    if (std::strftime(timestr, sizeof(timestr), "%Y%m%d",
                      std::localtime(&now_time))) {
        return std::string(timestr);
    }
    return "";
}

std::string TaskManager::UpdateTask(const spiderproto::BasicTask& btask) {
    m_taskinfos[btask.taskid()]->Update(btask);
    LOG(INFO) << "addtask sucess, the taskid is" << btask.taskid();
    return btask.taskid();
}

bool TaskManager::FindTaskInfo(const std::string& taskid) {
    if (m_taskinfos.find(taskid) != m_taskinfos.end()) {
        return true;
    } else {
        return false;
    }
}

void TaskManager::AddCrawlUrl(const std::string& taskid,
                              const spiderproto::CrawlUrl& crawlurl) {
    if (m_taskinfos[taskid]->AddCrawlUrl(crawlurl))
        LOG(INFO) << "add CrawlUrl success" << std::endl;
    else
        LOG(INFO) << "add CrawlUrl failed" << std::endl;
}

std::shared_ptr<TaskInfo> TaskManager::GetTaskInfo() {
    LOG(INFO) << "GetTaskInfo" << std::endl;
    for (auto& taskinfo : m_taskinfos) {
        taskinfo.second->ShowTaskInfo();
        return taskinfo.second;
    }
}
