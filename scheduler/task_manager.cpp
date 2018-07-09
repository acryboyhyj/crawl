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
    LOG(INFO) << "add task from db success" << std::endl;
    return true;
}

std::string TaskManager::AddTask(const spiderproto::BasicTask& btask) {
    std::lock_guard<std::mutex> lock(m_mutex);

    spiderproto::BasicTask temp_task;
    temp_task.CopyFrom(btask);

    temp_task.set_taskid(GenerateTaskid());
    std::shared_ptr<TaskInfo> p(new TaskInfo(temp_task));
    m_taskinfos[temp_task.taskid()] = p;

    LOG(INFO) << "addtask sucess, the taskid is" << temp_task.taskid();

    return temp_task.taskid();
}

std::shared_ptr<TaskInfo> TaskManager::FindTask(const std::string& taskid) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto search = m_taskinfos.find(taskid);
    if (search == m_taskinfos.end()) LOG(WARNING) << "not find this task";
    return search->second;
}

std::shared_ptr<TaskInfo> TaskManager::GetOptTask() {
    std::unique_lock<std::mutex> lock(m_mutex);
    // find min seq's TaskInfo
    uint64_t min_seq{UINT64_MAX};
    std::shared_ptr<TaskInfo> target_taskinfo;
    for (const auto& p : m_taskinfos) {
        if (p.second->GetSeq() < min_seq) {
            target_taskinfo = p.second;
            min_seq         = p.second->GetSeq();
        }
    }
    return target_taskinfo;
}
void TaskManager::AddCrawlUrl(const std::string& taskid,
                              const spiderproto::CrawlUrl& crawlurl) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_taskinfos[taskid]->AddCrawlUrl(crawlurl))
        LOG(INFO) << "add CrawlUrl success" << std::endl;
    else
        LOG(INFO) << "add CrawlUrl failed" << std::endl;
}

std::string TaskManager::UpdateTask(const spiderproto::BasicTask& btask) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_taskinfos[btask.taskid()]->Update(btask);
        LOG(INFO) << "Updateask sucess, the taskid is" << btask.taskid();
    }
    return btask.taskid();
}

bool TaskManager::Exist(const std::string& taskid) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_taskinfos.find(taskid) != m_taskinfos.end()) {
        return true;
    } else {
        return false;
    }
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
    if (std::strftime(timestr, sizeof(timestr), "%Y%m%d%H%M%S",
                      std::localtime(&now_time))) {
        LOG(INFO) << "generate a taskid: " << std::string(timestr);
        return std::string(timestr);
    }
    return "";
}
