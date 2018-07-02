#include "crawled_taskhandler.h"
#include <mysql++/mysql++.h>
#include "bloomfilter.h"
#include "mysqlpp.h"
#include "task_manager.h"
CrawledtaskHandler::CrawledtaskHandler(
    const std::shared_ptr<TaskManager>& task_manager,
    const std::shared_ptr<MySqlpp> mysqlpp,
    const std::shared_ptr<ConcurrentQueue<spiderproto::CrawledTask>>
        concurrent_queue)
    : m_task_manager(task_manager),
      m_mysqlpp(mysqlpp),
      m_concurrent_queue(concurrent_queue),
      m_thread(nullptr),
      m_stop(false),
      m_bf(new BloomFilter(100000, 0.1)) {}

CrawledtaskHandler::~CrawledtaskHandler() {}

void CrawledtaskHandler::AddCrawledTask() {
    while (!m_stop.load(std::memory_order_relaxed)) {
        // read a task  from queue
        // bloomfilter
        // write mysqlpp
        // wrie taskmanager
        spiderproto::CrawledTask cdtask = m_concurrent_queue->pop();
        spiderproto::BasicTask btask;
        btask.set_taskid(cdtask.taskid());
        for (int i = 0; i < cdtask.links_size(); ++i) {
            if (!m_bf->KeyMatch(cdtask.links(i).url())) {
                m_bf->Insert(cdtask.links(i).url());

                spiderproto::CrawlUrlList* crawlurl_list =
                    btask.mutable_crawl_list();

                spiderproto::CrawlUrl* crawlurl =
                    crawlurl_list->add_crawl_urls();

                crawlurl->CopyFrom(cdtask.links(i));
            }
        }

        m_mysqlpp->UpdateTask(btask);
        m_task_manager->UpdateTask(btask);
    }
}

void CrawledtaskHandler::Start() {
    m_thread.reset(new std::thread(&CrawledtaskHandler::AddCrawledTask, this));
}

void CrawledtaskHandler::Join() { m_thread->join(); }

void CrawledtaskHandler::Stop() {
    m_stop.store(true, std::memory_order_relaxed);
}
