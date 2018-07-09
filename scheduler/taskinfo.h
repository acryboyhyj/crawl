#ifndef _TASKINFO_H_
#define _TASKINFO_H_
#include <atomic>
#include <chrono>
#include <mutex>
#include <utility>
#include <vector>
#include "proto/spider.pb.h"
class TaskInfo {
    friend class MySqlpp;

public:
    ~TaskInfo();
    TaskInfo()                    = delete;
    TaskInfo(const TaskInfo& rhs) = delete;
    TaskInfo& operator=(const TaskInfo& rhs) = delete;
    TaskInfo(TaskInfo&& rhs)                 = delete;
    TaskInfo& operator=(TaskInfo&& rhs) = delete;

    explicit TaskInfo(const spiderproto::BasicTask& btask);

    bool AddCrawlUrl(const spiderproto::CrawlUrl& crawurl);
    void DelCrawledUrl(const spiderproto::CrawlUrl& crawurl);

    bool Update(const spiderproto::BasicTask& btask);
    std::vector<spiderproto::CrawlUrl> GetAllCrawlurl() const;
    std::vector<std::vector<spiderproto::CrawlUrl>> SpilitTask(int client_count,
                                                               int url_count);
    void SetSeq(uint64_t t) noexcept;
    uint64_t GetSeq() const noexcept;

    int GetCrawlurlCount();
    spiderproto::BasicTask GetBasicTask() const;
    void ShowTaskInfo() const;

    // time , speed control
    bool NotCalled();
    std::chrono::steady_clock::time_point GetTime();
    void SetTime();
    double GetDelay();
    int GetCrawlingUrlCount();
    int GetConcurrentCount();

private:
    spiderproto::BasicTask m_btask;
    std::vector<spiderproto::CrawlUrl> m_crawlurls;
    mutable std::mutex m_mutex;
    std::atomic<uint64_t> m_seq;
    std::chrono::steady_clock::time_point m_last_calltime;
};

#endif  // _TASKINFO_H_
