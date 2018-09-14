#include "scheduler.h"
#include <glog/logging.h>
#include <mysql++/mysql++.h>
#include <string>
#include <utility>
#include <vector>
#include "bloomfilter.h"
#include "concurrent_queue.h"
#include "crawled_taskhandler.h"
#include "dispatcher.h"
#include "fetcher_manager.h"
#include "mysqlpp.h"
#include "schedule_server.h"
#include "task_handler.h"
#include "task_manager.h"
Scheduler::Scheduler()
    : m_task_manager(new TaskManager()),
      m_fetcher_manager(new FetcherManager()),
      m_concurrent_queue(new ConcurrentQueue<spiderproto::CrawledTask>()),
      m_mysqlpp(new MySqlpp()),

      m_bloomfilter(new BloomFilter<std::string>(1000000)),
      m_task_handler(new TaskHandler(m_task_manager, m_mysqlpp)),
      m_crawledtask_handler(new CrawledtaskHandler(
          m_task_manager, m_mysqlpp, m_concurrent_queue, m_bloomfilter)),
      m_schedule_server(new ScheduleServer(m_fetcher_manager, m_task_handler,
                                           m_concurrent_queue)),

      m_dispatcher(new Dispatcher(m_task_manager, m_fetcher_manager)) {}

Scheduler::~Scheduler() {}

void Scheduler::LoadFilter(std::vector<spiderproto::BasicTask>* btasks) {
    if (btasks == nullptr) {
        return;
    } else {
        for (size_t i = 0; i < btasks->size(); ++i) {
            spiderproto::CrawlUrlList crawl_list = (*btasks)[i].crawl_list();
            int crawlurl_count                   = crawl_list.crawl_urls_size();
            for (int i = 0; i < crawlurl_count; ++i) {
                std::string url = crawl_list.crawl_urls(i).url();
                if (!m_bloomfilter->contains(url)) {
                    m_bloomfilter->insert(url);
                }
            }
        }
    }
}
bool Scheduler::Load() {
    std::vector<spiderproto::BasicTask> btasks = m_mysqlpp->QueryAllTblTask();
    if (!m_mysqlpp->QueryAllTblLink(&btasks)) {
        LOG(WARNING) << "query link is failed" << std::endl;
    }

    LoadFilter(&btasks);
    m_task_manager->AddTask(btasks);
    return false;
}

bool Scheduler::Start() {
    LOG(INFO) << "schedule server and dispathcer will start" << std::endl;

    m_schedule_server->Start();
    m_dispatcher->Start();
    m_crawledtask_handler->Start();
    return true;
}

void Scheduler::Join() {
    m_schedule_server->Join();
    m_dispatcher->Join();
    m_crawledtask_handler->Join();
}
