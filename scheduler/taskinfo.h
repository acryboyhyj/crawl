#ifndef _TASKINFO_H_
#define _TASKINFO_H_
#include <utility>
#include <vector>
#include "proto/spider.pb.h"
class TaskInfo {
    friend class MySqlpp;

public:
    ~TaskInfo() {}

    TaskInfo()                    = delete;
    TaskInfo(const TaskInfo& rhs) = delete;
    TaskInfo& operator=(const TaskInfo& rhs) = delete;
    TaskInfo(TaskInfo&& rhs)                 = delete;
    TaskInfo& operator=(TaskInfo&& rhs) = delete;

    explicit TaskInfo(const spiderproto::BasicTask& btask);

    bool AddCrawlUrl(const spiderproto::CrawlUrl& crawurl);
    void ShowTaskInfo() const;
    bool Update(const spiderproto::BasicTask& btask);
    spiderproto::BasicTask GetBasicTask() const;
    std::vector<spiderproto::CrawlUrl> GetAllCrawlurl() const;

    std::vector<std::vector<spiderproto::CrawlUrl>> SpilitTask(int client_count,
                                                               int url_count);

    int GetCrawlurlCount();

private:
    spiderproto::BasicTask m_btask;
    std::vector<spiderproto::CrawlUrl> m_crawlurls;
};

#endif  // _TASKINF0_H_
