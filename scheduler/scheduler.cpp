#include "scheduler.h"
#include <glog/logging.h>
#include <mysql++/mysql++.h>
#include <string>
#include <utility>
#include <vector>
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
      m_task_handler(new TaskHandler(m_task_manager, m_mysqlpp)),
      m_crawledtask_handler(new CrawledtaskHandler(m_task_manager, m_mysqlpp,
                                                   m_concurrent_queue)),
      m_schedule_server(new ScheduleServer(m_fetcher_manager, m_task_handler,
                                           m_concurrent_queue)),

      m_dispatcher(new Dispatcher(m_task_manager, m_fetcher_manager)) {}

Scheduler::~Scheduler() {}

bool Scheduler::Load() {
    std::vector<spiderproto::BasicTask> btasks = m_mysqlpp->QueryAllTblTask();
    if (!m_mysqlpp->QueryAllTblLink(&btasks)) {
        LOG(WARNING) << "query link is failed" << std::endl;
    }
    m_task_manager->AddTask(btasks);
    std::vector<spiderproto::Fetcher> fetchers = m_mysqlpp->QueryAllFetchers();
    m_fetcher_manager->AddFetcher(fetchers);

    m_fetcher_manager->ShowFetcher();
    return false;
}

bool Scheduler::Start() {
    LOG(INFO) << "schedule server and dispathcer will start" << std::endl;
    m_dispatcher->Start();

    m_schedule_server->Start();

    return true;
}

void Scheduler::Join() {
    m_schedule_server->Join();
    m_dispatcher->Join();
}
/*void Scheduler::Dispatch() {
    spiderproto::CrawlingTask crawling_task;
    crawling_task.set_taskid("00002");
    crawling_task.set_fetcher("fetcher1");
    spiderproto::CrawlUrl* crawlurl = crawling_task.add_crawl_urls();
    crawlurl->set_url("http://www.hdu.edu.cn/news/important");
    crawlurl->add_url_levels(spiderproto::LEVEL_INDEX);

    spiderproto::LinkRule* linkrule = crawling_task.add_rules();
    linkrule->set_in_level(spiderproto::LEVEL_LIST);
    std::string response = m_dispatcher->dispatch(crawling_task);
    std::cout << "addcrawling task respose: " << response << std::endl;
}*/
