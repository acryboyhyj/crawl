#ifndef _TASK_MANAGER_H_
#define _TASK_MANAGER_H_
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
    void ShowTasks() const;
    std::string AddTask(const spiderproto::BasicTask& btask);

    bool FindTaskInfo(const std::string& taskid);
    std::string UpdateTask(const spiderproto::BasicTask& btask);

    void AddCrawlUrl(const std::string& taskid,
                     const spiderproto::CrawlUrl& crawlurl);

    std::shared_ptr<TaskInfo> GetTaskInfo();

private:
    std::string GenerateTaskid();

private:
    std::map<std::string, std::shared_ptr<TaskInfo>> m_taskinfos;
    std::mutex m_mutex;
};

#endif  // _TASK_MANAGER_H_
