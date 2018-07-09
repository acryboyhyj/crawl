#ifndef _TASK_MANAGER_H_
#define _TASK_MANAGER_H_
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "proto/spider.pb.h"

class TaskInfo;

class TaskManager {
public:
    TaskManager();
    ~TaskManager();

    bool AddTask(const std::vector<spiderproto::BasicTask>& btasks);
    std::string AddTask(const spiderproto::BasicTask& btask);

    bool Exist(const std::string& taskid);
    std::shared_ptr<TaskInfo> FindTask(const std::string& taskid);
    std::string UpdateTask(const spiderproto::BasicTask& btask);

    void AddCrawlUrl(const std::string& taskid,
                     const spiderproto::CrawlUrl& crawlurl);

    std::shared_ptr<TaskInfo> GetOptTask();
    void ShowTasks() const;

private:
    std::string GenerateTaskid();

private:
    std::map<std::string, std::shared_ptr<TaskInfo>> m_taskinfos;
    std::mutex m_mutex;
};

#endif  // _TASK_MANAGER_H_
