#include "taskinfo.h"
#include <glog/logging.h>
TaskInfo::TaskInfo(const spiderproto::BasicTask& btask) : m_btask(btask) {
    int crawlurl_count = btask.crawl_list().crawl_urls_size();
    for (int i = 0; i < crawlurl_count; ++i) {
        m_crawlurls.push_back(btask.crawl_list().crawl_urls(i));
    }
}

bool TaskInfo::AddCrawlUrl(const spiderproto::CrawlUrl& crawlurl) {
    m_crawlurls.push_back(crawlurl);
    spiderproto::CrawlUrlList* crawl_list = m_btask.mutable_crawl_list();
    spiderproto::CrawlUrl* temp_crawlurl  = crawl_list->add_crawl_urls();
    temp_crawlurl->CopyFrom(crawlurl);
    return true;
}

void TaskInfo::ShowTaskInfo() const {
    LOG(INFO) << "Taskinfo'btask : " << m_btask.DebugString() << std::endl;

    for (size_t i = 0; i < m_crawlurls.size(); ++i) {
        LOG(INFO) << "CrawUrl : " << std::endl;
        LOG(INFO) << "url: " << m_crawlurls[i].url();
        for (int j = 0; j < m_crawlurls[i].url_levels_size(); j++) {
            LOG(INFO) << "  level : " << m_crawlurls[i].url_levels(j);
        }
        LOG(INFO) << std::endl;
    }
}

bool TaskInfo::Update(const spiderproto::BasicTask& btask) {
    m_btask.CopyFrom(btask);

    m_crawlurls.clear();
    int crawlurl_count = btask.crawl_list().crawl_urls_size();
    for (int i = 0; i < crawlurl_count; ++i) {
        m_crawlurls.push_back(btask.crawl_list().crawl_urls(i));
    }

    return true;
}

spiderproto::BasicTask TaskInfo::GetBasicTask() const { return m_btask; }

std::vector<spiderproto::CrawlUrl> TaskInfo::GetAllCrawlurl() const {
    return m_crawlurls;
}

std::vector<std::vector<spiderproto::CrawlUrl>> TaskInfo::SpilitTask(
    int client_count, int url_count) {
    LOG(INFO) << "SplitTask";
    std::vector<std::vector<spiderproto::CrawlUrl>> result{};
    int every_url_count = url_count / client_count + 1;

    LOG(INFO) << "every_url_count " << every_url_count;
    std::vector<spiderproto::CrawlUrl> temp{};
    int count = 0;
    LOG(INFO) << "urlcount" << m_crawlurls.size();
    for (size_t i = 0; i < m_crawlurls.size(); i++) {
        LOG(INFO) << m_crawlurls[i].url();
        temp.push_back(m_crawlurls[i]);
        count++;
        if (count == every_url_count) {
            for (const auto& c : temp) {
                LOG(INFO) << "temp" << c.url();
            }
            result.push_back(temp);
            temp.clear();
            count = 0;
        }
    }
    result.push_back(temp);
    return result;
}

int TaskInfo::GetCrawlurlCount() { return m_crawlurls.size(); }
