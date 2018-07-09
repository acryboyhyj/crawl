#include "taskinfo.h"
#include <glog/logging.h>
TaskInfo::TaskInfo(const spiderproto::BasicTask& btask)
    : m_btask(btask), m_seq(0), m_last_calltime() {
    int crawlurl_count = btask.crawl_list().crawl_urls_size();
    for (int i = 0; i < crawlurl_count; ++i) {
        m_crawlurls.push_back(btask.crawl_list().crawl_urls(i));
    }
}
TaskInfo::~TaskInfo() {}

bool TaskInfo::AddCrawlUrl(const spiderproto::CrawlUrl& crawlurl) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_crawlurls.push_back(crawlurl);
        spiderproto::CrawlUrlList* crawl_list = m_btask.mutable_crawl_list();
        spiderproto::CrawlUrl* temp_crawlurl  = crawl_list->add_crawl_urls();
        temp_crawlurl->CopyFrom(crawlurl);
    }
    return true;
}

void TaskInfo::DelCrawledUrl(const spiderproto::CrawlUrl& crawurl) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (std::vector<spiderproto::CrawlUrl>::iterator it =
                 m_crawlurls.begin();
             it != m_crawlurls.end(); ++it) {
            if (crawurl.url() == it->url()) {
                m_crawlurls.erase(it);
                break;
            }
        }
    }
    return;
}
void TaskInfo::ShowTaskInfo() const {
    LOG(INFO) << "Taskinfo'btask : " << m_btask.DebugString() << std::endl;

    for (size_t i = 0; i < m_crawlurls.size(); ++i) {
        LOG(INFO) << "CrawUrl : " << std::endl;
        LOG(INFO) << "url: " << m_crawlurls[i].url();
    }
}

bool TaskInfo::Update(const spiderproto::BasicTask& btask) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_btask.CopyFrom(btask);

        m_crawlurls.clear();
        int crawlurl_count = btask.crawl_list().crawl_urls_size();
        for (int i = 0; i < crawlurl_count; ++i) {
            m_crawlurls.push_back(btask.crawl_list().crawl_urls(i));
        }
    }
    return true;
}

spiderproto::BasicTask TaskInfo::GetBasicTask() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_btask;
}

std::vector<spiderproto::CrawlUrl> TaskInfo::GetAllCrawlurl() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<spiderproto::CrawlUrl> result{};
    for (const auto& url : m_crawlurls) {
        if (url.usedable() == false) {
            result.push_back(url);
        }
    }
    return result;
}

std::vector<std::vector<spiderproto::CrawlUrl>> TaskInfo::SpilitTask(
    int client_count, int url_count) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::vector<std::vector<spiderproto::CrawlUrl>> result{};
    if (url_count == 0) return result;
    int every_url_count = url_count / client_count + 1;
    std::vector<spiderproto::CrawlUrl> temp{};
    int count = 0;
    for (size_t i = 0; i < m_crawlurls.size(); i++) {
        LOG(INFO) << m_crawlurls[i].url();
        m_crawlurls[i].set_usedable(true);
        temp.push_back(m_crawlurls[i]);
        count++;
        if (count == every_url_count) {
            for (const auto& c : temp) {
                LOG(INFO) << "temp:" << c.url();
            }
            result.push_back(temp);
            temp.clear();
            count = 0;
        }
    }
    result.push_back(temp);
    return result;
}

int TaskInfo::GetCrawlurlCount() {
    std::lock_guard<std::mutex> lock(m_mutex);
    int count = 0;
    for (const auto& c : m_crawlurls) {
        if (c.usedable() == false) {
            count++;
        }
    }
    return count;
}

uint64_t TaskInfo::GetSeq() const noexcept {
    return m_seq.load(std::memory_order_relaxed);
}

void TaskInfo::SetSeq(uint64_t t) noexcept {
    m_seq.store(t, std::memory_order_relaxed);
}

bool TaskInfo::NotCalled() {
    std::chrono::steady_clock::time_point t;
    if (m_last_calltime == t)
        return true;
    else
        return false;
}

std::chrono::steady_clock::time_point TaskInfo::GetTime() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_last_calltime;
}

void TaskInfo::SetTime() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_last_calltime = std::chrono::steady_clock::now();
}

double TaskInfo::GetDelay() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_btask.runtime().download_delay();
}

int TaskInfo::GetCrawlingUrlCount() {
    std::lock_guard<std::mutex> lock(m_mutex);
    int count = 0;
    for (const auto& c : m_crawlurls) {
        if (c.usedable() == true) {
            count++;
        }
    }
    return count;
}

int TaskInfo::GetConcurrentCount() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_btask.runtime().concurrent_reqs();
}
